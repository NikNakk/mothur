#pragma once

/*
 *  makegroupcommand.h
 *  Mothur
 *
 *  Created by westcott on 5/7/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "command.hpp"

class MakeGroupCommand : public Command {

public:
	MakeGroupCommand(Settings& settings, string option);
	MakeGroupCommand(Settings& settings);
	~MakeGroupCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "make.group"; }
	string getCommandCategory() { return "General"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Make.group"; }
	string getDescription() { return "creates a group file"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:

	string fastaFileName, groups, outputDir, filename, output;
	vector<string> fastaFileNames;
	vector<string> groupsNames, outputNames;

	bool abort;
};



