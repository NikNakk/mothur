/*
 *  getrabundcommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 6/2/09.
 *  Copyright 2009 Schloss Lab Umass Amherst. All rights reserved.
 *
 */

#include "getrabundcommand.h"

 //**********************************************************************************************************************
vector<string> GetRAbundCommand::setParameters() {
	try {
		CommandParameter plist("list", "InputTypes", "", "", "LRSS", "LRSS", "none", "rabund", false, false, true); parameters.push_back(plist);
		CommandParameter pcount("count", "InputTypes", "", "", "none", "none", "none", "", false, false, false); parameters.push_back(pcount);
		CommandParameter psabund("sabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "rabund", false, false, true); parameters.push_back(psabund);
		nkParameters.add(new BooleanParameter("sorted", true, false, false));
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetRAbundCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetRAbundCommand::getHelpString() {
	try {
		string helpString = "The get.rabund command parameters are list, sabund, count, label and sorted.  list or sabund parameters are required, unless you have valid current files.\n"
			"The count parameter allows you to provide a count file associated with your list file. If you clustered with a countfile the list file only contains the unique sequences and you will want to add the redundant counts into the rabund file, providing the count file allows you to do so.\n"
			"The label parameter allows you to select what distance levels you would like included in your .rabund file, and are separated by dashes.\n"
			"The sorted parameters allows you to print the rabund results sorted by abundance or not.  The default is sorted.\n"
			"The get.rabund command should be in the following format: get.rabund(label=yourLabels, sorted=yourSorted).\n"
			"Example get.rabund(sorted=F).\n"
			"The default value for label is all labels in your inputfile.\n"
			"The get.rabund command outputs a .rabund file containing the lines you selected.\n"
			"Note: No spaces between parameter labels (i.e. label), '=' and parameters (i.e.yourLabels).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetRAbundCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetRAbundCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "rabund") { pattern = "[filename],rabund"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}

//**********************************************************************************************************************
GetRAbundCommand::GetRAbundCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["rabund"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetRAbundCommand, GetRAbundCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
GetRAbundCommand::GetRAbundCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["rabund"] = tempOutNames;

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

			it = parameters.find("sabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["sabund"] = inputDir + it->second; }
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
		if (listfile == "not open") { listfile = ""; abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { format = "list"; inputfile = listfile; settings.setCurrent("list", listfile); }

		sabundfile = validParameter.validFile(parameters, "sabund", true);
		if (sabundfile == "not open") { sabundfile = ""; abort = true; }
		else if (sabundfile == "not found") { sabundfile = ""; }
		else { format = "sabund"; inputfile = sabundfile; settings.setCurrent("sabund", sabundfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }


		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		string temp;
		temp = validParameter.validFile(parameters, "sorted", false);			if (temp == "not found") { temp = "T"; }
		sorted = m->isTrue(temp);

		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		if ((listfile == "") && (sabundfile == "")) {
			//is there are current file available for any of these?
			//give priority to shared, then list, then rabund, then sabund
			//if there is a current shared file, use it
			listfile = settings.getCurrent("list");
			if (listfile != "") { inputfile = listfile; format = "list"; LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
			else {
				sabundfile = settings.getCurrent("sabund");
				if (sabundfile != "") { inputfile = sabundfile; format = "sabund"; LOG(INFO) << "Using " + sabundfile + " as input file for the sabund parameter." << '\n'; }
				else {
					LOG(INFO) << "No valid current files. You must provide a list or sabund file." << '\n';
					abort = true;
				}
			}
		}

		if ((countfile != "") && (listfile == "")) { LOG(LOGERROR) << "You can only use the count file with a list file, aborting.\n"; abort = true; }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(inputfile); }

	}


}
//**********************************************************************************************************************

int GetRAbundCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
	filename = getOutputFileName("rabund", variables);
	File::openOutputFile(filename, out);

	if (countfile != "") {
		processList(out);
	}
	else {
		InputData input(inputfile, format);
		RAbundVector* rabund = input.getRAbundVector();
		string lastLabel = rabund->getLabel();

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		if (ctrlc_pressed) { outputTypes.clear();  out.close(); File::remove(filename); delete rabund;  return 0; }

		while ((rabund != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (allLines == 1 || labels.count(rabund->getLabel()) == 1) {
				LOG(INFO) << rabund->getLabel() << '\n';

				if (ctrlc_pressed) { outputTypes.clear(); out.close(); File::remove(filename);   delete rabund;  return 0; }

				if (sorted) { rabund->print(out); }
				else { rabund->nonSortedPrint(out); }

				processedLabels.insert(rabund->getLabel());
				userLabels.erase(rabund->getLabel());
			}

			if ((Utility::anyLabelsToProcess(rabund->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = rabund->getLabel();

				delete rabund;
				rabund = input.getRAbundVector(lastLabel);

				LOG(INFO) << rabund->getLabel() << '\n';

				if (ctrlc_pressed) { outputTypes.clear(); out.close(); File::remove(filename);  delete rabund;  return 0; }

				if (sorted) { rabund->print(out); }
				else { rabund->nonSortedPrint(out); }

				processedLabels.insert(rabund->getLabel());
				userLabels.erase(rabund->getLabel());

				//restore real lastlabel to save below
				rabund->setLabel(saveLabel);
			}

			lastLabel = rabund->getLabel();

			delete rabund;
			rabund = input.getRAbundVector();
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
			if (rabund != NULL) { delete rabund; }
			rabund = input.getRAbundVector(lastLabel);

			LOG(INFO) << rabund->getLabel() << '\n';

			if (ctrlc_pressed) { outputTypes.clear(); out.close(); File::remove(filename);   delete rabund;  return 0; }

			if (sorted) { rabund->print(out); }
			else { rabund->nonSortedPrint(out); }

			delete rabund;
		}
	}

	if (ctrlc_pressed) { outputTypes.clear();  out.close(); File::remove(filename);  return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	LOG(INFO) << filename << '\n';	outputNames.push_back(filename); outputTypes["rabund"].push_back(filename);
	LOG(INFO) << "";

	out.close();

	//set rabund file as new current rabundfile
	string current = "";
	itTypes = outputTypes.find("rabund");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("rabund", current); }
	}

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in GetRAbundCommand, execute";
	exit(1);
}
}
//**********************************************************************************************************************
int GetRAbundCommand::processList(ofstream& out) {
	try {
		CountTable ct;
		ct.readTable(countfile, false, false);

		InputData input(inputfile, format);
		ListVector* list = input.getListVector();
		string lastLabel = list->getLabel();

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		if (ctrlc_pressed) { delete list;  return 0; }

		while ((list != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (allLines == 1 || labels.count(list->getLabel()) == 1) {
				LOG(INFO) << list->getLabel() << '\n';

				if (ctrlc_pressed) { delete list;  return 0; }

				RAbundVector* rabund = new RAbundVector();
				createRabund(ct, list, rabund);

				if (sorted) { rabund->print(out); }
				else { rabund->nonSortedPrint(out); }

				delete rabund;
				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());
			}

			if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = list->getLabel();

				delete list;
				list = input.getListVector(lastLabel);

				LOG(INFO) << list->getLabel() << '\n';

				if (ctrlc_pressed) { delete list;  return 0; }

				RAbundVector* rabund = new RAbundVector();
				createRabund(ct, list, rabund);

				if (sorted) { rabund->print(out); }
				else { rabund->nonSortedPrint(out); }

				delete rabund;
				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());

				//restore real lastlabel to save below
				list->setLabel(saveLabel);
			}

			lastLabel = list->getLabel();

			delete list;
			list = input.getListVector();
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
			list = input.getListVector(lastLabel);

			LOG(INFO) << list->getLabel() << '\n';

			if (ctrlc_pressed) { delete list;  return 0; }

			RAbundVector* rabund = new RAbundVector();
			createRabund(ct, list, rabund);

			if (sorted) { rabund->print(out); }
			else { rabund->nonSortedPrint(out); }

			delete rabund;
			delete list;
		}

		return 0;
	}
	//**********************************************************************************************************************

	int GetRAbundCommand::createRabund(CountTable& ct, ListVector*& list, RAbundVector*& rabund) {

		rabund->setLabel(list->getLabel());
		for (int i = 0; i < list->getNumBins(); i++) {
			if (ctrlc_pressed) { return 0; }
			vector<string> binNames;
			string bin = list->get(i);
			m->splitAtComma(bin, binNames);
			int total = 0;
			for (int j = 0; j < binNames.size(); j++) {
				total += ct.getNumSeqs(binNames[j]);
			}
			rabund->push_back(total);
		}

		return 0;

	}

	//**********************************************************************************************************************


