/*
 *  removegroupscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 11/10/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "removegroupscommand.h"
#include "sequence.hpp"
#include "listvector.hpp"
#include "sharedutilities.h"
#include "inputdata.h"
#include "designmap.h"

 //**********************************************************************************************************************
vector<string> RemoveGroupsCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "FNGLT", "fasta", false, false, true); parameters.push_back(pfasta);
		CommandParameter pshared("shared", "InputTypes", "", "", "none", "sharedGroup", "none", "shared", false, false, true); parameters.push_back(pshared);
		nkParameters.add(new NameFileParameter(settings, "name", false, true, "NameCount"));
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "PhylipColumn", "none", "phylip", false, false, true); parameters.push_back(pphylip);
		CommandParameter pcolumn("column", "InputTypes", "", "", "none", "PhylipColumn", "none", "column", false, false, true); parameters.push_back(pcolumn);
		nkParameters.add(new CountFileParameter(settings, "count", false, true, "NameCount-CountGroup"));
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "sharedGroup", "FNGLT", "group", false, false, true); parameters.push_back(pgroup);
		CommandParameter pdesign("design", "InputTypes", "", "", "none", "sharedGroup", "FNGLT", "design", false, false); parameters.push_back(pdesign);
		CommandParameter plist("list", "InputTypes", "", "", "none", "none", "FNGLT", "list", false, false, true); parameters.push_back(plist);
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "none", "none", "FNGLT", "taxonomy", false, false, true); parameters.push_back(ptaxonomy);
		CommandParameter paccnos("accnos", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(paccnos);
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RemoveGroupsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string RemoveGroupsCommand::getHelpString() {
	try {
		string helpString = "The remove.groups command removes sequences from a specfic group or set of groups from the following file types: fasta, name, group, count, list, taxonomy, design, phylip, column or sharedfile.\n"
			"It outputs a file containing the sequences NOT in the those specified groups, or with a sharedfile eliminates the groups you selected.\n"
			"The remove.groups command parameters are accnos, fasta, name, group, list, taxonomy, shared, design, phylip, column and groups. The group or count parameter is required, unless you have a current group or count file or are using a sharedfile.\n"
			"You must also provide an accnos containing the list of groups to remove or set the groups parameter to the groups you wish to remove.\n"
			"The groups parameter allows you to specify which of the groups in your groupfile you would like removed.  You can separate group names with dashes.\n"
			"The remove.groups command should be in the following format: remove.groups(accnos=yourAccnos, fasta=yourFasta, group=yourGroupFile).\n"
			"Example remove.groups(accnos=amazon.accnos, fasta=amazon.fasta, group=amazon.groups).\n"
			"or remove.groups(groups=pasture, fasta=amazon.fasta, amazon.groups).\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RemoveGroupsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string RemoveGroupsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],pick,[extension]"; }
	else if (type == "taxonomy") { pattern = "[filename],pick,[extension]"; }
	else if (type == "name") { pattern = "[filename],pick,[extension]"; }
	else if (type == "group") { pattern = "[filename],pick,[extension]"; }
	else if (type == "count") { pattern = "[filename],pick,[extension]"; }
	else if (type == "phylip") { pattern = "[filename],pick,[extension]"; }
	else if (type == "column") { pattern = "[filename],pick,[extension]"; }
	else if (type == "list") { pattern = "[filename],[tag],pick,[extension]"; }
	else if (type == "shared") { pattern = "[filename],[tag],pick,[extension]"; }
	else if (type == "design") { pattern = "[filename],[tag],pick,[extension]-[filename],pick,[extension]"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
RemoveGroupsCommand::RemoveGroupsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["taxonomy"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["shared"] = tempOutNames;
		outputTypes["design"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["phylip"] = tempOutNames;
		outputTypes["column"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RemoveGroupsCommand, RemoveGroupsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
RemoveGroupsCommand::RemoveGroupsCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["taxonomy"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["shared"] = tempOutNames;
		outputTypes["design"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["phylip"] = tempOutNames;
		outputTypes["column"] = tempOutNames;


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

			it = parameters.find("accnos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["accnos"] = inputDir + it->second; }
			}

			it = parameters.find("list");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["list"] = inputDir + it->second; }
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

			it = parameters.find("taxonomy");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["taxonomy"] = inputDir + it->second; }
			}

			it = parameters.find("shared");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["shared"] = inputDir + it->second; }
			}

			it = parameters.find("design");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["design"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}

			it = parameters.find("phylip");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["phylip"] = inputDir + it->second; }
			}

			it = parameters.find("column");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["column"] = inputDir + it->second; }
			}
		}


		//check for required parameters
		accnosfile = validParameter.validFile(parameters, "accnos", true);
		if (accnosfile == "not open") { accnosfile = ""; abort = true; }
		else if (accnosfile == "not found") { accnosfile = ""; }
		else { settings.setCurrent("accnos", accnosfile); }

		phylipfile = validParameter.validFile(parameters, "phylip", true);
		if (phylipfile == "not open") { phylipfile = ""; abort = true; }
		else if (phylipfile == "not found") { phylipfile = ""; }
		else { settings.setCurrent("phylip", phylipfile); }

		columnfile = validParameter.validFile(parameters, "column", true);
		if (columnfile == "not open") { columnfile = ""; abort = true; }
		else if (columnfile == "not found") { columnfile = ""; }
		else { settings.setCurrent("column", columnfile); }

		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not open") { fastafile = ""; abort = true; }
		else if (fastafile == "not found") { fastafile = ""; }
		else { settings.setCurrent("fasta", fastafile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { namefile = ""; abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { settings.setCurrent("name", namefile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { groupfile = "";  abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { listfile = ""; abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { settings.setCurrent("list", listfile); }

		taxfile = validParameter.validFile(parameters, "taxonomy", true);
		if (taxfile == "not open") { taxfile = ""; abort = true; }
		else if (taxfile == "not found") { taxfile = ""; }
		else { settings.setCurrent("taxonomy", taxfile); }

		designfile = validParameter.validFile(parameters, "design", true);
		if (designfile == "not open") { designfile = ""; abort = true; }
		else if (designfile == "not found") { designfile = ""; }
		else { settings.setCurrent("design", designfile); }

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else {
			Utility::split(groups, '-', Groups);
			m->setGroups(Groups);
		}

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = ""; abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { settings.setCurrent("shared", sharedfile); }


		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		if ((namefile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: name or count." << '\n'; abort = true;
		}

		if ((groupfile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: group or count." << '\n'; abort = true;
		}


		if ((sharedfile == "") && (groupfile == "") && (designfile == "") && (countfile == "")) {
			//is there are current file available for any of these?
			if ((namefile != "") || (fastafile != "") || (listfile != "") || (taxfile != "")) {
				//give priority to group, then shared
				groupfile = settings.getCurrent("group");
				if (groupfile != "") { LOG(INFO) << "Using " + groupfile + " as input file for the group parameter." << '\n'; }
				else {
					sharedfile = settings.getCurrent("shared");
					if (sharedfile != "") { LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
					else {
						countfile = settings.getCurrent("counttable");
						if (countfile != "") { LOG(INFO) << "Using " + countfile + " as input file for the count parameter." << '\n'; }
						else {
							LOG(INFO) << "You have no current groupfile, countfile or sharedfile and one is required." << '\n'; abort = true;
						}
					}
				}
			}
			else {
				//give priority to shared, then group
				sharedfile = settings.getCurrent("shared");
				if (sharedfile != "") { LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
				else {
					groupfile = settings.getCurrent("group");
					if (groupfile != "") { LOG(INFO) << "Using " + groupfile + " as input file for the group parameter." << '\n'; }
					else {
						designfile = settings.getCurrent("design");
						if (designfile != "") { LOG(INFO) << "Using " + designfile + " as input file for the design parameter." << '\n'; }
						else {
							countfile = settings.getCurrent("counttable");
							if (countfile != "") { LOG(INFO) << "Using " + countfile + " as input file for the count parameter." << '\n'; }
							else {
								LOG(INFO) << "You have no current groupfile, designfile, countfile or sharedfile and one is required." << '\n'; abort = true;
							}

						}
					}
				}
			}
		}

		if ((accnosfile == "") && (Groups.size() == 0)) { LOG(INFO) << "You must provide an accnos file containing group names or specify groups using the groups parameter." << '\n'; abort = true; }

		if ((phylipfile == "") && (columnfile == "") && (fastafile == "") && (namefile == "") && (countfile == "") && (groupfile == "") && (designfile == "") && (sharedfile == "") && (listfile == "") && (taxfile == "")) { LOG(INFO) << "You must provide at least one of the following: fasta, name, taxonomy, group, shared, design, count, phylip, column or list." << '\n'; abort = true; }
		if (((groupfile == "") && (countfile == "")) && ((namefile != "") || (fastafile != "") || (listfile != "") || (taxfile != ""))) { LOG(INFO) << "If using a fasta, name, taxonomy, group or list, then you must provide a group or count file." << '\n'; abort = true; }

		if (countfile == "") {
			if ((namefile == "") && ((fastafile != "") || (taxfile != ""))) {
				vector<string> files; files.push_back(fastafile); files.push_back(taxfile);
				OptionParser::getNameFile(files);
			}
		}
	}

}
//**********************************************************************************************************************

int RemoveGroupsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//get groups you want to remove
	if (accnosfile != "") { m->readAccnos(accnosfile, Groups); m->setGroups(Groups); }

	if (groupfile != "") {
		groupMap = new GroupMap(groupfile);
		groupMap->readMap();

		//make sure groups are valid
		//takes care of user setting groupNames that are invalid or setting groups=all
		vector<string> namesGroups = groupMap->getNamesOfGroups();
		vector<string> checkedGroups;
		for (int i = 0; i < Groups.size(); i++) {
			if (m->inUsersGroups(Groups[i], namesGroups)) { checkedGroups.push_back(Groups[i]); }
			else { LOG(WARNING) << "" + Groups[i] + " is not a valid group in your groupfile, ignoring.\n"; }
		}

		if (checkedGroups.size() == 0) { LOG(LOGERROR) << "no valid groups, aborting.\n"; delete groupMap; return 0; }
		else {
			Groups = checkedGroups;
			m->setGroups(Groups);
		}

		//fill names with names of sequences that are from the groups we want to remove 
		fillNames();

		delete groupMap;
	}
	else if (countfile != "") {
		if ((fastafile != "") || (listfile != "") || (taxfile != "")) {
			LOG(INFO) << "\n[NOTE]: The count file should contain only unique names, so mothur assumes your fasta, list and taxonomy files also contain only uniques.\n\n";
		}
		CountTable ct;
		ct.readTable(countfile, true, false);
		if (!ct.hasGroupInfo()) { LOG(LOGERROR) << "your count file does not contain group info, aborting.\n"; return 0; }

		vector<string> gNamesOfGroups = ct.getNamesOfGroups();
		SharedUtil util;
		util.setGroups(Groups, gNamesOfGroups);
		vector<string> namesOfSeqs = ct.getNamesOfSeqs();
		sort(Groups.begin(), Groups.end());

		for (int i = 0; i < namesOfSeqs.size(); i++) {
			vector<string> thisSeqsGroups = ct.getGroups(namesOfSeqs[i]);
			if (m->isSubset(Groups, thisSeqsGroups)) { //you only have seqs from these groups so remove you
				names.insert(namesOfSeqs[i]);
			}
		}
	}


	if (ctrlc_pressed) { return 0; }

	//read through the correct file and output lines you want to keep
	if (namefile != "") { readName(); }
	if (fastafile != "") { readFasta(); }
	if (groupfile != "") { readGroup(); }
	if (countfile != "") { readCount(); }
	if (listfile != "") { readList(); }
	if (taxfile != "") { readTax(); }
	if (sharedfile != "") { readShared(); }
	if (designfile != "") { readDesign(); }
	if (phylipfile != "") { readPhylip(); }
	if (columnfile != "") { readColumn(); }

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (outputNames.size() != 0) {
		LOG(INFO) << '\n' << "Output File names: " << '\n';
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

		itTypes = outputTypes.find("list");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("list", current); }
		}

		itTypes = outputTypes.find("taxonomy");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("taxonomy", current); }
		}

		itTypes = outputTypes.find("shared");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("shared", current); }
		}

		itTypes = outputTypes.find("design");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("design", current); }
		}

		itTypes = outputTypes.find("count");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
		}

		itTypes = outputTypes.find("phylip");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("phylip", current); }
		}

		itTypes = outputTypes.find("column");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("column", current); }
		}
	}

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in RemoveGroupsCommand, execute";
	exit(1);
}
}

//**********************************************************************************************************************
int RemoveGroupsCommand::readFasta() {
	try {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(fastafile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(fastafile));
		variables["[extension]"] = m->getExtension(fastafile);
		string outputFileName = getOutputFileName("fasta", variables);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(fastafile, in);
		string name;

		bool wroteSomething = false;
		int removedCount = 0;

		while (!in.eof()) {
			if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

			Sequence currSeq(in);
			name = currSeq.getName();

			if (name != "") {
				//if this name is in the accnos file
				if (names.count(name) == 0) {
					wroteSomething = true;
					currSeq.printSequence(out);
				}
				else {
					//if you are not in the accnos file check if you are a name that needs to be changed
					map<string, string>::iterator it = uniqueToRedundant.find(name);
					if (it != uniqueToRedundant.end()) {
						wroteSomething = true;
						currSeq.setName(it->second);
						currSeq.printSequence(out);
					}
					else { removedCount++; }
				}
			}
			File::gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) { LOG(INFO) << "Your file contains only sequences from the groups you wish to remove." << '\n'; }
		outputTypes["fasta"].push_back(outputFileName);  outputNames.push_back(outputFileName);

		LOG(INFO) << "Removed " + toString(removedCount) + " sequences from your fasta file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************
	int RemoveGroupsCommand::readShared() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(sharedfile); }

		//get group names from sharedfile so we can set Groups to the groupNames we want to keep
		//that way we can take advantage of the reads in inputdata and sharedRabundVector
		InputData* tempInput = new InputData(sharedfile, "sharedfile");
		vector<SharedRAbundVector*> lookup = tempInput->getSharedRAbundVectors();

		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(sharedfile));
		variables["[extension]"] = m->getExtension(sharedfile);

		//save m->Groups
		vector<string> allGroupsNames = m->getAllGroups();
		vector<string> mothurOutGroups = settings.getGroups();

		vector<string> groupsToKeep;
		for (int i = 0; i < allGroupsNames.size(); i++) {
			if (!m->inUsersGroups(allGroupsNames[i], settings.getGroups())) {
				groupsToKeep.push_back(allGroupsNames[i]);
			}
		}

		if (allGroupsNames.size() == groupsToKeep.size()) { LOG(INFO) << "Your file does not contain any groups you wish to remove." << '\n'; m->setGroups(mothurOutGroups); delete tempInput; for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }  return 0; }

		//reset read 
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		delete tempInput;
		m->setGroups(groupsToKeep);
		m->clearAllGroups();
		m->saveNextLabel = "";
		m->printedSharedHeaders = false;
		settings.currentSharedBinLabels.clear();
		m->sharedBinLabelsInFile.clear();

		InputData input(sharedfile, "sharedfile");
		lookup = input.getSharedRAbundVectors();

		bool wroteSomething = false;

		while (lookup[0] != NULL) {

			variables["[tag]"] = lookup[0]->getLabel();
			string outputFileName = getOutputFileName("shared", variables);
			ofstream out;
			File::openOutputFile(outputFileName, out);
			outputTypes["shared"].push_back(outputFileName);  outputNames.push_back(outputFileName);

			if (ctrlc_pressed) { out.close();  File::remove(outputFileName);  for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } return 0; }

			lookup[0]->printHeaders(out);

			for (int i = 0; i < lookup.size(); i++) {
				out << lookup[i]->getLabel() << '\t' << lookup[i]->getGroup() << '\t';
				lookup[i]->print(out);
				wroteSomething = true;

			}

			//get next line to process
			//prevent memory leak
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input.getSharedRAbundVectors();

			out.close();
		}


		m->setGroups(mothurOutGroups);

		if (wroteSomething == false) { LOG(INFO) << "Your file contains only the groups you wish to remove." << '\n'; }

		string groupsString = "";
		for (int i = 0; i < Groups.size() - 1; i++) { groupsString += Groups[i] + ", "; }
		groupsString += Groups[Groups.size() - 1];

		LOG(INFO) << "Removed groups: " + groupsString + " from your shared file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************
	int RemoveGroupsCommand::readList() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(listfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(listfile));
		variables["[extension]"] = m->getExtension(listfile);

		ifstream in;
		File::openInputFile(listfile, in);

		bool wroteSomething = false;
		int removedCount = 0;

		while (!in.eof()) {

			removedCount = 0;

			//read in list vector
			ListVector list(in);

			variables["[tag]"] = list.getLabel();
			string outputFileName = getOutputFileName("list", variables);

			ofstream out;
			File::openOutputFile(outputFileName, out);
			outputTypes["list"].push_back(outputFileName);  outputNames.push_back(outputFileName);

			vector<string> binLabels = list.getLabels();
			vector<string> newBinLabels;

			//make a new list vector
			ListVector newList;
			newList.setLabel(list.getLabel());

			//for each bin
			for (int i = 0; i < list.getNumBins(); i++) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				//parse out names that are in accnos file
				string binnames = list.get(i);

				string newNames = "";
				while (binnames.find_first_of(',') != -1) {
					string name = binnames.substr(0, binnames.find_first_of(','));
					binnames = binnames.substr(binnames.find_first_of(',') + 1, binnames.length());

					//if that name is in the .accnos file, add it
					if (names.count(name) == 0) { newNames += name + ","; }
					else {
						//if you are not in the accnos file check if you are a name that needs to be changed
						map<string, string>::iterator it = uniqueToRedundant.find(name);
						if (it != uniqueToRedundant.end()) {
							newNames += it->second + ",";
						}
						else { removedCount++; }
					}
				}

				//get last name
				if (names.count(binnames) == 0) { newNames += binnames + ","; }
				else { //if you are not in the accnos file check if you are a name that needs to be changed
					map<string, string>::iterator it = uniqueToRedundant.find(binnames);
					if (it != uniqueToRedundant.end()) {
						newNames += it->second + ",";
					}
					else { removedCount++; }
				}

				//if there are names in this bin add to new list
				if (newNames != "") {
					newNames = newNames.substr(0, newNames.length() - 1); //rip off extra comma
					newList.push_back(newNames);
					newBinLabels.push_back(binLabels[i]);
				}
			}

			//print new listvector
			if (newList.getNumBins() != 0) {
				wroteSomething = true;
				newList.setLabels(newBinLabels);
				newList.printHeaders(out);
				newList.print(out);
			}

			File::gobble(in);
			out.close();
		}
		in.close();


		if (wroteSomething == false) { LOG(INFO) << "Your file contains only sequences from the groups you wish to remove." << '\n'; }
		LOG(INFO) << "Removed " + toString(removedCount) + " sequences from your list file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************
	int RemoveGroupsCommand::readName() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(namefile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(namefile));
		variables["[extension]"] = m->getExtension(namefile);
		string outputFileName = getOutputFileName("name", variables);
		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(namefile, in);
		string name, firstCol, secondCol;

		bool wroteSomething = false;
		int removedCount = 0;

		while (!in.eof()) {
			if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

			in >> firstCol;		File::gobble(in);
			in >> secondCol;

			vector<string> parsedNames;
			m->splitAtComma(secondCol, parsedNames);

			vector<string> validSecond;  validSecond.clear();
			for (int i = 0; i < parsedNames.size(); i++) {
				if (names.count(parsedNames[i]) == 0) {
					validSecond.push_back(parsedNames[i]);
				}
			}

			removedCount += parsedNames.size() - validSecond.size();

			//if the name in the first column is in the set then print it and any other names in second column also in set
			if (names.count(firstCol) == 0) {

				wroteSomething = true;

				out << firstCol << '\t';

				//you know you have at least one valid second since first column is valid
				for (int i = 0; i < validSecond.size() - 1; i++) { out << validSecond[i] << ','; }
				out << validSecond[validSecond.size() - 1] << endl;

				//make first name in set you come to first column and then add the remaining names to second column
			}
			else {

				//you want part of this row
				if (validSecond.size() != 0) {

					wroteSomething = true;

					out << validSecond[0] << '\t';

					//you know you have at least one valid second since first column is valid
					for (int i = 0; i < validSecond.size() - 1; i++) { out << validSecond[i] << ','; }
					out << validSecond[validSecond.size() - 1] << endl;
					uniqueToRedundant[firstCol] = validSecond[0];
				}
			}

			File::gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) { LOG(INFO) << "Your file contains only sequences from the groups you wish to remove." << '\n'; }
		outputTypes["name"].push_back(outputFileName); outputNames.push_back(outputFileName);

		LOG(INFO) << "Removed " + toString(removedCount) + " sequences from your name file." << '\n';

		return 0;
	}

	//**********************************************************************************************************************
	int RemoveGroupsCommand::readGroup() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(groupfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(groupfile));
		variables["[extension]"] = m->getExtension(groupfile);
		string outputFileName = getOutputFileName("group", variables);
		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(groupfile, in);
		string name, group;

		bool wroteSomething = false;
		int removedCount = 0;

		while (!in.eof()) {
			if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

			in >> name;				//read from first column
			in >> group;			//read from second column

			//if this name is in the accnos file
			if (names.count(name) == 0) {
				wroteSomething = true;
				out << name << '\t' << group << endl;
			}
			else { removedCount++; }

			File::gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) { LOG(INFO) << "Your file contains only sequences from the groups you wish to remove." << '\n'; }
		outputTypes["group"].push_back(outputFileName); outputNames.push_back(outputFileName);

		LOG(INFO) << "Removed " + toString(removedCount) + " sequences from your group file." << '\n';


		return 0;
	}
	//**********************************************************************************************************************
	int RemoveGroupsCommand::readCount() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(countfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(countfile));
		variables["[extension]"] = m->getExtension(countfile);
		string outputFileName = getOutputFileName("count", variables);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(countfile, in);

		bool wroteSomething = false;
		int removedCount = 0;

		string headers = File::getline(in); File::gobble(in);
		vector<string> columnHeaders = m->splitWhiteSpace(headers);

		vector<string> groups;
		map<int, string> originalGroupIndexes;
		map<string, int> GroupIndexes;
		set<int> indexOfGroupsChosen;
		for (int i = 2; i < columnHeaders.size(); i++) { groups.push_back(columnHeaders[i]);  originalGroupIndexes[i - 2] = columnHeaders[i]; }
		//sort groups to keep consistent with how we store the groups in groupmap
		sort(groups.begin(), groups.end());
		for (int i = 0; i < groups.size(); i++) { GroupIndexes[groups[i]] = i; }

		vector<string> groupsToKeep;
		for (int i = 0; i < groups.size(); i++) {
			if (!m->inUsersGroups(groups[i], Groups)) { groupsToKeep.push_back(groups[i]); }
		}
		sort(groupsToKeep.begin(), groupsToKeep.end());
		out << "Representative_Sequence\ttotal";
		for (int i = 0; i < groupsToKeep.size(); i++) { out << '\t' << groupsToKeep[i]; indexOfGroupsChosen.insert(GroupIndexes[groupsToKeep[i]]); }
		out << endl;

		string name; int oldTotal;
		while (!in.eof()) {

			if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

			in >> name; File::gobble(in); in >> oldTotal; File::gobble(in);
			if (app.isDebug) { LOG(DEBUG) << "" + name + '\t' + toString(oldTotal) + "\n"; }

			if (names.count(name) == 0) {
				//if group info, then read it
				vector<int> selectedCounts; int thisTotal = 0; int temp;
				for (int i = 0; i < groups.size(); i++) {
					int thisIndex = GroupIndexes[originalGroupIndexes[i]];
					in >> temp;  File::gobble(in);
					if (indexOfGroupsChosen.count(thisIndex) != 0) { //we want this group
						selectedCounts.push_back(temp); thisTotal += temp;
					}
				}

				out << name << '\t' << thisTotal;
				for (int i = 0; i < selectedCounts.size(); i++) { out << '\t' << selectedCounts[i]; }
				out << endl;

				wroteSomething = true;
				removedCount += (oldTotal - thisTotal);
			}
			else { File::getline(in); removedCount += oldTotal; }

			File::gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) { LOG(INFO) << "Your file does NOT contain sequences from the groups you wish to get." << '\n'; }
		outputTypes["count"].push_back(outputFileName); outputNames.push_back(outputFileName);

		LOG(INFO) << "Removed " + toString(removedCount) + " sequences from your count file." << '\n';

		return 0;
	}
	//**********************************************************************************************************************
	int RemoveGroupsCommand::readDesign() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(designfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(designfile));
		variables["[extension]"] = m->getExtension(designfile);
		string outputFileName = getOutputFileName("design", variables);

		DesignMap designMap(designfile);

		vector<string> groupsToKeep;
		vector<string> allGroups = designMap.getNamesGroups();
		for (int i = 0; i < allGroups.size(); i++) {
			if (!m->inUsersGroups(allGroups[i], Groups)) {
				groupsToKeep.push_back(allGroups[i]);
			}
		}

		bool wroteSomething = false;

		ofstream out;
		File::openOutputFile(outputFileName, out);

		int numGroupsFound = designMap.printGroups(out, groupsToKeep);

		if (numGroupsFound > 0) { wroteSomething = true; }

		out.close();

		int removedCount = allGroups.size() - numGroupsFound;

		if (wroteSomething == false) { LOG(INFO) << "Your file contains only groups from the groups you wish to remove." << '\n'; }
		outputTypes["design"].push_back(outputFileName); outputNames.push_back(outputFileName);

		LOG(INFO) << "Removed " + toString(removedCount) + " groups from your design file." << '\n';


		return 0;
	}

	//**********************************************************************************************************************
	int RemoveGroupsCommand::readTax() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(taxfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(taxfile));
		variables["[extension]"] = m->getExtension(taxfile);
		string outputFileName = getOutputFileName("taxonomy", variables);
		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(taxfile, in);
		string name, tax;

		bool wroteSomething = false;
		int removedCount = 0;

		while (!in.eof()) {
			if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

			in >> name;				//read from first column
			in >> tax;			//read from second column

			//if this name is in the accnos file
			if (names.count(name) == 0) {
				wroteSomething = true;
				out << name << '\t' << tax << endl;
			}
			else {  //if you are not in the accnos file check if you are a name that needs to be changed
				map<string, string>::iterator it = uniqueToRedundant.find(name);
				if (it != uniqueToRedundant.end()) {
					wroteSomething = true;
					out << it->second << '\t' << tax << endl;
				}
				else { removedCount++; }
			}

			File::gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) { LOG(INFO) << "Your file contains only sequences from the groups you wish to remove." << '\n'; }
		outputTypes["taxonomy"].push_back(outputFileName); outputNames.push_back(outputFileName);

		LOG(INFO) << "Removed " + toString(removedCount) + " sequences from your taxonomy file." << '\n';

		return 0;
	}
	//**********************************************************************************************************************
	int RemoveGroupsCommand::readPhylip() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(phylipfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(phylipfile));
		variables["[extension]"] = m->getExtension(phylipfile);
		string outputFileName = getOutputFileName("phylip", variables);

		ifstream in;
		File::openInputFile(phylipfile, in);

		float distance;
		int square, nseqs;
		string name;
		unsigned int row;
		set<unsigned int> rows; //converts names in names to a index
		row = 0;

		string numTest;
		in >> numTest >> name;

		if (!m->isContainingOnlyDigits(numTest)) { LOG(LOGERROR) << "expected a number and got " + numTest + ", quitting." << '\n'; exit(1); }
		else { convert(numTest, nseqs); }

		//not one we want to remove
		if (names.count(name) == 0) { rows.insert(row); }
		row++;

		//is the matrix square?
		char d;
		while ((d = in.get()) != EOF) {

			if (isalnum(d)) {
				square = 1;
				in.putback(d);
				for (int i = 0;i < nseqs;i++) {
					in >> distance;
				}
				break;
			}
			if (d == '\n') {
				square = 0;
				break;
			}
		}

		//map name to row/column
		if (square == 0) {
			for (int i = 1;i < nseqs;i++) {
				in >> name;
				if (names.count(name) == 0) { rows.insert(row); }
				row++;

				for (int j = 0;j < i;j++) {
					if (ctrlc_pressed) { in.close(); return 0; }
					in >> distance;
				}
			}
		}
		else {
			for (int i = 1;i < nseqs;i++) {
				in >> name;
				if (names.count(name) == 0) { rows.insert(row); }
				row++;
				for (int j = 0;j < nseqs;j++) {
					if (ctrlc_pressed) { in.close(); return 0; }
					in >> distance;
				}
			}
		}
		in.close();

		if (ctrlc_pressed) { return 0; }

		//read through file only printing rows and columns of seqs in names
		ifstream inPhylip;
		File::openInputFile(phylipfile, inPhylip);

		inPhylip >> numTest;

		ofstream out;
		File::openOutputFile(outputFileName, out);
		outputTypes["phylip"].push_back(outputFileName);  outputNames.push_back(outputFileName);
		out << (nseqs - names.size()) << endl;

		unsigned int count = 0;
		unsigned int keptCount = 0;
		if (square == 0) {
			for (int i = 0;i < nseqs;i++) {
				inPhylip >> name;
				bool ignoreRow = false;

				if (names.count(name) != 0) { ignoreRow = true; count++; }
				else { out << name; keptCount++; }

				for (int j = 0;j < i;j++) {
					if (ctrlc_pressed) { inPhylip.close(); out.close();  return 0; }
					inPhylip >> distance;
					if (!ignoreRow) {
						//is this a column we want
						if (rows.count(j) != 0) { out << '\t' << distance; }
					}
				}
				if (!ignoreRow) { out << endl; }
			}
		}
		else {
			for (int i = 0;i < nseqs;i++) {
				inPhylip >> name;

				bool ignoreRow = false;

				if (names.count(name) != 0) { ignoreRow = true; count++; }
				else { out << name; keptCount++; }

				for (int j = 0;j < nseqs;j++) {
					if (ctrlc_pressed) { inPhylip.close(); out.close(); return 0; }
					inPhylip >> distance;
					if (!ignoreRow) {
						//is this a column we want
						if (rows.count(j) != 0) { out << '\t' << distance; }
					}
				}
				if (!ignoreRow) { out << endl; }
			}
		}
		inPhylip.close();
		out.close();

		if (keptCount == 0) { LOG(INFO) << "Your file contains ONLY distances related to groups or sequences listed in the accnos file." << '\n'; }
		else if (count != names.size()) {
			LOG(WARNING) << "Your accnos file contains " + toString(names.size()) + " groups or sequences, but I only found " + toString(count) + " of them in the phylip file." << '\n';
			//rewrite with new number
			m->renameFile(outputFileName, outputFileName + ".temp");
			ofstream out2;
			File::openOutputFile(outputFileName, out2);
			out2 << keptCount << endl;

			ifstream in3;
			File::openInputFile(outputFileName + ".temp", in3);
			in3 >> nseqs; File::gobble(in3);
			char buffer[4096];
			while (!in3.eof()) {
				in3.read(buffer, 4096);
				out2.write(buffer, in3.gcount());
			}
			in3.close();
			out2.close();
			File::remove(outputFileName + ".temp");
		}

		LOG(INFO) << "Removed " + toString(count) + " groups or sequences from your phylip file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************
	int RemoveGroupsCommand::readColumn() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(columnfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(columnfile));
		variables["[extension]"] = m->getExtension(columnfile);
		string outputFileName = getOutputFileName("column", variables);
		outputTypes["column"].push_back(outputFileName);  outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(columnfile, in);

		set<string> removeNames;
		string firstName, secondName;
		float distance;
		bool wrote = false;
		while (!in.eof()) {

			if (ctrlc_pressed) { out.close(); in.close(); return 0; }

			in >> firstName >> secondName >> distance; File::gobble(in);

			//is either names in the accnos file
			if (names.count(firstName) != 0) {
				removeNames.insert(firstName);
				if (names.count(secondName) != 0) { removeNames.insert(secondName); }
			}
			else if (names.count(secondName) != 0) {
				removeNames.insert(secondName);
				if (names.count(firstName) != 0) { removeNames.insert(firstName); }
			}
			else {
				wrote = true;
				out << firstName << '\t' << secondName << '\t' << distance << endl;
			}
		}
		in.close();
		out.close();

		if (!wrote) { LOG(INFO) << "Your file contains ONLY distances related to groups or sequences listed in the accnos file." << '\n'; }
		else if (removeNames.size() != names.size()) {
			LOG(WARNING) << "Your accnos file contains " + toString(names.size()) + " groups or sequences, but I only found " + toString(removeNames.size()) + " of them in the column file." << '\n';
		}

		LOG(INFO) << "Removed " + toString(removeNames.size()) + " groups or sequences from your column file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************
	int RemoveGroupsCommand::fillNames() {
		vector<string> seqs = groupMap->getNamesSeqs();

		for (int i = 0; i < seqs.size(); i++) {

			if (ctrlc_pressed) { return 0; }

			string group = groupMap->getGroup(seqs[i]);

			if (m->inUsersGroups(group, Groups)) {
				names.insert(seqs[i]);
			}
		}

		return 0;
	}

	//**********************************************************************************************************************



