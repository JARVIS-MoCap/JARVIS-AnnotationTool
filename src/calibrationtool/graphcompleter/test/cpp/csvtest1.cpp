#include "../../src/graphcompleter.hpp" 
#include <fstream>
#include <string>
#include <sstream>
  
int main() {
	std::string fname = "./test/csv/testCase1.csv";

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
			int daisyChainer;
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

				if (getDaisyChainer) {
					daisyChainer    = std::stoi(word);
					getDaisyChainer = false;
				}

				if (word.compare("baseCam") == 0) {
					getBase = true;
				}

				if (word.compare("pairCounts") == 0) {
					collectEdges = true;
				}

				if (word.compare("daisyChainer") == 0) {
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

			if (validGraph && pathsToBase[daisyChainer].size() > 1) {
				// std::cout << " passed";
			} else {
				// std::cout << " failed";
				++numFailedCases;
			}
			
			++testCaseIndex;
		}
	} else {
		std::cout << "======================================================================================================" << std::endl;
		std::cout << "Test cases for set 1 failed: ";
		std::cout << "Could not open the file" << std::endl;
		std::cout << "======================================================================================================" << std::endl << std::endl;
		return 1;
	}

	std::cout << "======================================================================================================" << std::endl;
	std::cout << "Number of failed test cases for set 1: " << numFailedCases << std::endl;
	std::cout << "======================================================================================================" << std::endl << std::endl;

	return 0;
}