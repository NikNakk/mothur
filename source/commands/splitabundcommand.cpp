/*
 *  splitabundcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/17/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "splitabundcommand.h"
#include "sharedutilities.h"

 //**********************************************************************************************************************
vector<string> SplitAbundCommand::setParameters() {
	try {
		nkParameters.add(new FastaFileParameter(settings, "fasta", true, true));
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "FNGLT", "none", "name", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "none", "none", "count", false, false); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none", "group", false, false); parameters.push_back(pgroup);
		CommandParameter plist("list", "InputTypes", "", "", "none", "FNGLT", "none", "list", false, false, true); parameters.push_back(plist);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("cutoff", -INFINITY, INFINITY, 0, false, true));
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new BooleanParameter("accnos", false, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string SplitAbundCommand::getHelpString() {
	try {
		string helpString = "The split.abund command reads a fasta file and a list or a names file splits the sequences into rare and abundant groups. \n"
			"The split.abund command parameters are fasta, list, name, count, cutoff, group, label, groups, cutoff and accnos.\n"
			"The fasta and a list or name or count parameter are required, and you must provide a cutoff value.\n"
			"The cutoff parameter is used to qualify what is abundant and rare.\n"
			"The group parameter allows you to parse a group file into rare and abundant groups.\n"
			"The label parameter is used to read specific labels in your listfile you want to use.\n"
			"The accnos parameter allows you to output a .rare.accnos and .abund.accnos files to use with the get.seqs and remove.seqs commands.\n"
			"The groups parameter allows you to parse the files into rare and abundant files by group.  \n"
			"For example if you set groups=A-B-C, you will get a .A.abund, .A.rare, .B.abund, .B.rare, .C.abund, .C.rare files.  \n"
			"If you want .abund and .rare files for all groups, set groups=all.  \n"
			"The split.abund command should be used in the following format: split.abund(fasta=yourFasta, list=yourListFile, group=yourGroupFile, label=yourLabels, cutoff=yourCutoff).\n"
			"Example: split.abund(fasta=abrecovery.fasta, list=abrecovery.fn.list, group=abrecovery.groups, label=0.03, cutoff=2).\n"
			"Note: No spaces between parameter labels (i.e. list), '=' and parameters (i.e.yourListfile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, getHelpString";
		exit(1);
	}
}

//**********************************************************************************************************************
string SplitAbundCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],[tag],[tag2],fasta-[filename],[tag],[group],[tag2],fasta"; }
	else if (type == "list") { pattern = "[filename],[tag],[tag2],list-[filename],[group],[tag],[tag2],list"; }
	else if (type == "name") { pattern = "[filename],[tag],names-[filename],[group],[tag],names"; }
	else if (type == "count") { pattern = "[filename],[tag],[tag2],count_table-[filename],[tag],count_table"; }
	else if (type == "group") { pattern = "[filename],[tag],[tag2],groups-[filename],[tag],[group],[tag2],groups"; }
	else if (type == "accnos") { pattern = "[filename],[tag],[tag2],accnos-[filename],[tag],[group],[tag2],accnos"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
SplitAbundCommand::SplitAbundCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["fasta"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, SplitAbundCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
SplitAbundCommand::SplitAbundCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["name"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["count"] = tempOutNames;

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

			it = parameters.find("fasta");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fasta"] = inputDir + it->second; }
			}

			it = parameters.find("name");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["name"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}
		}


		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//check for required parameters
		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { inputFile = listfile; settings.setCurrent("list", listfile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { inputFile = namefile; settings.setCurrent("name", namefile); }

		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not open") { abort = true; }
		else if (fastafile == "not found") {
			fastafile = settings.getCurrent("fasta");
			if (fastafile != "") { LOG(INFO) << "Using " + fastafile + " as input file for the fasta parameter." << '\n'; }
			else { LOG(INFO) << "You have no current fastafile and the fasta parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("fasta", fastafile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { groupfile = ""; abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else {
			int error = groupMap.readMap(groupfile);
			if (error == 1) { abort = true; }
			settings.setCurrent("group", groupfile);
		}

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else {
			settings.setCurrent("counttable", countfile);
			ct.readTable(countfile, true, false);
		}

		if ((namefile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: name or count." << '\n'; abort = true;
		}

		if ((groupfile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: group or count." << '\n'; abort = true;
		}

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else { Utility::split(groups, '-', Groups); }

		if (((groupfile == "") && (countfile == "")) && (groups != "")) { LOG(INFO) << "You cannot select groups without a valid group or count file, I will disregard your groups selection. " << '\n'; groups = "";  Groups.clear(); }

		if (countfile != "") {
			if (!ct.hasGroupInfo()) { LOG(INFO) << "You cannot pick groups without group info in your count file; I will disregard your groups selection."; m->mothurOutEndLine(); groups = "";  Groups.clear(); }
		}

		//do you have all files needed
		if ((listfile == "") && (namefile == "") && (countfile == "")) {
			namefile = settings.getCurrent("name");
			if (namefile != "") { LOG(INFO) << "Using " + namefile + " as input file for the name parameter." << '\n'; }
			else {
				listfile = settings.getCurrent("list");
				if (listfile != "") { LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
				else {
					countfile = settings.getCurrent("counttable");
					if (countfile != "") { LOG(INFO) << "Using " + countfile + " as input file for the count parameter." << '\n'; }
					else { LOG(INFO) << "You have no current list, count or namefile and one is required." << '\n'; abort = true; }
				}
			}
		}

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = "";  allLines = 1; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		string temp = validParameter.validFile(parameters, "accnos", false);		if (temp == "not found") { temp = "F"; }
		accnos = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "cutoff", false);				if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, cutoff);

		if (cutoff == 0) { LOG(INFO) << "You must provide a cutoff to qualify what is abundant for the split.abund command. " << '\n'; abort = true; }
	}

}
//**********************************************************************************************************************
SplitAbundCommand::~SplitAbundCommand() {}
//**********************************************************************************************************************
int SplitAbundCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	if (Groups.size() != 0) {
		vector<string> allGroups;
		if (countfile != "") { allGroups = ct.getNamesOfGroups(); }
		else { allGroups = groupMap.getNamesOfGroups(); }
		SharedUtil util;
		util.setGroups(Groups, allGroups);
	}

	if (listfile != "") { //you are using a listfile to determine abundance
		if (outputDir == "") { outputDir = File::getPath(listfile); }

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		InputData input(listfile, "list");
		ListVector* list = input.getListVector();
		string lastLabel = list->getLabel();

		//do you have a namefile or do we need to similate one?
		if (namefile != "") { readNamesFile(); }
		else { createNameMap(list); }

		if (ctrlc_pressed) { delete list; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

		while ((list != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { delete list; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

			if (allLines == 1 || labels.count(list->getLabel()) == 1) {

				LOG(INFO) << list->getLabel() << '\n';
				splitList(list);

				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());
			}

			if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = list->getLabel();

				delete list;
				list = input.getListVector(lastLabel); //get new list vector to process

				LOG(INFO) << list->getLabel() << '\n';
				splitList(list);

				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());

				//restore real lastlabel to save below
				list->setLabel(saveLabel);
			}


			lastLabel = list->getLabel();

			delete list;
			list = input.getListVector(); //get new list vector to process
		}

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

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

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

		//run last label if you need to
		if (needToRun == true) {
			if (list != NULL) { delete list; }
			list = input.getListVector(lastLabel); //get new list vector to process

			LOG(INFO) << list->getLabel() << '\n';
			splitList(list);

			delete list;
		}

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }	return 0; }

	}
	else if (namefile != "") { //you are using the namefile to determine abundance
		if (outputDir == "") { outputDir = File::getPath(namefile); }

		splitNames();
		writeNames();

		string tag = "";
		if (groupfile != "") { parseGroup(tag); }
		if (accnos) { writeAccnos(tag); }
		if (fastafile != "") { parseFasta(tag); }
	}
	else {
		//split by countfile
		string tag = "";
		splitCount();

		if (accnos) { writeAccnos(tag); }
		if (fastafile != "") { parseFasta(tag); }
	}

	//set fasta file as new current fastafile
	string current = "";
	itTypes = outputTypes.find("fasta");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("fasta", current); }
	}

	itTypes = outputTypes.find("name");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("name", current); }
	}

	itTypes = outputTypes.find("group");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("group", current); }
	}

	itTypes = outputTypes.find("list");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("list", current); }
	}

	itTypes = outputTypes.find("accnos");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("accnos", current); }
	}

	itTypes = outputTypes.find("count");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
/**********************************************************************************************************************/
int SplitAbundCommand::splitList(ListVector* thisList) {
	rareNames.clear();
	abundNames.clear();

	//get rareNames and abundNames
	int numRareBins = 0;
	for (int i = 0; i < thisList->getNumBins(); i++) {
		if (ctrlc_pressed) { return 0; }

		string bin = thisList->get(i);

		vector<string> names;
		m->splitAtComma(bin, names);  //parses bin into individual sequence names
		int size = names.size();

		//if countfile is not blank we assume the list file is unique, otherwise we assume it includes all seqs
		if (countfile != "") {
			size = 0;
			for (int j = 0; j < names.size(); j++) { size += ct.getNumSeqs(names[j]); }
		}

		if (size <= cutoff) {
			numRareBins++;
			for (int j = 0; j < names.size(); j++) { rareNames.insert(names[j]); }
		}
		else {
			for (int j = 0; j < names.size(); j++) { abundNames.insert(names[j]); }
		}
	}//end for


	string tag = thisList->getLabel();

	writeList(thisList, tag, numRareBins);

	if (groupfile != "") { parseGroup(tag); }
	if (accnos) { writeAccnos(tag); }
	if (fastafile != "") { parseFasta(tag); }
	if (countfile != "") { parseCount(tag); }

	return 0;

}
/**********************************************************************************************************************/
int SplitAbundCommand::writeList(ListVector* thisList, string tag, int numRareBins) {
	try {

		map<string, ofstream*> filehandles;

		if (Groups.size() == 0) {
			int numAbundBins = thisList->getNumBins() - numRareBins;

			ofstream aout;
			ofstream rout;

			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(listfile));
			variables["[tag]"] = tag;
			variables["[tag2]"] = "rare";
			string rare = getOutputFileName("list", variables);
			File::openOutputFile(rare + ".temp", rout);
			outputNames.push_back(rare); outputTypes["list"].push_back(rare);

			variables["[tag2]"] = "abund";
			string abund = getOutputFileName("list", variables);
			File::openOutputFile(abund + ".temp", aout);
			outputNames.push_back(abund); outputTypes["list"].push_back(abund);

			if (rareNames.size() != 0) { rout << thisList->getLabel() << '\t' << numRareBins; }
			if (abundNames.size() != 0) { aout << thisList->getLabel() << '\t' << numAbundBins; }

			vector<string> binLabels = thisList->getLabels();
			string rareHeader = "label\tnumOtus"; string abundHeader = "label\tnumOtus";
			for (int i = 0; i < thisList->getNumBins(); i++) {
				if (ctrlc_pressed) { break; }

				string bin = thisList->get(i);
				vector<string> names;
				m->splitAtComma(bin, names);

				int size = names.size();
				if (countfile != "") {
					size = 0;
					for (int j = 0; j < names.size(); j++) { size += ct.getNumSeqs(names[j]); }
				}

				if (size <= cutoff) { rout << '\t' << bin;  rareHeader += '\t' + binLabels[i]; }
				else { aout << '\t' << bin;  abundHeader += '\t' + binLabels[i]; }
			}

			if (rareNames.size() != 0) { rout << endl; }
			if (abundNames.size() != 0) { aout << endl; }

			rout.close();
			aout.close();

			//add headers
			ofstream r;
			File::openOutputFile(rare, r);
			r << rareHeader << endl;
			r.close();
			File::appendFiles(rare + ".temp", rare);
			File::remove(rare + ".temp");

			ofstream a;
			File::openOutputFile(abund, a);
			a << abundHeader << endl;
			a.close();
			File::appendFiles(abund + ".temp", abund);
			File::remove(abund + ".temp");

		}
		else { //parse names by abundance and group
			string fileroot = outputDir + File::getRootName(File::getSimpleName(listfile));
			ofstream* temp;
			ofstream* temp2;
			//map<string, bool> wroteFile;
			map<string, ofstream*> filehandles;
			map<string, ofstream*>::iterator it3;

			for (int i = 0; i < Groups.size(); i++) {
				temp = new ofstream;
				filehandles[Groups[i] + ".rare"] = temp;
				temp2 = new ofstream;
				filehandles[Groups[i] + ".abund"] = temp2;

				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[tag]"] = tag;
				variables["[tag2]"] = "rare";
				variables["[group]"] = Groups[i];
				string rareGroupFileName = getOutputFileName("list", variables);
				variables["[tag2]"] = "abund";
				string abundGroupFileName = getOutputFileName("list", variables);
				File::openOutputFile(rareGroupFileName, *(filehandles[Groups[i] + ".rare"]));
				File::openOutputFile(abundGroupFileName, *(filehandles[Groups[i] + ".abund"]));
				outputNames.push_back(rareGroupFileName); outputTypes["list"].push_back(rareGroupFileName);
				outputNames.push_back(abundGroupFileName); outputTypes["list"].push_back(abundGroupFileName);
			}

			map<string, string> groupVector;
			map<string, string> groupLabels;
			map<string, string>::iterator itGroup;
			map<string, int> groupNumBins;

			for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
				groupNumBins[it3->first] = 0;
				groupVector[it3->first] = "";
				groupLabels[it3->first] = "label\tnumOtus";
			}
			vector<string> binLabels = thisList->getLabels();
			for (int i = 0; i < thisList->getNumBins(); i++) {
				if (ctrlc_pressed) { break; }

				map<string, string> groupBins;
				string bin = thisList->get(i);

				vector<string> names;
				m->splitAtComma(bin, names);  //parses bin into individual sequence names

				//parse bin into list of sequences in each group
				for (int j = 0; j < names.size(); j++) {
					string rareAbund;
					if (rareNames.count(names[j]) != 0) { //you are a rare name
						rareAbund = ".rare";
					}
					else { //you are a abund name
						rareAbund = ".abund";
					}

					if (countfile == "") {
						string group = groupMap.getGroup(names[j]);

						if (m->inUsersGroups(group, Groups)) { //only add if this is in a group we want
							itGroup = groupBins.find(group + rareAbund);
							if (itGroup == groupBins.end()) {
								groupBins[group + rareAbund] = names[j];  //add first name
								groupNumBins[group + rareAbund]++;
							}
							else { //add another name
								groupBins[group + rareAbund] += "," + names[j];
							}
						}
						else if (group == "not found") {
							LOG(INFO) << names[j] + " is not in your groupfile. Ignoring." << '\n';
						}
					}
					else {
						vector<string> thisSeqsGroups = ct.getGroups(names[j]);
						for (int k = 0; k < thisSeqsGroups.size(); k++) {
							if (m->inUsersGroups(thisSeqsGroups[k], Groups)) { //only add if this is in a group we want
								itGroup = groupBins.find(thisSeqsGroups[k] + rareAbund);
								if (itGroup == groupBins.end()) {
									groupBins[thisSeqsGroups[k] + rareAbund] = names[j];  //add first name
									groupNumBins[thisSeqsGroups[k] + rareAbund]++;
								}
								else { //add another name
									groupBins[thisSeqsGroups[k] + rareAbund] += "," + names[j];
								}
							}
						}
					}
				}


				for (itGroup = groupBins.begin(); itGroup != groupBins.end(); itGroup++) {
					groupVector[itGroup->first] += '\t' + itGroup->second;
					groupLabels[itGroup->first] += '\t' + binLabels[i];
				}
			}

			//end list vector
			for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
				(*(filehandles[it3->first])) << groupLabels[it3->first] << endl;
				(*(filehandles[it3->first])) << thisList->getLabel() << '\t' << groupNumBins[it3->first] << groupVector[it3->first] << endl;  // label numBins  listvector for that group
				(*(filehandles[it3->first])).close();
				delete it3->second;
			}
		}

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, writeList";
		exit(1);
	}
}
/**********************************************************************************************************************/
int SplitAbundCommand::splitCount() { //countfile
	try {
		rareNames.clear();
		abundNames.clear();

		vector<string> allNames = ct.getNamesOfSeqs();
		for (int i = 0; i < allNames.size(); i++) {

			if (ctrlc_pressed) { return 0; }

			int size = ct.getNumSeqs(allNames[i]);
			nameMap[allNames[i]] = allNames[i];

			if (size <= cutoff) {
				rareNames.insert(allNames[i]);
			}
			else {
				abundNames.insert(allNames[i]);
			}
		}

		//write out split count files
		parseCount("");

		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, splitCount";
		exit(1);
	}
}
/**********************************************************************************************************************/
int SplitAbundCommand::splitNames() { //namefile
	try {

		rareNames.clear();
		abundNames.clear();

		//open input file
		ifstream in;
		File::openInputFile(namefile, in);

		while (!in.eof()) {
			if (ctrlc_pressed) { break; }

			string firstCol, secondCol;
			in >> firstCol >> secondCol; File::gobble(in);

			nameMap[firstCol] = secondCol;

			int size = m->getNumNames(secondCol);

			if (size <= cutoff) {
				rareNames.insert(firstCol);
			}
			else {
				abundNames.insert(firstCol);
			}
		}
		in.close();

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, splitNames";
		exit(1);
	}
}
/**********************************************************************************************************************/
int SplitAbundCommand::readNamesFile() {
	try {
		//open input file
		ifstream in;
		File::openInputFile(namefile, in);

		while (!in.eof()) {
			if (ctrlc_pressed) { break; }

			string firstCol, secondCol;
			in >> firstCol >> secondCol; File::gobble(in);

			nameMap[firstCol] = secondCol;
		}
		in.close();

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, readNamesFile";
		exit(1);
	}
}
/**********************************************************************************************************************/
int SplitAbundCommand::createNameMap(ListVector* thisList) {

	if (thisList != NULL) {
		for (int i = 0; i < thisList->getNumBins(); i++) {
			if (ctrlc_pressed) { return 0; }

			string bin = thisList->get(i);

			vector<string> names;
			m->splitAtComma(bin, names);  //parses bin into individual sequence names

			for (int j = 0; j < names.size(); j++) { nameMap[names[j]] = names[j]; }
		}//end for
	}

	return 0;
}
/**********************************************************************************************************************/
int SplitAbundCommand::parseCount(string tag) { //namefile
	try {

		map<string, ofstream*> filehandles;

		if (Groups.size() == 0) {
			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(countfile));
			variables["[tag]"] = tag;
			variables["[tag2]"] = "rare";
			string rare = getOutputFileName("count", variables);
			outputNames.push_back(rare); outputTypes["count"].push_back(rare);
			variables["[tag2]"] = "abund";
			string abund = getOutputFileName("count", variables);
			outputNames.push_back(abund); outputTypes["count"].push_back(abund);

			CountTable rareTable;
			CountTable abundTable;
			if (ct.hasGroupInfo()) {
				vector<string> ctGroups = ct.getNamesOfGroups();
				for (int i = 0; i < ctGroups.size(); i++) { rareTable.addGroup(ctGroups[i]);  abundTable.addGroup(ctGroups[i]); }
			}

			if (rareNames.size() != 0) {
				for (set<string>::iterator itRare = rareNames.begin(); itRare != rareNames.end(); itRare++) {
					if (ct.hasGroupInfo()) {
						vector<int> groupCounts = ct.getGroupCounts(*itRare);
						rareTable.push_back(*itRare, groupCounts);
					}
					else {
						int groupCounts = ct.getNumSeqs(*itRare);
						rareTable.push_back(*itRare, groupCounts);
					}
				}
				if (rareTable.hasGroupInfo()) {
					vector<string> ctGroups = rareTable.getNamesOfGroups();
					for (int i = 0; i < ctGroups.size(); i++) {
						if (rareTable.getGroupCount(ctGroups[i]) == 0) { rareTable.removeGroup(ctGroups[i]); }
					}
				}
				rareTable.printTable(rare);
			}


			if (abundNames.size() != 0) {
				for (set<string>::iterator itAbund = abundNames.begin(); itAbund != abundNames.end(); itAbund++) {
					if (ct.hasGroupInfo()) {
						vector<int> groupCounts = ct.getGroupCounts(*itAbund);
						abundTable.push_back(*itAbund, groupCounts);
					}
					else {
						int groupCounts = ct.getNumSeqs(*itAbund);
						abundTable.push_back(*itAbund, groupCounts);
					}
				}
				if (abundTable.hasGroupInfo()) {
					vector<string> ctGroups = abundTable.getNamesOfGroups();
					for (int i = 0; i < ctGroups.size(); i++) {
						if (abundTable.getGroupCount(ctGroups[i]) == 0) { abundTable.removeGroup(ctGroups[i]); }
					}
				}
				abundTable.printTable(abund);
			}

		}
		else { //parse names by abundance and group
			map<string, CountTable*> countTableMap;
			map<string, CountTable*>::iterator it3;

			for (int i = 0; i < Groups.size(); i++) {
				CountTable* rareCt = new CountTable();
				rareCt->addGroup(Groups[i]);
				countTableMap[Groups[i] + ".rare"] = rareCt;
				CountTable* abundCt = new CountTable();
				abundCt->addGroup(Groups[i]);
				countTableMap[Groups[i] + ".abund"] = abundCt;
			}

			vector<string> allNames = ct.getNamesOfSeqs();
			for (int i = 0; i < allNames.size(); i++) {
				string rareAbund;
				if (rareNames.count(allNames[i]) != 0) { //you are a rare name
					rareAbund = ".rare";
				}
				else { //you are a abund name
					rareAbund = ".abund";
				}

				vector<string> thisSeqsGroups = ct.getGroups(allNames[i]);
				for (int j = 0; j < thisSeqsGroups.size(); j++) {
					if (m->inUsersGroups(thisSeqsGroups[j], Groups)) { //only add if this is in a group we want
						int num = ct.getGroupCount(allNames[i], thisSeqsGroups[j]);
						vector<int> nums; nums.push_back(num);
						countTableMap[thisSeqsGroups[j] + rareAbund]->push_back(allNames[i], nums);
					}
				}
			}


			for (it3 = countTableMap.begin(); it3 != countTableMap.end(); it3++) {
				string fileroot = outputDir + File::getRootName(File::getSimpleName(countfile));
				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[tag]"] = it3->first;
				string filename = getOutputFileName("count", variables);
				outputNames.push_back(filename);  outputTypes["count"].push_back(filename);
				(it3->second)->printTable(filename);
				delete it3->second;
			}
		}

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, parseCount";
		exit(1);
	}
}
/**********************************************************************************************************************/
int SplitAbundCommand::writeNames() { //namefile
	try {

		map<string, ofstream*> filehandles;

		if (Groups.size() == 0) {
			ofstream aout;
			ofstream rout;

			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(namefile));
			variables["[tag]"] = "rare";
			string rare = getOutputFileName("name", variables);
			File::openOutputFile(rare, rout);
			outputNames.push_back(rare); outputTypes["name"].push_back(rare);

			variables["[tag]"] = "abund";
			string abund = getOutputFileName("name", variables);
			File::openOutputFile(abund, aout);
			outputNames.push_back(abund); outputTypes["name"].push_back(abund);

			if (rareNames.size() != 0) {
				for (set<string>::iterator itRare = rareNames.begin(); itRare != rareNames.end(); itRare++) {
					rout << (*itRare) << '\t' << nameMap[(*itRare)] << endl;
				}
			}
			rout.close();

			if (abundNames.size() != 0) {
				for (set<string>::iterator itAbund = abundNames.begin(); itAbund != abundNames.end(); itAbund++) {
					aout << (*itAbund) << '\t' << nameMap[(*itAbund)] << endl;
				}
			}
			aout.close();

		}
		else { //parse names by abundance and group
			string fileroot = outputDir + File::getRootName(File::getSimpleName(namefile));
			ofstream* temp;
			ofstream* temp2;
			map<string, ofstream*> filehandles;
			map<string, ofstream*>::iterator it3;

			for (int i = 0; i < Groups.size(); i++) {
				temp = new ofstream;
				filehandles[Groups[i] + ".rare"] = temp;
				temp2 = new ofstream;
				filehandles[Groups[i] + ".abund"] = temp2;

				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[tag]"] = "rare";
				variables["[group]"] = Groups[i];
				string rareGroupFileName = getOutputFileName("name", variables);
				variables["[tag]"] = "abund";
				string abundGroupFileName = getOutputFileName("name", variables);
				File::openOutputFile(rareGroupFileName, *(filehandles[Groups[i] + ".rare"]));
				File::openOutputFile(abundGroupFileName, *(filehandles[Groups[i] + ".abund"]));
			}

			for (map<string, string>::iterator itName = nameMap.begin(); itName != nameMap.end(); itName++) {
				vector<string> names;
				m->splitAtComma(itName->second, names);  //parses bin into individual sequence names

				string rareAbund;
				if (rareNames.count(itName->first) != 0) { //you are a rare name
					rareAbund = ".rare";
				}
				else { //you are a abund name
					rareAbund = ".abund";
				}

				map<string, string> outputStrings;
				map<string, string>::iterator itout;
				for (int i = 0; i < names.size(); i++) {

					string group = groupMap.getGroup(names[i]);

					if (m->inUsersGroups(group, Groups)) { //only add if this is in a group we want
						itout = outputStrings.find(group + rareAbund);
						if (itout == outputStrings.end()) {
							outputStrings[group + rareAbund] = names[i] + '\t' + names[i];
						}
						else { outputStrings[group + rareAbund] += "," + names[i]; }
					}
					else if (group == "not found") {
						LOG(INFO) << names[i] + " is not in your groupfile. Ignoring." << '\n';
					}
				}

				for (itout = outputStrings.begin(); itout != outputStrings.end(); itout++) { *(filehandles[itout->first]) << itout->second << endl; }
			}


			for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
				(*(filehandles[it3->first])).close();
				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[tag]"] = it3->first;
				outputNames.push_back(getOutputFileName("name", variables));  outputTypes["name"].push_back(getOutputFileName("name", variables));
				delete it3->second;
			}
		}

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, writeNames";
		exit(1);
	}
}
/**********************************************************************************************************************/
//just write the unique names - if a namesfile is given
int SplitAbundCommand::writeAccnos(string tag) {
	try {

		map<string, ofstream*> filehandles;

		if (Groups.size() == 0) {
			ofstream aout;
			ofstream rout;

			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputFile));
			variables["[tag]"] = tag;
			variables["[tag2]"] = "rare";
			string rare = getOutputFileName("accnos", variables);
			File::openOutputFile(rare, rout);
			outputNames.push_back(rare); outputTypes["accnos"].push_back(rare);

			for (set<string>::iterator itRare = rareNames.begin(); itRare != rareNames.end(); itRare++) {
				rout << (*itRare) << endl;
			}
			rout.close();

			variables["[tag2]"] = "abund";
			string abund = getOutputFileName("accnos", variables);
			File::openOutputFile(abund, aout);
			outputNames.push_back(abund); outputTypes["accnos"].push_back(abund);

			for (set<string>::iterator itAbund = abundNames.begin(); itAbund != abundNames.end(); itAbund++) {
				aout << (*itAbund) << endl;
			}
			aout.close();

		}
		else { //parse names by abundance and group
			string fileroot = outputDir + File::getRootName(File::getSimpleName(inputFile));
			ofstream* temp;
			ofstream* temp2;
			map<string, ofstream*> filehandles;
			map<string, ofstream*>::iterator it3;

			for (int i = 0; i < Groups.size(); i++) {
				temp = new ofstream;
				filehandles[Groups[i] + ".rare"] = temp;
				temp2 = new ofstream;
				filehandles[Groups[i] + ".abund"] = temp2;

				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[tag]"] = tag;
				variables["[tag2]"] = "rare";
				variables["[group]"] = Groups[i];
				File::openOutputFile(getOutputFileName("accnos", variables), *(filehandles[Groups[i] + ".rare"]));
				variables["[tag2]"] = "abund";
				File::openOutputFile(getOutputFileName("accnos", variables), *(filehandles[Groups[i] + ".abund"]));
			}

			//write rare
			for (set<string>::iterator itRare = rareNames.begin(); itRare != rareNames.end(); itRare++) {
				string group = groupMap.getGroup(*itRare);

				if (m->inUsersGroups(group, Groups)) { //only add if this is in a group we want
					*(filehandles[group + ".rare"]) << *itRare << endl;
				}
			}

			//write abund	
			for (set<string>::iterator itAbund = abundNames.begin(); itAbund != abundNames.end(); itAbund++) {
				string group = groupMap.getGroup(*itAbund);

				if (m->inUsersGroups(group, Groups)) { //only add if this is in a group we want
					*(filehandles[group + ".abund"]) << *itAbund << endl;
				}
			}

			//close files
			for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
				(*(filehandles[it3->first])).close();
				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[tag]"] = tag;
				variables["[tag2]"] = it3->first;
				outputNames.push_back(getOutputFileName("accnos", variables));  outputTypes["accnos"].push_back(getOutputFileName("accnos", variables));
				delete it3->second;
			}
		}

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, writeAccnos";
		exit(1);
	}
}
/**********************************************************************************************************************/
int SplitAbundCommand::parseGroup(string tag) { //namefile
	try {

		map<string, ofstream*> filehandles;

		if (Groups.size() == 0) {
			ofstream aout;
			ofstream rout;

			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(groupfile));
			variables["[tag]"] = tag;
			variables["[tag2]"] = "rare";
			string rare = getOutputFileName("group", variables);
			File::openOutputFile(rare, rout);
			outputNames.push_back(rare); outputTypes["group"].push_back(rare);

			variables["[tag2]"] = "abund";
			string abund = getOutputFileName("group", variables);
			;
			File::openOutputFile(abund, aout);
			outputNames.push_back(abund); outputTypes["group"].push_back(abund);

			for (map<string, string>::iterator itName = nameMap.begin(); itName != nameMap.end(); itName++) {
				vector<string> names;
				m->splitAtComma(itName->second, names);  //parses bin into individual sequence names

				for (int i = 0; i < names.size(); i++) {

					string group = groupMap.getGroup(names[i]);

					if (group == "not found") {
						LOG(INFO) << names[i] + " is not in your groupfile, ignoring, please correct." << '\n';
					}
					else {
						if (rareNames.count(itName->first) != 0) { //you are a rare name
							rout << names[i] << '\t' << group << endl;
						}
						else { //you are a abund name
							aout << names[i] << '\t' << group << endl;
						}
					}
				}
			}

			rout.close();
			aout.close();

		}
		else { //parse names by abundance and group
			string fileroot = outputDir + File::getRootName(File::getSimpleName(groupfile));
			ofstream* temp;
			ofstream* temp2;
			map<string, ofstream*> filehandles;
			map<string, ofstream*>::iterator it3;

			for (int i = 0; i < Groups.size(); i++) {
				temp = new ofstream;
				filehandles[Groups[i] + ".rare"] = temp;
				temp2 = new ofstream;
				filehandles[Groups[i] + ".abund"] = temp2;

				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[tag]"] = tag;
				variables["[tag2]"] = "rare";
				variables["[group]"] = Groups[i];
				File::openOutputFile(getOutputFileName("group", variables), *(filehandles[Groups[i] + ".rare"]));
				variables["[tag2]"] = "abund";
				File::openOutputFile(getOutputFileName("group", variables), *(filehandles[Groups[i] + ".abund"]));
			}

			for (map<string, string>::iterator itName = nameMap.begin(); itName != nameMap.end(); itName++) {
				vector<string> names;
				m->splitAtComma(itName->second, names);  //parses bin into individual sequence names

				string rareAbund;
				if (rareNames.count(itName->first) != 0) { //you are a rare name
					rareAbund = ".rare";
				}
				else { //you are a abund name
					rareAbund = ".abund";
				}

				for (int i = 0; i < names.size(); i++) {

					string group = groupMap.getGroup(names[i]);

					if (m->inUsersGroups(group, Groups)) { //only add if this is in a group we want
						*(filehandles[group + rareAbund]) << names[i] << '\t' << group << endl;
					}
				}
			}

			for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
				(*(filehandles[it3->first])).close();
				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[tag]"] = tag;
				variables["[tag2]"] = it3->first;
				outputNames.push_back(getOutputFileName("group", variables));  outputTypes["group"].push_back(getOutputFileName("group", variables));
				delete it3->second;
			}
		}

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, parseGroups";
		exit(1);
	}
}
/**********************************************************************************************************************/
int SplitAbundCommand::parseFasta(string tag) { //namefile
	try {

		map<string, ofstream*> filehandles;

		if (Groups.size() == 0) {
			ofstream aout;
			ofstream rout;

			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastafile));
			variables["[tag]"] = tag;
			variables["[tag2]"] = "rare";
			string rare = getOutputFileName("fasta", variables);
			File::openOutputFile(rare, rout);
			outputNames.push_back(rare); outputTypes["fasta"].push_back(rare);

			variables["[tag2]"] = "abund";
			string abund = getOutputFileName("fasta", variables);
			File::openOutputFile(abund, aout);
			outputNames.push_back(abund); outputTypes["fasta"].push_back(abund);

			//open input file
			ifstream in;
			File::openInputFile(fastafile, in);

			while (!in.eof()) {
				if (ctrlc_pressed) { break; }

				Sequence seq(in); File::gobble(in);

				if (seq.getName() != "") {

					map<string, string>::iterator itNames;

					itNames = nameMap.find(seq.getName());

					if (itNames == nameMap.end()) {
						LOG(INFO) << seq.getName() + " is not in your names or list file, ignoring." << '\n';
					}
					else {
						if (rareNames.count(seq.getName()) != 0) { //you are a rare name
							seq.printSequence(rout);
						}
						else { //you are a abund name
							seq.printSequence(aout);
						}
					}
				}
			}
			in.close();
			rout.close();
			aout.close();

		}
		else { //parse names by abundance and group
			string fileroot = outputDir + File::getRootName(File::getSimpleName(fastafile));
			ofstream* temp;
			ofstream* temp2;
			map<string, ofstream*> filehandles;
			map<string, ofstream*>::iterator it3;

			for (int i = 0; i < Groups.size(); i++) {
				temp = new ofstream;
				filehandles[Groups[i] + ".rare"] = temp;
				temp2 = new ofstream;
				filehandles[Groups[i] + ".abund"] = temp2;

				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[tag]"] = tag;
				variables["[tag2]"] = "rare";
				variables["[group]"] = Groups[i];
				File::openOutputFile(getOutputFileName("fasta", variables), *(filehandles[Groups[i] + ".rare"]));
				variables["[tag2]"] = "abund";
				File::openOutputFile(getOutputFileName("fasta", variables), *(filehandles[Groups[i] + ".abund"]));
			}

			//open input file
			ifstream in;
			File::openInputFile(fastafile, in);

			while (!in.eof()) {
				if (ctrlc_pressed) { break; }

				Sequence seq(in); File::gobble(in);

				if (seq.getName() != "") {
					map<string, string>::iterator itNames = nameMap.find(seq.getName());

					if (itNames == nameMap.end()) {
						LOG(INFO) << seq.getName() + " is not in your names or list file, ignoring." << '\n';
					}
					else {
						vector<string> names;
						m->splitAtComma(itNames->second, names);  //parses bin into individual sequence names

						string rareAbund;
						if (rareNames.count(itNames->first) != 0) { //you are a rare name
							rareAbund = ".rare";
						}
						else { //you are a abund name
							rareAbund = ".abund";
						}

						if (countfile == "") {
							for (int i = 0; i < names.size(); i++) {
								string group = groupMap.getGroup(seq.getName());

								if (m->inUsersGroups(group, Groups)) { //only add if this is in a group we want
									seq.printSequence(*(filehandles[group + rareAbund]));
								}
								else if (group == "not found") {
									LOG(INFO) << seq.getName() + " is not in your groupfile. Ignoring." << '\n';
								}
							}
						}
						else {
							vector<string> thisSeqsGroups = ct.getGroups(names[0]); //we only need names[0], because there is no namefile
							for (int i = 0; i < thisSeqsGroups.size(); i++) {
								if (m->inUsersGroups(thisSeqsGroups[i], Groups)) { //only add if this is in a group we want
									seq.printSequence(*(filehandles[thisSeqsGroups[i] + rareAbund]));
								}
							}
						}
					}
				}
			}
			in.close();

			for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
				(*(filehandles[it3->first])).close();
				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[tag]"] = tag;
				variables["[tag2]"] = it3->first;
				outputNames.push_back(getOutputFileName("fasta", variables));  outputTypes["fasta"].push_back(getOutputFileName("fasta", variables));
				delete it3->second;
			}
		}

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitAbundCommand, parseFasta";
		exit(1);
	}
}
/**********************************************************************************************************************/

