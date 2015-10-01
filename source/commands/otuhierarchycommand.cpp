/*
 *  otuhierarchycommand.cpp
 *  Mothur
 *
 *  Created by westcott on 1/19/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "otuhierarchycommand.h"
#include "inputdata.h"

 //**********************************************************************************************************************
vector<string> OtuHierarchyCommand::setParameters() {
	try {
		CommandParameter poutput("output", "Multiple", "name-number", "name", "", "", "", "", false, false); parameters.push_back(poutput);
		CommandParameter plist("list", "InputTypes", "", "", "none", "none", "none", "otuheirarchy", false, true, true); parameters.push_back(plist);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OtuHierarchyCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string OtuHierarchyCommand::getHelpString() {
	try {
		string helpString = "The otu.hierarchy command is used to see how otus relate at two distances. \n"
			"The otu.hierarchy command parameters are list, label and output.  list and label parameters are required. \n"
			"The output parameter allows you to output the names of the sequence in the OTUs or the OTU numbers. Options are name and number, default is name. \n"
			"The otu.hierarchy command should be in the following format: \n"
			"otu.hierarchy(list=yourListFile, label=yourLabels).\n"
			"Example otu.hierarchy(list=amazon.fn.list, label=0.01-0.03).\n"
			"The otu.hierarchy command outputs a .otu.hierarchy file which is described on the wiki.\n"
			"Note: No spaces between parameter labels (i.e. list), '=' and parameters (i.e.yourListFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OtuHierarchyCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string OtuHierarchyCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "otuheirarchy") { pattern = "[filename],[distance1],[tag],[distance2],otu.hierarchy"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
OtuHierarchyCommand::OtuHierarchyCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["otuheirarchy"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OtuHierarchyCommand, OtuHierarchyCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
OtuHierarchyCommand::OtuHierarchyCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["otuheirarchy"] = tempOutNames;

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

		listFile = validParameter.validFile(parameters, "list", true);
		if (listFile == "not found") {
			listFile = settings.getCurrent("list");
			if (listFile != "") { LOG(INFO) << "Using " + listFile + " as input file for the list parameter." << '\n'; }
			else {
				LOG(INFO) << "No valid current list file. You must provide a list file." << '\n';
				abort = true;
			}
		}
		else if (listFile == "not open") { abort = true; }
		else { settings.setCurrent("list", listFile); }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = "";
			outputDir += File::getPath(listFile); //if user entered a file with a path then preserve it	
		}

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { LOG(INFO) << "label is a required parameter for the otu.hierarchy command." << '\n'; abort = true; }
		else {
			Utility::split(label, '-', mylabels);
			if (mylabels.size() != 2) { LOG(INFO) << "You must provide 2 labels." << '\n'; abort = true; }
		}

		output = validParameter.validFile(parameters, "output", false);			if (output == "not found") { output = "name"; }

		if ((output != "name") && (output != "number")) { LOG(INFO) << "output options are name and number. I will use name." << '\n'; output = "name"; }
	}

}
//**********************************************************************************************************************

int OtuHierarchyCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//get listvectors that correspond to labels requested, (or use smart distancing to get closest listvector)
	vector< vector<string> > lists = getListVectors();

	if (ctrlc_pressed) { outputTypes.clear(); return 0; }

	//determine which is little and which is big, putting little first
	if (lists.size() == 4) {
		//if big is first swap them
		if (lists[0].size() < lists[2].size()) {
			vector< vector<string> > tempLists;
			tempLists.push_back(lists[2]);
			tempLists.push_back(lists[3]);
			tempLists.push_back(lists[0]);
			tempLists.push_back(lists[1]);
			lists = tempLists;
			string tempLabel = list2Label;
			list2Label = list1Label;
			list1Label = tempLabel;
		}
	}
	else {
		LOG(INFO) << "error getting listvectors, unable to read 2 different vectors, check your label inputs." << '\n'; return 0;
	}

	//map sequences to bin number in the "little" otu
	map<string, int> littleBins;
	vector<string> binLabels0 = lists[0];
	for (int i = 0; i < lists[0].size(); i++) {

		if (ctrlc_pressed) { return 0; }
		string bin = lists[1][i];
		vector<string> names; m->splitAtComma(bin, names);
		for (int j = 0; j < names.size(); j++) { littleBins[names[j]] = i; }
	}

	ofstream out;
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(listFile));
	variables["[distance1]"] = list1Label;
	variables["[tag]"] = "-";
	variables["[distance2]"] = list2Label;
	string outputFileName = getOutputFileName("otuheirarchy", variables);
	File::openOutputFile(outputFileName, out);

	//go through each bin in "big" otu and output the bins in "little" otu which created it
	vector<string> binLabels1 = lists[2];
	for (int i = 0; i < lists[2].size(); i++) {

		if (ctrlc_pressed) { outputTypes.clear(); out.close(); File::remove(outputFileName); return 0; }

		string binnames = lists[3][i];
		vector<string> names; m->splitAtComma(binnames, names);

		//output column 1
		if (output == "name") { out << binnames << '\t'; }
		else { out << binLabels1[i] << '\t'; }

		map<int, int> bins; //bin numbers in little that are in this bin in big
		map<int, int>::iterator it;

		//parse bin
		for (int j = 0; j < names.size(); j++) { bins[littleBins[names[j]]] = littleBins[names[j]]; }

		string col2 = "";
		for (it = bins.begin(); it != bins.end(); it++) {
			if (output == "name") { col2 += lists[1][it->first] + "\t"; }
			else { col2 += binLabels0[it->first] + "\t"; }
		}

		//output column 2
		out << col2 << endl;
	}

	out.close();

	if (ctrlc_pressed) { outputTypes.clear(); File::remove(outputFileName); return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	LOG(INFO) << outputFileName << '\n';	outputNames.push_back(outputFileName); outputTypes["otuheirarchy"].push_back(outputFileName);
	LOG(INFO) << "";

	return 0;
}

//**********************************************************************************************************************
//returns a vector of listVectors where "little" vector is first
vector< vector<string> > OtuHierarchyCommand::getListVectors() { //return value [0] -> otulabelsFirstLabel [1] -> binsFirstLabel [2] -> otulabelsSecondLabel [3] -> binsSecondLabel
	try {
		vector< vector<string> > lists;

		int count = 0;
		for (set<string>::iterator it = mylabels.begin(); it != mylabels.end(); it++) {
			string realLabel;
			vector< vector<string> > thisList = getListVector(*it, realLabel);

			if (ctrlc_pressed) { return lists; }

			for (int i = 0; i < thisList.size(); i++) { lists.push_back(thisList[i]); }

			if (count == 0) { list1Label = realLabel; count++; }
			else { list2Label = realLabel; }
		}

		return lists;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OtuHierarchyCommand, getListVectors";
		exit(1);
	}
}
//**********************************************************************************************************************
vector< vector<string> > OtuHierarchyCommand::getListVector(string label, string& realLabel) { //return value [0] -> otulabels [1] -> bins
	try {
		vector< vector<string> > myList;

		InputData input(listFile, "list");
		ListVector* list = input.getListVector();
		string lastLabel = list->getLabel();

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> labels; labels.insert(label);
		set<string> processedLabels;
		set<string> userLabels = labels;

		//as long as you are not at the end of the file or done wih the lines you want
		while ((list != NULL) && (userLabels.size() != 0)) {
			if (ctrlc_pressed) { return myList; }

			if (labels.count(list->getLabel()) == 1) {
				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());
				break;
			}

			if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = list->getLabel();

				delete list;
				list = input.getListVector(lastLabel);

				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());

				//restore real lastlabel to save below
				//list->setLabel(saveLabel);
				break;
			}

			lastLabel = list->getLabel();

			//get next line to process
			//prevent memory leak
			delete list;
			list = input.getListVector();
		}


		if (ctrlc_pressed) { return myList; }

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
		}

		//at this point the list vector has the right distance
		myList.push_back(list->getLabels());
		vector<string> bins;
		for (int i = 0; i < list->getNumBins(); i++) {
			if (ctrlc_pressed) { return myList; }
			bins.push_back(list->get(i));
		}
		myList.push_back(bins);
		realLabel = list->getLabel();

		delete list;

		return myList;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OtuHierarchyCommand, getListVector";
		exit(1);
	}
}

//**********************************************************************************************************************





