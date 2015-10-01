/*
 *  getrelabundcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 6/21/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "getrelabundcommand.h"

 //**********************************************************************************************************************
vector<string> GetRelAbundCommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "none", "none", "none", "relabund", false, true, true); parameters.push_back(pshared);
		nkParameters.add(new StringParameter("groups", "", false, false));
		CommandParameter pscale("scale", "Multiple", "totalgroup-totalotu-averagegroup-averageotu", "totalgroup", "", "", "", "", false, false); parameters.push_back(pscale);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetRelAbundCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetRelAbundCommand::getHelpString() {
	try {
		string helpString = "The get.relabund command parameters are shared, groups, scale and label.  shared is required, unless you have a valid current file.\n"
			"The groups parameter allows you to specify which of the groups in your groupfile you would like included. The group names are separated by dashes.\n"
			"The label parameter allows you to select what distance levels you would like, and are also separated by dashes.\n"
			"The scale parameter allows you to select what scale you would like to use. Choices are totalgroup, totalotu, averagegroup, averageotu, default is totalgroup.\n"
			"The get.relabund command should be in the following format: get.relabund(groups=yourGroups, label=yourLabels).\n"
			"Example get.relabund(groups=A-B-C, scale=averagegroup).\n"
			"The default value for groups is all the groups in your groupfile, and all labels in your inputfile will be used.\n"
			"The get.relabund command outputs a .relabund file.\n"
			"Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetRelAbundCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetRelAbundCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "relabund") { pattern = "[filename],relabund"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
GetRelAbundCommand::GetRelAbundCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["relabund"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetRelAbundCommand, GetRelAbundCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
GetRelAbundCommand::GetRelAbundCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	allLines = 1;

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

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["relabund"] = tempOutNames;

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

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; pickedGroups = false; }
		else {
			pickedGroups = true;
			Utility::split(groups, '-', Groups);
			m->setGroups(Groups);
		}

		scale = validParameter.validFile(parameters, "scale", false);				if (scale == "not found") { scale = "totalgroup"; }

		if ((scale != "totalgroup") && (scale != "totalotu") && (scale != "averagegroup") && (scale != "averageotu")) {
			LOG(INFO) << scale + " is not a valid scaling option for the get.relabund command. Choices are totalgroup, totalotu, averagegroup, averageotu." << '\n'; abort = true;
		}
	}

}
//**********************************************************************************************************************

int GetRelAbundCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(sharedfile));
	string outputFileName = getOutputFileName("relabund", variables);
	ofstream out;
	File::openOutputFile(outputFileName, out);
	out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);

	input = new InputData(sharedfile, "sharedfile");
	lookup = input->getSharedRAbundVectors();
	string lastLabel = lookup[0]->getLabel();

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	//as long as you are not at the end of the file or done wih the lines you want
	while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

		if (ctrlc_pressed) { outputTypes.clear();  for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } m->clearGroups(); delete input;  out.close(); File::remove(outputFileName); return 0; }

		if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

			LOG(INFO) << lookup[0]->getLabel() << '\n';
			if (!m->printedSharedHeaders) { lookup[0]->printHeaders(out); }
			getRelAbundance(lookup, out);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());
		}

		if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = lookup[0]->getLabel();

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input->getSharedRAbundVectors(lastLabel);
			LOG(INFO) << lookup[0]->getLabel() << '\n';
			if (!m->printedSharedHeaders) { lookup[0]->printHeaders(out); }
			getRelAbundance(lookup, out);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());

			//restore real lastlabel to save below
			lookup[0]->setLabel(saveLabel);
		}

		lastLabel = lookup[0]->getLabel();
		//prevent memory leak
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }

		if (ctrlc_pressed) { outputTypes.clear();  m->clearGroups(); delete input;  out.close(); File::remove(outputFileName); return 0; }

		//get next line to process
		lookup = input->getSharedRAbundVectors();
	}

	if (ctrlc_pressed) { outputTypes.clear(); m->clearGroups(); delete input;  out.close(); File::remove(outputFileName);  return 0; }

	//output error messages about any remaining user labels
	set<string>::iterator it;
	bool needToRun = false;
	for (it = userLabels.begin(); it != userLabels.end(); it++) {
		LOG(INFO) << "Your file does not include the label " + *it;
		if (processedLabels.count(lastLabel) != 1) {
			LOG(INFO) << ". I will use " + lastLabel + "." << '\n';
			needToRun = true;
		}
		else {
			LOG(INFO) << ". Please refer to " + lastLabel + "." << '\n';
		}
	}

	//run last label if you need to
	if (needToRun == true) {
		for (int i = 0; i < lookup.size(); i++) { if (lookup[i] != NULL) { delete lookup[i]; } }
		lookup = input->getSharedRAbundVectors(lastLabel);

		LOG(INFO) << lookup[0]->getLabel() << '\n';
		if (!m->printedSharedHeaders) { lookup[0]->printHeaders(out); }
		getRelAbundance(lookup, out);

		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
	}

	//reset groups parameter
	m->clearGroups();
	delete input;
	out.close();

	if (ctrlc_pressed) { outputTypes.clear(); File::remove(outputFileName); return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	LOG(INFO) << outputFileName << '\n'; outputNames.push_back(outputFileName); outputTypes["relabund"].push_back(outputFileName);
	LOG(INFO) << "";

	//set relabund file as new current relabundfile
	string current = "";
	itTypes = outputTypes.find("relabund");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("relabund", current); }
	}

	return 0;
}
//**********************************************************************************************************************

int GetRelAbundCommand::getRelAbundance(vector<SharedRAbundVector*>& thisLookUp, ofstream& out) {

	for (int i = 0; i < thisLookUp.size(); i++) {
		out << thisLookUp[i]->getLabel() << '\t' << thisLookUp[i]->getGroup() << '\t' << thisLookUp[i]->getNumBins();

		for (int j = 0; j < thisLookUp[i]->getNumBins(); j++) {

			if (ctrlc_pressed) { return 0; }

			int abund = thisLookUp[i]->getAbundance(j);

			float relabund = 0.0;

			if (scale == "totalgroup") {
				relabund = abund / (float)thisLookUp[i]->getNumSeqs();
			}
			else if (scale == "totalotu") {
				//calc the total in this otu
				int totalOtu = 0;
				for (int l = 0; l < thisLookUp.size(); l++) { totalOtu += thisLookUp[l]->getAbundance(j); }
				relabund = abund / (float)totalOtu;
			}
			else if (scale == "averagegroup") {
				relabund = abund / (float)(thisLookUp[i]->getNumSeqs() / (float)thisLookUp[i]->getNumBins());
			}
			else if (scale == "averageotu") {
				//calc the total in this otu
				int totalOtu = 0;
				for (int l = 0; l < thisLookUp.size(); l++) { totalOtu += thisLookUp[l]->getAbundance(j); }
				float averageOtu = totalOtu / (float)thisLookUp.size();

				relabund = abund / (float)averageOtu;
			}
			else { LOG(INFO) << scale + " is not a valid scaling option." << '\n'; ctrlc_pressed = true; return 0; }

			out << '\t' << relabund;
		}
		out << endl;
	}

	return 0;
}
//**********************************************************************************************************************


