/*
 *  getsabundcommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 6/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "getsabundcommand.h"

 //**********************************************************************************************************************
vector<string> GetSAbundCommand::setParameters() {
	try {
		CommandParameter plist("list", "InputTypes", "", "", "LRSS", "LRSS", "none", "sabund", false, false, true); parameters.push_back(plist);
		CommandParameter pcount("count", "InputTypes", "", "", "none", "none", "none", "", false, false, false); parameters.push_back(pcount);
		CommandParameter prabund("rabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "sabund", false, false, true); parameters.push_back(prabund);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetSAbundCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetSAbundCommand::getHelpString() {
	try {
		string helpString = "The get.sabund command parameters is list, rabund, count and label.  list or rabund is required unless a valid current file exists.\n"
			"The count parameter allows you to provide a count file associated with your list file. If you clustered with a countfile the list file only contains the unique sequences and you will want to add the redundant counts into the sabund file, providing the count file allows you to do so.\n"
			"The label parameter allows you to select what distance levels you would like included in your .sabund file, and are separated by dashes.\n"
			"The get.sabund command should be in the following format: get.sabund(label=yourLabels).\n"
			"Example get.sabund().\n"
			"The default value for label is all labels in your inputfile.\n"
			"The get.sabund command outputs a .sabund file containing the labels you selected.\n"
			"Note: No spaces between parameter labels (i.e. label), '=' and parameters (i.e.yourLabel).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetSAbundCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetSAbundCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "sabund") { pattern = "[filename],sabund"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
GetSAbundCommand::GetSAbundCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["sabund"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetSAbundCommand, GetSAbundCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
GetSAbundCommand::GetSAbundCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["sabund"] = tempOutNames;

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

			it = parameters.find("rabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["rabund"] = inputDir + it->second; }
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

		rabundfile = validParameter.validFile(parameters, "rabund", true);
		if (rabundfile == "not open") { rabundfile = ""; abort = true; }
		else if (rabundfile == "not found") { rabundfile = ""; }
		else { format = "rabund"; inputfile = rabundfile; settings.setCurrent("rabund", rabundfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		//check for optional parameter and set defaults
// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		if ((listfile == "") && (rabundfile == "")) {
			//is there are current file available for any of these?
			//give priority to shared, then list, then rabund, then sabund
			//if there is a current shared file, use it
			listfile = settings.getCurrent("list");
			if (listfile != "") { inputfile = listfile; format = "list"; LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
			else {
				rabundfile = settings.getCurrent("rabund");
				if (rabundfile != "") { inputfile = rabundfile; format = "rabund"; LOG(INFO) << "Using " + rabundfile + " as input file for the rabund parameter." << '\n'; }
				else {
					LOG(INFO) << "No valid current files. You must provide a list or rabund file." << '\n';
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

int GetSAbundCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
	filename = getOutputFileName("sabund", variables);
	File::openOutputFile(filename, out);

	if (countfile != "") {
		processList(out);
	}
	else {
		InputData input(inputfile, format);
		SAbundVector* sabund = input.getSAbundVector();
		string lastLabel = sabund->getLabel();


		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		if (ctrlc_pressed) { outputTypes.clear(); out.close(); File::remove(filename);  delete sabund;  return 0; }


		while ((sabund != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (allLines == 1 || labels.count(sabund->getLabel()) == 1) {
				LOG(INFO) << sabund->getLabel() << '\n';

				sabund->print(out);

				if (ctrlc_pressed) { outputTypes.clear();  out.close(); File::remove(filename);  delete sabund;   return 0; }

				processedLabels.insert(sabund->getLabel());
				userLabels.erase(sabund->getLabel());
			}

			if ((Utility::anyLabelsToProcess(sabund->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = sabund->getLabel();

				delete sabund;
				sabund = (input.getSAbundVector(lastLabel));

				LOG(INFO) << sabund->getLabel() << '\n';
				sabund->print(out);

				if (ctrlc_pressed) { outputTypes.clear(); out.close(); File::remove(filename);  delete sabund;   return 0; }

				processedLabels.insert(sabund->getLabel());
				userLabels.erase(sabund->getLabel());

				//restore real lastlabel to save below
				sabund->setLabel(saveLabel);
			}


			lastLabel = sabund->getLabel();

			delete sabund;
			sabund = (input.getSAbundVector());
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
			if (sabund != NULL) { delete sabund; }
			sabund = (input.getSAbundVector(lastLabel));

			LOG(INFO) << sabund->getLabel() << '\n';
			sabund->print(out);
			delete sabund;

			if (ctrlc_pressed) { outputTypes.clear(); out.close(); File::remove(filename); return 0; }

		}
	}
	out.close();

	if (ctrlc_pressed) { outputTypes.clear();  File::remove(filename);  return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	LOG(INFO) << filename << '\n';	outputNames.push_back(filename); outputTypes["sabund"].push_back(filename);
	LOG(INFO) << "";

	//set sabund file as new current sabundfile
	string current = "";
	itTypes = outputTypes.find("sabund");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("sabund", current); }
	}

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in GetSAbundCommand, execute";
	exit(1);
}
}
//**********************************************************************************************************************
int GetSAbundCommand::processList(ofstream& out) {
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
				SAbundVector sabund = rabund->getSAbundVector();
				sabund.print(out);
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
				SAbundVector sabund = rabund->getSAbundVector();
				sabund.print(out);
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
			SAbundVector sabund = rabund->getSAbundVector();
			sabund.print(out);
			delete rabund;

			delete list;
		}

		return 0;
	}
	//**********************************************************************************************************************

	int GetSAbundCommand::createRabund(CountTable& ct, ListVector*& list, RAbundVector*& rabund) {

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




