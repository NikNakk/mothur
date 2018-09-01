//
//  removedistscommand.h
//  Mothur
//
//  Created by Sarah Westcott on 1/29/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#pragma once

#include "command.hpp"

class RemoveDistsCommand : public Command {

public:

	RemoveDistsCommand(Settings& settings, string option);
	RemoveDistsCommand(Settings& settings);
	~RemoveDistsCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "remove.dists"; }
	string getCommandCategory() { return "General"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Remove.dists"; }
	string getDescription() { return "removes distances from a phylip or column file related to groups or sequences listed in an accnos file"; }


	int execute();
	void help() { LOG(INFO) << getHelpString(); }


private:
	set<string> names;
	string accnosfile, phylipfile, columnfile, outputDir;
	bool abort;
	vector<string> outputNames;

	int readPhylip();
	int readColumn();

};



