#pragma once

/*
 *  mergefilecommand.h
 *  Mothur
 *
 *  Created by Pat Schloss on 6/14/09.
 *  Copyright 2009 Patrick D. Schloss. All rights reserved.
 *
 */

#include "mothur.h"
#include "command.hpp"

class MergeFileCommand : public Command {
public:
	MergeFileCommand(Settings& settings, string option);
	MergeFileCommand(Settings& settings);
	~MergeFileCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "merge.files"; }
	string getCommandCategory() { return "General"; }
	string getHelpString();
	string getOutputPattern(string) { return ""; }
	string getCitation() { return "http://www.mothur.org/wiki/Merge.files"; }
	string getDescription() { return "appends files creating one file"; }


	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:
	vector<string> fileNames, outputNames;
	string outputFileName;
	int numInputFiles;
	bool abort;
};


