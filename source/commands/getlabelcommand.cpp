/*
 *  GetlabelCommand.cpp
 *  Mothur
 *
 *  Created by Thomas Ryabin on 1/30/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "getlabelcommand.h"


 //**********************************************************************************************************************
vector<string> GetlabelCommand::setParameters() {
	try {
		CommandParameter plist("list", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false, true); parameters.push_back(plist);
		CommandParameter prabund("rabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false, true); parameters.push_back(prabund);
		CommandParameter psabund("sabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false, true); parameters.push_back(psabund);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetlabelCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
GetlabelCommand::GetlabelCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetlabelCommand, CollectCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetlabelCommand::getHelpString() {
	try {
		string helpString = "The get.label command parameters are list, sabund and rabund file. \n"
			"The get.label command should be in the following format: \n"
			"get.label()\n"
			"Example get.label().\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetlabelCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************

GetlabelCommand::GetlabelCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

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

			it = parameters.find("rabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["rabund"] = inputDir + it->second; }
			}

			it = parameters.find("sabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["sabund"] = inputDir + it->second; }
			}

			it = parameters.find("list");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["list"] = inputDir + it->second; }
			}
		}

		//check for required parameters
		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { listfile = ""; abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { format = "list"; inputfile = listfile; settings.setCurrent("list", listfile); }

		sabundfile = validParameter.validFile(parameters, "sabund", true);
		if (sabundfile == "not open") { sabundfile = ""; abort = true; }
		else if (sabundfile == "not found") { sabundfile = ""; }
		else { format = "sabund"; inputfile = sabundfile; settings.setCurrent("sabund", sabundfile); }

		rabundfile = validParameter.validFile(parameters, "rabund", true);
		if (rabundfile == "not open") { rabundfile = ""; abort = true; }
		else if (rabundfile == "not found") { rabundfile = ""; }
		else { format = "rabund"; inputfile = rabundfile; settings.setCurrent("rabund", rabundfile); }

		if ((listfile == "") && (rabundfile == "") && (sabundfile == "")) {
			//is there are current file available for any of these?
			//give priority to list, then rabund, then sabund
			//if there is a current shared file, use it

			listfile = settings.getCurrent("list");
			if (listfile != "") { inputfile = listfile; format = "list"; LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
			else {
				rabundfile = settings.getCurrent("rabund");
				if (rabundfile != "") { inputfile = rabundfile; format = "rabund"; LOG(INFO) << "Using " + rabundfile + " as input file for the rabund parameter." << '\n'; }
				else {
					sabundfile = settings.getCurrent("sabund");
					if (sabundfile != "") { inputfile = sabundfile; format = "sabund"; LOG(INFO) << "Using " + sabundfile + " as input file for the sabund parameter." << '\n'; }
					else {
						LOG(INFO) << "No valid current files. You must provide a list, sabund or rabund file." << '\n';
						abort = true;
					}
				}
			}
		}
	}

}
//**********************************************************************************************************************

int GetlabelCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		InputData* input = new InputData(inputfile, format);
		OrderVector* order = input->getOrderVector();
		string label = order->getLabel();

		while (order != NULL) {

			if (ctrlc_pressed) { delete input;  delete order; return 0; }

			label = order->getLabel();

			LOG(INFO) << label << '\n';

			delete order;
			order = input->getOrderVector();
		}

		delete input;

		return 0;
	}

	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetlabelCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************


