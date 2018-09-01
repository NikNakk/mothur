#pragma once

/*
 *  getlabelcommand.h
 *  Mothur
 *
 *  Created by Thomas Ryabin on 1/30/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "command.hpp"
#include "ordervector.hpp"
#include "inputdata.h"


class GetlabelCommand : public Command {
public:
	GetlabelCommand(Settings& settings, string option);
	GetlabelCommand(Settings& settings);
	~GetlabelCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "get.label"; }
	string getCommandCategory() { return "OTU-Based Approaches"; }
	string getHelpString();
	string getOutputPattern(string) { return ""; }
	string getCitation() { return "http://www.mothur.org/wiki/Get.label"; }
	string getDescription() { return "outputs labels"; }


	int execute();
	void help() { LOG(INFO) << getHelpString(); }


private:
	string inputfile, listfile, rabundfile, sabundfile, format;
	bool abort;
	vector<string> outputNames;
};


