/*
 *  getgroupcommand.cpp
 *  Mothur
 *
 *  Created by Thomas Ryabin on 2/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "getgroupcommand.h"
#include "inputdata.h"

 //**********************************************************************************************************************
vector<string> GetgroupCommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "current", "none", "none", "none", "", false, true, true); parameters.push_back(pshared);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetgroupCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetgroupCommand::getHelpString() {
	try {
		string helpString = "The get.group command parameter is shared and it's required if you have no valid current file.\n"
			"You may not use any parameters with the get.group command.\n"
			"The get.group command should be in the following format: \n"
			"get.group()\n"
			"Example get.group().\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetgroupCommand, getHelpString";
		exit(1);
	}
}

//**********************************************************************************************************************
GetgroupCommand::GetgroupCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetgroupCommand, GetgroupCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
GetgroupCommand::GetgroupCommand(Settings& settings, string option) : Command(settings, option) {
	Y		abort = false; calledHelp = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();
		map<string, string>::iterator it;

		ValidParameters validParameter;
		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("shared");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["shared"] = inputDir + it->second; }
			}
		}

		//get shared file
		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = ""; abort = true; }
		else if (sharedfile == "not found") {
			//if there is a current shared file, use it
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else { LOG(INFO) << "You have no current sharedfile and the shared parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("shared", sharedfile); }


		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(sharedfile); }
	}
}
//**********************************************************************************************************************

int GetgroupCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		InputData input(sharedfile, "sharedfile");
		vector<SharedRAbundVector*> lookup = input.getSharedRAbundVectors();

		for (int i = 0; i < lookup.size(); i++) {
			LOG(INFO) << lookup[i]->getGroup() << '\n';
			delete lookup[i];
		}

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		LOG(INFO) << "";

		return 0;
	}

	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetgroupCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************


