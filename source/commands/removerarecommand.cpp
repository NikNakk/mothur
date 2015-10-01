/*
 *  removerarecommand.cpp
 *  mothur
 *
 *  Created by westcott on 1/21/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "removerarecommand.h"
#include "sequence.hpp"
#include "groupmap.h"
#include "sharedutilities.h"
#include "inputdata.h"

 //**********************************************************************************************************************
vector<string> RemoveRareCommand::setParameters() {
	try {
		CommandParameter plist("list", "InputTypes", "", "", "none", "atleast", "none", "list", false, false, true); parameters.push_back(plist);
		CommandParameter prabund("rabund", "InputTypes", "", "", "none", "atleast", "none", "rabund", false, false, true); parameters.push_back(prabund);
		CommandParameter psabund("sabund", "InputTypes", "", "", "none", "atleast", "none", "sabund", false, false, true); parameters.push_back(psabund);
		CommandParameter pshared("shared", "InputTypes", "", "", "none", "atleast", "none", "shared", false, false, true); parameters.push_back(pshared);
		CommandParameter pcount("count", "InputTypes", "", "", "CountGroup", "none", "none", "count", false, false); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none", "group", false, false); parameters.push_back(pgroup);
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new StringParameter("label", "", false, false));
		CommandParameter pnseqs("nseqs", "Number", "", "0", "", "", "", "", false, true, true); parameters.push_back(pnseqs);
		nkParameters.add(new BooleanParameter("bygroup", false, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RemoveRareCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string RemoveRareCommand::getHelpString() {
	try {
		string helpString = "The remove.rare command parameters are list, rabund, sabund, shared, group, count, label, groups, bygroup and nseqs.\n"
			"The remove.rare command reads one of the following file types: list, rabund, sabund or shared file. It outputs a new file after removing the rare otus.\n"
			"The groups parameter allows you to specify which of the groups you would like analyzed.  Default=all. You may separate group names with dashes.\n"
			"The label parameter is used to analyze specific labels in your input. default=all. You may separate label names with dashes.\n"
			"The bygroup parameter is only valid with the shared file. default=f, meaning remove any OTU that has nseqs or fewer sequences across all groups.\n"
			"bygroups=T means remove any OTU that has nseqs or fewer sequences in each group (if groupA has 1 sequence and group B has 100 sequences in OTUZ and nseqs=1, then set the groupA count for OTUZ to 0 and keep groupB's count at 100.) \n"
			"The nseqs parameter allows you to set the cutoff for an otu to be deemed rare. It is required.\n"
			"The remove.rare command should be in the following format: remove.rare(shared=yourSharedFile, nseqs=yourRareCutoff).\n"
			"Example remove.rare(shared=amazon.fn.shared, nseqs=2).\n"
			"Note: No spaces between parameter labels (i.e. shared), '=' and parameters (i.e.yourSharedFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RemoveRareCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string RemoveRareCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "rabund") { pattern = "[filename],pick,[extension]"; }
	else if (type == "sabund") { pattern = "[filename],pick,[extension]"; }
	else if (type == "group") { pattern = "[filename],pick,[extension]"; }
	else if (type == "count") { pattern = "[filename],pick,[extension]"; }
	else if (type == "list") { pattern = "[filename],[tag],pick,[extension]"; }
	else if (type == "shared") { pattern = "[filename],[tag],pick,[extension]"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
RemoveRareCommand::RemoveRareCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["rabund"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["shared"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RemoveRareCommand, RemoveRareCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
RemoveRareCommand::RemoveRareCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["rabund"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["shared"] = tempOutNames;
		outputTypes["count"] = tempOutNames;

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

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

			it = parameters.find("sabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["sabund"] = inputDir + it->second; }
			}

			it = parameters.find("rabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["rabund"] = inputDir + it->second; }
			}

			it = parameters.find("shared");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["shared"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}
		}


		//check for file parameters
		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { settings.setCurrent("list", listfile); }

		sabundfile = validParameter.validFile(parameters, "sabund", true);
		if (sabundfile == "not open") { abort = true; }
		else if (sabundfile == "not found") { sabundfile = ""; }
		else { settings.setCurrent("sabund", sabundfile); }

		rabundfile = validParameter.validFile(parameters, "rabund", true);
		if (rabundfile == "not open") { abort = true; }
		else if (rabundfile == "not found") { rabundfile = ""; }
		else { settings.setCurrent("rabund", rabundfile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { groupfile = ""; abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = "";  abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { settings.setCurrent("shared", sharedfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		if ((groupfile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: group or count." << '\n'; abort = true;
		}

		if ((sharedfile == "") && (listfile == "") && (rabundfile == "") && (sabundfile == "")) {
			//is there are current file available for any of these?
			//give priority to shared, then list, then rabund, then sabund
			//if there is a current shared file, use it
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else {
				listfile = settings.getCurrent("list");
				if (listfile != "") { LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
				else {
					rabundfile = settings.getCurrent("rabund");
					if (rabundfile != "") { LOG(INFO) << "Using " + rabundfile + " as input file for the rabund parameter." << '\n'; }
					else {
						sabundfile = settings.getCurrent("sabund");
						if (sabundfile != "") { LOG(INFO) << "Using " + sabundfile + " as input file for the sabund parameter." << '\n'; }
						else {
							LOG(INFO) << "No valid current files. You must provide a list, sabund, rabund or shared file." << '\n';
							abort = true;
						}
					}
				}
			}
		}

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = "all"; }
		Utility::split(groups, '-', Groups);

		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		string temp = validParameter.validFile(parameters, "nseqs", false);
		if (temp == "not found") { LOG(INFO) << "nseqs is a required parameter." << '\n'; abort = true; }
		else { Utility::mothurConvert(temp, nseqs); }

		temp = validParameter.validFile(parameters, "bygroup", false);	 if (temp == "not found") { temp = "f"; }
		byGroup = m->isTrue(temp);

		if (byGroup && (sharedfile == "")) { LOG(INFO) << "The byGroup parameter is only valid with a shared file." << '\n'; }

		if (((groupfile != "") || (countfile != "")) && (listfile == "")) { LOG(INFO) << "A group or count file is only valid with a list file." << '\n'; groupfile = ""; countfile = ""; }
	}

}
//**********************************************************************************************************************

int RemoveRareCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	if (ctrlc_pressed) { return 0; }

	//read through the correct file and output lines you want to keep
	if (sabundfile != "") { processSabund(); }
	if (rabundfile != "") { processRabund(); }
	if (listfile != "") { processList(); }
	if (sharedfile != "") { processShared(); }

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (outputNames.size() != 0) {
		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		//set rabund file as new current rabundfile
		string current = "";
		itTypes = outputTypes.find("rabund");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("rabund", current); }
		}

		itTypes = outputTypes.find("sabund");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("sabund", current); }
		}

		itTypes = outputTypes.find("group");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("group", current); }
		}

		itTypes = outputTypes.find("list");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("list", current); }
		}

		itTypes = outputTypes.find("shared");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("shared", current); }
		}

		itTypes = outputTypes.find("count");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
		}
	}

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in RemoveRareCommand, execute";
	exit(1);
}
}

//**********************************************************************************************************************
int RemoveRareCommand::processList() {
	try {

		//you must provide a label because the names in the listfile need to be consistent
		string thisLabel = "";
		if (allLines) { LOG(INFO) << "For the listfile you must select one label, using first label in your listfile." << '\n'; }
		else if (labels.size() > 1) { LOG(INFO) << "For the listfile you must select one label, using " + (*labels.begin()) + "." << '\n'; thisLabel = *labels.begin(); }
		else { thisLabel = *labels.begin(); }

		InputData input(listfile, "list");
		ListVector* list = input.getListVector();

		//get first one or the one we want
		if (thisLabel != "") {
			//use smart distancing
			set<string> userLabels; userLabels.insert(thisLabel);
			set<string> processedLabels;
			string lastLabel = list->getLabel();
			while ((list != NULL) && (userLabels.size() != 0)) {
				if (userLabels.count(list->getLabel()) == 1) {
					processedLabels.insert(list->getLabel());
					userLabels.erase(list->getLabel());
					break;
				}

				if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
					processedLabels.insert(list->getLabel());
					userLabels.erase(list->getLabel());
					delete list;
					list = input.getListVector(lastLabel);
					break;
				}
				lastLabel = list->getLabel();
				delete list;
				list = input.getListVector();
			}
			if (userLabels.size() != 0) {
				LOG(INFO) << "Your file does not include the label " + thisLabel + ". I will use " + lastLabel + "." << '\n';
				list = input.getListVector(lastLabel);
			}
		}

		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(listfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(listfile));
		variables["[extension]"] = m->getExtension(listfile);
		variables["[tag]"] = list->getLabel();
		string outputFileName = getOutputFileName("list", variables);
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(groupfile));
		variables["[extension]"] = m->getExtension(groupfile);
		string outputGroupFileName = getOutputFileName("group", variables);
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(countfile));
		variables["[extension]"] = m->getExtension(countfile);
		string outputCountFileName = getOutputFileName("count", variables);

		ofstream out, outGroup;
		File::openOutputFile(outputFileName, out);

		bool wroteSomething = false;


		//if groupfile is given then use it
		GroupMap* groupMap;
		CountTable ct;
		if (groupfile != "") {
			groupMap = new GroupMap(groupfile); groupMap->readMap();
			SharedUtil util;
			vector<string> namesGroups = groupMap->getNamesOfGroups();
			util.setGroups(Groups, namesGroups);
			File::openOutputFile(outputGroupFileName, outGroup);
		}
		else if (countfile != "") {
			ct.readTable(countfile, true, false);
			if (ct.hasGroupInfo()) {
				vector<string> namesGroups = ct.getNamesOfGroups();
				SharedUtil util;
				util.setGroups(Groups, namesGroups);
			}
		}


		if (list != NULL) {

			vector<string> binLabels = list->getLabels();
			vector<string> newLabels;

			//make a new list vector
			ListVector newList;
			newList.setLabel(list->getLabel());

			//for each bin
			for (int i = 0; i < list->getNumBins(); i++) {
				if (ctrlc_pressed) { if (groupfile != "") { delete groupMap; outGroup.close(); File::remove(outputGroupFileName); } out.close();  File::remove(outputFileName);  return 0; }

				//parse out names that are in accnos file
				string binnames = list->get(i);
				vector<string> names;
				string saveBinNames = binnames;
				m->splitAtComma(binnames, names);
				int binsize = names.size();

				vector<string> newGroupFile;
				if (groupfile != "") {
					vector<string> newNames;
					saveBinNames = "";
					for (int k = 0; k < names.size(); k++) {
						string group = groupMap->getGroup(names[k]);

						if (m->inUsersGroups(group, Groups)) {
							newGroupFile.push_back(names[k] + "\t" + group);

							newNames.push_back(names[k]);
							saveBinNames += names[k] + ",";
						}
					}
					names = newNames; binsize = names.size();
					saveBinNames = saveBinNames.substr(0, saveBinNames.length() - 1);
				}
				else if (countfile != "") {
					saveBinNames = "";
					binsize = 0;
					for (int k = 0; k < names.size(); k++) {
						if (ct.hasGroupInfo()) {
							vector<string> thisSeqsGroups = ct.getGroups(names[k]);

							int thisSeqsCount = 0;
							for (int n = 0; n < thisSeqsGroups.size(); n++) {
								if (m->inUsersGroups(thisSeqsGroups[n], Groups)) {
									thisSeqsCount += ct.getGroupCount(names[k], thisSeqsGroups[n]);
								}
							}
							binsize += thisSeqsCount;
							//if you don't have any seqs from the groups the user wants, then remove you.
							if (thisSeqsCount == 0) { newGroupFile.push_back(names[k]); }
							else { saveBinNames += names[k] + ","; }
						}
						else {
							binsize += ct.getNumSeqs(names[k]);
							saveBinNames += names[k] + ",";
						}
					}
					saveBinNames = saveBinNames.substr(0, saveBinNames.length() - 1);
				}

				if (binsize > nseqs) { //keep bin
					newList.push_back(saveBinNames);
					newLabels.push_back(binLabels[i]);
					if (groupfile != "") { for (int k = 0; k < newGroupFile.size(); k++) { outGroup << newGroupFile[k] << endl; } }
					else if (countfile != "") { for (int k = 0; k < newGroupFile.size(); k++) { ct.remove(newGroupFile[k]); } }
				}
				else { if (countfile != "") { for (int k = 0; k < names.size(); k++) { ct.remove(names[k]); } } }
			}

			//print new listvector
			if (newList.getNumBins() != 0) {
				wroteSomething = true;
				newList.setLabels(newLabels);
				newList.printHeaders(out);
				newList.print(out);
			}
		}

		out.close();
		if (groupfile != "") { outGroup.close(); outputTypes["group"].push_back(outputGroupFileName); outputNames.push_back(outputGroupFileName); }
		if (countfile != "") {
			if (ct.hasGroupInfo()) {
				vector<string> allGroups = ct.getNamesOfGroups();
				for (int i = 0; i < allGroups.size(); i++) {
					if (!m->inUsersGroups(allGroups[i], Groups)) { ct.removeGroup(allGroups[i]); }
				}

			}
			ct.printTable(outputCountFileName);
			outputTypes["count"].push_back(outputCountFileName); outputNames.push_back(outputCountFileName);
		}

		if (wroteSomething == false) { LOG(INFO) << "Your file contains only rare sequences." << '\n'; }
		outputTypes["list"].push_back(outputFileName); outputNames.push_back(outputFileName);

		return 0;
	}
	//**********************************************************************************************************************
	int RemoveRareCommand::processSabund() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(sabundfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(sabundfile));
		variables["[extension]"] = m->getExtension(sabundfile);
		string outputFileName = getOutputFileName("sabund", variables);
		outputTypes["sabund"].push_back(outputFileName); outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		InputData input(sabundfile, "sabund");
		SAbundVector* sabund = input.getSAbundVector();
		string lastLabel = sabund->getLabel();
		set<string> processedLabels;
		set<string> userLabels = labels;

		while ((sabund != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { delete sabund; out.close(); return 0; }

			if (allLines == 1 || labels.count(sabund->getLabel()) == 1) {

				LOG(INFO) << sabund->getLabel() << '\n';
				processedLabels.insert(sabund->getLabel());
				userLabels.erase(sabund->getLabel());

				if (sabund->getMaxRank() > nseqs) {
					for (int i = 1; i <= nseqs; i++) { sabund->set(i, 0); }
				}
				else { sabund->clear(); }

				if (sabund->getNumBins() > 0) { sabund->print(out); }
			}

			if ((Utility::anyLabelsToProcess(sabund->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = sabund->getLabel();

				delete sabund;
				sabund = input.getSAbundVector(lastLabel);

				LOG(INFO) << sabund->getLabel() << '\n';
				processedLabels.insert(sabund->getLabel());
				userLabels.erase(sabund->getLabel());

				if (sabund->getMaxRank() > nseqs) {
					for (int i = 1; i <= nseqs; i++) { sabund->set(i, 0); }
				}
				else { sabund->clear(); }

				if (sabund->getNumBins() > 0) { sabund->print(out); }

				//restore real lastlabel to save below
				sabund->setLabel(saveLabel);
			}

			lastLabel = sabund->getLabel();

			delete sabund;
			sabund = input.getSAbundVector();
		}

		if (ctrlc_pressed) { out.close(); return 0; }

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
			sabund = input.getSAbundVector(lastLabel);

			LOG(INFO) << sabund->getLabel() << '\n';

			if (sabund->getMaxRank() > nseqs) {
				for (int i = 1; i <= nseqs; i++) { sabund->set(i, 0); }
			}
			else { sabund->clear(); }

			if (sabund->getNumBins() > 0) { sabund->print(out); }

			delete sabund;
		}

		return 0;
	}
	//**********************************************************************************************************************
	int RemoveRareCommand::processRabund() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(rabundfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(rabundfile));
		variables["[extension]"] = m->getExtension(rabundfile);
		string outputFileName = getOutputFileName("rabund", variables);
		outputTypes["rabund"].push_back(outputFileName); outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		InputData input(rabundfile, "rabund");
		RAbundVector* rabund = input.getRAbundVector();
		string lastLabel = rabund->getLabel();
		set<string> processedLabels;
		set<string> userLabels = labels;

		while ((rabund != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { delete rabund; out.close(); return 0; }

			if (allLines == 1 || labels.count(rabund->getLabel()) == 1) {

				LOG(INFO) << rabund->getLabel() << '\n';
				processedLabels.insert(rabund->getLabel());
				userLabels.erase(rabund->getLabel());

				RAbundVector newRabund; newRabund.setLabel(rabund->getLabel());
				for (int i = 0; i < rabund->getNumBins(); i++) {
					if (rabund->get(i) > nseqs) {
						newRabund.push_back(rabund->get(i));
					}
				}
				if (newRabund.getNumBins() > 0) { newRabund.print(out); }
			}

			if ((Utility::anyLabelsToProcess(rabund->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = rabund->getLabel();

				delete rabund;
				rabund = input.getRAbundVector(lastLabel);

				LOG(INFO) << rabund->getLabel() << '\n';
				processedLabels.insert(rabund->getLabel());
				userLabels.erase(rabund->getLabel());

				RAbundVector newRabund; newRabund.setLabel(rabund->getLabel());
				for (int i = 0; i < rabund->getNumBins(); i++) {
					if (rabund->get(i) > nseqs) {
						newRabund.push_back(rabund->get(i));
					}
				}
				if (newRabund.getNumBins() > 0) { newRabund.print(out); }

				//restore real lastlabel to save below
				rabund->setLabel(saveLabel);
			}

			lastLabel = rabund->getLabel();

			delete rabund;
			rabund = input.getRAbundVector();
		}

		if (ctrlc_pressed) { out.close(); return 0; }

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

			RAbundVector newRabund; newRabund.setLabel(rabund->getLabel());
			for (int i = 0; i < rabund->getNumBins(); i++) {
				if (rabund->get(i) > nseqs) {
					newRabund.push_back(rabund->get(i));
				}
			}
			if (newRabund.getNumBins() > 0) { newRabund.print(out); }

			delete rabund;
		}

		return 0;
	}
	//**********************************************************************************************************************
	int RemoveRareCommand::processShared() {
		m->setGroups(Groups);

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		InputData input(sharedfile, "sharedfile");
		vector<SharedRAbundVector*> lookup = input.getSharedRAbundVectors();
		string lastLabel = lookup[0]->getLabel();
		set<string> processedLabels;
		set<string> userLabels = labels;

		while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }   return 0; }

			if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

				LOG(INFO) << lookup[0]->getLabel() << '\n';
				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());

				processLookup(lookup);
			}

			if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = lookup[0]->getLabel();

				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
				lookup = input.getSharedRAbundVectors(lastLabel);

				LOG(INFO) << lookup[0]->getLabel() << '\n';
				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());

				processLookup(lookup);

				//restore real lastlabel to save below
				lookup[0]->setLabel(saveLabel);
			}

			lastLabel = lookup[0]->getLabel();

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input.getSharedRAbundVectors();
		}

		if (ctrlc_pressed) { return 0; }

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
			processLookup(lookup);

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		}

		return 0;
	}
	//**********************************************************************************************************************
	int RemoveRareCommand::processLookup(vector<SharedRAbundVector*>& lookup) {

		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(sharedfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(sharedfile));
		variables["[extension]"] = m->getExtension(sharedfile);
		variables["[tag]"] = lookup[0]->getLabel();
		string outputFileName = getOutputFileName("shared", variables);
		outputTypes["shared"].push_back(outputFileName); outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		vector<SharedRAbundVector> newRabunds;  newRabunds.resize(lookup.size());
		vector<string> headers;
		for (int i = 0; i < lookup.size(); i++) {
			newRabunds[i].setGroup(lookup[i]->getGroup());
			newRabunds[i].setLabel(lookup[i]->getLabel());
		}

		if (byGroup) {

			//for each otu
			for (int i = 0; i < lookup[0]->getNumBins(); i++) {
				bool allZero = true;

				if (ctrlc_pressed) { out.close(); return 0; }

				//for each group
				for (int j = 0; j < lookup.size(); j++) {

					//are you rare?
					if (lookup[j]->getAbundance(i) > nseqs) {
						newRabunds[j].push_back(lookup[j]->getAbundance(i), newRabunds[j].getGroup());
						allZero = false;
					}
					else {
						newRabunds[j].push_back(0, newRabunds[j].getGroup());
					}
				}

				//eliminates zero otus
				if (allZero) { for (int j = 0; j < newRabunds.size(); j++) { newRabunds[j].pop_back(); } }
				else { headers.push_back(settings.currentSharedBinLabels[i]); }
			}
		}
		else {
			//for each otu
			for (int i = 0; i < lookup[0]->getNumBins(); i++) {

				if (ctrlc_pressed) { out.close(); return 0; }

				int totalAbund = 0;
				//get total otu abundance
				for (int j = 0; j < lookup.size(); j++) {
					newRabunds[j].push_back(lookup[j]->getAbundance(i), newRabunds[j].getGroup());
					totalAbund += lookup[j]->getAbundance(i);
				}

				//eliminates otus below rare cutoff
				if (totalAbund <= nseqs) { for (int j = 0; j < newRabunds.size(); j++) { newRabunds[j].pop_back(); } }
				else { headers.push_back(settings.currentSharedBinLabels[i]); }
			}
		}

		//do we have any otus above the rare cutoff
		if (newRabunds[0].getNumBins() != 0) {
			out << "label\tGroup\tnumOtus";
			for (int j = 0; j < headers.size(); j++) { out << '\t' << headers[j]; }
			out << endl;
			for (int j = 0; j < newRabunds.size(); j++) {
				out << newRabunds[j].getLabel() << '\t' << newRabunds[j].getGroup() << '\t';
				newRabunds[j].print(out);
			}
		}

		out.close();

		return 0;
	}
	//**********************************************************************************************************************




