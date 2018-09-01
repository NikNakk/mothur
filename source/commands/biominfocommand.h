//
//  biominfocommand.h
//  Mothur
//
//  Created by Sarah Westcott on 8/5/15.
//  Copyright (c) 2015 Schloss Lab. All rights reserved.
//

#pragma once

#include "command.hpp"
#include "inputdata.h"
#include "phylosummary.h"

class BiomInfoCommand : public Command {

#ifdef UNIT_TEST
	friend class TestBiomInfoCommand;
#endif

public:
	BiomInfoCommand(Settings& settings, string option);
	BiomInfoCommand(Settings& settings);
	~BiomInfoCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "biom.info"; }
	string getCommandCategory() { return "OTU-Based Approaches"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Biom.info"; }
	string getDescription() { return "create 'mothur' files from a biom file. ie: shared, taxonomy, constaxonomy"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:
	void printSharedData(vector<SharedRAbundVector*>, ofstream&);
	int createFilesFromBiom();
	string getTag(string&);
	string getName(string);
	string getTaxonomy(string, string);
	string addUnclassifieds(string tax);
	vector< vector<string> > readRows(string, int&, bool&);
	int getDims(string, int&, int&);
	vector<SharedRAbundVector*> readData(string, string, string, vector<string>&, int);
	vector<string> getNamesAndTaxonomies(string);

	vector<string> outputNames;
	string fileroot, outputDir, biomfile, label, basis;
	bool firsttime, abort, relabund;
	int maxLevel;

};


 /* defined(__Mothur__biominfocommand__) */
