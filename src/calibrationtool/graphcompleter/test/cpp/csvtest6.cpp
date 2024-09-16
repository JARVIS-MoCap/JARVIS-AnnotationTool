#include "../../src/graphcompleter.hpp" 
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>

int main() {
	std::string fname = "./test/csv/testCase6.csv";

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
			bool getIslander = false;
			int basecam;
			std::unordered_set<int> islanders;
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

				// weird order, but meant to prevent getIslander from processing text instead of numerals
				if (word.compare("pairCounts") == 0) {
					collectEdges = true;
					getIslander = false;
				}

				if (getIslander) {
					islanders.insert(std::stoi(word));
				}

				if (word.compare("baseCam") == 0) {
					getBase = true;
				}

				if (word.compare("islanders") == 0) {
					getIslander = true;
				}
			}

			// next, test the graph completer
			GraphCompleter gc(&edges,basecam,threshold,maxchain);
			bool validGraph = gc.completeGraph();
			std::vector<std::vector<int>> pathsToBase = gc.getPathsToBase();
			std::vector<float> resampleFactors = gc.getResampleFactors();

			// std::cout << "======================================================================================================" << std::endl;
			// std::cout << "Test case " << testCaseIndex;

			// check all other fellas to see if they have a valid path
			bool validExceptIslanders = true;

			for (int idx=0; idx < resampleFactors.size(); ++idx){
				if (islanders.count(idx)==1) {
					validExceptIslanders = resampleFactors[idx] > 1;
				} else if (idx==basecam) {
					continue;
				} else {
					validExceptIslanders = resampleFactors[idx] == 1;
				}
				if (!validExceptIslanders){
					break;
				}
			}

			if (!validGraph && validExceptIslanders) {
				// std::cout << " passed";
			} else {
				// std::cout << " failed";
				++numFailedCases;
			}
			
			++testCaseIndex;
		}
	} else {
		std::cout << "======================================================================================================" << std::endl;
		std::cout << "Test cases for set 6 failed: ";
		std::cout << "Could not open the file" << std::endl;
		std::cout << "======================================================================================================" << std::endl << std::endl;
		return 1;
	}

	std::cout << "======================================================================================================" << std::endl;
	std::cout << "Number of failed test cases for set 6: " << numFailedCases << std::endl;
	std::cout << "======================================================================================================" << std::endl << std::endl;

	return 0;
}