/*
 *  parselistcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 2/24/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "parselistscommand.h"

 //**********************************************************************************************************************
vector<string> ParseListCommand::setParameters() {
	try {
		CommandParameter plist("list", "InputTypes", "", "", "none", "none", "none", "list", false, true, true); parameters.push_back(plist);
		CommandParameter pcount("count", "InputTypes", "", "", "CountGroup", "CountGroup", "none", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "CountGroup", "none", "", false, false, true); parameters.push_back(pgroup);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ParseListCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ParseListCommand::getHelpString() {
	try {
		string helpString = "The parse.list command reads a list and group or count file and generates a list file for each group in the group or count file. \n"
			"The parse.list command parameters are list, group, count and label.\n"
			"The list and group or count parameters are required.\n"
			"If a count file is provided, mothur assumes the list file contains only unique names.\n"
			"If a group file is provided, mothur assumes the list file contains all names.\n"
			"The label parameter is used to read specific labels in your input you want to use.\n"
			"The parse.list command should be used in the following format: parse.list(list=yourListFile, group=yourGroupFile, label=yourLabels).\n"
			"Example: parse.list(list=abrecovery.fn.list, group=abrecovery.groups, label=0.03).\n"
			"Note: No spaces between parameter labels (i.e. list), '=' and parameters (i.e.yourListfile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ParseListCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ParseListCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "list") { pattern = "[filename],[group],[distance],list"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ParseListCommand::ParseListCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["list"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ParseListCommand, ParseListCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
ParseListCommand::ParseListCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["list"] = tempOutNames;

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

			it = parameters.find("group");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["group"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}
		}




		//check for required parameters
		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { abort = true; }
		else if (listfile == "not found") {
			listfile = settings.getCurrent("list");
			if (listfile != "") { LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
			else {
				LOG(INFO) << "No valid current list file. You must provide a list file." << '\n';
				abort = true;

			}
		}
		else { settings.setCurrent("list", listfile); }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(listfile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not found") { groupfile = "";   groupMap = NULL; }
		else if (groupfile == "not open") { abort = true; groupfile = ""; groupMap = NULL; }
		else {
			settings.setCurrent("group", groupfile);
			groupMap = new GroupMap(groupfile);

			int error = groupMap->readMap();
			if (error == 1) { abort = true; }
		}

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not found") { countfile = ""; }
		else if (countfile == "not open") { abort = true; countfile = ""; }
		else {
			settings.setCurrent("counttable", countfile);
			ct.readTable(countfile, true, false);
			if (!ct.hasGroupInfo()) {
				abort = true;
				LOG(LOGERROR) << "The parse.list command requires group info to be present in your countfile, quitting." << '\n';
			}

		}

		if ((groupfile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: group or count." << '\n'; abort = true;
		}
		else if ((groupfile == "") && (countfile == "")) {
			LOG(LOGERROR) << "you must provide one of the following: group or count." << '\n'; abort = true;
		}

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = "";  allLines = 1; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}
	}

}
//**********************************************************************************************************************
int ParseListCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	InputData input(listfile, "list");
	list = input.getListVector();
	string lastLabel = list->getLabel();

	if (ctrlc_pressed) {
		delete list; if (groupfile != "") { delete groupMap; }
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear(); return 0;
	}

	while ((list != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

		if (ctrlc_pressed) {
			delete list; if (groupfile != "") { delete groupMap; }
			for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
			return 0;
		}

		if (allLines == 1 || labels.count(list->getLabel()) == 1) {

			LOG(INFO) << list->getLabel() << '\n';
			parse(list);

			processedLabels.insert(list->getLabel());
			userLabels.erase(list->getLabel());
		}

		if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = list->getLabel();

			list = input.getListVector(lastLabel); //get new list vector to process

			LOG(INFO) << list->getLabel() << '\n';
			parse(list);

			processedLabels.insert(list->getLabel());
			userLabels.erase(list->getLabel());

			//restore real lastlabel to save below
			list->setLabel(saveLabel);
		}


		lastLabel = list->getLabel();

		delete list;
		list = input.getListVector(); //get new list vector to process
	}

	if (ctrlc_pressed) {
		if (groupfile != "") { delete groupMap; }
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
		return 0;
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

	if (ctrlc_pressed) {
		if (groupfile != "") { delete groupMap; }
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
		return 0;
	}

	//run last label if you need to
	if (needToRun == true) {
		if (list != NULL) { delete list; }
		list = input.getListVector(lastLabel); //get new list vector to process

		LOG(INFO) << list->getLabel() << '\n';
		parse(list);

		delete list;
	}

	if (groupfile != "") { delete groupMap; }

	if (ctrlc_pressed) {
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
		return 0;
	}

	//set fasta file as new current fastafile
	string current = "";
	itTypes = outputTypes.find("list");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("list", current); }
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
/**********************************************************************************************************************/
int ParseListCommand::parse(ListVector* thisList) {
	map<string, ofstream*> filehandles;
	map<string, ofstream*>::iterator it3;

	//set fileroot
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(listfile));
	variables["[distance]"] = thisList->getLabel();

	//fill filehandles with neccessary ofstreams
	ofstream* temp;
	vector<string> gGroups;
	if (groupfile != "") { gGroups = groupMap->getNamesOfGroups(); }
	else { gGroups = ct.getNamesOfGroups(); }

	for (int i = 0; i < gGroups.size(); i++) {
		temp = new ofstream;
		filehandles[gGroups[i]] = temp;

		variables["[group]"] = gGroups[i];
		string filename = getOutputFileName("list", variables);
		File::openOutputFile(filename, *temp);
		outputNames.push_back(filename); outputTypes["list"].push_back(filename);
	}


	map<string, string> groupVector;
	map<string, string> groupLabels;
	map<string, string>::iterator itGroup;
	map<string, int> groupNumBins;

	//print label
	for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
		groupNumBins[it3->first] = 0;
		groupVector[it3->first] = "";
		groupLabels[it3->first] = "label\tnumOtus";
	}

	vector<string> binLabels = thisList->getLabels();
	for (int i = 0; i < thisList->getNumBins(); i++) {
		if (ctrlc_pressed) { break; }

		map<string, string> groupBins;
		string bin = list->get(i);

		vector<string> names;
		m->splitAtComma(bin, names);  //parses bin into individual sequence names

		//parse bin into list of sequences in each group
		for (int j = 0; j < names.size(); j++) {
			if (groupfile != "") {
				string group = groupMap->getGroup(names[j]);

				if (group == "not found") { LOG(INFO) << names[j] + " is not in your groupfile. please correct." << '\n'; exit(1); }

				itGroup = groupBins.find(group);
				if (itGroup == groupBins.end()) {
					groupBins[group] = names[j];  //add first name
					groupNumBins[group]++;
				}
				else { //add another name
					groupBins[group] = groupBins[group] + "," + names[j];
				}
			}
			else {
				vector<string> thisSeqsGroups = ct.getGroups(names[j]);

				for (int k = 0; k < thisSeqsGroups.size(); k++) {
					string group = thisSeqsGroups[k];
					itGroup = groupBins.find(group);
					if (itGroup == groupBins.end()) {
						groupBins[group] = names[j];  //add first name
						groupNumBins[group]++;
					}
					else { //add another name
						groupBins[group] = groupBins[group] + "," + names[j];
					}

				}
			}
		}

		//print parsed bin info to files
		for (itGroup = groupBins.begin(); itGroup != groupBins.end(); itGroup++) {
			groupVector[itGroup->first] += '\t' + itGroup->second;
			groupLabels[itGroup->first] += '\t' + binLabels[i];
		}

	}

	if (ctrlc_pressed) {
		for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
			(*(filehandles[it3->first])).close();
			delete it3->second;
		}
		return 0;
	}

	//end list vector
	for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
		(*(filehandles[it3->first])) << groupLabels[it3->first] << endl;
		(*(filehandles[it3->first])) << thisList->getLabel() << '\t' << groupNumBins[it3->first] << groupVector[it3->first] << endl;  // label numBins  listvector for that group
		(*(filehandles[it3->first])).close();
		delete it3->second;
	}

	return 0;

}

/**********************************************************************************************************************/


