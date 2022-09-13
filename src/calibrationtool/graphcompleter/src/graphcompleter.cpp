/*******************************************************************************
 * File:		graphcompleter.cpp
 * Created: 	22. Aug 2022
 * Author:		James Goodman
 * Contact: 	j.mike.goodman@gmail.com
 * Copyright:   2022 James Goodman
 * License:     LGPL v2.1
 ******************************************************************************/

#include "graphcompleter.hpp"
 
GraphCompleter::GraphCompleter(std::vector<int> *edges, int basecam, int threshold, int maxchain) :
edges(edges), b(basecam), t(threshold), k(maxchain), p(int(edges->size())) 
{
	n = nodesFromPairs();

	try {
		testValidGraph();
	} catch (std::invalid_argument &e) {
		std::cout << "error: " << e.what() << std::endl;
		exit(1);
	}
};

int GraphCompleter::pairsFromNodes(void) {
	return n*(n-1)/2;
}

int GraphCompleter::nodesFromPairs(void) {
	return (1 + int(std::sqrt(1+8*p)))/2;
}

void GraphCompleter::testValidGraph(void) {
	if (pairsFromNodes() != p) {
		throw std::invalid_argument("edges vector does not have a valid number of pairs.");
	}
}

int GraphCompleter::oddCeilRoot(int x) {
	int res = int(std::sqrt(x));

	if (int(std::pow(res,2))<x || res%2==0) {
		res = 2*((res-1)/2 + 1) + 1;
	}

	return res;
}

int GraphCompleter::getRow(int idx) {
	if (idx >= p || idx < 0) {
		throw std::invalid_argument("index argument to getRow exceeded array extents.");
	}
	return n - (1 + oddCeilRoot(4*int(std::pow(n,2))-4*n+1-8*idx) )/2;
}

int GraphCompleter::getCol(int idx, int row) {
	if (idx >= p || row >= n || idx < 0 || row < 0) {
		throw std::invalid_argument("index or row argument to getCol exceeded array extents.");
	}
	return idx + row + 1 - row*n + (row*(row+1))/2;
}


int GraphCompleter::getIndex(int row_, int col_) {
	// row should always be the smaller of the two
	std::pair<int,int> rowcol = std::minmax(row_,col_);

	int &row = rowcol.first;
	int &col = rowcol.second;

	if (row >= (n-1) || col >= n || row < 0 || col < 0) {
		throw std::invalid_argument("row_ or col_ argument to getIndex exceeded array extents.");
	}
	return row*n-(row*(row+1))/2+col-row-1;
}

std::vector<std::vector<int>> GraphCompleter::getPathsToBase(void) {
	return pathsToBase;
}

std::vector<float> GraphCompleter::getResampleFactors(void) {
	return resampleFactors;
}

float GraphCompleter::getFrameAdd(int edgeIndex) {
	// think of "frame add" as: recommended upsampling factor, minus one (will always be zero if edge is above threshold)
	if (edges->at(edgeIndex) == 0) {
		return std::numeric_limits<float>::max(); // no dividing by zero!
	}
	if (edges->at(edgeIndex) < t) {
		return float(t - edges->at(edgeIndex)) / float(edges->at(edgeIndex));
	} else {
		return 0;
	}
}

bool GraphCompleter::completeGraph(void) {
	// hierarchical depth progression like Ncams does: O(kp)
	// Kruskal's algorithm: O(p log n), but ignores the b and k parameters, which the JARVIS framework would like to give users control over 
	// (b to permit an intuitive coordinate frame, k to mitigate the instability that daisy-chaining can introduce to extrinsics inference)
	// in any case, it's probably safe to assume that k will be on the order of log n anyway
	// ergo, hierarchical depth progression it is!

	std::vector<std::vector<int>> &paths = pathsToBase;
	paths.clear(); // idempotence
	std::vector<float> pathlens; // in terms of frame adds, not frame counts!

	// start with depth=0 case, wherein you actually first populate paths & pathlens
	bool valid = true;

	for (int i = 0; i < n; ++i) {
		std::vector<int> path;
		float pathlen;
		if (b != i) {
			int pairIndex = getIndex(b,i);
			path.push_back(pairIndex);
			pathlen = getFrameAdd(pairIndex);

			if (pathlen > 0) {
				valid = false;
			}
		} else {
			pathlen = 0; // the base camera is ground truth!
		}

		paths.push_back(path);
		pathlens.push_back(pathlen);
	}

	// now iterate for each depth until k
	int depth = 1;
	while (depth < k && !valid) {
		valid = true; // set this back to false if you fail to find a valid path for EVERY node

		// save a snapshot of the current state to avoid accidentally testing super long daisy chains as you update state
		// there might be a more memory-efficient way to do this
		std::vector<float> templens = pathlens;
		std::vector<std::vector<int>> temppaths = paths;

		for (int i = 0; i < (n-1); ++i) {
			for (int j = (i+1); j < n; ++j) {
				int pairIndex = getIndex(i,j);
				float pathleni = templens[j];
				float pathlenj = templens[i];
				float frameadd = getFrameAdd(pairIndex);

				// conditionality so that path lengths are now the sum of nodal resampling factors, rather than merely being assessed at the edges
				if (pathleni == 0) {
					pathleni += 2*frameadd; // two cameras must now be resampled
				} else if ( getFrameAdd(temppaths[j].back()) < frameadd ) {
					// new camera must be resampled, and previous one must be further upsampled
					pathleni += 2*frameadd;
					pathleni -= getFrameAdd(temppaths[j].back());
				} else {
					// just the latest camera needs to be upsampled, the previous one has enough new frames to cover it
					pathleni += frameadd;
				}

				if (pathlenj==0) {
					pathlenj += 2*frameadd;
				} else if (getFrameAdd(temppaths[i].back()) < frameadd ) {
					pathlenj += 2*frameadd;
					pathlenj -= getFrameAdd(temppaths[i].back());
				} else {
					pathlenj += frameadd;
				}

				if (pathleni < pathlens[i]) {
					pathlens[i] = pathleni;
					paths[i]    = temppaths[j];
					paths[i].push_back(pairIndex);
				}

				if (pathlenj < pathlens[j]) {
					pathlens[j] = pathlenj;
					paths[j]    = temppaths[i];
					paths[j].push_back(pairIndex);
				}
			}

			// check to see if you're still on pace to produce a valid tree, or if you still haven't found *the one*
			if (pathlens[i]>0) {
				valid = false;
			}
		}
		// make sure to check the LAST index, too! (since "i" only iterates up to, but not THROUGH, n-1)
		if (pathlens[n-1]>0) {
			valid = false;
		}
		++depth;
	}

	// now determine the upsampling factors you want to use:

	// init (for idempotence)
	resampleFactors.clear();
	for (int i = 0; i < paths.size(); ++i) {
		resampleFactors.push_back(1); // resampling factor of 1 = change nothing.
	}

	// compute
	for (int i = 0; i < paths.size(); ++i) {
		if (pathlens[i] > 0) {
			for (int j=0; j<paths[i].size(); ++j) {
				if (edges->at(paths[i][j]) < t) {
					float candidate = 1 + getFrameAdd(paths[i][j]);
					int row = getRow(paths[i][j]);
					int col = getCol(paths[i][j],row);

					if (resampleFactors[row] < candidate) {
						resampleFactors[row] = candidate;
					}

					if (resampleFactors[col] < candidate) {
						resampleFactors[col] = candidate;
					}
				}
			}
		}
	}

	// and now return if the path was valid or not
	return valid;

}