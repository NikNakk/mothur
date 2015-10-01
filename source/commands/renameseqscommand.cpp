//
//  renameseqscommand.cpp
//  Mothur
//
//  Created by SarahsWork on 5/28/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#include "renameseqscommand.h"
#include "sequence.hpp"
#include "groupmap.h"
#include "counttable.h"

//**********************************************************************************************************************
vector<string> RenameSeqsCommand::setParameters() {
	nkParameters.add(new FastaFileParameter(settings, "fasta", true, true));
	CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "none", "", false, false, true); parameters.push_back(pname);
	CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "GroupCount", "none", "", false, false, true); parameters.push_back(pcount);
	CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "GroupCount", "none", "", false, false, true); parameters.push_back(pgroup);
	nkParameters.add(new StringParameter("delim", "_", false, false));
	CommandParameter pplacement("placement", "Multiple", "front-back", "back", "", "", "", "", false, false); parameters.push_back(pplacement);
	nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
	nkParameters.add(new StringParameter("inputdir", "", false, false));
	nkParameters.add(new StringParameter("outputdir", "", false, false));

	vector<string> myArray;
	for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
	return myArray;
}
//**********************************************************************************************************************
string RenameSeqsCommand::getHelpString() {
	string helpString = "The rename.seqs command reads a fastafile and groupfile or count file with an optional namefile. It creates files with the sequence names concatenated with the group."
		"The rename.seqs command parameters are fasta, name, group, count, placement, delim. Fasta and group or count are required, unless a current file is available for both.\n"
		"The placement parameter allows you to indicate whether you would like the group name appended to the front or back of the sequence name.  Options are front or back. Default=back.\n"
		"The delim parameter allow you to enter the character or characters you would like to separate the sequence name from the group name. Default='_'.\n"
		"The rename.seqs command should be in the following format: \n"
		"The rename.seqs command should be in the following format: \n"
		"rename.seqs(fasta=yourFastaFile, group=yourGroupFile) \n"
		"Example rename.seqs(fasta=abrecovery.unique.fasta, group=abrecovery.group).\n"
		"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
	return helpString;
}
//**********************************************************************************************************************
string RenameSeqsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],renamed,[extension]"; }
	else if (type == "name") { pattern = "[filename],renamed,[extension]"; }
	else if (type == "group") { pattern = "[filename],renamed,[extension]"; }
	else if (type == "count") { pattern = "[filename],renamed,[extension]"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
RenameSeqsCommand::RenameSeqsCommand(Settings& settings) : Command(settings) {
	abort = true; calledHelp = true;
	setParameters();
	vector<string> tempOutNames;
	outputTypes["fasta"] = tempOutNames;
	outputTypes["name"] = tempOutNames;
	outputTypes["group"] = tempOutNames;
	outputTypes["count"] = tempOutNames;
}
/**************************************************************************************/
RenameSeqsCommand::RenameSeqsCommand(Settings& settings, string option) : Command(settings, option) {
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

		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["count"] = tempOutNames;

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

			it = parameters.find("name");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["name"] = inputDir + it->second; }
			}

			it = parameters.find("group");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["group"] = inputDir + it->second; }
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
		fastaFile = validParameter.validFile(parameters, "fasta", true);
		if (fastaFile == "not open") { abort = true; }
		else if (fastaFile == "not found") {
			fastaFile = settings.getCurrent("fasta");
			if (fastaFile != "") { LOG(INFO) << "Using " + fastaFile + " as input file for the fasta parameter." << '\n'; }
			else { LOG(INFO) << "You have no current fastafile and the fasta parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("fasta", fastaFile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else {
			settings.setCurrent("counttable", countfile);
			CountTable temp;
			if (!temp.testGroups(countfile)) { LOG(LOGERROR) << "Your count file does not have group info, aborting.\n"; abort = true; }
		}

		nameFile = validParameter.validFile(parameters, "name", true);
		if (nameFile == "not open") { abort = true; }
		else if (nameFile == "not found") { nameFile = ""; }
		else { settings.setCurrent("name", nameFile); }

		//look for current files
		if ((groupfile == "") && (countfile == "")) {
			groupfile = settings.getCurrent("group");
			if (groupfile != "") { LOG(INFO) << "Using " + groupfile + " as input file for the group parameter." << '\n'; }
			else {
				countfile = settings.getCurrent("counttable");
				if (countfile != "") {
					LOG(INFO) << "Using " + countfile + " as input file for the count parameter." << '\n';
				}
				CountTable temp;
				if (!temp.testGroups(countfile)) { LOG(LOGERROR) << "Your count file does not have group info, aborting.\n"; abort = true; }
				else {
					LOG(LOGERROR) << "You need to provide a groupfile or countfile." << '\n';
					abort = true;
				}
			}
		}

		if ((countfile != "") && (nameFile != "")) { LOG(INFO) << "You must enter ONLY ONE of the following: count or name." << '\n'; abort = true; }

		placement = validParameter.validFile(parameters, "placement", false);		if (placement == "not found") { placement = "back"; }
		if ((placement == "front") || (placement == "back")) {}
		else { LOG(LOGERROR) << "" + placement + " is not a valid placement option.  Valid placement options are front or back.\n"; abort = true; }

		delim = validParameter.validFile(parameters, "delim", false);			if (delim == "not found") { delim = "_"; }

		if (countfile == "") {
			if (nameFile == "") {
				vector<string> files; files.push_back(fastaFile);
				OptionParser::getNameFile(files);
			}
		}
	}

}
/**************************************************************************************/
int RenameSeqsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//prepare filenames and open files
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(fastaFile); }
	string outFastaFile = thisOutputDir + File::getRootName(File::getSimpleName(fastaFile));
	map<string, string> variables;
	variables["[filename]"] = outFastaFile;
	variables["[extension]"] = m->getExtension(fastaFile);
	outFastaFile = getOutputFileName("fasta", variables);
	outputNames.push_back(outFastaFile); outputTypes["fasta"].push_back(outFastaFile);

	ofstream outFasta;
	File::openOutputFile(outFastaFile, outFasta);

	ifstream in;
	File::openInputFile(fastaFile, in);

	GroupMap* groupMap = NULL;
	CountTable* countTable = NULL;
	if (groupfile != "") {
		groupMap = new GroupMap(groupfile);

		int groupError = groupMap->readMap();
		if (groupError == 1) { delete groupMap; return 0; }
		vector<string> allGroups = groupMap->getNamesOfGroups();
		m->setAllGroups(allGroups);
	}
	else {
		countTable = new CountTable();
		countTable->readTable(countfile, true, false);
	}

	while (!in.eof()) {
		if (ctrlc_pressed) { break; }

		Sequence seq(in); File::gobble(in);
		string group = "not found";
		if (groupfile != "") { group = groupMap->getGroup(seq.getName()); }
		else {
			vector<string> groups = countTable->getGroups(seq.getName());
			if (group.size() == 0) { group = "not found"; }
			else {
				group = groups[0];
				for (int i = 1; i < groups.size(); i++) { group += "_" + groups[i]; }
			}

		}
		if (group == "not found") { LOG(LOGERROR) << "" + seq.getName() + " is not in your file, please correct.\n"; ctrlc_pressed = true; }
		else {
			string newName = "";
			if (placement == "back") { newName = seq.getName() + delim + group; }
			else { newName = group + delim + seq.getName(); }

			//rename sequence in count table
			if (countfile != "") { countTable->renameSeq(seq.getName(), newName); }

			seq.setName(newName);
			seq.printSequence(outFasta);
		}
	}
	in.close();

	if (ctrlc_pressed) { if (groupMap != NULL) { delete groupMap; } if (countTable != NULL) { delete countTable; } for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	bool notDone = true;
	if (nameFile != "") {
		thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(nameFile); }
		string outNameFile = thisOutputDir + File::getRootName(File::getSimpleName(nameFile));
		variables["[filename]"] = outNameFile;
		variables["[extension]"] = m->getExtension(nameFile);
		outNameFile = getOutputFileName("group", variables);
		outputNames.push_back(outNameFile); outputTypes["name"].push_back(outNameFile);

		ofstream outName;
		File::openOutputFile(outNameFile, outName);

		map<string, vector<string> > nameMap;
		m->readNames(nameFile, nameMap);

		//process name file changing names
		for (map<string, vector<string> >::iterator it = nameMap.begin(); it != nameMap.end(); it++) {
			for (int i = 0; i < (it->second).size() - 1; i++) {
				if (ctrlc_pressed) { break; }
				string group = groupMap->getGroup((it->second)[i]);
				if (group == "not found") { LOG(LOGERROR) << "" + (it->second)[i] + " is not in your group file, please correct.\n"; ctrlc_pressed = true; }
				else {
					string newName = "";
					if (placement == "back") { newName = (it->second)[i] + delim + group; }
					else { newName = group + delim + (it->second)[i]; }
					groupMap->renameSeq((it->second)[i], newName); //change in group file
					(it->second)[i] = newName; //change in namefile
				}
				if (i == 0) { outName << (it->second)[i] << '\t' << (it->second)[i] << ','; }
				else { outName << (it->second)[i] << ','; }
			}

			//print last one
			if ((it->second).size() == 1) {
				string group = groupMap->getGroup((it->second)[0]);
				if (group == "not found") { LOG(LOGERROR) << "" + (it->second)[0] + " is not in your group file, please correct.\n"; ctrlc_pressed = true; }
				else {
					string newName = "";
					if (placement == "back") { newName = (it->second)[0] + delim + group; }
					else { newName = group + delim + (it->second)[0]; }
					groupMap->renameSeq((it->second)[0], newName); //change in group file
					(it->second)[0] = newName; //change in namefile

					outName << (it->second)[0] << '\t' << (it->second)[0] << endl;
				}
			}
			else {
				string group = groupMap->getGroup((it->second)[(it->second).size() - 1]);
				if (group == "not found") { LOG(LOGERROR) << "" + (it->second)[(it->second).size() - 1] + " is not in your group file, please correct.\n"; ctrlc_pressed = true; }
				else {
					string newName = "";
					if (placement == "back") { newName = (it->second)[(it->second).size() - 1] + delim + group; }
					else { newName = group + delim + (it->second)[(it->second).size() - 1]; }
					groupMap->renameSeq((it->second)[(it->second).size() - 1], newName); //change in group file
					(it->second)[(it->second).size() - 1] = newName; //change in namefile

					outName << (it->second)[(it->second).size() - 1] << endl;
				}
			}
		}
		notDone = false;
		outName.close();
	}

	if (ctrlc_pressed) { if (groupMap != NULL) { delete groupMap; } if (countTable != NULL) { delete countTable; } for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (groupfile != "") {
		if (notDone) {
			vector<string> seqs = groupMap->getNamesSeqs();
			for (int i = 0; i < seqs.size(); i++) {
				if (ctrlc_pressed) { break; }
				string group = groupMap->getGroup(seqs[i]);
				string newName = "";
				if (placement == "back") { newName = seqs[i] + delim + group; }
				else { newName = group + delim + seqs[i]; }
				groupMap->renameSeq(seqs[i], newName);
			}
		}

		thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(groupfile); }
		string outGroupFile = thisOutputDir + File::getRootName(File::getSimpleName(groupfile));
		variables["[filename]"] = outGroupFile;
		variables["[extension]"] = m->getExtension(groupfile);
		outGroupFile = getOutputFileName("group", variables);
		outputNames.push_back(outGroupFile); outputTypes["group"].push_back(outGroupFile);

		ofstream outGroup;
		File::openOutputFile(outGroupFile, outGroup);
		groupMap->print(outGroup);
		outGroup.close();

	}
	else {
		thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(countfile); }
		string outCountFile = thisOutputDir + File::getRootName(File::getSimpleName(countfile));
		variables["[filename]"] = outCountFile;
		variables["[extension]"] = m->getExtension(countfile);
		outCountFile = getOutputFileName("count", variables);
		outputNames.push_back(outCountFile); outputTypes["count"].push_back(outCountFile);
		countTable->printTable(outCountFile);
	}

	if (groupMap != NULL) { delete groupMap; }
	if (countTable != NULL) { delete countTable; }

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

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

	itTypes = outputTypes.find("count");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
	}

	return 0;
}
/**************************************************************************************/

