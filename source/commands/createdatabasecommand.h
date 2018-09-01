#pragma once

//
//  createdatabasecommand.h
//  Mothur
//
//  Created by Sarah Westcott on 3/28/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "command.hpp"
#include "listvector.hpp"
#include "sequence.hpp"

class CreateDatabaseCommand : public Command {
public:
	CreateDatabaseCommand(Settings& settings, string option);
	CreateDatabaseCommand(Settings& settings);
	~CreateDatabaseCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "create.database"; }
	string getCommandCategory() { return "OTU-Based Approaches"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Create.database"; }
	string getDescription() { return "creates database file that includes, abundances across groups, representative sequences, and taxonomy for each OTU"; }


	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:

	bool abort;
	string sharedfile, listfile, groupfile, repfastafile, repnamesfile, contaxonomyfile, label, outputDir, countfile;

	vector<string> outputNames;

	vector<int> readFasta(vector<Sequence>&);
	vector<int> readTax(vector<string>&, vector<string>&);
	ListVector* getList();
	vector<SharedRAbundVector*> getShared();
	int findIndex(vector<string>&, string);

};





