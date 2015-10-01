#ifndef GETRABUNDCOMMAND_H
#define GETRABUNDCOMMAND_H

/*
 *  getrabundcommand.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 6/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */


#include "command.hpp"
#include "inputdata.h"
#include "listvector.hpp"


class GetRAbundCommand : public Command {
public:
	GetRAbundCommand(Settings& settings, string option);
	GetRAbundCommand(Settings& settings);
	~GetRAbundCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "get.rabund"; }
	string getCommandCategory() { return "OTU-Based Approaches"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Get.rabund"; }
	string getDescription() { return "creates a rabund file"; }


	int execute();
	void help() { LOG(INFO) << getHelpString(); }


private:

	string filename, listfile, sabundfile, inputfile, format, outputDir, countfile;
	ofstream out;
	vector<string> outputNames;

	bool abort, allLines, sorted;
	set<string> labels; //holds labels to be used
	string label;

	int processList(ofstream& out);
	int createRabund(CountTable& ct, ListVector*& list, RAbundVector*& rabund);
};

#endif

