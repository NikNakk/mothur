#pragma once

/*
 *  removerarecommand.h
 *  mothur
 *
 *  Created by westcott on 1/21/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */


#include "command.hpp"
#include "listvector.hpp"


class RemoveRareCommand : public Command {

public:

	RemoveRareCommand(Settings& settings, string option);
	RemoveRareCommand(Settings& settings);
	~RemoveRareCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "remove.rare"; }
	string getCommandCategory() { return "OTU-Based Approaches"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Remove.rare"; }
	string getDescription() { return "removes rare sequences from a sabund, rabund, shared or list and group file"; }


	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:
	string sabundfile, rabundfile, sharedfile, groupfile, countfile, listfile, outputDir, groups, label;
	int nseqs, allLines;
	bool abort, byGroup;
	vector<string> outputNames, Groups;
	set<string> labels;

	int processSabund();
	int processRabund();
	int processList();
	int processShared();
	int processLookup(vector<SharedRAbundVector*>&);

};






