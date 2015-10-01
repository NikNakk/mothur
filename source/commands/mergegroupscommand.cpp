/*
 *  mergegroupscommand.cpp
 *  mothur
 *
 *  Created by westcott on 1/24/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "mergegroupscommand.h"
#include "sharedutilities.h"
#include "counttable.h"
#include "removeseqscommand.h"

 //**********************************************************************************************************************
vector<string> MergeGroupsCommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "none", "sharedGroup", "none", "shared", false, false, true); parameters.push_back(pshared);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "sharedGroup", "none", "group", false, false, true); parameters.push_back(pgroup);
		CommandParameter pcount("count", "InputTypes", "", "", "CountGroup", "sharedGroup", "countfasta", "count", false, false, true); parameters.push_back(pcount);
		CommandParameter pdesign("design", "InputTypes", "", "", "none", "none", "none", "", false, true, true); parameters.push_back(pdesign);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "countfasta", "fasta", false, false, true); parameters.push_back(pfasta);
		nkParameters.add(new MultipleParameter("method", vector<string>{"sum", "average", "median"}, "sum", false, false, true));
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MergeGroupsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string MergeGroupsCommand::getHelpString() {
	try {
		string helpString = "The merge.groups command input files are shared, group, count, fasta and a design file.  It reads the design file and merges the groups in the other files accordingly.\n"
			"The design parameter allows you to assign your groups to sets. It is required. \n"
			"The fasta parameter allows you to provide a fasta file associated with your count file.  This is used if you are using the median method, so that sequences that are entirely removed from the counttable will also be removed from the fasta file. \n"
			"The groups parameter allows you to specify which of the groups in your shared or group file you would like included. The group names are separated by dashes. By default all groups are selected.\n"
			"The label parameter allows you to select what distance levels you would like, and are also separated by dashes.\n"
			"The groups parameter allows you to select groups you would like, and are also separated by dashes.\n"
			"The method parameter allows you to select method you would like to use to merge the groups. Options are sum, average and median. Default=sum.\n"
			"The merge.groups command should be in the following format: merge.groups(design=yourDesignFile, shared=yourSharedFile).\n"
			"Example merge.groups(design=temp.design, groups=A-B-C, shared=temp.shared).\n"
			"The default value for groups is all the groups in your sharedfile, and all labels in your inputfile will be used.\n"
			"Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MergeGroupsCommand, getHelpString";
		exit(1);
	}
}

//**********************************************************************************************************************
string MergeGroupsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "shared") { pattern = "[filename],merge,[extension]"; }
	else if (type == "group") { pattern = "[filename],merge,[extension]"; }
	else if (type == "count") { pattern = "[filename],merge,[extension]"; }
	else if (type == "fasta") { pattern = "[filename],merge,[extension]"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
MergeGroupsCommand::MergeGroupsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["shared"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["fasta"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MergeGroupsCommand, MergeGroupsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

MergeGroupsCommand::MergeGroupsCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["group"] = tempOutNames;
		outputTypes["count"] = tempOutNames;

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("design");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["design"] = inputDir + it->second; }
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

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}

			it = parameters.find("fasta");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fasta"] = inputDir + it->second; }
			}

		}

		//check for required parameters
		designfile = validParameter.validFile(parameters, "design", true);
		if (designfile == "not open") { abort = true; }
		else if (designfile == "not found") {
			//if there is a current shared file, use it
			designfile = settings.getCurrent("design");
			if (designfile != "") { LOG(INFO) << "Using " + designfile + " as input file for the design parameter." << '\n'; }
			else { LOG(INFO) << "You have no current designfile and the design parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("design", designfile); }

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { abort = true; sharedfile = ""; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { settings.setCurrent("shared", sharedfile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { abort = true; groupfile = ""; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { abort = true; countfile = ""; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not open") { abort = true; countfile = ""; }
		else if (fastafile == "not found") { fastafile = ""; }
		else { settings.setCurrent("fasta", fastafile); }


		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = "all"; }
		Utility::split(groups, '-', Groups);
		m->setGroups(Groups);

		method = validParameter.validFile(parameters, "method", false);		if (method == "not found") { method = "sum"; }

		if ((method != "sum") && (method != "average") && (method != "median")) { LOG(INFO) << method + " is not a valid method. Options are sum, average and median. I will use sum." << '\n'; method = "sum"; }


		if ((groupfile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: group or count." << '\n'; abort = true;
		}

		if ((sharedfile == "") && (groupfile == "") && (countfile == "")) {
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

		if ((countfile == "") && (fastafile != "")) { LOG(LOGERROR) << "You may only use the fasta file with the count file, quitting." << '\n'; abort = true; }
		else if ((countfile != "") && (method == "average")) { LOG(INFO) << "You may not use the average method with the count file. I will use the sum method." << '\n'; method = "sum"; }
		else if ((countfile != "") && (method == "median") && (fastafile == "")) {
			fastafile = settings.getCurrent("fasta");
			if (fastafile != "") { LOG(INFO) << "Using " + fastafile + " as input file for the fasta parameter." << '\n'; }
			else {
				LOG(LOGERROR) << "Fasta file is required with the median method and a count file so that sequences removed from your count table can also be removed from your fasta file to avoid downstream file mismatches, quitting.\n"; abort = true;
			}
		}

	}

}
//**********************************************************************************************************************

int MergeGroupsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	designMap = new DesignMap(designfile);

	if (method != "sum") {
		string defaultClass = designMap->getDefaultClass();
		vector<string> treatments = designMap->getCategory(defaultClass);
		set<int> numGroupsPerTreatment;
		for (int i = 0; i < treatments.size(); i++) {
			if (ctrlc_pressed) { break; }
			map<string, vector<string> > checkTreatments;
			vector<string> temp; temp.push_back(treatments[i]);
			checkTreatments[defaultClass] = temp;
			numGroupsPerTreatment.insert(designMap->getNumUnique(checkTreatments));
		}
		if (numGroupsPerTreatment.size() > 1) { LOG(LOGERROR) << "The median and average methods require you to have the same number of sequences in each treatment, quitting.\n"; delete designMap; return 0; }
	}

	if (groupfile != "") { processGroupFile(designMap); }
	if (sharedfile != "") { processSharedFile(designMap); }
	if (countfile != "") { processCountFile(designMap); }

	//reset groups parameter
	m->clearGroups();
	delete designMap;

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }


	//set shared file as new current sharedfile
	string current = "";
	itTypes = outputTypes.find("shared");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("shared", current); }
	}

	itTypes = outputTypes.find("group");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("group", current); }
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
//**********************************************************************************************************************

int MergeGroupsCommand::process(vector<SharedRAbundVector*>& thisLookUp, ofstream& out) {
	if (method == "average") {
		//create sharedRabundFloatVectors
		vector<SharedRAbundFloatVector*> temp = thisLookUp[0]->getSharedRAbundFloatVectors(thisLookUp);

		//follow code below
		map<string, SharedRAbundFloatVector> merged;
		map<string, SharedRAbundFloatVector>::iterator it;
		map<string, vector<int> > clearGroupAbunds;
		map<string, vector<int> >::iterator itAbunds;

		for (int i = 0; i < temp.size(); i++) {
			if (ctrlc_pressed) { return 0; }
			//what grouping does this group belong to
			string grouping = designMap->get(temp[i]->getGroup());
			if (grouping == "not found") { LOG(LOGERROR) << "" + temp[i]->getGroup() + " is not in your design file. Ignoring!" << '\n'; grouping = "NOTFOUND"; }
			else {
				//do we already have a member of this grouping?
				it = merged.find(grouping);

				if (it == merged.end()) { //nope, so create it
					merged[grouping] = *temp[i];
					merged[grouping].setGroup(grouping);
					vector<int> temp;
					clearGroupAbunds[grouping] = temp;
				}
			}
		}

		for (int j = 0; j < temp[0]->getNumBins(); j++) {
			if (ctrlc_pressed) { return 0; }

			map<string, vector<int> > otusGroupAbunds = clearGroupAbunds;
			for (int i = 0; i < temp.size(); i++) {

				string grouping = designMap->get(temp[i]->getGroup());
				if (grouping == "not found") { LOG(LOGERROR) << "" + temp[i]->getGroup() + " is not in your design file. Ignoring!" << '\n'; grouping = "NOTFOUND"; }
				else {
					otusGroupAbunds[grouping].push_back(temp[i]->getAbundance(j));
				}
			}

			for (itAbunds = otusGroupAbunds.begin(); itAbunds != otusGroupAbunds.end(); itAbunds++) {
				int abund = mergeAbund(itAbunds->second);
				merged[itAbunds->first].set(j, abund, itAbunds->first);
			}
		}

		if (method == "median") {
			vector<SharedRAbundFloatVector*> temp2;
			for (it = merged.begin(); it != merged.end(); it++) { temp2.push_back(&(it->second)); }
			eliminateZeroOTUS(temp2);
		}

		//print new file
		for (it = merged.begin(); it != merged.end(); it++) {
			if (!m->printedSharedHeaders) { (it->second).printHeaders(out); }
			out << (it->second).getLabel() << '\t' << it->first << '\t';
			(it->second).print(out);
		}
	}
	else {
		map<string, SharedRAbundVector> merged;
		map<string, SharedRAbundVector>::iterator it;
		map<string, vector<int> > clearGroupAbunds;
		map<string, vector<int> >::iterator itAbunds;

		for (int i = 0; i < thisLookUp.size(); i++) {
			if (ctrlc_pressed) { return 0; }
			//what grouping does this group belong to
			string grouping = designMap->get(thisLookUp[i]->getGroup());
			if (grouping == "not found") { LOG(LOGERROR) << "" + thisLookUp[i]->getGroup() + " is not in your design file. Ignoring!" << '\n'; grouping = "NOTFOUND"; }
			else {
				//do we already have a member of this grouping?
				it = merged.find(grouping);

				if (it == merged.end()) { //nope, so create it
					merged[grouping] = *thisLookUp[i];
					merged[grouping].setGroup(grouping);
					vector<int> temp;
					clearGroupAbunds[grouping] = temp;
				}
			}
		}

		for (int j = 0; j < thisLookUp[0]->getNumBins(); j++) {
			if (ctrlc_pressed) { return 0; }

			map<string, vector<int> > otusGroupAbunds = clearGroupAbunds;
			for (int i = 0; i < thisLookUp.size(); i++) {

				string grouping = designMap->get(thisLookUp[i]->getGroup());
				if (grouping == "not found") { LOG(LOGERROR) << "" + thisLookUp[i]->getGroup() + " is not in your design file. Ignoring!" << '\n'; grouping = "NOTFOUND"; }
				else {
					otusGroupAbunds[grouping].push_back(thisLookUp[i]->getAbundance(j));
				}
			}

			for (itAbunds = otusGroupAbunds.begin(); itAbunds != otusGroupAbunds.end(); itAbunds++) {
				int abund = mergeAbund(itAbunds->second);
				merged[itAbunds->first].set(j, abund, itAbunds->first);
			}
		}

		if (method == "median") {
			vector<SharedRAbundVector*> temp;
			for (it = merged.begin(); it != merged.end(); it++) { temp.push_back(&(it->second)); }
			eliminateZeroOTUS(temp);
		}

		//print new file
		for (it = merged.begin(); it != merged.end(); it++) {
			if (!m->printedSharedHeaders) { (it->second).printHeaders(out); }
			out << (it->second).getLabel() << '\t' << it->first << '\t';
			(it->second).print(out);
		}
	}
	return 0;

}
//**********************************************************************************************************************

int MergeGroupsCommand::processSharedFile(DesignMap*& designMap) {

	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(sharedfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(sharedfile));
	variables["[extension]"] = m->getExtension(sharedfile);
	string outputFileName = getOutputFileName("shared", variables);
	outputTypes["shared"].push_back(outputFileName); outputNames.push_back(outputFileName);

	ofstream out;
	File::openOutputFile(outputFileName, out);

	InputData input(sharedfile, "sharedfile");
	lookup = input.getSharedRAbundVectors();
	string lastLabel = lookup[0]->getLabel();

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	//as long as you are not at the end of the file or done wih the lines you want
	while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

		if (ctrlc_pressed) { out.close(); for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } m->clearGroups();  delete designMap;  for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

		if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

			LOG(INFO) << lookup[0]->getLabel() << '\n';
			process(lookup, out);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());
		}

		if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = lookup[0]->getLabel();

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input.getSharedRAbundVectors(lastLabel);
			LOG(INFO) << lookup[0]->getLabel() << '\n';

			process(lookup, out);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());

			//restore real lastlabel to save below
			lookup[0]->setLabel(saveLabel);
		}

		lastLabel = lookup[0]->getLabel();
		//prevent memory leak
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }

		if (ctrlc_pressed) { out.close(); m->clearGroups();   delete designMap;  for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

		//get next line to process
		lookup = input.getSharedRAbundVectors();
	}

	if (ctrlc_pressed) { out.close(); m->clearGroups();  delete designMap;  for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }

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
		process(lookup, out);

		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
	}

	out.close();


	return 0;

}
//**********************************************************************************************************************

int MergeGroupsCommand::processGroupFile(DesignMap*& designMap) {

	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(groupfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(groupfile));
	variables["[extension]"] = m->getExtension(groupfile);
	string outputFileName = getOutputFileName("group", variables);
	outputTypes["group"].push_back(outputFileName); outputNames.push_back(outputFileName);

	ofstream out;
	File::openOutputFile(outputFileName, out);

	//read groupfile
	GroupMap groupMap(groupfile);
	groupMap.readMap();

	//fill Groups - checks for "all" and for any typo groups
	SharedUtil* util = new SharedUtil();
	vector<string> nameGroups = groupMap.getNamesOfGroups();
	util->setGroups(Groups, nameGroups);
	delete util;

	vector<string> namesOfSeqs = groupMap.getNamesSeqs();
	bool error = false;

	for (int i = 0; i < namesOfSeqs.size(); i++) {

		if (ctrlc_pressed) { break; }

		string thisGroup = groupMap.getGroup(namesOfSeqs[i]);

		//are you in a group the user wants
		if (m->inUsersGroups(thisGroup, Groups)) {
			string thisGrouping = designMap->get(thisGroup);

			if (thisGrouping == "not found") { LOG(LOGERROR) << "" + namesOfSeqs[i] + " is from group " + thisGroup + " which is not in your design file, please correct." << '\n';  error = true; }
			else {
				out << namesOfSeqs[i] << '\t' << thisGrouping << endl;
			}
		}
	}

	if (error) { ctrlc_pressed = true; }

	out.close();

	return 0;

}
//**********************************************************************************************************************

int MergeGroupsCommand::processCountFile(DesignMap*& designMap) {
	CountTable countTable;
	if (!countTable.testGroups(countfile)) { LOG(LOGERROR) << "your countfile contains no group information, please correct.\n"; ctrlc_pressed = true; return 0; }

	//read countTable
	countTable.readTable(countfile, true, false);

	//fill Groups - checks for "all" and for any typo groups
	SharedUtil util;
	vector<string> nameGroups = countTable.getNamesOfGroups();
	util.setGroups(Groups, nameGroups);

	vector<string> dnamesGroups = designMap->getNamesGroups();

	//sanity check
	bool error = false;
	if (nameGroups.size() == dnamesGroups.size()) { //at least there are the same number
		//is every group in counttable also in designmap
		for (int i = 0; i < nameGroups.size(); i++) {
			if (ctrlc_pressed) { break; }
			if (!m->inUsersGroups(nameGroups[i], dnamesGroups)) { error = true; break; }
		}

	}
	if (error) { LOG(LOGERROR) << "Your countfile does not contain the same groups as your design file, please correct\n"; ctrlc_pressed = true; return 0; }

	//user selected groups - remove some groups from table
	if (Groups.size() != nameGroups.size()) {
		for (int i = 0; i < nameGroups.size(); i++) {
			if (!m->inUsersGroups(nameGroups[i], Groups)) { countTable.removeGroup(nameGroups[i]); }
		}
	}
	//ask again in case order changed
	nameGroups = countTable.getNamesOfGroups();
	int numGroups = nameGroups.size();

	//create new table
	CountTable newTable;
	vector<string> treatments = designMap->getCategory();
	map<string, vector<int> > clearedMap;
	for (int i = 0; i < treatments.size(); i++) {
		newTable.addGroup(treatments[i]);
		vector<int> temp;
		clearedMap[treatments[i]] = temp;
	}
	treatments = newTable.getNamesOfGroups();

	set<string> namesToRemove;
	vector<string> namesOfSeqs = countTable.getNamesOfSeqs();
	for (int i = 0; i < namesOfSeqs.size(); i++) {

		if (ctrlc_pressed) { break; }

		vector<int> thisSeqsCounts = countTable.getGroupCounts(namesOfSeqs[i]);
		map<string, vector<int> > thisSeqsMap = clearedMap;

		for (int j = 0; j < numGroups; j++) {
			thisSeqsMap[designMap->get(nameGroups[j])].push_back(thisSeqsCounts[j]);
		}

		//create new counts for seq for new table
		vector<int> newCounts; int totalAbund = 0;
		for (int j = 0; j < treatments.size(); j++) {
			int abund = mergeAbund(thisSeqsMap[treatments[j]]);
			newCounts.push_back(abund);  //order matters, add in count for each treatment in new table.
			totalAbund += abund;
		}

		//add seq to new table
		if (totalAbund == 0) {
			namesToRemove.insert(namesOfSeqs[i]);
		}
		else { newTable.push_back(namesOfSeqs[i], newCounts); }
	}

	if (error) { ctrlc_pressed = true; return 0; }

	//remove sequences zeroed out by median method
	if (namesToRemove.size() != 0) {
		//print names
		ofstream out;
		string accnosFile = "accnosFile.temp";
		File::openOutputFile(accnosFile, out);

		//output to .accnos file
		for (set<string>::iterator it = namesToRemove.begin(); it != namesToRemove.end(); it++) {
			if (ctrlc_pressed) { out.close(); File::remove(accnosFile); return 0; }
			out << *it << endl;
		}
		out.close();

		//run remove.seqs
		string inputString = "accnos=" + accnosFile + ", fasta=" + fastafile;

		LOG(INFO) << "/******************************************/" << '\n';
		LOG(INFO) << "Running command: remove.seqs(" + inputString + ")" << '\n';
		m->mothurCalling = true;

		Command* removeCommand = new RemoveSeqsCommand(inputString);
		removeCommand->execute();

		map<string, vector<string> > filenames = removeCommand->getOutputFiles();

		delete removeCommand;
		m->mothurCalling = false;
		LOG(INFO) << "/******************************************/" << '\n';

		File::remove(accnosFile);
	}

	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(countfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(countfile));
	variables["[extension]"] = m->getExtension(countfile);
	string outputFileName = getOutputFileName("count", variables);
	outputTypes["count"].push_back(outputFileName); outputNames.push_back(outputFileName);

	newTable.printTable(outputFileName);

	return 0;

}
//**********************************************************************************************************************

int MergeGroupsCommand::mergeAbund(vector<int> values) {
	int abund = 0;

	if (method == "sum") {
		abund = m->sum(values);
	}
	else if (method == "average") {
		abund = m->average(values);
	}
	else if (method == "median") {
		abund = m->median(values);
	}
	else {
		LOG(LOGERROR) << "Invalid method. \n"; ctrlc_pressed = true; return 0;
	}

	return abund;
}
//**********************************************************************************************************************
int MergeGroupsCommand::eliminateZeroOTUS(vector<SharedRAbundVector*>& thislookup) {

	vector<SharedRAbundVector*> newLookup;
	for (int i = 0; i < thislookup.size(); i++) {
		SharedRAbundVector* temp = new SharedRAbundVector();
		temp->setLabel(thislookup[i]->getLabel());
		temp->setGroup(thislookup[i]->getGroup());
		newLookup.push_back(temp);
	}

	//for each bin
	vector<string> newBinLabels;
	string snumBins = toString(thislookup[0]->getNumBins());
	for (int i = 0; i < thislookup[0]->getNumBins(); i++) {
		if (ctrlc_pressed) { for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; } return 0; }

		//look at each sharedRabund and make sure they are not all zero
		bool allZero = true;
		for (int j = 0; j < thislookup.size(); j++) {
			if (thislookup[j]->getAbundance(i) != 0) { allZero = false;  break; }
		}

		//if they are not all zero add this bin
		if (!allZero) {
			for (int j = 0; j < thislookup.size(); j++) {
				newLookup[j]->push_back(thislookup[j]->getAbundance(i), thislookup[j]->getGroup());
			}
			//if there is a bin label use it otherwise make one
			string binLabel = "Otu";
			string sbinNumber = toString(i + 1);
			if (sbinNumber.length() < snumBins.length()) {
				int diff = snumBins.length() - sbinNumber.length();
				for (int h = 0; h < diff; h++) { binLabel += "0"; }
			}
			binLabel += sbinNumber;
			if (i < settings.currentSharedBinLabels.size()) { binLabel = settings.currentSharedBinLabels[i]; }

			newBinLabels.push_back(binLabel);
		}
	}

	for (int j = 0; j < thislookup.size(); j++) { delete thislookup[j]; }
	thislookup.clear();

	thislookup = newLookup;
	settings.currentSharedBinLabels = newBinLabels;

	return 0;

}
//**********************************************************************************************************************
int MergeGroupsCommand::eliminateZeroOTUS(vector<SharedRAbundFloatVector*>& thislookup) {

	vector<SharedRAbundFloatVector*> newLookup;
	for (int i = 0; i < thislookup.size(); i++) {
		SharedRAbundFloatVector* temp = new SharedRAbundFloatVector();
		temp->setLabel(thislookup[i]->getLabel());
		temp->setGroup(thislookup[i]->getGroup());
		newLookup.push_back(temp);
	}

	//for each bin
	vector<string> newBinLabels;
	string snumBins = toString(thislookup[0]->getNumBins());
	for (int i = 0; i < thislookup[0]->getNumBins(); i++) {
		if (ctrlc_pressed) { for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; } return 0; }

		//look at each sharedRabund and make sure they are not all zero
		bool allZero = true;
		for (int j = 0; j < thislookup.size(); j++) {
			if (thislookup[j]->getAbundance(i) != 0) { allZero = false;  break; }
		}

		//if they are not all zero add this bin
		if (!allZero) {
			for (int j = 0; j < thislookup.size(); j++) {
				newLookup[j]->push_back(thislookup[j]->getAbundance(i), thislookup[j]->getGroup());
			}
			//if there is a bin label use it otherwise make one
			string binLabel = "Otu";
			string sbinNumber = toString(i + 1);
			if (sbinNumber.length() < snumBins.length()) {
				int diff = snumBins.length() - sbinNumber.length();
				for (int h = 0; h < diff; h++) { binLabel += "0"; }
			}
			binLabel += sbinNumber;
			if (i < settings.currentSharedBinLabels.size()) { binLabel = settings.currentSharedBinLabels[i]; }

			newBinLabels.push_back(binLabel);
		}
	}

	for (int j = 0; j < thislookup.size(); j++) { delete thislookup[j]; }

	thislookup = newLookup;
	settings.currentSharedBinLabels = newBinLabels;

	return 0;

}
//**********************************************************************************************************************



