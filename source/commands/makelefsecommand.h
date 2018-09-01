//
//  makelefse.h
//  Mothur
//
//  Created by SarahsWork on 6/3/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#pragma once

#include "command.hpp"
#include "inputdata.h"
#include "sharedutilities.h"
#include "phylosummary.h"

/**************************************************************************************************/

class MakeLefseCommand : public Command {
public:
	MakeLefseCommand(Settings& settings, string option);
	MakeLefseCommand(Settings& settings);
	~MakeLefseCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "make.lefse"; }
	string getCommandCategory() { return "General"; }

	string getOutputPattern(string);
	string getHelpString();
	string getCitation() { return "http://huttenhower.sph.harvard.edu/galaxy/root?tool_id=lefse_upload http://www.mothur.org/wiki/Make.lefse"; }
	string getDescription() { return "creates LEfSe input file"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:
	bool abort, allLines, otulabel, hasGroupInfo;
	string outputDir;
	vector<string> outputNames, Groups;
	string sharedfile, designfile, constaxonomyfile, relabundfile, scale, label, inputFile;

	int runRelabund(map<int, consTax2>&, vector<SharedRAbundFloatVector*>&);

	vector<SharedRAbundFloatVector*> getRelabund();
	vector<SharedRAbundFloatVector*> getSharedRelabund();
};

/**************************************************************************************************/




 /* defined(__Mothur__makelefse__) */
