/*
 *  getlistcountcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 10/12/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "getlistcountcommand.h"

 //**********************************************************************************************************************
vector<string> GetListCountCommand::setParameters() {
	try {
		CommandParameter plist("list", "InputTypes", "", "", "none", "none", "none", "otu", false, true, true); parameters.push_back(plist);
		nkParameters.add(new StringParameter("label", "", false, false));
		CommandParameter parasort("sort", "Multiple", "name-otu", "otu", "", "", "", "", false, false); parameters.push_back(parasort);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetListCountCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetListCountCommand::getHelpString() {
	try {
		string helpString = "The get.otulist command parameters are list, sort and label.  list is required, unless you have a valid current list file.\n"
			"The label parameter allows you to select what distance levels you would like a output files created for, and are separated by dashes.\n"
			"The sort parameter allows you to select how you want the output displayed. Options are otu and name.\n"
			"If otu is selected the output will be otu number followed by the list of names in that otu.\n"
			"If name is selected the output will be a sequence name followed by its otu number.\n"
			"The get.otulist command should be in the following format: get.otulist(list=yourlistFile, label=yourLabels).\n"
			"Example get.otulist(list=amazon.fn.list, label=0.10).\n"
			"The default value for label is all lines in your inputfile.\n"
			"The get.otulist command outputs a .otu file for each distance you specify listing the bin number and the names of the sequences in that bin.\n"
			"Note: No spaces between parameter labels (i.e. list), '=' and parameters (i.e.yourListFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetListCountCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetListCountCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "otu") { pattern = "[filename],[tag],otu"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
GetListCountCommand::GetListCountCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["otu"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetListCountCommand, GetListCountCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
GetListCountCommand::GetListCountCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	allLines = 1;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter;
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["otu"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("list");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["list"] = inputDir + it->second; }
			}
		}

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//check for required parameters
		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not found") {
			listfile = settings.getCurrent("list");
			if (listfile != "") { LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
			else { LOG(INFO) << "You have no current list file and the list parameter is required." << '\n'; abort = true; }
		}
		else if (listfile == "not open") { abort = true; }
		else { settings.setCurrent("list", listfile); }


		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		sort = validParameter.validFile(parameters, "sort", false);	  if (sort == "not found") { sort = "otu"; }
		if ((sort != "otu") && (sort != "name")) { LOG(INFO) << sort + " is not a valid sort option. Options are otu and name. I will use otu." << '\n'; sort = "otu"; }

		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}
	}
}
//**********************************************************************************************************************

int GetListCountCommand::execute() {
	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	input = new InputData(listfile, "list");
	list = input->getListVector();
	string lastLabel = list->getLabel();

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	if (ctrlc_pressed) { delete input; delete list; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	while ((list != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

		if (allLines == 1 || labels.count(list->getLabel()) == 1) {

			process(list);

			if (ctrlc_pressed) { delete input; delete list; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

			processedLabels.insert(list->getLabel());
			userLabels.erase(list->getLabel());
		}

		if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = list->getLabel();

			delete list;
			list = input->getListVector(lastLabel);

			process(list);

			if (ctrlc_pressed) { delete input; delete list; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }


			processedLabels.insert(list->getLabel());
			userLabels.erase(list->getLabel());

			//restore real lastlabel to save below
			list->setLabel(saveLabel);
		}

		lastLabel = list->getLabel();

		delete list;
		list = input->getListVector();
	}


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
		if (list != NULL) { delete list; }
		list = input->getListVector(lastLabel);

		process(list);

		if (ctrlc_pressed) { delete input; delete list; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

		delete list;
	}

	delete input;

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}

//**********************************************************************************************************************
//return 1 if error, 0 otherwise
void GetListCountCommand::process(ListVector* list) {
	string binnames;
	if (outputDir == "") { outputDir += File::getPath(listfile); }
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(listfile));
	variables["[tag]"] = list->getLabel();
	string outputFileName = getOutputFileName("otu", variables);

	File::openOutputFile(outputFileName, out);
	outputNames.push_back(outputFileName); outputTypes["otu"].push_back(outputFileName);

	LOG(INFO) << list->getLabel() << '\n';

	//for each bin in the list vector
	vector<string> binLabels = list->getLabels();
	for (int i = 0; i < list->getNumBins(); i++) {
		if (ctrlc_pressed) { break; }

		binnames = list->get(i);

		if (sort == "otu") {
			out << binLabels[i] << '\t' << binnames << endl;
		}
		else { //sort = name
			vector<string> names;
			m->splitAtComma(binnames, names);

			for (int j = 0; j < names.size(); j++) {
				out << names[j] << '\t' << binLabels[i] << endl;
			}
		}
	}

	out.close();
}
//**********************************************************************************************************************


