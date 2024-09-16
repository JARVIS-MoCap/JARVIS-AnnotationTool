#include "../../src/graphcompleter.hpp" 
#include <fstream>
#include <string>
#include <sstream>
  
int main() {
	std::string fname = "./test/csv/testCase2.csv";

	// some params
	int threshold = 30;
	int maxchain  = 2;
	 
	std::string line, word;
	 
	std::fstream file (fname, std::ios::in);

	int testCaseIndex  = 0;
	int numFailedCases = 0;

	if(file.is_open()) {
		while(std::getline(file, line)) {
			bool collectEdges    = false;
			bool getBase         = false;
			bool getDaisyChainer = false;
			int basecam;
			std::vector<int> daisyChainers;
			std::vector<int> edges;
		 
			std::stringstream str(line);

			while(std::getline(str, word, '\t')) {
				if (collectEdges) {
					edges.push_back(std::stoi(word));
				}

				if (getBase) {
					basecam = std::stoi(word);
					getBase = false;
				}

				// weird order, but meant to prevent getDaisyChainer from processing text instead of numerals
				if (word.compare("pairCounts") == 0) {
					collectEdges = true;
					getDaisyChainer = false;
				}

				if (getDaisyChainer) {
					daisyChainers.push_back(std::stoi(word));
				}

				if (word.compare("baseCam") == 0) {
					getBase = true;
				}

				if (word.compare("daisyChainers") == 0) {
					getDaisyChainer = true;
				}
			}

			// next, test the graph completer
			GraphCompleter gc(&edges,basecam,threshold,maxchain);
			bool validGraph = gc.completeGraph();
			std::vector<std::vector<int>> pathsToBase = gc.getPathsToBase();
			std::vector<float> resampleFactors = gc.getResampleFactors();

			// std::cout << "======================================================================================================" << std::endl;
			// std::cout << "Test case " << testCaseIndex;

			int whichChainer = 0;
			while (validGraph && whichChainer < daisyChainers.size()) {
				int thisChainer = daisyChainers[whichChainer];
				validGraph = pathsToBase[thisChainer].size() > 1;
				++whichChainer;
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
		std::cout << "Test cases for set 2 failed: ";
		std::cout << "Could not open the file" << std::endl;
		std::cout << "======================================================================================================" << std::endl << std::endl;
		return 1;
	}

	std::cout << "======================================================================================================" << std::endl;
	std::cout << "Number of failed test cases for set 2: " << numFailedCases << std::endl;
	std::cout << "======================================================================================================" << std::endl << std::endl;

	return 0;
}