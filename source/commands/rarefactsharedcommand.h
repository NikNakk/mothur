#ifndef RAREFACTSHAREDCOMMAND_H
#define RAREFACTSHAREDCOMMAND_H
/*
 *  rarefactsharedcommand.h
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/6/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "command.hpp"
#include "inputdata.h"
#include "rarefact.h"
#include "display.h"
#include "validcalculator.h"
#include "designmap.h"

class RareFactSharedCommand : public Command {

public:
	RareFactSharedCommand(Settings& settings, string option);
	RareFactSharedCommand(Settings& settings);
	~RareFactSharedCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "rarefaction.shared"; }
	string getCommandCategory() { return "OTU-Based Approaches"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "Magurran AE (2004). Measuring biological diversity. Blackwell Pub.: Malden, Ma. \nhttp://www.mothur.org/wiki/Rarefaction.shared"; }
	string getDescription() { return "generate inter-sample rarefaction curves using a re-sampling without replacement approach"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }


private:

	vector<SharedRAbundVector*> lookup;
	int nIters, subsampleSize, iters;
	string format;
	float freq;

	map<int, string> file2Group; //index in outputNames[i] -> group
	bool abort, allLines, jumble, groupMode, subsample;
	set<string> labels; //holds labels to be used
	string label, calc, groups, outputDir, sharedfile, designfile;
	vector<string>  Estimators, Groups, outputNames, Sets;

	int process(DesignMap&, string);
	vector<string> createGroupFile(vector<string>&);
	int subsampleLookup(vector<SharedRAbundVector*>&, string);

};

#endif
