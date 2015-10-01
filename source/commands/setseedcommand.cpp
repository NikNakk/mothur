//
//  setseedcommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 3/24/15.
//  Copyright (c) 2015 Schloss Lab. All rights reserved.
//

#include "setseedcommand.h"

//**********************************************************************************************************************
vector<string> SetSeedCommand::setParameters() {
	nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
	nkParameters.add(new StringParameter("inputdir", "", false, false));
	nkParameters.add(new StringParameter("outputdir", "", false, false));

	vector<string> myArray;
	for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
	return myArray;
}
//**********************************************************************************************************************
string SetSeedCommand::getHelpString() {
	string helpString = "The set.seed command is used to seed random.\n"
		"The set.seed command parameter is seed, and it is required.\n"
		"To seed random set seed=yourRandomValue. By default mothur seeds random with the start time.\n"
		"Example set.seed(seed=12345).\n"
		"Note: No spaces between parameter labels (i.e. seed), '=' and parameters (i.e.yourSeedValue).\n";
	return helpString;
}
//**********************************************************************************************************************

SetSeedCommand::SetSeedCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter;
		//check to make sure all parameters are valid for command
		for (map<string, string>::iterator it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		bool seed = false;
		string temp = validParameter.validFile(parameters, "seed", false);
		if (temp == "not found") { random = 0;  LOG(LOGERROR) << "You must provide a seed value or set seed to clear." << '\n'; abort = true; }
		else if (temp == "clear") {
			random = time(NULL);
			seed = true;
		}
		else {
			if (m->isInteger(temp)) { Utility::mothurConvert(temp, random); seed = true; }
			else { LOG(LOGERROR) << "Seed must be an integer for the set.dir command." << '\n'; abort = true; }
		}
	}
}
//**********************************************************************************************************************

int SetSeedCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	srand(random);
	LOG(INFO) << "Setting random seed to " + toString(random) + ".\n\n";

	return 0;
}
//**********************************************************************************************************************/
