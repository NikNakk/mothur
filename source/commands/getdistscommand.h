//
//  getdistscommand.h
//  Mothur
//
//  Created by Sarah Westcott on 1/28/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#pragma once

#include "command.hpp"

class GetDistsCommand : public Command {

public:

	GetDistsCommand(Settings& settings, string option);
	GetDistsCommand(Settings& settings);
	~GetDistsCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "get.dists"; }
	string getCommandCategory() { return "General"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Get.dists"; }
	string getDescription() { return "gets distances from a phylip or column file related to groups or sequences listed in an accnos file"; }


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



