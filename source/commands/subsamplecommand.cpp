/*
 *  subsamplecommand.cpp
 *  Mothur
 *
 *  Created by westcott on 10/27/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "subsamplecommand.h"
#include "sharedutilities.h"
#include "deconvolutecommand.h"
#include "getseqscommand.h"
#include "subsample.h"

 //**********************************************************************************************************************
vector<string> SubSampleCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "FLSSR", "none", "fasta", false, false, true); parameters.push_back(pfasta);
		nkParameters.add(new NameFileParameter(settings, "name", false, true, "NameCount"));
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "none", "none", "none", "taxonomy", false, false, true); parameters.push_back(ptaxonomy);
		nkParameters.add(new CountFileParameter(settings, "count", false, true, "NameCount-CountGroup"));
		nkParameters.add(new GroupFileParameter(settings, "group", false, true, "CountGroup"));
		CommandParameter plist("list", "InputTypes", "", "", "none", "FLSSR", "none", "list", false, false, true); parameters.push_back(plist);
		CommandParameter pshared("shared", "InputTypes", "", "", "none", "FLSSR", "none", "shared", false, false, true); parameters.push_back(pshared);
		CommandParameter prabund("rabund", "InputTypes", "", "", "none", "FLSSR", "none", "rabund", false, false); parameters.push_back(prabund);
		CommandParameter psabund("sabund", "InputTypes", "", "", "none", "FLSSR", "none", "sabund", false, false); parameters.push_back(psabund);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new StringParameter("groups", "", false, false));
		CommandParameter psize("size", "Number", "", "0", "", "", "", "", false, false, true); parameters.push_back(psize);
		CommandParameter ppersample("persample", "Boolean", "", "F", "", "", "", "", false, false, true); parameters.push_back(ppersample);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SubSampleCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string SubSampleCommand::getHelpString() {
	try {
		string helpString = "The sub.sample command is designed to be used as a way to normalize your data, or create a smaller set from your original set.\n"
			"The sub.sample command parameters are fasta, name, list, group, count, rabund, sabund, shared, taxonomy, groups, size, persample and label.  You must provide a fasta, list, sabund, rabund or shared file as an input file.\n"
			"The namefile is only used with the fasta file, not with the listfile, because the list file should contain all sequences.\n"
			"The groups parameter allows you to specify which of the groups in your groupfile you would like included. The group names are separated by dashes.\n"
			"The label parameter allows you to select what distance levels you would like, and are also separated by dashes.\n"
			"The size parameter allows you indicate the size of your subsample.\n"
			"The persample parameter allows you indicate you want to select subsample of the same size from each of your groups, default=false. It is only used with the list and fasta files if a groupfile is given.\n"
			"persample=false will select a random set of sequences of the size you select, but the number of seqs from each group may differ.\n"
			"The size parameter is not set: with shared file size=number of seqs in smallest sample, with all other files if a groupfile is given and persample=true, then size=number of seqs in smallest sample, otherwise size=10% of number of seqs.\n"
			"The sub.sample command should be in the following format: sub.sample(list=yourListFile, group=yourGroupFile, groups=yourGroups, label=yourLabels).\n"
			"Example sub.sample(list=abrecovery.fn.list, group=abrecovery.groups, groups=B-C, size=20).\n"
			"The default value for groups is all the groups in your groupfile, and all labels in your inputfile will be used.\n"
			"The sub.sample command outputs a .subsample file.\n"
			"Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SubSampleCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string SubSampleCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],subsample,[extension]"; }
	else if (type == "sabund") { pattern = "[filename],subsample,[extension]"; }
	else if (type == "name") { pattern = "[filename],subsample,[extension]"; }
	else if (type == "group") { pattern = "[filename],subsample,[extension]"; }
	else if (type == "count") { pattern = "[filename],subsample,[extension]"; }
	else if (type == "list") { pattern = "[filename],[distance],subsample,[extension]"; }
	else if (type == "taxonomy") { pattern = "[filename],subsample,[extension]"; }
	else if (type == "shared") { pattern = "[filename],[distance],subsample,[extension]"; }
	else if (type == "rabund") { pattern = "[filename],subsample,[extension]"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
SubSampleCommand::SubSampleCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["shared"] = tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["rabund"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["taxonomy"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SubSampleCommand, GetRelAbundCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
SubSampleCommand::SubSampleCommand(Settings& settings, string option) : Command(settings, option) {
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

		//check to make sure all parameters are valid for command
		map<string, string>::iterator it;
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["shared"] = tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["rabund"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["taxonomy"] = tempOutNames;

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

			it = parameters.find("fasta");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fasta"] = inputDir + it->second; }
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

			it = parameters.find("taxonomy");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["taxonomy"] = inputDir + it->second; }
			}
		}

		//check for required parameters
		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { listfile = ""; abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { settings.setCurrent("list", listfile); }

		sabundfile = validParameter.validFile(parameters, "sabund", true);
		if (sabundfile == "not open") { sabundfile = ""; abort = true; }
		else if (sabundfile == "not found") { sabundfile = ""; }
		else { settings.setCurrent("sabund", sabundfile); }

		rabundfile = validParameter.validFile(parameters, "rabund", true);
		if (rabundfile == "not open") { rabundfile = ""; abort = true; }
		else if (rabundfile == "not found") { rabundfile = ""; }
		else { settings.setCurrent("rabund", rabundfile); }

		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not open") { fastafile = ""; abort = true; }
		else if (fastafile == "not found") { fastafile = ""; }
		else { settings.setCurrent("fasta", fastafile); }

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = ""; abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { settings.setCurrent("shared", sharedfile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { namefile = ""; abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { settings.setCurrent("name", namefile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { groupfile = ""; abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		taxonomyfile = validParameter.validFile(parameters, "taxonomy", true);
		if (taxonomyfile == "not open") { taxonomyfile = ""; abort = true; }
		else if (taxonomyfile == "not found") { taxonomyfile = ""; }
		else { settings.setCurrent("taxonomy", taxonomyfile); }

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

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; pickedGroups = false; }
		else {
			pickedGroups = true;
			Utility::split(groups, '-', Groups);
			m->setGroups(Groups);
		}

		string temp = validParameter.validFile(parameters, "size", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, size);

		temp = validParameter.validFile(parameters, "persample", false);		if (temp == "not found") { temp = "f"; }
		persample = m->isTrue(temp);

		if ((groupfile == "") && (countfile == "")) { persample = false; }
		if (countfile != "") {
			if (!ct.hasGroupInfo()) {
				persample = false;
				if (pickedGroups) { LOG(INFO) << "You cannot pick groups without group info in your count file." << '\n'; abort = true; }
			}
		}

		if ((namefile != "") && ((fastafile == "") && (taxonomyfile == ""))) { LOG(INFO) << "You may only use a name file with a fasta file or taxonomy file." << '\n'; abort = true; }

		if ((taxonomyfile != "") && ((fastafile == "") && (listfile == ""))) { LOG(INFO) << "You may only use a taxonomyfile with a fastafile or listfile." << '\n'; abort = true; }


		if ((fastafile == "") && (listfile == "") && (sabundfile == "") && (rabundfile == "") && (sharedfile == "")) {
			LOG(INFO) << "You must provide a fasta, list, sabund, rabund or shared file as an input file." << '\n'; abort = true;
		}

		if (pickedGroups && ((groupfile == "") && (sharedfile == "") && (countfile == ""))) {
			LOG(INFO) << "You cannot pick groups without a valid group, count or shared file." << '\n'; abort = true;
		}

		if (((groupfile != "") || (countfile != "")) && ((fastafile == "") && (listfile == ""))) {
			LOG(INFO) << "Group or count files are only valid with listfile or fastafile." << '\n'; abort = true;
		}

		if (((groupfile != "") || (countfile != "")) && ((fastafile != "") && (listfile != ""))) {
			LOG(INFO) << "A new group or count file can only be made from the subsample of a listfile or fastafile, not both. Please correct." << '\n'; abort = true;
		}

		if (countfile == "") {
			if ((fastafile != "") && (namefile == "")) {
				vector<string> files; files.push_back(fastafile);
				OptionParser::getNameFile(files);
			}
		}
	}

}
//**********************************************************************************************************************

int SubSampleCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	if (sharedfile != "") { getSubSampleShared(); }
	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (listfile != "") { getSubSampleList(); }
	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (rabundfile != "") { getSubSampleRabund(); }
	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (sabundfile != "") { getSubSampleSabund(); }
	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (fastafile != "") { getSubSampleFasta(); }
	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

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

	itTypes = outputTypes.find("shared");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("shared", current); }
	}

	itTypes = outputTypes.find("rabund");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("rabund", current); }
	}

	itTypes = outputTypes.find("sabund");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("sabund", current); }
	}

	itTypes = outputTypes.find("count");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
	}

	itTypes = outputTypes.find("taxonomy");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("taxonomy", current); }
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
//**********************************************************************************************************************
int SubSampleCommand::getSubSampleFasta() {

	if (namefile != "") { readNames(); }	//fills names with all names in namefile.
	else { getNames(); }//no name file, so get list of names to pick from

	GroupMap groupMap;
	if (groupfile != "") {
		groupMap.readMap(groupfile);

		//takes care of user setting groupNames that are invalid or setting groups=all
		SharedUtil util;
		vector<string> namesGroups = groupMap.getNamesOfGroups();
		util.setGroups(Groups, namesGroups);

		//file mismatch quit
		if (names.size() != groupMap.getNumSeqs()) {
			LOG(LOGERROR) << "your fasta file contains " + toString(names.size()) + " sequences, and your groupfile contains " + toString(groupMap.getNumSeqs()) + ", please correct.";
			LOG(INFO) << "";
			return 0;
		}
	}
	else if (countfile != "") {
		if (ct.hasGroupInfo()) {
			SharedUtil util;
			vector<string> namesGroups = ct.getNamesOfGroups();
			util.setGroups(Groups, namesGroups);
		}

		//file mismatch quit
		if (names.size() != ct.getNumUniqueSeqs()) {
			LOG(LOGERROR) << "your fasta file contains " + toString(names.size()) + " sequences, and your count file contains " + toString(ct.getNumUniqueSeqs()) + " unique sequences, please correct.";
			LOG(INFO) << "";
			return 0;
		}
	}

	if (ctrlc_pressed) { return 0; }

	//make sure that if your picked groups size is not too big
	int thisSize = 0;
	if (countfile == "") { thisSize = names.size(); }
	else { thisSize = ct.getNumSeqs(); }  //all seqs not just unique

	if (persample) {
		if (size == 0) { //user has not set size, set size = smallest samples size
			if (countfile == "") { size = groupMap.getNumSeqs(Groups[0]); }
			else { size = ct.getGroupCount(Groups[0]); }

			for (int i = 1; i < Groups.size(); i++) {
				int thisSize = 0;
				if (countfile == "") { thisSize = groupMap.getNumSeqs(Groups[i]); }
				else { thisSize = ct.getGroupCount(Groups[i]); }

				if (thisSize < size) { size = thisSize; }
			}
		}
		else { //make sure size is not too large
			vector<string> newGroups;
			for (int i = 0; i < Groups.size(); i++) {
				int thisSize = 0;
				if (countfile == "") { thisSize = groupMap.getNumSeqs(Groups[i]); }
				else { thisSize = ct.getGroupCount(Groups[i]); }

				if (thisSize >= size) { newGroups.push_back(Groups[i]); }
				else { LOG(INFO) << "You have selected a size that is larger than " + Groups[i] + " number of sequences, removing " + Groups[i] + "." << '\n'; }
			}
			Groups = newGroups;
			if (newGroups.size() == 0) { LOG(LOGERROR) << "all groups removed." << '\n'; ctrlc_pressed = true; }
		}

		LOG(INFO) << "Sampling " + toString(size) + " from each group." << '\n';
	}
	else {
		if (pickedGroups) {
			int total = 0;
			for (int i = 0; i < Groups.size(); i++) {
				if (countfile == "") { total += groupMap.getNumSeqs(Groups[i]); }
				else { total += ct.getGroupCount(Groups[i]); }
			}

			if (size == 0) { //user has not set size, set size = 10% samples size
				size = int(total * 0.10);
			}

			if (total < size) {
				if (size != 0) {
					LOG(INFO) << "Your size is too large for the number of groups you selected. Adjusting to " + toString(int(total * 0.10)) + "." << '\n';
				}
				size = int(total * 0.10);
			}

			LOG(INFO) << "Sampling " + toString(size) + " from " + toString(total) + "." << '\n';
		}

		if (size == 0) { //user has not set size, set size = 10% samples size
			if (countfile == "") { size = int(names.size() * 0.10); }
			else { size = int(ct.getNumSeqs() * 0.10); }
		}


		if (size > thisSize) {
			LOG(INFO) << "Your fasta file only contains " + toString(thisSize) + " sequences. Setting size to " + toString(thisSize) + "." << '\n';
			size = thisSize;
		}

		if (!pickedGroups) { LOG(INFO) << "Sampling " + toString(size) + " from " + toString(thisSize) + "." << '\n'; }

	}
	random_shuffle(names.begin(), names.end());

	set<string> subset; //dont want repeat sequence names added
	if (persample) {
		if (countfile == "") {
			//initialize counts
			map<string, int> groupCounts;
			map<string, int>::iterator itGroupCounts;
			for (int i = 0; i < Groups.size(); i++) { groupCounts[Groups[i]] = 0; }

			for (int j = 0; j < names.size(); j++) {

				if (ctrlc_pressed) { return 0; }

				string group = groupMap.getGroup(names[j]);
				if (group == "not found") { LOG(LOGERROR) << "" + names[j] + " is not in your groupfile. please correct." << '\n'; group = "NOTFOUND"; }
				else {
					itGroupCounts = groupCounts.find(group);
					if (itGroupCounts != groupCounts.end()) {
						if (itGroupCounts->second < size) { subset.insert(names[j]); 	(itGroupCounts->second)++; }
					}
				}
			}
		}
		else {
			SubSample sample;
			CountTable sampledCt = sample.getSample(ct, size, Groups);
			vector<string> sampledSeqs = sampledCt.getNamesOfSeqs();
			for (int i = 0; i < sampledSeqs.size(); i++) { subset.insert(sampledSeqs[i]); }

			string countOutputDir = outputDir;
			if (outputDir == "") { countOutputDir += File::getPath(countfile); }
			map<string, string> variables;
			variables["[filename]"] = countOutputDir + File::getRootName(File::getSimpleName(countfile));
			variables["[extension]"] = m->getExtension(countfile);
			string countOutputFileName = getOutputFileName("count", variables);
			outputTypes["count"].push_back(countOutputFileName);  outputNames.push_back(countOutputFileName);
			sampledCt.printTable(countOutputFileName);
		}
	}
	else {
		if (countfile == "") {
			//randomly select a subset of those names to include in the subsample
			//since names was randomly shuffled just grab the next one
			for (int j = 0; j < names.size(); j++) {

				if (ctrlc_pressed) { return 0; }

				if (groupfile != "") { //if there is a groupfile given fill in group info
					string group = groupMap.getGroup(names[j]);
					if (group == "not found") { LOG(LOGERROR) << "" + names[j] + " is not in your groupfile. please correct." << '\n'; group = "NOTFOUND"; }

					if (pickedGroups) { //if hte user picked groups, we only want to keep the names of sequences from those groups
						if (m->inUsersGroups(group, Groups)) { subset.insert(names[j]); }
					}
					else { subset.insert(names[j]); }
				}
				else { //save everyone, group
					subset.insert(names[j]);
				}

				//do we have enough??
				if (subset.size() == size) { break; }
			}
		}
		else {
			SubSample sample;
			CountTable sampledCt = sample.getSample(ct, size, Groups, pickedGroups);
			vector<string> sampledSeqs = sampledCt.getNamesOfSeqs();
			for (int i = 0; i < sampledSeqs.size(); i++) { subset.insert(sampledSeqs[i]); }

			string countOutputDir = outputDir;
			if (outputDir == "") { countOutputDir += File::getPath(countfile); }
			map<string, string> variables;
			variables["[filename]"] = countOutputDir + File::getRootName(File::getSimpleName(countfile));
			variables["[extension]"] = m->getExtension(countfile);
			string countOutputFileName = getOutputFileName("count", variables);
			outputTypes["count"].push_back(countOutputFileName);  outputNames.push_back(countOutputFileName);
			sampledCt.printTable(countOutputFileName);
		}
	}

	if (subset.size() == 0) { LOG(INFO) << "The size you selected is too large, skipping fasta file." << '\n';  return 0; }

	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(fastafile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(fastafile));
	variables["[extension]"] = m->getExtension(fastafile);
	string outputFileName = getOutputFileName("fasta", variables);
	ofstream out;
	File::openOutputFile(outputFileName, out);

	//read through fasta file outputting only the names on the subsample list
	ifstream in;
	File::openInputFile(fastafile, in);

	string thisname;
	int count = 0;
	map<string, vector<string> >::iterator itNameMap;

	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); out.close();  return 0; }

		Sequence currSeq(in);
		thisname = currSeq.getName();

		if (thisname != "") {

			//does the subset contain a sequence that this sequence represents
			itNameMap = nameMap.find(thisname);
			if (itNameMap != nameMap.end()) {
				vector<string> nameRepresents = itNameMap->second;

				for (int i = 0; i < nameRepresents.size(); i++) {
					if (subset.count(nameRepresents[i]) != 0) {
						out << ">" << nameRepresents[i] << endl << currSeq.getAligned() << endl;
						count++;
					}
				}
			}
			else {
				LOG(LOGERROR) << "" + thisname + " is not in your namefile, please correct." << '\n';
			}
		}
		File::gobble(in);
	}
	in.close();
	out.close();


	if (count != subset.size()) {
		LOG(LOGERROR) << "The subset selected contained " + toString(subset.size()) + " sequences, but I only found " + toString(count) + " of those in the fastafile." << '\n';
	}

	if (namefile != "") {
		LOG(INFO) << "Deconvoluting subsampled fasta file... " << '\n';
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(namefile));
		variables["[extension]"] = m->getExtension(namefile);
		string outputNameFileName = getOutputFileName("name", variables);
		//use unique.seqs to create new name and fastafile
		string inputString = "fasta=" + outputFileName;
		LOG(INFO) << "/******************************************/" << '\n';
		LOG(INFO) << "Running command: unique.seqs(" + inputString + ")" << '\n';
		m->mothurCalling = true;

		Command* uniqueCommand = new DeconvoluteCommand(inputString);
		uniqueCommand->execute();

		map<string, vector<string> > filenames = uniqueCommand->getOutputFiles();

		delete uniqueCommand;
		m->mothurCalling = false;

		m->renameFile(filenames["name"][0], outputNameFileName);
		m->renameFile(filenames["fasta"][0], outputFileName);

		outputTypes["name"].push_back(outputNameFileName);  outputNames.push_back(outputNameFileName);

		LOG(INFO) << "/******************************************/" << '\n';

		LOG(INFO) << "Done." << '\n';

		if (taxonomyfile != "") {
			set<string> tempSubset;
			//get new unique names from fasta file
			//read through fasta file outputting only the names on the subsample list after deconvolute
			ifstream in2;
			File::openInputFile(outputFileName, in2);

			while (!in2.eof()) {
				Sequence seq(in2); File::gobble(in2);
				if (seq.getName() != "") {
					tempSubset.insert(seq.getName());
				}
			}
			in2.close();

			//send that list to getTax
			int tcount = getTax(tempSubset);
			if (tcount != tempSubset.size()) { LOG(LOGERROR) << "subsampled fasta file contains " + toString(tempSubset.size()) + " sequences, but I only found " + toString(tcount) + " in your taxonomy file, please correct." << '\n'; }
		}
	}
	else {
		if (taxonomyfile != "") {
			int tcount = getTax(subset);
			if (tcount != subset.size()) { LOG(LOGERROR) << "subsampled fasta file contains " + toString(subset.size()) + " sequences, but I only found " + toString(tcount) + " in your taxonomy file, please correct." << '\n'; }

		}  //should only contain uniques.
	}

	outputTypes["fasta"].push_back(outputFileName);  outputNames.push_back(outputFileName);

	//if a groupfile is provided read through the group file only outputting the names on the subsample list
	if (groupfile != "") {

		string groupOutputDir = outputDir;
		if (outputDir == "") { groupOutputDir += File::getPath(groupfile); }
		map<string, string> variables;
		variables["[filename]"] = groupOutputDir + File::getRootName(File::getSimpleName(groupfile));
		variables["[extension]"] = m->getExtension(groupfile);
		string groupOutputFileName = getOutputFileName("group", variables);

		ofstream outGroup;
		File::openOutputFile(groupOutputFileName, outGroup);
		outputTypes["group"].push_back(groupOutputFileName);  outputNames.push_back(groupOutputFileName);

		ifstream inGroup;
		File::openInputFile(groupfile, inGroup);
		string name, group;

		while (!inGroup.eof()) {

			if (ctrlc_pressed) { inGroup.close(); outGroup.close(); return 0; }

			inGroup >> name;	File::gobble(inGroup);			//read from first column
			inGroup >> group;			//read from second column

			//if this name is in the accnos file
			if (subset.count(name) != 0) {
				outGroup << name << '\t' << group << endl;
				subset.erase(name);
			}

			File::gobble(inGroup);
		}
		inGroup.close();
		outGroup.close();

		//sanity check
		if (subset.size() != 0) {
			LOG(INFO) << "Your groupfile does not match your fasta file." << '\n';
			for (set<string>::iterator it = subset.begin(); it != subset.end(); it++) {
				LOG(LOGERROR) << "" + *it + " is missing from your groupfile." << '\n';
			}
		}
	}

	return 0;

}
//**********************************************************************************************************************
int SubSampleCommand::getNames() {

	ifstream in;
	File::openInputFile(fastafile, in);

	string thisname;
	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); return 0; }

		Sequence currSeq(in);
		thisname = currSeq.getName();

		if (thisname != "") {
			vector<string> temp; temp.push_back(thisname);
			nameMap[thisname] = temp;
			names.push_back(thisname);
		}
		File::gobble(in);
	}
	in.close();

	return 0;

}
//**********************************************************************************************************************
int SubSampleCommand::readNames() {

	nameMap.clear();
	m->readNames(namefile, nameMap);

	//save names of all sequences
	map<string, vector<string> >::iterator it;
	for (it = nameMap.begin(); it != nameMap.end(); it++) { for (int i = 0; i < (it->second).size(); i++) { names.push_back((it->second)[i]); } }

	return 0;

}
//**********************************************************************************************************************
int SubSampleCommand::getSubSampleShared() {

	InputData* input = new InputData(sharedfile, "sharedfile");
	vector<SharedRAbundVector*> lookup = input->getSharedRAbundVectors();
	string lastLabel = lookup[0]->getLabel();

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	if (size == 0) { //user has not set size, set size = smallest samples size
		size = lookup[0]->getNumSeqs();
		for (int i = 1; i < lookup.size(); i++) {
			int thisSize = lookup[i]->getNumSeqs();

			if (thisSize < size) { size = thisSize; }
		}
	}
	else {
		m->clearGroups();
		Groups.clear();
		vector<SharedRAbundVector*> temp;
		for (int i = 0; i < lookup.size(); i++) {
			if (lookup[i]->getNumSeqs() < size) {
				LOG(INFO) << lookup[i]->getGroup() + " contains " + toString(lookup[i]->getNumSeqs()) + ". Eliminating." << '\n';
				delete lookup[i];
			}
			else {
				Groups.push_back(lookup[i]->getGroup());
				temp.push_back(lookup[i]);
			}
		}
		lookup = temp;
		m->setGroups(Groups);
	}

	if (lookup.size() == 0) { LOG(INFO) << "The size you selected is too large, skipping shared file." << '\n'; delete input; return 0; }

	LOG(INFO) << "Sampling " + toString(size) + " from each group." << '\n';

	//as long as you are not at the end of the file or done wih the lines you want
	while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
		if (ctrlc_pressed) { delete input; for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }  return 0; }

		if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

			LOG(INFO) << lookup[0]->getLabel() << '\n';

			processShared(lookup);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());
		}

		if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = lookup[0]->getLabel();

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }

			lookup = input->getSharedRAbundVectors(lastLabel);
			LOG(INFO) << lookup[0]->getLabel() << '\n';

			processShared(lookup);

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
		lookup = input->getSharedRAbundVectors(lastLabel);

		LOG(INFO) << lookup[0]->getLabel() << '\n';

		processShared(lookup);

		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
	}

	delete input;

	return 0;

}
//**********************************************************************************************************************
int SubSampleCommand::processShared(vector<SharedRAbundVector*>& thislookup) {

	//save mothurOut's binLabels to restore for next label
	vector<string> saveBinLabels = settings.currentSharedBinLabels;

	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(sharedfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(sharedfile));
	variables["[extension]"] = m->getExtension(sharedfile);
	variables["[distance]"] = thislookup[0]->getLabel();
	string outputFileName = getOutputFileName("shared", variables);
	SubSample sample;
	vector<string> subsampledLabels = sample.getSample(thislookup, size);

	if (ctrlc_pressed) { return 0; }

	ofstream out;
	File::openOutputFile(outputFileName, out);
	outputTypes["shared"].push_back(outputFileName);  outputNames.push_back(outputFileName);

	settings.currentSharedBinLabels = subsampledLabels;

	thislookup[0]->printHeaders(out);

	for (int i = 0; i < thislookup.size(); i++) {
		out << thislookup[i]->getLabel() << '\t' << thislookup[i]->getGroup() << '\t';
		thislookup[i]->print(out);
	}
	out.close();


	//save mothurOut's binLabels to restore for next label
	settings.currentSharedBinLabels = saveBinLabels;

	return 0;

}
//**********************************************************************************************************************
int SubSampleCommand::getSubSampleList() {

	if (namefile != "") { m->readNames(namefile, nameMap); }

	InputData* input = new InputData(listfile, "list");
	ListVector* list = input->getListVector();
	string lastLabel = list->getLabel();

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	ofstream outGroup;
	GroupMap groupMap;
	if (groupfile != "") {
		groupMap.readMap(groupfile);

		//takes care of user setting groupNames that are invalid or setting groups=all
		SharedUtil util; vector<string> namesGroups = groupMap.getNamesOfGroups(); util.setGroups(Groups, namesGroups);

		//create outputfiles
		string groupOutputDir = outputDir;
		if (outputDir == "") { groupOutputDir += File::getPath(groupfile); }
		string groupOutputFileName = groupOutputDir + File::getRootName(File::getSimpleName(groupfile)) + "subsample" + m->getExtension(groupfile);
		File::openOutputFile(groupOutputFileName, outGroup);
		outputTypes["group"].push_back(groupOutputFileName);  outputNames.push_back(groupOutputFileName);

		//file mismatch quit
		if (list->getNumSeqs() != groupMap.getNumSeqs()) {
			LOG(LOGERROR) << "your list file contains " + toString(list->getNumSeqs()) + " sequences, and your groupfile contains " + toString(groupMap.getNumSeqs()) + ", please correct.";
			LOG(INFO) << ""; delete list; delete input;  outGroup.close(); return 0;
		}
	}
	else if (countfile != "") {
		if (ct.hasGroupInfo()) {
			SharedUtil util;
			vector<string> namesGroups = ct.getNamesOfGroups();
			util.setGroups(Groups, namesGroups);
		}

		//file mismatch quit
		if (list->getNumSeqs() != ct.getNumUniqueSeqs()) {
			LOG(LOGERROR) << "your list file contains " + toString(list->getNumSeqs()) + " sequences, and your count file contains " + toString(ct.getNumUniqueSeqs()) + " unique sequences, please correct.";
			LOG(INFO) << "";
			return 0;
		}
	}

	//make sure that if your picked groups size is not too big
	if (persample) {
		if (size == 0) { //user has not set size, set size = smallest samples size
			if (countfile == "") { size = groupMap.getNumSeqs(Groups[0]); }
			else { size = ct.getGroupCount(Groups[0]); }

			for (int i = 1; i < Groups.size(); i++) {
				int thisSize = 0;
				if (countfile == "") { thisSize = groupMap.getNumSeqs(Groups[i]); }
				else { thisSize = ct.getGroupCount(Groups[i]); }

				if (thisSize < size) { size = thisSize; }
			}
		}
		else { //make sure size is not too large
			vector<string> newGroups;
			for (int i = 0; i < Groups.size(); i++) {
				int thisSize = 0;
				if (countfile == "") { thisSize = groupMap.getNumSeqs(Groups[i]); }
				else { thisSize = ct.getGroupCount(Groups[i]); }

				if (thisSize >= size) { newGroups.push_back(Groups[i]); }
				else { LOG(INFO) << "You have selected a size that is larger than " + Groups[i] + " number of sequences, removing " + Groups[i] + "." << '\n'; }
			}
			Groups = newGroups;
			if (newGroups.size() == 0) { LOG(LOGERROR) << "all groups removed." << '\n'; ctrlc_pressed = true; }
		}

		LOG(INFO) << "Sampling " + toString(size) + " from each group." << '\n';
	}
	else {
		if (pickedGroups) {
			int total = 0;
			for (int i = 0; i < Groups.size(); i++) {
				if (countfile == "") { total += groupMap.getNumSeqs(Groups[i]); }
				else { total += ct.getGroupCount(Groups[i]); }
			}

			if (size == 0) { //user has not set size, set size = 10% samples size
				size = int(total * 0.10);
			}

			if (total < size) {
				if (size != 0) {
					LOG(INFO) << "Your size is too large for the number of groups you selected. Adjusting to " + toString(int(total * 0.10)) + "." << '\n';
				}
				size = int(total * 0.10);
			}

			LOG(INFO) << "Sampling " + toString(size) + " from " + toString(total) + "." << '\n';
		}
		else {
			if (size == 0) { //user has not set size, set size = 10% samples size
				if (countfile == "") { size = int(list->getNumSeqs() * 0.10); }
				else { size = int(ct.getNumSeqs() * 0.10); }
			}

			int thisSize = 0;
			if (countfile == "") { thisSize = list->getNumSeqs(); }
			else { thisSize = ct.getNumSeqs(); }

			if (size > thisSize) {
				LOG(INFO) << "Your list file only contains " + toString(thisSize) + " sequences. Setting size to " + toString(thisSize) + "." << '\n';
				size = thisSize;
			}

			LOG(INFO) << "Sampling " + toString(size) + " from " + toString(thisSize) + "." << '\n';
		}
	}

	set<string> subset; //dont want repeat sequence names added
	if (countfile == "") {
		//fill names
		for (int i = 0; i < list->getNumBins(); i++) {
			string binnames = list->get(i);
			vector<string> thisBin;
			m->splitAtComma(binnames, thisBin);

			for (int j = 0;j < thisBin.size();j++) {
				if (groupfile != "") { //if there is a groupfile given fill in group info
					string group = groupMap.getGroup(thisBin[j]);
					if (group == "not found") { LOG(LOGERROR) << "" + thisBin[j] + " is not in your groupfile. please correct." << '\n'; group = "NOTFOUND"; }

					//if hte user picked groups, we only want to keep the names of sequences from those groups
					if (pickedGroups) { if (m->inUsersGroups(group, Groups)) { names.push_back(thisBin[j]); } }
					else { names.push_back(thisBin[j]); }
				}//save everyone, group
				else { names.push_back(thisBin[j]); }
			}
		}

		random_shuffle(names.begin(), names.end());

		//randomly select a subset of those names to include in the subsample
		if (persample) {
			//initialize counts
			map<string, int> groupCounts;
			map<string, int>::iterator itGroupCounts;
			for (int i = 0; i < Groups.size(); i++) { groupCounts[Groups[i]] = 0; }

			for (int j = 0; j < names.size(); j++) {

				if (ctrlc_pressed) { delete list; delete input;  return 0; }

				string group = groupMap.getGroup(names[j]);
				if (group == "not found") { LOG(LOGERROR) << "" + names[j] + " is not in your groupfile. please correct." << '\n'; group = "NOTFOUND"; }
				else {
					itGroupCounts = groupCounts.find(group);
					if (itGroupCounts != groupCounts.end()) {
						if (groupCounts[group] < size) { subset.insert(names[j]); 	groupCounts[group]++; }
					}
				}
			}
		}
		else {
			for (int j = 0; j < size; j++) {
				if (ctrlc_pressed) { break; }
				subset.insert(names[j]);
			}
		}

		if (groupfile != "") {
			//write out new groupfile
			for (set<string>::iterator it = subset.begin(); it != subset.end(); it++) {
				string group = groupMap.getGroup(*it);
				if (group == "not found") { group = "NOTFOUND"; }
				outGroup << *it << '\t' << group << endl;
			}
			outGroup.close();
		}
	}
	else {
		SubSample sample; CountTable sampledCt;

		if (persample) { sampledCt = sample.getSample(ct, size, Groups); }
		else { sampledCt = sample.getSample(ct, size, Groups, pickedGroups); }

		vector<string> sampledSeqs = sampledCt.getNamesOfSeqs();
		for (int i = 0; i < sampledSeqs.size(); i++) { subset.insert(sampledSeqs[i]); }

		string countOutputDir = outputDir;
		if (outputDir == "") { countOutputDir += File::getPath(countfile); }
		map<string, string> variables;
		variables["[filename]"] = countOutputDir + File::getRootName(File::getSimpleName(countfile));
		variables["[extension]"] = m->getExtension(countfile);
		string countOutputFileName = getOutputFileName("count", variables);
		outputTypes["count"].push_back(countOutputFileName);  outputNames.push_back(countOutputFileName);
		sampledCt.printTable(countOutputFileName);
	}

	//as long as you are not at the end of the file or done wih the lines you want
	while ((list != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

		if (ctrlc_pressed) { delete list; delete input;  return 0; }

		if (allLines == 1 || labels.count(list->getLabel()) == 1) {

			LOG(INFO) << list->getLabel() << '\n';

			processList(list, subset);

			processedLabels.insert(list->getLabel());
			userLabels.erase(list->getLabel());
		}

		if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = list->getLabel();

			delete list;

			list = input->getListVector(lastLabel);
			LOG(INFO) << list->getLabel() << '\n';

			processList(list, subset);

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


	if (ctrlc_pressed) { if (list != NULL) { delete list; } delete input;  return 0; }

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

		LOG(INFO) << list->getLabel() << '\n';

		processList(list, subset);

		delete list; list = NULL;
	}

	if (list != NULL) { delete list; }
	delete input;

	if (taxonomyfile != "") {
		if (namefile == "") {
			InputData input(listfile, "list");
			ListVector* list = input.getListVector();
			string lastLabel = list->getLabel();

			for (int i = 0; i < list->getNumBins(); i++) {
				vector<string> temp;
				string bin = list->get(i);
				m->splitAtComma(bin, temp);
				for (int j = 0; j < temp.size(); j++) { vector<string> tempFakeOut; tempFakeOut.push_back(temp[j]); nameMap[temp[j]] = tempFakeOut; }
			}
			delete list;

			int tcount = getTax(subset);
			if (tcount != subset.size()) { LOG(LOGERROR) << "subsampled list file contains " + toString(subset.size()) + " sequences, but I only found " + toString(tcount) + " in your taxonomy file, did you forget a name file? Please correct." << '\n'; }
		}
		else {
			string tempAccnos = "temp.accnos";
			ofstream outAccnos;
			File::openOutputFile(tempAccnos, outAccnos);
			for (set<string>::iterator it = subset.begin(); it != subset.end(); it++) { outAccnos << *it << endl; }
			outAccnos.close();

			LOG(INFO) << "Sampling taxonomy and name file... " << '\n';
			string thisNameOutputDir = outputDir;
			if (outputDir == "") { thisNameOutputDir += File::getPath(namefile); }
			map<string, string> variables;
			variables["[filename]"] = thisNameOutputDir + File::getRootName(File::getSimpleName(namefile));
			variables["[extension]"] = m->getExtension(namefile);
			string outputNameFileName = getOutputFileName("name", variables);

			string thisTaxOutputDir = outputDir;
			if (outputDir == "") { thisTaxOutputDir += File::getPath(taxonomyfile); }
			variables["[filename]"] = thisTaxOutputDir + File::getRootName(File::getSimpleName(taxonomyfile));
			variables["[extension]"] = m->getExtension(taxonomyfile);
			string outputTaxFileName = getOutputFileName("taxonomy", variables);


			//use unique.seqs to create new name and fastafile
			string inputString = "dups=f, name=" + namefile + ", taxonomy=" + taxonomyfile + ", accnos=" + tempAccnos;
			LOG(INFO) << "/******************************************/" << '\n';
			LOG(INFO) << "Running command: get.seqs(" + inputString + ")" << '\n';
			m->mothurCalling = true;

			Command* getCommand = new GetSeqsCommand(inputString);
			getCommand->execute();

			map<string, vector<string> > filenames = getCommand->getOutputFiles();

			delete getCommand;
			m->mothurCalling = false;

			m->renameFile(filenames["name"][0], outputNameFileName);
			m->renameFile(filenames["taxonomy"][0], outputTaxFileName);

			outputTypes["name"].push_back(outputNameFileName);  outputNames.push_back(outputNameFileName);
			outputNames.push_back(outputTaxFileName); outputTypes["taxonomy"].push_back(outputTaxFileName);

			LOG(INFO) << "/******************************************/" << '\n';

			LOG(INFO) << "Done." << '\n';
		}
	}

	return 0;

}
//**********************************************************************************************************************
int SubSampleCommand::processList(ListVector*& list, set<string>& subset) {
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(listfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(listfile));
	variables["[extension]"] = m->getExtension(listfile);
	variables["[distance]"] = list->getLabel();
	string outputFileName = getOutputFileName("list", variables);
	ofstream out;
	File::openOutputFile(outputFileName, out);
	outputTypes["list"].push_back(outputFileName);  outputNames.push_back(outputFileName);

	int numBins = list->getNumBins();

	ListVector* temp = new ListVector();
	temp->setLabel(list->getLabel());

	vector<string> binLabels = list->getLabels();
	vector<string> newLabels;
	for (int i = 0; i < numBins; i++) {

		if (ctrlc_pressed) { break; }

		string bin = list->get(i);
		vector<string> binnames;
		m->splitAtComma(bin, binnames);

		string newNames = "";
		for (int j = 0;j < binnames.size();j++) { if (subset.count(binnames[j]) != 0) { newNames += binnames[j] + ","; } }

		//if there are names in this bin add to new list
		if (newNames != "") {
			newNames = newNames.substr(0, newNames.length() - 1); //rip off extra comma
			temp->push_back(newNames);
			newLabels.push_back(binLabels[i]);
		}
	}

	temp->setLabels(newLabels);
	delete list;
	list = temp;

	if (ctrlc_pressed) { out.close(); return 0; }

	list->printHeaders(out);
	list->print(out);
	out.close();

	return 0;

}
//**********************************************************************************************************************
int SubSampleCommand::getSubSampleRabund() {
	InputData* input = new InputData(rabundfile, "rabund");
	RAbundVector* rabund = input->getRAbundVector();
	string lastLabel = rabund->getLabel();

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	if (size == 0) { //user has not set size, set size = 10%
		size = int((rabund->getNumSeqs()) * 0.10);
	}
	else if (size > rabund->getNumSeqs()) { LOG(INFO) << "The size you selected is too large, skipping rabund file." << '\n'; delete input; delete rabund; return 0; }

	LOG(INFO) << "Sampling " + toString(size) + " from " + toString(rabund->getNumSeqs()) + "." << '\n';

	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(rabundfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(rabundfile));
	variables["[extension]"] = m->getExtension(rabundfile);
	string outputFileName = getOutputFileName("rabund", variables);
	ofstream out;
	File::openOutputFile(outputFileName, out);
	outputTypes["rabund"].push_back(outputFileName);  outputNames.push_back(outputFileName);

	//as long as you are not at the end of the file or done wih the lines you want
	while ((rabund != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
		if (ctrlc_pressed) { delete input; delete rabund; out.close(); return 0; }

		if (allLines == 1 || labels.count(rabund->getLabel()) == 1) {

			LOG(INFO) << rabund->getLabel() << '\n';

			processRabund(rabund, out);

			processedLabels.insert(rabund->getLabel());
			userLabels.erase(rabund->getLabel());
		}

		if ((Utility::anyLabelsToProcess(rabund->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = rabund->getLabel();

			delete rabund;

			rabund = input->getRAbundVector(lastLabel);
			LOG(INFO) << rabund->getLabel() << '\n';

			processRabund(rabund, out);

			processedLabels.insert(rabund->getLabel());
			userLabels.erase(rabund->getLabel());

			//restore real lastlabel to save below
			rabund->setLabel(saveLabel);
		}

		lastLabel = rabund->getLabel();

		//prevent memory leak
		delete rabund; rabund = NULL;

		//get next line to process
		rabund = input->getRAbundVector();
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

		rabund = input->getRAbundVector(lastLabel);

		LOG(INFO) << rabund->getLabel() << '\n';

		processRabund(rabund, out);

		delete rabund;
	}

	delete input;
	out.close();

	return 0;

}
//**********************************************************************************************************************
int SubSampleCommand::processRabund(RAbundVector*& rabund, ofstream& out) {

	int numBins = rabund->getNumBins();
	int thisSize = rabund->getNumSeqs();

	if (thisSize != size) {

		OrderVector* order = new OrderVector();
		for (int p = 0;p < numBins;p++) {
			for (int j = 0;j < rabund->get(p);j++) {
				order->push_back(p);
			}
		}
		random_shuffle(order->begin(), order->end());

		RAbundVector* temp = new RAbundVector(numBins);
		temp->setLabel(rabund->getLabel());

		delete rabund;
		rabund = temp;

		for (int j = 0; j < size; j++) {

			if (ctrlc_pressed) { delete order; return 0; }

			int bin = order->get(j);

			int abund = rabund->get(bin);
			rabund->set(bin, (abund + 1));
		}

		delete order;
	}

	if (ctrlc_pressed) { return 0; }

	rabund->print(out);

	return 0;

}
//**********************************************************************************************************************
int SubSampleCommand::getSubSampleSabund() {

	InputData* input = new InputData(sabundfile, "sabund");
	SAbundVector* sabund = input->getSAbundVector();
	string lastLabel = sabund->getLabel();

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	if (size == 0) { //user has not set size, set size = 10%
		size = int((sabund->getNumSeqs()) * 0.10);
	}
	else if (size > sabund->getNumSeqs()) { LOG(INFO) << "The size you selected is too large, skipping sabund file." << '\n'; delete input; delete sabund; return 0; }


	LOG(INFO) << "Sampling " + toString(size) + " from " + toString(sabund->getNumSeqs()) + "." << '\n';

	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(sabundfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(sabundfile));
	variables["[extension]"] = m->getExtension(sabundfile);
	string outputFileName = getOutputFileName("sabund", variables);
	ofstream out;
	File::openOutputFile(outputFileName, out);
	outputTypes["sabund"].push_back(outputFileName);  outputNames.push_back(outputFileName);


	//as long as you are not at the end of the file or done wih the lines you want
	while ((sabund != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
		if (ctrlc_pressed) { delete input; delete sabund; out.close(); return 0; }

		if (allLines == 1 || labels.count(sabund->getLabel()) == 1) {

			LOG(INFO) << sabund->getLabel() << '\n';

			processSabund(sabund, out);

			processedLabels.insert(sabund->getLabel());
			userLabels.erase(sabund->getLabel());
		}

		if ((Utility::anyLabelsToProcess(sabund->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = sabund->getLabel();

			delete sabund;

			sabund = input->getSAbundVector(lastLabel);
			LOG(INFO) << sabund->getLabel() << '\n';

			processSabund(sabund, out);

			processedLabels.insert(sabund->getLabel());
			userLabels.erase(sabund->getLabel());

			//restore real lastlabel to save below
			sabund->setLabel(saveLabel);
		}

		lastLabel = sabund->getLabel();

		//prevent memory leak
		delete sabund; sabund = NULL;

		//get next line to process
		sabund = input->getSAbundVector();
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

		sabund = input->getSAbundVector(lastLabel);

		LOG(INFO) << sabund->getLabel() << '\n';

		processSabund(sabund, out);

		delete sabund;
	}

	delete input;
	out.close();

	return 0;

}
//**********************************************************************************************************************
int SubSampleCommand::processSabund(SAbundVector*& sabund, ofstream& out) {

	RAbundVector* rabund = new RAbundVector();
	*rabund = sabund->getRAbundVector();

	int numBins = rabund->getNumBins();
	int thisSize = rabund->getNumSeqs();

	if (thisSize != size) {

		OrderVector* order = new OrderVector();
		for (int p = 0;p < numBins;p++) {
			for (int j = 0;j < rabund->get(p);j++) {
				order->push_back(p);
			}
		}
		random_shuffle(order->begin(), order->end());

		RAbundVector* temp = new RAbundVector(numBins);
		temp->setLabel(rabund->getLabel());

		delete rabund;
		rabund = temp;

		for (int j = 0; j < size; j++) {

			if (ctrlc_pressed) { delete order; return 0; }

			int bin = order->get(j);

			int abund = rabund->get(bin);
			rabund->set(bin, (abund + 1));
		}

		delete order;
	}

	if (ctrlc_pressed) { return 0; }

	delete sabund;
	sabund = new SAbundVector();
	*sabund = rabund->getSAbundVector();
	delete rabund;

	sabund->print(out);

	return 0;

}

//**********************************************************************************************************************
int SubSampleCommand::getTax(set<string>& subset) {

	string thisTaxOutputDir = outputDir;
	if (outputDir == "") { thisTaxOutputDir += File::getPath(taxonomyfile); }
	map<string, string> variables;
	variables["[filename]"] = thisTaxOutputDir + File::getRootName(File::getSimpleName(taxonomyfile));
	variables["[extension]"] = m->getExtension(taxonomyfile);
	string outputTaxFileName = getOutputFileName("taxonomy", variables);
	ofstream outTax;
	File::openOutputFile(outputTaxFileName, outTax);
	outputNames.push_back(outputTaxFileName); outputTypes["taxonomy"].push_back(outputTaxFileName);

	//read through fasta file outputting only the names on the subsample list
	ifstream inTax;
	File::openInputFile(taxonomyfile, inTax);

	string tname, tax;
	int tcount = 0;
	map<string, vector<string> >::iterator itNameMap;

	while (!inTax.eof()) {

		if (ctrlc_pressed) { inTax.close(); outTax.close();  return 0; }

		inTax >> tname;	File::gobble(inTax);			//read from first column
		inTax >> tax;	File::gobble(inTax);		//read from second column

		//does the subset contain a sequence that this sequence represents
		itNameMap = nameMap.find(tname);
		if (itNameMap != nameMap.end()) {
			vector<string> nameRepresents = itNameMap->second;


			for (int i = 0; i < nameRepresents.size(); i++) {
				if (subset.count(nameRepresents[i]) != 0) {
					outTax << nameRepresents[i] << '\t' << tax << endl;
					tcount++;

				}
			}
		}
		else { LOG(LOGERROR) << "" + tname + " is missing, please correct." << '\n'; }
	}
	inTax.close();
	outTax.close();

	return tcount;
}

//**********************************************************************************************************************



