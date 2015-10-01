/*
 *  getotuscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 11/10/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "getotuscommand.h"
#include "inputdata.h"
#include "sharedutilities.h"


 //**********************************************************************************************************************
vector<string> GetOtusCommand::setParameters() {
	try {
		CommandParameter pgroup("group", "InputTypes", "", "", "none", "none", "none", "group", false, true, true); parameters.push_back(pgroup);
		CommandParameter plist("list", "InputTypes", "", "", "none", "none", "none", "list", false, true, true); parameters.push_back(plist);
		CommandParameter paccnos("accnos", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(paccnos);
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetOtusCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetOtusCommand::getHelpString() {
	try {
		string helpString = "The get.otus command selects otus containing sequences from a specfic group or set of groups.\n"
			"It outputs a new list file containing the otus containing sequences from in the those specified groups.\n"
			"The get.otus command parameters are accnos, group, list, label and groups. The group and list parameters are required, unless you have valid current files.\n"
			"You must also provide an accnos containing the list of groups to get or set the groups parameter to the groups you wish to select.\n"
			"The groups parameter allows you to specify which of the groups in your groupfile you would like.  You can separate group names with dashes.\n"
			"The label parameter allows you to specify which distance you want to process.\n"
			"The get.otus command should be in the following format: get.otus(accnos=yourAccnos, list=yourListFile, group=yourGroupFile, label=yourLabel).\n"
			"Example get.otus(accnos=amazon.accnos, list=amazon.fn.list, group=amazon.groups, label=0.03).\n"
			"or get.otus(groups=pasture, list=amazon.fn.list, amazon.groups, label=0.03).\n"
			"Note: No spaces between parameter labels (i.e. list), '=' and parameters (i.e.yourListFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetOtusCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetOtusCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "group") { pattern = "[filename],[tag],pick,[extension]"; }
	else if (type == "list") { pattern = "[filename],[tag],pick,[extension]"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
GetOtusCommand::GetOtusCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["list"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetOtusCommand, GetOtusCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
GetOtusCommand::GetOtusCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

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
		outputTypes["group"] = tempOutNames;
		outputTypes["list"] = tempOutNames;


		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("accnos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["accnos"] = inputDir + it->second; }
			}

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
		}


		//check for required parameters
		accnosfile = validParameter.validFile(parameters, "accnos", true);
		if (accnosfile == "not open") { abort = true; }
		else if (accnosfile == "not found") { accnosfile = ""; }
		else { settings.setCurrent("accnos", accnosfile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { abort = true; }
		else if (groupfile == "not found") {
			groupfile = settings.getCurrent("group");
			if (groupfile != "") { LOG(INFO) << "Using " + groupfile + " as input file for the group parameter." << '\n'; }
			else { LOG(INFO) << "You have no current group file and the group parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("group", groupfile); }

		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { abort = true; }
		else if (listfile == "not found") {
			listfile = settings.getCurrent("list");
			if (listfile != "") { LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
			else { LOG(INFO) << "You have no current list file and the list parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("list", listfile); }

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else {
			Utility::split(groups, '-', Groups);
		}

		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; LOG(INFO) << "You did not provide a label, I will use the first label in your inputfile." << '\n'; label = ""; }

		if ((accnosfile == "") && (Groups.size() == 0)) { LOG(INFO) << "You must provide an accnos file or specify groups using the groups parameter." << '\n'; abort = true; }
	}

}
//**********************************************************************************************************************

int GetOtusCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	groupMap = new GroupMap(groupfile);
	groupMap->readMap();

	//get groups you want to get
	if (accnosfile != "") { m->readAccnos(accnosfile, Groups); m->setGroups(Groups); }

	//make sure groups are valid
	//takes care of user setting groupNames that are invalid or setting groups=all
	SharedUtil* util = new SharedUtil();
	vector<string> gNamesOfGroups = groupMap->getNamesOfGroups();
	util->setGroups(Groups, gNamesOfGroups);
	groupMap->setNamesOfGroups(gNamesOfGroups);
	delete util;

	if (ctrlc_pressed) { delete groupMap; return 0; }

	//read through the list file keeping any otus that contain any sequence from the groups selected
	readListGroup();

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (outputNames.size() != 0) {
		LOG(INFO) << '\n' << "Output File names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		//set list file as new current listfile
		string current = "";
		itTypes = outputTypes.find("group");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("group", current); }
		}

		itTypes = outputTypes.find("list");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("list", current); }
		}
	}

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in GetOtusCommand, execute";
	exit(1);
}
}
//**********************************************************************************************************************
int GetOtusCommand::readListGroup() {
	try {
		InputData* input = new InputData(listfile, "list");
		ListVector* list = input->getListVector();
		string lastLabel = list->getLabel();

		//using first label seen if none is provided
		if (label == "") { label = lastLabel; }

		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(listfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(listfile));
		variables["[tag]"] = label;
		variables["[extension]"] = m->getExtension(listfile);
		string outputFileName = getOutputFileName("list", variables);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		string GroupOutputDir = outputDir;
		if (outputDir == "") { GroupOutputDir += File::getPath(groupfile); }
		variables["[filename]"] = GroupOutputDir + File::getRootName(File::getSimpleName(groupfile));
		variables["[extension]"] = m->getExtension(groupfile);
		string outputGroupFileName = getOutputFileName("group", variables);

		ofstream outGroup;
		File::openOutputFile(outputGroupFileName, outGroup);


		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> labels; labels.insert(label);
		set<string> processedLabels;
		set<string> userLabels = labels;

		bool wroteSomething = false;

		//as long as you are not at the end of the file or done wih the lines you want
		while ((list != NULL) && (userLabels.size() != 0)) {

			if (ctrlc_pressed) { delete list; delete input; out.close();  outGroup.close(); File::remove(outputFileName);  File::remove(outputGroupFileName);return 0; }

			if (labels.count(list->getLabel()) == 1) {
				processList(list, groupMap, out, outGroup, wroteSomething);

				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());
			}

			if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = list->getLabel();

				delete list;

				list = input->getListVector(lastLabel);

				processList(list, groupMap, out, outGroup, wroteSomething);

				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());

				//restore real lastlabel to save below
				list->setLabel(saveLabel);
			}

			lastLabel = list->getLabel();

			delete list; list = NULL;

			//get next line to process
			list = input->getListVector();
		}


		if (ctrlc_pressed) { if (list != NULL) { delete list; } delete input; out.close(); outGroup.close(); File::remove(outputFileName);  File::remove(outputGroupFileName); return 0; }

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

			processList(list, groupMap, out, outGroup, wroteSomething);

			delete list; list = NULL;
		}

		out.close();
		outGroup.close();

		if (wroteSomething == false) { LOG(INFO) << "At distance " + label + " your file does NOT contain any otus containing sequences from the groups you wish to get." << '\n'; }
		outputTypes["list"].push_back(outputFileName); outputNames.push_back(outputFileName);
		outputTypes["group"].push_back(outputGroupFileName); outputNames.push_back(outputGroupFileName);

		return 0;

	}
	//**********************************************************************************************************************
	int GetOtusCommand::processList(ListVector*& list, GroupMap*& groupMap, ofstream& out, ofstream& outGroup, bool& wroteSomething) {

		//make a new list vector
		ListVector newList;
		newList.setLabel(list->getLabel());

		int numOtus = 0;
		//for each bin
		vector<string> binLabels = list->getLabels();
		vector<string> newBinLabels;
		for (int i = 0; i < list->getNumBins(); i++) {
			if (ctrlc_pressed) { return 0; }

			//parse out names that are in accnos file
			string binnames = list->get(i);

			bool keepBin = false;
			string groupFileOutput = "";

			//parse names
			string individual = "";
			int length = binnames.length();
			for (int j = 0;j < length;j++) {
				if (binnames[j] == ',') {
					string group = groupMap->getGroup(individual);
					if (group == "not found") { LOG(LOGERROR) << "" + individual + " is not in your groupfile. please correct." << '\n'; group = "NOTFOUND"; }

					if (m->inUsersGroups(group, Groups)) { keepBin = true; }
					groupFileOutput += individual + "\t" + group + "\n";
					individual = "";

				}
				else { individual += binnames[j]; }
			}

			string group = groupMap->getGroup(individual);
			if (group == "not found") { LOG(LOGERROR) << "" + individual + " is not in your groupfile. please correct." << '\n'; group = "NOTFOUND"; }

			if (m->inUsersGroups(group, Groups)) { keepBin = true; }
			groupFileOutput += individual + "\t" + group + "\n";

			//if there are sequences from the groups we want in this bin add to new list, output to groupfile
			if (keepBin) {
				newList.push_back(binnames);
				newBinLabels.push_back(binLabels[i]);
				outGroup << groupFileOutput;
				numOtus++;
			}
		}

		//print new listvector
		if (newList.getNumBins() != 0) {
			wroteSomething = true;
			newList.setLabels(newBinLabels);
			newList.printHeaders(out);
			newList.print(out);
		}

		LOG(INFO) << newList.getLabel() + " - selected " + toString(numOtus) + " of the " + toString(list->getNumBins()) + " OTUs." << '\n';

		return 0;
	}
	//**********************************************************************************************************************


