#include "../../src/graphcompleter.hpp" 
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>
  
int main() {
	std::string fname = "./test/csv/testCase4.csv";

	// some params
	int threshold = 30;
	int maxchain;
	 
	std::string line, word;
	 
	std::fstream file (fname, std::ios::in);

	int testCaseIndex  = 0;
	int numFailedCases = 0;

	if(file.is_open()) {
		while(std::getline(file, line)) {
			bool collectEdges    = false;
			bool getBase         = false;
			bool getDaisyChainer = false;
			bool getCamCount     = false;
			int basecam;
			std::vector<std::vector<int>> daisyChainers;
			std::vector<int> tempChainers;
			std::vector<int> edges;
		 
			std::stringstream str(line);

			while(std::getline(str, word, '\t')) {
				// compare function returns 0 for "true", otherwise it gives a little info about how they don't match with a nonzero signed int
				if (collectEdges) {
					edges.push_back(std::stoi(word));
				}

				if (getBase) {
					basecam = std::stoi(word);
					getBase = false;
				}

				if (getCamCount) {
					maxchain = std::stoi(word) / 2 + 2; // consistent with the test case parameters
					getCamCount = false;
				}

				// weird order, but meant to prevent getDaisyChainer from processing text instead of numerals
				if (word.compare("pairCounts") == 0) {
					collectEdges = true;
					getDaisyChainer = false;
					daisyChainers.push_back(tempChainers); // don't skip the last one!
				}

				if (word.compare("longDaisyChainers") == 0) {
					getDaisyChainer = true;
					if (tempChainers.size() > 0) {
						daisyChainers.push_back(tempChainers);
					}
					tempChainers.clear();
					continue; // messy control statement use but helps to prevent getDaisyChainer from processing text. In retrospect, a switch-case structure or else if structure would've worked better.
				}

				if (getDaisyChainer) {
					tempChainers.push_back(std::stoi(word));
				}

				if (word.compare("baseCam") == 0) {
					getBase = true;
				}

				if (word.compare("nCams") == 0) {
					getCamCount = true;
				}
			}

			// next, test the graph completer
			GraphCompleter gc(&edges,basecam,threshold,maxchain);
			bool validGraph = gc.completeGraph();
			std::vector<std::vector<int>> pathsToBase = gc.getPathsToBase();
			std::vector<float> resampleFactors = gc.getResampleFactors();

			// std::cout << "======================================================================================================" << std::endl;
			// std::cout << "Test case " << testCaseIndex;

			// let's make a lookup table of all base-connectors
			std::unordered_set<int> baseConnectors;

			for (const auto &chain : daisyChainers) {
				baseConnectors.insert( chain[0] );
			}

			std::unordered_set<int> longChainers;

			// now run through all chains, make sure all of the fellas that aren't a member of baseConnectors are ending up with larger-than-1 chains
			int whichChain = 0;
			while (validGraph && whichChain < daisyChainers.size()) {
				std::vector<int> &chain = daisyChainers[whichChain];
				int whichChainer = 1; // ignore the first index, you already processed them
				while (validGraph && whichChainer < chain.size()) {
					int &chainer = chain[whichChainer];
					if (baseConnectors.count(chainer) == 0) {
						validGraph = pathsToBase[chainer].size() > 1;
						longChainers.insert(chainer);
					}
					++whichChainer;
				}
				++whichChain;
			}

			if (validGraph) {
				// std::cout << " passed";
			} else {
				// std::cout << " failed";
				++numFailedCases;
			}
			
			++testCaseIndex;
		}
	} else {
		std::cout << "======================================================================================================" << std::endl;
		std::cout << "Test cases for set 4 failed: ";
		std::cout << "Could not open the file" << std::endl;
		std::cout << "======================================================================================================" << std::endl << std::endl;
		return 1;
	}

	std::cout << "======================================================================================================" << std::endl;
	std::cout << "Number of failed test cases for set 4: " << numFailedCases << std::endl;
	std::cout << "======================================================================================================" << std::endl << std::endl;

	return 0;
}