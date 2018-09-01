#pragma once

/*
 *  getsabundcommand.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 6/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */


#include "command.hpp"
#include "inputdata.h"
#include "sabundvector.hpp"

class GetSAbundCommand : public Command {
public:
	GetSAbundCommand(Settings& settings, string option);
	GetSAbundCommand(Settings& settings);
	~GetSAbundCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "get.sabund"; }
	string getCommandCategory() { return "OTU-Based Approaches"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Get.sabund"; }
	string getDescription() { return "creates a sabund file"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:
	string filename, format, inputfile, listfile, rabundfile, outputDir, countfile;
	ofstream out;
	vector<string> outputNames;

	bool abort, allLines;
	set<string> labels; //holds labels to be used
	string label;

	int processList(ofstream& out);
	int createRabund(CountTable& ct, ListVector*& list, RAbundVector*& rabund);

};


