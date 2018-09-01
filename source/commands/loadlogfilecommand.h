#pragma once

//
//  loadlogfilecommand.h
//  Mothur
//
//  Created by Sarah Westcott on 6/13/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//


#include "command.hpp"

/**************************************************************************************************/

class LoadLogfileCommand : public Command {
public:
	LoadLogfileCommand(Settings& settings, string option);
	LoadLogfileCommand(Settings& settings);
	~LoadLogfileCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "load.logfile"; }
	string getCommandCategory() { return "General"; }
	string getHelpString();
	string getOutputPattern(string) { return ""; }
	string getCitation() { return "http://www.mothur.org/wiki/Load.logfile"; }
	string getDescription() { return "extracts current files from a logfile"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:
	bool abort;
	string outputDir, logfile;
	vector<string> outputNames;

	int updateCurrent(string pattern, string type, string, vector<string> outputNames, map<string, string>& currentFiles);
};

/**************************************************************************************************/





