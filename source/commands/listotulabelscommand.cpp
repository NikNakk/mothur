//
//  listotucommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 5/15/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "listotulabelscommand.h"
#include "inputdata.h"

//**********************************************************************************************************************
vector<string> ListOtuLabelsCommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "SharedRel", "SharedRel", "none", "otulabels", false, false, true); parameters.push_back(pshared);
		CommandParameter prelabund("relabund", "InputTypes", "", "", "SharedRel", "SharedRel", "none", "otulabels", false, false); parameters.push_back(prelabund);
		CommandParameter plist("list", "InputTypes", "", "", "SharedRel", "SharedRel", "none", "otulabels", false, false); parameters.push_back(plist);
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new StringParameter("label", "", false, false));
		//every command must have inputdir and outputdir.  This allows mothur users to redirect input and output files.
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ListOtuLabelsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ListOtuLabelsCommand::getHelpString() {
	try {
		string helpString = "The list.otulabels lists otu labels from shared, relabund or list file. The results can be used by the get.otulabels to select specific otus with the output from classify.otu, otu.association, or corr.axes.\n"
			"The list.otulabels parameters are: shared, relabund, label and groups.\n"
			"The label parameter is used to analyze specific labels in your input.\n"
			"The groups parameter allows you to specify which of the groups you would like analyzed.\n"
			"The list.otulabels commmand should be in the following format: \n"
			"list.otulabels(shared=yourSharedFile, groups=yourGroup1-yourGroup2)\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ListOtuLabelsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ListOtuLabelsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "otulabels") { pattern = "[filename],[distance],otulabels"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ListOtuLabelsCommand::ListOtuLabelsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["otulabels"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ListOtuLabelsCommand, ListOtuLabelsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
ListOtuLabelsCommand::ListOtuLabelsCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	allLines = 1;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		//valid paramters for this command
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter;
		map<string, string>::iterator it;
		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}


		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {

			//edit file types below to include only the types you added as parameters

			string path;
			it = parameters.find("relabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["relabund"] = inputDir + it->second; }
			}

			it = parameters.find("shared");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["shared"] = inputDir + it->second; }
			}

			it = parameters.find("list");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["list"] = inputDir + it->second; }
			}
		}

		vector<string> tempOutNames;
		outputTypes["otulabels"] = tempOutNames;

		//check for parameters
		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { inputFileName = sharedfile; format = "sharedfile"; settings.setCurrent("shared", sharedfile); }

		relabundfile = validParameter.validFile(parameters, "relabund", true);
		if (relabundfile == "not open") { abort = true; }
		else if (relabundfile == "not found") { relabundfile = ""; }
		else { inputFileName = relabundfile; format = "relabund"; settings.setCurrent("relabund", relabundfile); }

		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { inputFileName = listfile; format = "list"; settings.setCurrent("list", listfile); }


		if ((relabundfile == "") && (sharedfile == "") && (listfile == "")) {
			//is there are current file available for either of these?
			//give priority to shared, then relabund
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { inputFileName = sharedfile; format = "sharedfile"; LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else {
				relabundfile = settings.getCurrent("relabund");
				if (relabundfile != "") { inputFileName = relabundfile; format = "relabund"; LOG(INFO) << "Using " + relabundfile + " as input file for the relabund parameter." << '\n'; }
				else {
					listfile = settings.getCurrent("list");
					if (listfile != "") { inputFileName = listfile; format = "list"; LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
					else {
						LOG(INFO) << "No valid current files. You must provide a shared, list or relabund." << '\n';
						abort = true;
					}
				}
			}
		}

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = File::getPath(inputFileName); //if user entered a file with a path then preserve it	
		}

		string groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else { Utility::split(groups, '-', Groups); }
		m->setGroups(Groups);

		string label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}
	}

}
//**********************************************************************************************************************

int ListOtuLabelsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	InputData input(inputFileName, format);

	if (format == "relabund") {
		vector<SharedRAbundFloatVector*> lookup = input.getSharedRAbundFloatVectors();
		string lastLabel = lookup[0]->getLabel();

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		//as long as you are not at the end of the file or done wih the lines you want
		while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }  for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

			if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

				LOG(INFO) << lookup[0]->getLabel() << '\n';

				createList(lookup);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());
			}

			if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = lookup[0]->getLabel();

				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
				lookup = input.getSharedRAbundFloatVectors(lastLabel);
				LOG(INFO) << lookup[0]->getLabel() << '\n';

				createList(lookup);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());

				//restore real lastlabel to save below
				lookup[0]->setLabel(saveLabel);
			}

			lastLabel = lookup[0]->getLabel();
			//prevent memory leak
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }

			//get next line to process
			lookup = input.getSharedRAbundFloatVectors();
		}

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }

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
			lookup = input.getSharedRAbundFloatVectors(lastLabel);

			LOG(INFO) << lookup[0]->getLabel() << '\n';

			createList(lookup);

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		}
	}
	else if (format == "sharedfile") {

		vector<SharedRAbundVector*> lookup = input.getSharedRAbundVectors();
		string lastLabel = lookup[0]->getLabel();

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		//as long as you are not at the end of the file or done wih the lines you want
		while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }  for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

			if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

				LOG(INFO) << lookup[0]->getLabel() << '\n';

				createList(lookup);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());
			}

			if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = lookup[0]->getLabel();

				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
				lookup = input.getSharedRAbundVectors(lastLabel);
				LOG(INFO) << lookup[0]->getLabel() << '\n';

				createList(lookup);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());

				//restore real lastlabel to save below
				lookup[0]->setLabel(saveLabel);
			}

			lastLabel = lookup[0]->getLabel();
			//prevent memory leak
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }

			//get next line to process
			lookup = input.getSharedRAbundVectors();
		}

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }

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
			lookup = input.getSharedRAbundVectors(lastLabel);

			LOG(INFO) << lookup[0]->getLabel() << '\n';

			createList(lookup);

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		}
	}
	else {
		ListVector* list = input.getListVector();
		string lastLabel = list->getLabel();

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		//as long as you are not at the end of the file or done wih the lines you want
		while ((list != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { delete list;  for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

			if (allLines == 1 || labels.count(list->getLabel()) == 1) {

				LOG(INFO) << list->getLabel() << '\n';

				createList(list);

				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());
			}

			if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = list->getLabel();

				delete list;
				list = input.getListVector(lastLabel);
				LOG(INFO) << list->getLabel() << '\n';

				createList(list);

				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());

				//restore real lastlabel to save below
				list->setLabel(saveLabel);
			}

			lastLabel = list->getLabel();
			//prevent memory leak
			delete list; list = NULL;

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }

			//get next line to process
			list = input.getListVector();
		}

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }

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
			delete list;
			list = input.getListVector(lastLabel);

			LOG(INFO) << list->getLabel() << '\n';

			createList(list);

			delete list;
		}
	}

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }

	//output files created by command
	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";
	return 0;

}
//**********************************************************************************************************************

int ListOtuLabelsCommand::createList(vector<SharedRAbundVector*>& lookup) {

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputFileName));
	variables["[distance]"] = lookup[0]->getLabel();
	string outputFileName = getOutputFileName("otulabels", variables);
	outputNames.push_back(outputFileName);  outputTypes["otulabels"].push_back(outputFileName);
	ofstream out;
	File::openOutputFile(outputFileName, out);

	for (int i = 0; i < settings.currentSharedBinLabels.size(); i++) { out << settings.currentSharedBinLabels[i] << endl; }

	out.close();

	return 0;
}

//**********************************************************************************************************************

int ListOtuLabelsCommand::createList(vector<SharedRAbundFloatVector*>& lookup) {
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputFileName));
	variables["[distance]"] = lookup[0]->getLabel();
	string outputFileName = getOutputFileName("otulabels", variables);
	outputNames.push_back(outputFileName);  outputTypes["accnos"].push_back(outputFileName);
	ofstream out;
	File::openOutputFile(outputFileName, out);

	for (int i = 0; i < settings.currentSharedBinLabels.size(); i++) { out << settings.currentSharedBinLabels[i] << endl; }

	out.close();

	return 0;
}
//**********************************************************************************************************************
int ListOtuLabelsCommand::createList(ListVector*& list) {
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputFileName));
	variables["[distance]"] = list->getLabel();
	string outputFileName = getOutputFileName("otulabels", variables);
	outputNames.push_back(outputFileName);  outputTypes["accnos"].push_back(outputFileName);
	ofstream out;
	File::openOutputFile(outputFileName, out);

	vector<string> binLabels = list->getLabels();
	for (int i = 0; i < binLabels.size(); i++) { out << binLabels[i] << endl; }

	out.close();

	return 0;
}

//**********************************************************************************************************************

