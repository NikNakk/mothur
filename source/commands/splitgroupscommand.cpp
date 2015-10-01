/*
 *  splitgroupscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 9/20/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "splitgroupscommand.h"
#include "sharedutilities.h"
#include "sequenceparser.h"
#include "counttable.h"

 //**********************************************************************************************************************
vector<string> SplitGroupCommand::setParameters() {
	try {
		nkParameters.add(new FastaFileParameter(settings, "fasta", true, true));
		nkParameters.add(new NameFileParameter(settings, "name", false, true, "NameCount"));
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "CountGroup", "none", "count", false, false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "CountGroup", "none", "group", false, false, true); parameters.push_back(pgroup);
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitGroupCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string SplitGroupCommand::getHelpString() {
	try {
		string helpString = "The split.groups command reads a group or count file, and parses your fasta and names or count files by groups. \n"
			"The split.groups command parameters are fasta, name, group, count and groups.\n"
			"The fasta and group or count parameters are required.\n"
			"The groups parameter allows you to select groups to create files for.  \n"
			"For example if you set groups=A-B-C, you will get a .A.fasta, .A.names, .B.fasta, .B.names, .C.fasta, .C.names files.  \n"
			"If you want .fasta and .names files for all groups, set groups=all.  \n"
			"The split.groups command should be used in the following format: split.group(fasta=yourFasta, group=yourGroupFile).\n"
			"Example: split.groups(fasta=abrecovery.fasta, group=abrecovery.groups).\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitGroupCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string SplitGroupCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],[group],fasta"; }
	else if (type == "name") { pattern = "[filename],[group],names"; }
	else if (type == "count") { pattern = "[filename],[group],count_table"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}

//**********************************************************************************************************************
SplitGroupCommand::SplitGroupCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SplitGroupCommand, SplitGroupCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
SplitGroupCommand::SplitGroupCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["fasta"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["count"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
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


		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { namefile = ""; abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { settings.setCurrent("name", namefile); }

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
		else if (groupfile == "not found") {
			groupfile = "";
		}
		else { settings.setCurrent("group", groupfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		if ((countfile != "") && (namefile != "")) { LOG(INFO) << "You must enter ONLY ONE of the following: count or name." << '\n'; abort = true; }

		if ((countfile != "") && (groupfile != "")) { LOG(INFO) << "You must enter ONLY ONE of the following: count or group." << '\n'; abort = true; }

		if ((countfile == "") && (groupfile == "")) {
			if (namefile == "") { //check for count then group
				countfile = settings.getCurrent("counttable");
				if (countfile != "") { LOG(INFO) << "Using " + countfile + " as input file for the count parameter." << '\n'; }
				else {
					groupfile = settings.getCurrent("group");
					if (groupfile != "") { LOG(INFO) << "Using " + groupfile + " as input file for the group parameter." << '\n'; }
					else {
						LOG(INFO) << "You need to provide a count or group file." << '\n';
						abort = true;
					}
				}
			}
			else { //check for group
				groupfile = settings.getCurrent("group");
				if (groupfile != "") { LOG(INFO) << "Using " + groupfile + " as input file for the group parameter." << '\n'; }
				else {
					LOG(INFO) << "You need to provide a count or group file." << '\n';
					abort = true;
				}
			}
		}

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else { Utility::split(groups, '-', Groups); }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			if (groupfile != "") { outputDir = File::getPath(groupfile); }
			else { outputDir = File::getPath(countfile); }
		}

		if (countfile == "") {
			if (namefile == "") {
				vector<string> files; files.push_back(fastafile);
				OptionParser::getNameFile(files);
			}
		}
	}

}
//**********************************************************************************************************************
int SplitGroupCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	if (countfile == "") { runNameGroup(); }
	else { runCount(); }

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	string current = "";
	itTypes = outputTypes.find("fasta");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("fasta", current); }
	}

	itTypes = outputTypes.find("name");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("name", current); }
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
//**********************************************************************************************************************
int SplitGroupCommand::runNameGroup() {
	SequenceParser* parser;
	if (namefile == "") { parser = new SequenceParser(groupfile, fastafile); }
	else { parser = new SequenceParser(groupfile, fastafile, namefile); }

	if (ctrlc_pressed) { delete parser; return 0; }

	vector<string> namesGroups = parser->getNamesOfGroups();
	SharedUtil util;  util.setGroups(Groups, namesGroups);

	string fastafileRoot = outputDir + File::getRootName(File::getSimpleName(fastafile));
	string namefileRoot = outputDir + File::getRootName(File::getSimpleName(namefile));

	LOG(INFO) << "";
	for (int i = 0; i < Groups.size(); i++) {

		LOG(INFO) << "Processing group: " + Groups[i] << '\n';

		map<string, string> variables;
		variables["[filename]"] = fastafileRoot;
		variables["[group]"] = Groups[i];

		string newFasta = getOutputFileName("fasta", variables);
		variables["[filename]"] = namefileRoot;
		string newName = getOutputFileName("name", variables);

		parser->getSeqs(Groups[i], newFasta, false);
		outputNames.push_back(newFasta); outputTypes["fasta"].push_back(newFasta);
		if (ctrlc_pressed) { delete parser; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

		if (namefile != "") {
			parser->getNameMap(Groups[i], newName);
			outputNames.push_back(newName); outputTypes["name"].push_back(newName);
		}

		if (ctrlc_pressed) { delete parser; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }
	}

	delete parser;

	return 0;

}
//**********************************************************************************************************************
int SplitGroupCommand::runCount() {

	CountTable ct;
	ct.readTable(countfile, true, false);
	if (!ct.hasGroupInfo()) { LOG(LOGERROR) << "your count file does not contain group info, cannot split by group.\n"; ctrlc_pressed = true; }

	if (ctrlc_pressed) { return 0; }

	vector<string> namesGroups = ct.getNamesOfGroups();
	SharedUtil util;  util.setGroups(Groups, namesGroups);

	//fill filehandles with neccessary ofstreams
	map<string, ofstream*> ffiles;
	map<string, ofstream*> cfiles;
	ofstream* temp;
	for (int i = 0; i < Groups.size(); i++) {
		temp = new ofstream;
		ffiles[Groups[i]] = temp;
		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastafile));
		variables["[group]"] = Groups[i];
		string newFasta = getOutputFileName("fasta", variables);
		outputNames.push_back(newFasta); outputTypes["fasta"].push_back(newFasta);
		File::openOutputFile(newFasta, (*temp));
		temp = new ofstream;
		cfiles[Groups[i]] = temp;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(countfile));
		string newCount = getOutputFileName("count", variables);
		File::openOutputFile(newCount, (*temp));
		outputNames.push_back(newCount); outputTypes["count"].push_back(newCount);
		(*temp) << "Representative_Sequence\ttotal\t" << Groups[i] << endl;
	}

	ifstream in;
	File::openInputFile(fastafile, in);

	while (!in.eof()) {
		Sequence seq(in); File::gobble(in);

		if (ctrlc_pressed) { break; }
		if (seq.getName() != "") {
			vector<string> thisSeqsGroups = ct.getGroups(seq.getName());
			for (int i = 0; i < thisSeqsGroups.size(); i++) {
				if (m->inUsersGroups(thisSeqsGroups[i], Groups)) { //if this sequence belongs to a group we want them print
					seq.printSequence(*(ffiles[thisSeqsGroups[i]]));
					int numSeqs = ct.getGroupCount(seq.getName(), thisSeqsGroups[i]);
					(*(cfiles[thisSeqsGroups[i]])) << seq.getName() << '\t' << numSeqs << '\t' << numSeqs << endl;
				}
			}
		}
	}
	in.close();

	//close and delete ofstreams
	for (int i = 0; i < Groups.size(); i++) {
		(*ffiles[Groups[i]]).close(); delete ffiles[Groups[i]];
		(*cfiles[Groups[i]]).close(); delete cfiles[Groups[i]];
	}

	return 0;
	Y
}
//**********************************************************************************************************************


