#include "command.hpp"
#include <iterator>

Command::Command() : m(MothurOut::getInstance()), abort(true), calledHelp(true) {
	this->setParameters();
}

Command::Command(string option) : m(MothurOut::getInstance()), abort(false), calledHelp(false) {
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }
	else {
		try {
			this->setParameters();
			this->setOutputTypes();
			nkParameters.parseOptionString(option);
		}
		catch (exception& e) {
			m->mothurOut(string("[ERROR] setting up command: ") + e.what());
			m->control_pressed = true;
		}
	}
}



string Command::getOutputFileName(string type, map<string, string> variableParts) {
	//uses the pattern to create an output filename for a given type and input file name.  
	try {
		string filename = "";
		map<string, vector<string> >::iterator it;

		//is this a type this command creates
		it = outputTypes.find(type);
		if (it == outputTypes.end()) { m->mothurOut("[ERROR]: this command doesn't create a " + type + " output file.\n"); }
		else {

			string patternTemp = getOutputPattern(type);
			vector<string> patterns;
			m->splitAtDash(patternTemp, patterns);

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
				if (!foundPattern) { m->mothurOut("[ERROR]: Not enough variable pieces for " + type + ".\n"); m->control_pressed = true; }
			}

			if (pattern != "") {
				int numVariables = 0;
				for (int i = 0; i < pattern.length(); i++) { if (pattern[i] == '[') { numVariables++; } }

				vector<string> pieces;
				m->splitAtComma(pattern, pieces);


				for (int i = 0; i < pieces.size(); i++) {
					if (pieces[i][0] == '[') {
						map<string, string>::iterator it = variableParts.find(pieces[i]);
						if (it == variableParts.end()) {
							m->mothurOut("[ERROR]: Did not provide variable for " + pieces[i] + ".\n"); m->control_pressed = true;
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
		}
		return filename;
	}
	catch (exception& e) {
		m->errorOut(e, "command", "getOutputFileName");
		exit(1);
	}
}

void Command::help() {
	m->mothurOut(getHelpString());
}

void Command::citation() {
	m->mothurOutEndLine(); m->mothurOut(getCitation()); m->mothurOutEndLine();
}
