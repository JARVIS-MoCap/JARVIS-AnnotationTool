/*******************************************************************************
 * File:		graphcompleter.hpp
 * Created: 	22. Aug 2022
 * Author:		James Goodman
 * Contact: 	j.mike.goodman@gmail.com
 * Copyright:   2022 James Goodman
 * License:     LGPL v2.1
 ******************************************************************************/

// include guard: classic
#ifndef GRAPHCOMPLETER_H
#define GRAPHCOMPLETER_H

#include <vector> // vector, naturally
#include <cmath> // sqrt and pow
#include <stdexcept> // exceptions
#include <algorithm> // minmax, stable_sort
#include <iostream> // implementation of error messaging
#include <limits> // numeric_limits for handling division-by-zero edge cases

class GraphCompleter {
private:
	std::vector<int> *edges; // vector of edge weights. more intuitively represented as a symmetric matrix of shared frame counts between each pair of cameras, this class assumes these data have been converted to the more memory-efficient vector form.
	
	std::vector<std::vector<int>> pathsToBase; // for each node, the best path the base camera can take to reach it.
	std::vector<float> resampleFactors; // recommended resampling factors for each of the cameras

	int k; // maximum valid length of a daisy chain
	int t; // threshold frame count for valid edge
	int n; // number of cameras (nodes)
	int p; // number of pairs (edges)
	int b; // index of baseline camera

	int pairsFromNodes(void);

	int nodesFromPairs(void);

	void testValidGraph(void);

	int oddCeilRoot(int x);

	int getRow(int idx);

	int getCol(int idx, int row);

	int getIndex(int row_, int col_);

	float getFrameAdd(int edgeIndex);

public:
	explicit GraphCompleter(std::vector<int> *edges, int basecam, int threshold, int maxchain);

	std::vector<std::vector<int>> getPathsToBase(void);

	std::vector<float> getResampleFactors(void);

	bool completeGraph(void);
};

#endif