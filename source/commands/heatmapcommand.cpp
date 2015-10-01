/*
 *  heatmapcommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 3/25/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "heatmapcommand.h"

 //**********************************************************************************************************************
vector<string> HeatMapCommand::setParameters() {
	try {
		CommandParameter plist("list", "InputTypes", "", "", "LRSS", "LRSS", "none", "svg", false, false, true); parameters.push_back(plist);
		CommandParameter prabund("rabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "svg", false, false); parameters.push_back(prabund);
		CommandParameter psabund("sabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "svg", false, false); parameters.push_back(psabund);
		CommandParameter pshared("shared", "InputTypes", "", "", "LRSS", "LRSS", "none", "svg", false, false, true); parameters.push_back(pshared);
		CommandParameter prelabund("relabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "svg", false, false); parameters.push_back(prelabund);
		nkParameters.add(new StringParameter("groups", "", false, false));
		CommandParameter pscale("scale", "Multiple", "log10-log2-linear", "log10", "", "", "", "", false, false); parameters.push_back(pscale);
		CommandParameter psorted("sorted", "Multiple", "none-shared-topotu-topgroup", "shared", "", "", "", "", false, false); parameters.push_back(psorted);
		nkParameters.add(new NumberParameter("numotu", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("fontsize", -INFINITY, INFINITY, 24, false, false));
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in HeatMapCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string HeatMapCommand::getHelpString() {
	try {
		string helpString = "The heatmap.bin command parameters are shared, relabund, list, rabund, sabund, groups, sorted, scale, numotu, fontsize and label.  shared, relabund, list, rabund or sabund is required unless you have a valid current file.\n"
			"The groups parameter allows you to specify which of the groups in your groupfile you would like included in your heatmap.\n"
			"The sorted parameter allows you to order the otus displayed, default=shared, meaning display the shared otus first. Other options for sorted are none, meaning the exact representation of your otus, \n"
			"topotu, meaning the otus with the greatest abundance when totaled across groups, topgroup, meaning the top otus for each group. \n"
			"The scale parameter allows you to choose the range of color your bin information will be displayed with.\n"
			"The numotu parameter allows you to display only the top N otus, by default all the otus are displayed. You could choose to look at the top 10, by setting numotu=10. The default for sorted is topotu when numotu is used.\n"
			"The group names are separated by dashes. The label parameter allows you to select what distance levels you would like a heatmap created for, and are also separated by dashes.\n"
			"The fontsize parameter allows you to adjust the font size of the picture created, default=24.\n"
			"The heatmap.bin command should be in the following format: heatmap.bin(groups=yourGroups, sorted=yourSorted, label=yourLabels).\n"
			"Example heatmap.bin(groups=A-B-C, sorted=none, scale=log10).\n"
			"The default value for groups is all the groups in your groupfile, and all labels in your inputfile will be used.\n"
			"The default value for scale is log10; your other options are log2 and linear.\n"
			"The heatmap.bin command outputs a .svg file for each label you specify.\n"
			"Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in HeatMapCommand, getHelpString";
		exit(1);
	}
}

//**********************************************************************************************************************
string HeatMapCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "svg") { pattern = "[filename],svg"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}

//**********************************************************************************************************************
HeatMapCommand::HeatMapCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["svg"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in HeatMapCommand, HeatMapCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

HeatMapCommand::HeatMapCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["svg"] = tempOutNames;

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

			it = parameters.find("relabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["relabund"] = inputDir + it->second; }
			}
		}

		//check for required parameters
		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { format = "list"; inputfile = listfile; settings.setCurrent("list", listfile); }

		sabundfile = validParameter.validFile(parameters, "sabund", true);
		if (sabundfile == "not open") { abort = true; }
		else if (sabundfile == "not found") { sabundfile = ""; }
		else { format = "sabund"; inputfile = sabundfile; settings.setCurrent("sabund", sabundfile); }

		rabundfile = validParameter.validFile(parameters, "rabund", true);
		if (rabundfile == "not open") { abort = true; }
		else if (rabundfile == "not found") { rabundfile = ""; }
		else { format = "rabund"; inputfile = rabundfile; settings.setCurrent("rabund", rabundfile); }

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { format = "sharedfile"; inputfile = sharedfile; settings.setCurrent("shared", sharedfile); }

		relabundfile = validParameter.validFile(parameters, "relabund", true);
		if (relabundfile == "not open") { abort = true; }
		else if (relabundfile == "not found") { relabundfile = ""; }
		else { format = "relabund"; inputfile = relabundfile; settings.setCurrent("relabund", relabundfile); }


		if ((sharedfile == "") && (listfile == "") && (rabundfile == "") && (sabundfile == "") && (relabundfile == "")) {
			//is there are current file available for any of these?
			//give priority to shared, then list, then rabund, then sabund
			//if there is a current shared file, use it
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { inputfile = sharedfile; format = "sharedfile"; LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else {
				listfile = settings.getCurrent("list");
				if (listfile != "") { inputfile = listfile; format = "list"; LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
				else {
					rabundfile = settings.getCurrent("rabund");
					if (rabundfile != "") { inputfile = rabundfile; format = "rabund"; LOG(INFO) << "Using " + rabundfile + " as input file for the rabund parameter." << '\n'; }
					else {
						sabundfile = settings.getCurrent("sabund");
						if (sabundfile != "") { inputfile = sabundfile; format = "sabund"; LOG(INFO) << "Using " + sabundfile + " as input file for the sabund parameter." << '\n'; }
						else {
							relabundfile = settings.getCurrent("relabund");
							if (relabundfile != "") { inputfile = relabundfile; format = "relabund"; LOG(INFO) << "Using " + relabundfile + " as input file for the relabund parameter." << '\n'; }
							else {
								LOG(INFO) << "No valid current files. You must provide a list, sabund, rabund, relabund or shared file." << '\n';
								abort = true;
							}
						}
					}
				}
			}
		}


		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(inputfile); }

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}


		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else {
			Utility::split(groups, '-', Groups);
			m->setGroups(Groups);
		}

		string temp = validParameter.validFile(parameters, "numotu", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, numOTU);

		temp = validParameter.validFile(parameters, "fontsize", false);				if (temp == "not found") { temp = "24"; }
		Utility::mothurConvert(temp, fontSize);

		sorted = validParameter.validFile(parameters, "sorted", false);
		if (sorted == "not found") {
			//if numOTU is used change default
			if (numOTU != 0) { sorted = "topotu"; }
			else { sorted = "shared"; }
		}

		scale = validParameter.validFile(parameters, "scale", false);				if (scale == "not found") { scale = "log10"; }

		if ((sorted != "none") && (sorted != "shared") && (sorted != "topotu") && (sorted != "topgroup")) { LOG(INFO) << sorted + " is not a valid sorting option. Sorted options are: none, shared, topotu, topgroup" << '\n'; abort = true; }
	}

}
//**********************************************************************************************************************

int HeatMapCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	heatmap = new HeatMap(sorted, scale, numOTU, fontSize, outputDir, inputfile);

	string lastLabel;
	input = new InputData(inputfile, format);

	if (format == "sharedfile") {
		//you have groups
		lookup = input->getSharedRAbundVectors();
		lastLabel = lookup[0]->getLabel();

	}
	else if ((format == "list") || (format == "rabund") || (format == "sabund")) {
		//you are using just a list file and have only one group
		rabund = input->getRAbundVector();
		lastLabel = rabund->getLabel();
	}
	else if (format == "relabund") {
		//you have groups
		lookupFloat = input->getSharedRAbundFloatVectors();
		lastLabel = lookupFloat[0]->getLabel();
	}

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	if (format == "sharedfile") {

		//as long as you are not at the end of the file or done wih the lines you want
		while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
			if (ctrlc_pressed) {
				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
				for (int i = 0; i < outputNames.size(); i++) { if (outputNames[i] != "control") { File::remove(outputNames[i]); } } outputTypes.clear();
				m->clearGroups();
				delete input; delete heatmap; return 0;
			}

			if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

				LOG(INFO) << lookup[0]->getLabel() << '\n';
				string outputFileName = heatmap->getPic(lookup);
				outputNames.push_back(outputFileName); outputTypes["svg"].push_back(outputFileName);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());
			}

			if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = lookup[0]->getLabel();

				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }

				lookup = input->getSharedRAbundVectors(lastLabel);
				LOG(INFO) << lookup[0]->getLabel() << '\n';

				string outputFileName = heatmap->getPic(lookup);
				outputNames.push_back(outputFileName); outputTypes["svg"].push_back(outputFileName);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());

				//restore real lastlabel to save below
				lookup[0]->setLabel(saveLabel);
			}

			lastLabel = lookup[0]->getLabel();
			//prevent memory leak
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }

			//get next line to process
			lookup = input->getSharedRAbundVectors();
		}


		if (ctrlc_pressed) {
			for (int i = 0; i < outputNames.size(); i++) { if (outputNames[i] != "control") { File::remove(outputNames[i]); } } outputTypes.clear();
			m->clearGroups();
			delete input; delete heatmap; return 0;
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
			for (int i = 0; i < lookup.size(); i++) { if (lookup[i] != NULL) { delete lookup[i]; } }
			lookup = input->getSharedRAbundVectors(lastLabel);

			LOG(INFO) << lookup[0]->getLabel() << '\n';
			string outputFileName = heatmap->getPic(lookup);
			outputNames.push_back(outputFileName); outputTypes["svg"].push_back(outputFileName);
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		}

		//reset groups parameter
		m->clearGroups();

	}
	else if ((format == "list") || (format == "rabund") || (format == "sabund")) {

		while ((rabund != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
			if (ctrlc_pressed) {
				for (int i = 0; i < outputNames.size(); i++) { if (outputNames[i] != "control") { File::remove(outputNames[i]); } } outputTypes.clear();
				delete rabund;  delete input; delete heatmap; return 0;
			}

			if (allLines == 1 || labels.count(rabund->getLabel()) == 1) {

				LOG(INFO) << rabund->getLabel() << '\n';
				string outputFileName = heatmap->getPic(rabund);
				outputNames.push_back(outputFileName); outputTypes["svg"].push_back(outputFileName);

				processedLabels.insert(rabund->getLabel());
				userLabels.erase(rabund->getLabel());
			}

			if ((Utility::anyLabelsToProcess(rabund->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = rabund->getLabel();

				delete rabund;
				rabund = input->getRAbundVector(lastLabel);
				LOG(INFO) << rabund->getLabel() << '\n';

				string outputFileName = heatmap->getPic(rabund);
				outputNames.push_back(outputFileName); outputTypes["svg"].push_back(outputFileName);

				processedLabels.insert(rabund->getLabel());
				userLabels.erase(rabund->getLabel());

				//restore real lastlabel to save below
				rabund->setLabel(saveLabel);
			}



			lastLabel = rabund->getLabel();
			delete rabund;
			rabund = input->getRAbundVector();
		}

		if (ctrlc_pressed) {
			for (int i = 0; i < outputNames.size(); i++) { if (outputNames[i] != "control") { File::remove(outputNames[i]); } } outputTypes.clear();
			delete input; delete heatmap; return 0;
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
			rabund = input->getRAbundVector(lastLabel);
			LOG(INFO) << rabund->getLabel() << '\n';

			string outputFileName = heatmap->getPic(rabund);
			outputNames.push_back(outputFileName); outputTypes["svg"].push_back(outputFileName);
			delete rabund;
		}

	}
	else {

		//as long as you are not at the end of the file or done wih the lines you want
		while ((lookupFloat[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
			if (ctrlc_pressed) {
				for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; }
				for (int i = 0; i < outputNames.size(); i++) { if (outputNames[i] != "control") { File::remove(outputNames[i]); } } outputTypes.clear();
				m->clearGroups();
				delete input; delete heatmap; return 0;
			}

			if (allLines == 1 || labels.count(lookupFloat[0]->getLabel()) == 1) {

				LOG(INFO) << lookupFloat[0]->getLabel() << '\n';
				string outputFileName = heatmap->getPic(lookupFloat);
				outputNames.push_back(outputFileName); outputTypes["svg"].push_back(outputFileName);

				processedLabels.insert(lookupFloat[0]->getLabel());
				userLabels.erase(lookupFloat[0]->getLabel());
			}

			if ((Utility::anyLabelsToProcess(lookupFloat[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = lookupFloat[0]->getLabel();

				for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; }
				lookupFloat = input->getSharedRAbundFloatVectors(lastLabel);
				LOG(INFO) << lookupFloat[0]->getLabel() << '\n';

				string outputFileName = heatmap->getPic(lookupFloat);
				outputNames.push_back(outputFileName); outputTypes["svg"].push_back(outputFileName);

				processedLabels.insert(lookupFloat[0]->getLabel());
				userLabels.erase(lookupFloat[0]->getLabel());

				//restore real lastlabel to save below
				lookupFloat[0]->setLabel(saveLabel);
			}

			lastLabel = lookupFloat[0]->getLabel();
			//prevent memory leak
			for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; lookupFloat[i] = NULL; }

			//get next line to process
			lookupFloat = input->getSharedRAbundFloatVectors();
		}


		if (ctrlc_pressed) {
			for (int i = 0; i < outputNames.size(); i++) { if (outputNames[i] != "control") { File::remove(outputNames[i]); } } outputTypes.clear();
			m->clearGroups();
			delete input; delete heatmap; return 0;
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
			for (int i = 0; i < lookupFloat.size(); i++) { if (lookupFloat[i] != NULL) { delete lookupFloat[i]; } }
			lookupFloat = input->getSharedRAbundFloatVectors(lastLabel);

			LOG(INFO) << lookupFloat[0]->getLabel() << '\n';
			string outputFileName = heatmap->getPic(lookupFloat);
			outputNames.push_back(outputFileName); outputTypes["svg"].push_back(outputFileName);
			for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; }
		}

		//reset groups parameter
		m->clearGroups();

	}

	delete input;
	delete heatmap;

	if (ctrlc_pressed) {
		for (int i = 0; i < outputNames.size(); i++) { if (outputNames[i] != "control") { File::remove(outputNames[i]); } } outputTypes.clear(); return 0;
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}

//**********************************************************************************************************************


