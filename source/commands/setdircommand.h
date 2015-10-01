#ifndef SETDIRCOMMAND_H
#define SETDIRCOMMAND_H

/*
 *  setoutdircommand.h
 *  Mothur
 *
 *  Created by westcott on 1/21/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "command.hpp"
#include "commandfactory.hpp"

 /**********************************************************/

class SetDirectoryCommand : public Command {

public:
	SetDirectoryCommand(Settings& settings, string option);
	SetDirectoryCommand(Settings& settings) : Command(settings) { abort = true; calledHelp = true; setParameters(); }
	~SetDirectoryCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "set.dir"; }
	string getCommandCategory() { return "General"; }

	string getHelpString();
	string getOutputPattern(string) { return ""; }
	string getCitation() { return "http://www.mothur.org/wiki/Set.dir"; }
	string getDescription() { return "set input, output and default directories"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:
	CommandFactory* commandFactory;
	string output, input, tempdefault;
	bool abort, debugorSeedOnly, modifyNames;
	int random;
	vector<string> outputNames;


};

/**********************************************************/

#endif

