#include "command.hpp"
#include "utility.h"
#include <iterator>

Command::Command(Settings& settings) : settings(settings), abort(true), calledHelp(true), parameters(settings) {}

Command::Command(Settings& settings, ParameterListToProcess ptp) : settings(settings), parameters(settings), paramsToProcess(ptp) {}

void Command::setup() {
	if (paramsToProcess.size() > 0 && paramsToProcess[0].parameterName == "help") {
		abort = true;
		calledHelp = true;
		this->help();
	}
	else if (paramsToProcess.size() > 0 && paramsToProcess[0].parameterName == "citation") {
		abort = true;
		calledHelp = true;
		this->citation();
	}
	else {
		try {
			this->setParameters();
			this->setOutputTypes();
			parameters.validateAndSet(paramsToProcess);
			this->validateSpecial();
		}
		catch (exception& e) {
			LOG(WARNING) << string("setting up command: ") + e.what();
			abort = true;
		}
	}
}


std::string Command::getOutputFileName(std::string type, std::map<std::string, std::string> variableParts, std::string existingName, std::map<std::string, std::string> vPartsIfExisting) {
	std::string outName = getOutputFileName(type, variableParts);
	if (outName == existingName) {
		outName = getOutputFileName(type, vPartsIfExisting);
	}
	return outName;
}


string Command::getOutputFileName(string type, map<string, string> variableParts) {
	//uses the pattern to create an output filename for a given type and input file name.  
	std::string filename = "";
	OutputTypeCollection::iterator it;

	//is this a type this command creates
	it = outputTypes.find(type);
	if (it == outputTypes.end()) {
		LOG(LOGERROR) << "this command doesn't create a " + type + " output file.\n";
	}
	else {
		string patternTemp = it->second.getFilePattern();
		vector<string> patterns = Utility::split(patternTemp, '-', true);

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
			if (!foundPattern) { LOG(LOGERROR) << "Not enough variable pieces for " + type + ".\n"; abort = true; }
		}

		if (pattern != "") {
			int numVariables = 0;
			for (int i = 0; i < pattern.length(); i++) { if (pattern[i] == '[') { numVariables++; } }

			vector<string> pieces = Utility::split(pattern, ',', true);


			for (int i = 0; i < pieces.size(); i++) {
				if (pieces[i][0] == '[') {
					map<string, string>::iterator it = variableParts.find(pieces[i]);
					if (it == variableParts.end()) {
						LOG(LOGERROR) << "Did not provide variable for " + pieces[i] + ".\n"; abort = true;
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

void Command::help() {
	LOG(INFO) << getHelpString();
}

void Command::citation() {
	LOG(INFO) << getCitation();
}

void Command::removeAllOutput()
{
	for (auto ot : outputTypes) {
		for (auto fil : ot.second.files) {
			if (File::FileExists(fil)) {
				File::remove(fil);
			}
		}
	}
}