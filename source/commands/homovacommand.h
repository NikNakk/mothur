#pragma once

/*
 *  homovacommand.h
 *  mothur
 *
 *  Created by westcott on 2/8/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */


#include "command.hpp"

class DesignMap;

class HomovaCommand : public Command {

public:
	HomovaCommand(Settings& settings, string option);
	HomovaCommand(Settings& settings);
	~HomovaCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "homova"; }
	string getCommandCategory() { return "Hypothesis Testing"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "Stewart CN, Excoffier L (1996). Assessing population genetic structure and variability with RAPD data: Application to Vaccinium macrocarpon (American Cranberry). J Evol Biol 9: 153-71. \nhttp://www.mothur.org/wiki/Homova"; }
	string getDescription() { return "homova"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:
	double runHOMOVA(ofstream&, map<string, vector<int> >, double);
	double calcSigleSSWithin(vector<int>);
	double calcBValue(map<string, vector<int> >, vector<double>&);
	map<string, vector<int> > getRandomizedGroups(map<string, vector<int> >);

	bool abort;
	vector<string> outputNames, Sets;

	string outputDir, inputDir, designFileName, phylipFileName;
	DesignMap* designMap;
	vector< vector<double> > distanceMatrix;
	int iters;
	double experimentwiseAlpha;
};


