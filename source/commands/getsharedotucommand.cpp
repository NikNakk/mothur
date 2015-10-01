/*
 *  getsharedotucommand.cpp
 *  Mothur
 *
 *  Created by westcott on 9/22/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "getsharedotucommand.h"
#include "sharedutilities.h"

 //**********************************************************************************************************************
vector<string> GetSharedOTUCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "sharedFasta", "none", "none", "fasta", false, false); parameters.push_back(pfasta);
		CommandParameter pgroup("group", "InputTypes", "", "", "none", "none", "groupList", "", false, false, true); parameters.push_back(pgroup);
		CommandParameter plist("list", "InputTypes", "", "", "sharedList", "sharedList", "groupList", "sharedseq", false, false, true); parameters.push_back(plist);
		CommandParameter pshared("shared", "InputTypes", "", "", "sharedList-sharedFasta", "sharedList", "none", "sharedseq", false, false, true); parameters.push_back(pshared);
		CommandParameter poutput("output", "Multiple", "accnos-default", "default", "", "", "", "", false, false); parameters.push_back(poutput);
		nkParameters.add(new StringParameter("label", "", false, false));
		CommandParameter puniquegroups("uniquegroups", "String", "", "", "", "", "", "", false, false, true); parameters.push_back(puniquegroups);
		CommandParameter psharedgroups("sharedgroups", "String", "", "", "", "", "", "", false, false, true); parameters.push_back(psharedgroups);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetSharedOTUCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetSharedOTUCommand::getHelpString() {
	try {
		string helpString = "The get.sharedseqs command parameters are list, group, shared, label, uniquegroups, sharedgroups, output and fasta.  The list and group or shared parameters are required, unless you have valid current files.\n"
			"The label parameter allows you to select what distance levels you would like output files for, and are separated by dashes.\n"
			"The uniquegroups and sharedgroups parameters allow you to select groups you would like to know the shared info for, and are separated by dashes.\n"
			"If you enter your groups under the uniquegroups parameter mothur will return the otus that contain ONLY sequences from those groups.\n"
			"If you enter your groups under the sharedgroups parameter mothur will return the otus that contain sequences from those groups and may also contain sequences from other groups.\n"
			"If you do not enter any groups then the get.sharedseqs command will return sequences that are unique to all groups in your group or shared file.\n"
			"The fasta parameter allows you to input a fasta file and outputs a fasta file for each distance level containing only the sequences that are in OTUs shared by the groups specified. It can only be used with a list and group file not the shared file input.\n"
			"The output parameter allows you to output the list of names without the group and bin number added. \n"
			"With this option you can use the names file as an input in get.seqs and remove.seqs commands. To do this enter output=accnos. \n"
			"The get.sharedseqs command outputs a .names file for each distance level containing a list of sequences in the OTUs shared by the groups specified.\n"
			"The get.sharedseqs command should be in the following format: get.sharedseqs(list=yourListFile, group=yourGroupFile, label=yourLabels, uniquegroups=yourGroups, fasta=yourFastafile, output=yourOutput).\n"
			"Example get.sharedseqs(list=amazon.fn.list, label=unique-0.01, group=amazon.groups, uniquegroups=forest-pasture, fasta=amazon.fasta, output=accnos).\n"
			"The output to the screen is the distance and the number of otus at that distance for the groups you specified.\n"
			"The default value for label is all labels in your inputfile. The default for groups is all groups in your file.\n"
			"Note: No spaces between parameter labels (i.e. label), '=' and parameters (i.e.yourLabel).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetSharedOTUCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetSharedOTUCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],[distance],[group],shared.fasta"; }
	else if (type == "accnos") { pattern = "[filename],[distance],[group],accnos"; }
	else if (type == "sharedseqs") { pattern = "[filename],[distance],[group],shared.seqs"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
GetSharedOTUCommand::GetSharedOTUCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["sharedseqs"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetSharedOTUCommand, GetSharedOTUCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
GetSharedOTUCommand::GetSharedOTUCommand(Settings& settings, string option) : Command(settings, option) {

	abort = false; calledHelp = false;
	unique = true;
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
		outputTypes["fasta"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["sharedseqs"] = tempOutNames;

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("fasta");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fasta"] = inputDir + it->second; }
			}

			it = parameters.find("list");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["list"] = inputDir + it->second; }
			}

			it = parameters.find("shared");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["shared"] = inputDir + it->second; }
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
		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { format = "list"; 	settings.setCurrent("list", listfile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { settings.setCurrent("shared", sharedfile); }

		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not open") { abort = true; }
		else if (fastafile == "not found") { fastafile = ""; }
		else { settings.setCurrent("fasta", fastafile); }


		if ((sharedfile == "") && (listfile == "")) { //look for currents
			//is there are current file available for either of these?
			//give priority to shared, then list
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else {
				listfile = settings.getCurrent("list");
				if (listfile != "") { LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
				else {
					LOG(INFO) << "No valid current files. You must provide a shared or list file." << '\n';
					abort = true;
				}
			}
		}
		else if ((sharedfile != "") && (listfile != "")) {
			LOG(INFO) << "You may enter ONLY ONE of the following: shared or list." << '\n'; abort = true;
		}

		if (listfile != "") {
			if (groupfile == "") {
				groupfile = settings.getCurrent("group");
				if (groupfile != "") { LOG(INFO) << "Using " + groupfile + " as input file for the group parameter." << '\n'; }
				else {
					LOG(INFO) << "You need to provide a group file if you are going to use the list format." << '\n'; abort = true;
				}
			}
		}

		if ((sharedfile != "") && (fastafile != "")) { LOG(INFO) << "You cannot use the fasta file with the shared file." << '\n'; abort = true; }

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		output = validParameter.validFile(parameters, "output", false);
		if (output == "not found") { output = ""; }
		else if (output == "default") { output = ""; }

		groups = validParameter.validFile(parameters, "uniquegroups", false);
		if (groups == "not found") { groups = ""; }
		else {
			userGroups = "unique." + groups;
			Utility::split(groups, '-', Groups);
			if (Groups.size() > 4) { userGroups = "unique.selected_groups"; } //if too many groups then the filename becomes too big.
		}

		groups = validParameter.validFile(parameters, "sharedgroups", false);
		if (groups == "not found") { groups = ""; }
		else {
			userGroups = groups;
			Utility::split(groups, '-', Groups);
			if (Groups.size() > 4) { userGroups = "selected_groups"; } //if too many groups then the filename becomes too big.
			unique = false;
		}

	}

}
//**********************************************************************************************************************

int GetSharedOTUCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	if (sharedfile != "") { runShared(); }
	else {
		m->setGroups(Groups);
		groupMap = new GroupMap(groupfile);
		int error = groupMap->readMap();
		if (error == 1) { delete groupMap; return 0; }

		if (ctrlc_pressed) { delete groupMap; return 0; }

		if (Groups.size() == 0) {
			Groups = groupMap->getNamesOfGroups();

			//make string for outputfile name
			userGroups = "unique.";
			for (int i = 0; i < Groups.size(); i++) { userGroups += Groups[i] + "-"; }
			userGroups = userGroups.substr(0, userGroups.length() - 1);
			if (Groups.size() > 4) { userGroups = "unique.selected_groups"; } //if too many groups then the filename becomes too big.
		}
		else {
			//sanity check for group names
			SharedUtil util;
			vector<string> namesOfGroups = groupMap->getNamesOfGroups();
			util.setGroups(Groups, namesOfGroups);
			groupMap->setNamesOfGroups(namesOfGroups);
		}

		//put groups in map to find easier
		for (int i = 0; i < Groups.size(); i++) {
			groupFinder[Groups[i]] = Groups[i];
		}

		if (fastafile != "") {
			ifstream inFasta;
			File::openInputFile(fastafile, inFasta);

			while (!inFasta.eof()) {
				if (ctrlc_pressed) { outputTypes.clear(); inFasta.close(); delete groupMap; return 0; }

				Sequence seq(inFasta); File::gobble(inFasta);
				if (seq.getName() != "") { seqs.push_back(seq); }
			}
			inFasta.close();
		}

		ListVector* lastlist = NULL;
		string lastLabel = "";

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		ifstream in;
		File::openInputFile(listfile, in);

		//as long as you are not at the end of the file or done wih the lines you want
		while ((!in.eof()) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) {
				if (lastlist != NULL) { delete lastlist; }
				for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  outputTypes.clear();
				delete groupMap; return 0;
			}

			list = new ListVector(in);

			if (allLines == 1 || labels.count(list->getLabel()) == 1) {
				LOG(INFO) << list->getLabel();
				process(list);

				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());
			}

			if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = list->getLabel();

				LOG(INFO) << lastlist->getLabel();
				process(lastlist);

				processedLabels.insert(lastlist->getLabel());
				userLabels.erase(lastlist->getLabel());

				//restore real lastlabel to save below
				list->setLabel(saveLabel);
			}

			lastLabel = list->getLabel();

			if (lastlist != NULL) { delete lastlist; }
			lastlist = list;
		}

		in.close();

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
			LOG(INFO) << lastlist->getLabel();
			process(lastlist);

			processedLabels.insert(lastlist->getLabel());
			userLabels.erase(lastlist->getLabel());
		}


		//reset groups parameter
		m->clearGroups();

		if (lastlist != NULL) { delete lastlist; }

		if (ctrlc_pressed) { outputTypes.clear(); for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  delete groupMap; return 0; }
	}
	//set fasta file as new current fastafile
	string current = "";
	itTypes = outputTypes.find("fasta");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("fasta", current); }
	}

	if (output == "accnos") {
		itTypes = outputTypes.find("accnos");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("accnos", current); }
		}
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";


	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in GetSharedOTUCommand, execute";
	exit(1);
}
}
/***********************************************************/
int GetSharedOTUCommand::process(ListVector* shared) {
	try {

		map<string, string> fastaMap;

		ofstream outNames;
		string outputFileNames;

		if (outputDir == "") { outputDir += File::getPath(listfile); }
		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(listfile));
		variables["[distance]"] = shared->getLabel();
		variables["[group]"] = userGroups;
		if (output != "accnos") { outputFileNames = getOutputFileName("sharedseqs", variables); }
		else { outputFileNames = getOutputFileName("accnos", variables); }

		File::openOutputFile(outputFileNames, outNames);

		bool wroteSomething = false;
		int num = 0;

		//go through each bin, find out if shared
		vector<string> binLabels = shared->getLabels();
		for (int i = 0; i < shared->getNumBins(); i++) {
			if (ctrlc_pressed) { outNames.close(); File::remove(outputFileNames); return 0; }

			bool uniqueOTU = true;

			map<string, int> atLeastOne;
			for (int f = 0; f < Groups.size(); f++) {
				atLeastOne[Groups[f]] = 0;
			}

			vector<string> namesOfSeqsInThisBin;

			string names = shared->get(i);
			vector<string> binNames;
			m->splitAtComma(names, binNames);
			for (int j = 0; j < binNames.size(); j++) {
				string name = binNames[j];

				//find group
				string seqGroup = groupMap->getGroup(name);
				if (output != "accnos") {
					namesOfSeqsInThisBin.push_back((name + "|" + seqGroup + "|" + binLabels[i]));
				}
				else { namesOfSeqsInThisBin.push_back(name); }

				if (seqGroup == "not found") { LOG(INFO) << name + " is not in your groupfile. Please correct." << '\n'; exit(1); }

				//is this seq in one of hte groups we care about
				it = groupFinder.find(seqGroup);
				if (it == groupFinder.end()) { uniqueOTU = false; } //you have a sequence from a group you don't want
				else { atLeastOne[seqGroup]++; }
			}

			//make sure you have at least one seq from each group you want
			bool sharedByAll = true;
			map<string, int>::iterator it2;
			for (it2 = atLeastOne.begin(); it2 != atLeastOne.end(); it2++) {
				if (it2->second == 0) { sharedByAll = false; }
			}

			//if the user wants unique bins and this is unique then print
			//or this the user wants shared bins and this bin is shared then print
			if ((unique && uniqueOTU && sharedByAll) || (!unique && sharedByAll)) {

				wroteSomething = true;
				num++;

				//output list of names 
				for (int j = 0; j < namesOfSeqsInThisBin.size(); j++) {
					outNames << namesOfSeqsInThisBin[j] << endl;

					if (fastafile != "") {
						if (output != "accnos") {
							string seqName = namesOfSeqsInThisBin[j].substr(0, namesOfSeqsInThisBin[j].find_last_of('|'));
							seqName = seqName.substr(0, seqName.find_last_of('|'));
							fastaMap[seqName] = namesOfSeqsInThisBin[j];  //fastaMap needs to contain just the seq name for output later
						}
						else {
							fastaMap[namesOfSeqsInThisBin[j]] = namesOfSeqsInThisBin[j];
						}
					}
				}
			}
		}

		outNames.close();

		if (!wroteSomething) {
			File::remove(outputFileNames);
			string outputString = "\t" + toString(num) + " - No otus shared by groups";

			string groupString = "";
			for (int h = 0; h < Groups.size(); h++) {
				groupString += "  " + Groups[h];
			}

			outputString += groupString + ".";
			LOG(INFO) << outputString << '\n';
		}
		else {
			LOG(INFO) << "\t" + toString(num) << '\n';
			outputNames.push_back(outputFileNames);
			if (output != "accnos") { outputTypes["sharedseqs"].push_back(outputFileNames); }
			else { outputTypes["accnos"].push_back(outputFileNames); }
		}

		//if fasta file provided output new fasta file
		if ((fastafile != "") && wroteSomething) {
			if (outputDir == "") { outputDir += File::getPath(fastafile); }
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastafile));
			string outputFileFasta = getOutputFileName("fasta", variables);
			ofstream outFasta;
			File::openOutputFile(outputFileFasta, outFasta);
			outputNames.push_back(outputFileFasta); outputTypes["fasta"].push_back(outputFileFasta);

			for (int k = 0; k < seqs.size(); k++) {
				if (ctrlc_pressed) { outFasta.close(); return 0; }

				//if this is a sequence we want, output it
				it = fastaMap.find(seqs[k].getName());
				if (it != fastaMap.end()) {

					if (output != "accnos") {
						outFasta << ">" << it->second << endl;
					}
					else {
						outFasta << ">" << it->first << endl;
					}

					outFasta << seqs[k].getAligned() << endl;
				}
			}

			outFasta.close();
		}

		return 0;

	}
	/***********************************************************/
	int GetSharedOTUCommand::runShared() {
		InputData input(sharedfile, "sharedfile");
		vector<SharedRAbundVector*> lookup = input.getSharedRAbundVectors();
		string lastLabel = lookup[0]->getLabel();

		if (Groups.size() == 0) {
			Groups = settings.getGroups();

			//make string for outputfile name
			userGroups = "unique.";
			for (int i = 0; i < Groups.size(); i++) { userGroups += Groups[i] + "-"; }
			userGroups = userGroups.substr(0, userGroups.length() - 1);
			if (Groups.size() > 4) { userGroups = "unique.selected_groups"; } //if too many groups then the filename becomes too big.
		}
		else {
			//sanity check for group names
			SharedUtil util;
			vector<string> allGroups = m->getAllGroups();
			util.setGroups(Groups, allGroups);
		}

		//put groups in map to find easier
		for (int i = 0; i < Groups.size(); i++) {
			groupFinder[Groups[i]] = Groups[i];
		}

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		//as long as you are not at the end of the file or done wih the lines you want
		while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
			if (ctrlc_pressed) {
				outputTypes.clear(); for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }
				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } m->clearGroups(); return 0;
			}


			if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {
				LOG(INFO) << lookup[0]->getLabel();
				process(lookup);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());
			}

			if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = lookup[0]->getLabel();

				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
				lookup = input.getSharedRAbundVectors(lastLabel);

				LOG(INFO) << lookup[0]->getLabel();
				process(lookup);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());

				//restore real lastlabel to save below
				lookup[0]->setLabel(saveLabel);
			}

			lastLabel = lookup[0]->getLabel();

			//get next line to process
			//prevent memory leak
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input.getSharedRAbundVectors();
		}

		if (ctrlc_pressed) {
			outputTypes.clear(); for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } m->clearGroups(); return 0;
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
			lookup = input.getSharedRAbundVectors(lastLabel);

			LOG(INFO) << lookup[0]->getLabel();
			process(lookup);
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		}

		//reset groups parameter
		m->clearGroups();

		return 0;

	}
	/***********************************************************/
	int GetSharedOTUCommand::process(vector<SharedRAbundVector*>& lookup) {

		string outputFileNames;
		if (outputDir == "") { outputDir += File::getPath(sharedfile); }
		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(sharedfile));
		variables["[distance]"] = lookup[0]->getLabel();
		variables["[group]"] = userGroups;
		if (output != "accnos") { outputFileNames = getOutputFileName("sharedseqs", variables); }
		else { outputFileNames = getOutputFileName("accnos", variables); }

		ofstream outNames;
		File::openOutputFile(outputFileNames, outNames);

		bool wroteSomething = false;
		int num = 0;

		//go through each bin, find out if shared
		for (int i = 0; i < lookup[0]->getNumBins(); i++) {
			if (ctrlc_pressed) { outNames.close(); File::remove(outputFileNames); return 0; }

			bool uniqueOTU = true;
			map<string, int> atLeastOne;
			for (int f = 0; f < Groups.size(); f++) { atLeastOne[Groups[f]] = 0; }

			set<string> namesOfGroupsInThisBin;

			for (int j = 0; j < lookup.size(); j++) {
				string seqGroup = lookup[j]->getGroup();
				string name = settings.currentSharedBinLabels[i];

				if (lookup[j]->getAbundance(i) != 0) {
					if (output != "accnos") {
						namesOfGroupsInThisBin.insert(name + "|" + seqGroup + "|" + toString(lookup[j]->getAbundance(i)));
					}
					else { namesOfGroupsInThisBin.insert(name); }

					//is this seq in one of the groups we care about
					it = groupFinder.find(seqGroup);
					if (it == groupFinder.end()) { uniqueOTU = false; } //you have sequences from a group you don't want
					else { atLeastOne[seqGroup]++; }
				}
			}

			//make sure you have at least one seq from each group you want
			bool sharedByAll = true;
			map<string, int>::iterator it2;
			for (it2 = atLeastOne.begin(); it2 != atLeastOne.end(); it2++) {
				if (it2->second == 0) { sharedByAll = false; }
			}

			//if the user wants unique bins and this is unique then print
			//or this the user wants shared bins and this bin is shared then print
			if ((unique && uniqueOTU && sharedByAll) || (!unique && sharedByAll)) {

				wroteSomething = true;
				num++;

				//output list of names
				for (set<string>::iterator itNames = namesOfGroupsInThisBin.begin(); itNames != namesOfGroupsInThisBin.end(); itNames++) {
					outNames << (*itNames) << endl;
				}
			}
		}
		outNames.close();

		if (!wroteSomething) {
			File::remove(outputFileNames);
			string outputString = "\t" + toString(num) + " - No otus shared by groups";

			string groupString = "";
			for (int h = 0; h < Groups.size(); h++) {
				groupString += "  " + Groups[h];
			}

			outputString += groupString + ".";
			LOG(INFO) << outputString << '\n';
		}
		else {
			LOG(INFO) << "\t" + toString(num) << '\n';
			outputNames.push_back(outputFileNames);
			if (output != "accnos") { outputTypes["sharedseqs"].push_back(outputFileNames); }
			else { outputTypes["accnos"].push_back(outputFileNames); }
		}

		return 0;
	}

	//**********************************************************************************************************************
