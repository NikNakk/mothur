#include "outputtype.h"
#include "utility.h"
#include <sstream>
#include "g3log/g3log.hpp"

string OutputType::getOutputFileName(map<string, string> variableParts)
//uses the pattern to create an output filename for a given type and input file name.  
{
	try {
		string filename = "";
		vector<string> patterns;
		Utility::split(filePattern, '-', patterns);
		//find pattern to use based on number of variables passed in
		string pattern = "";
		bool foundPattern = false;
		vector<int> numVariablesPerPattern;
		for (int i = 0; i < patterns.size(); i++) {
			int numVariables = 0;
			for (int j = 0; j < patterns[i].length(); j++) { if (patterns[i][j] == '[') { numVariables++; } }
			numVariablesPerPattern.push_back(numVariables);

			if (numVariables == variableParts.size()) { pattern = patterns[i]; foundPattern = true; break; }
		}

		//if you didn't find an exact match do we have something that might work
		if (!foundPattern) {
			for (int i = 0; i < numVariablesPerPattern.size(); i++) {
				if (numVariablesPerPattern[i] < variableParts.size()) { pattern = patterns[i]; foundPattern = true; break; }
			}
			if (!foundPattern) { LOG(LOGERROR) << "[ERROR]: Not enough variable pieces."; }//ctrlc_pressed = true; }
		}

		if (pattern != "") {
			int numVariables = 0;
			for (int i = 0; i < pattern.length(); i++) { if (pattern[i] == '[') { numVariables++; } }

			vector<string> pieces;
			Utility::split(pattern, ',', pieces);


			for (int i = 0; i < pieces.size(); i++) {
				if (pieces[i][0] == '[') {
					map<string, string>::iterator it = variableParts.find(pieces[i]);
					if (it == variableParts.end()) {
						LOG(LOGERROR) << "Did not provide variable for " + pieces[i] + ".";
						throw(invalid_argument("Did not provide variable for " + pieces[i] + "."));
					}
					else {
						if (it->second != "") {
							if (it->first == "[filename]") { filename += it->second; }
							else if (it->first == "[extension]") {
								if (filename.length() > 0) { //rip off last "."
									filename = filename.substr(0, filename.length() - 1);
								}
								filename += it->second + ".";
							}
							else { filename += it->second + "."; }
						}
					}
				}
				else {
					filename += pieces[i] + ".";
				}
			}
			if (filename.length() > 0) { //rip off last "."
				filename = filename.substr(0, filename.length() - 1);
			}
		}
		return filename;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what();
		exit(1);
	}
}
