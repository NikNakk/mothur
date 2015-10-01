/*
 *  countgroupscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 8/9/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "countgroupscommand.h"
#include "sharedutilities.h"
#include "inputdata.h"

 //**********************************************************************************************************************
vector<string> CountGroupsCommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "sharedGroup", "sharedGroup", "none", "summary", false, false, true); parameters.push_back(pshared);
		CommandParameter pgroup("group", "InputTypes", "", "", "sharedGroup", "sharedGroup", "none", "summary", false, false, true); parameters.push_back(pgroup);
		CommandParameter pcount("count", "InputTypes", "", "", "sharedGroup", "sharedGroup", "none", "summary", false, false, true); parameters.push_back(pcount);
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
		LOG(FATAL) << e.what() << " in CountGroupsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string CountGroupsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "summary") { pattern = "[filename],count.summary"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
string CountGroupsCommand::getHelpString() {
	try {
		string helpString = "The count.groups command counts sequences from a specific group or set of groups from the following file types: group, count or shared file.\n"
			"The count.groups command parameters are accnos, group, shared and groups. You must provide a group or shared file.\n"
			"The accnos parameter allows you to provide a file containing the list of groups.\n"
			"The groups parameter allows you to specify which of the groups in your groupfile you would like.  You can separate group names with dashes.\n"
			"The count.groups command should be in the following format: count.groups(accnos=yourAccnos, group=yourGroupFile).\n"
			"Example count.groups(accnos=amazon.accnos, group=amazon.groups).\n"
			"or count.groups(groups=pasture, group=amazon.groups).\n"
			"Note: No spaces between parameter labels (i.e. group), '=' and parameters (i.e.yourGroupFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CountGroupsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
CountGroupsCommand::CountGroupsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["summary"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CountGroupsCommand, CountGroupsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
CountGroupsCommand::CountGroupsCommand(Settings& settings, string option) : Command(settings, option) {
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

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("accnos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["accnos"] = inputDir + it->second; }
			}

			it = parameters.find("group");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["group"] = inputDir + it->second; }
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

		vector<string> tempOutNames;
		outputTypes["summary"] = tempOutNames;

		//check for required parameters
		accnosfile = validParameter.validFile(parameters, "accnos", true);
		if (accnosfile == "not open") { abort = true; }
		else if (accnosfile == "not found") { accnosfile = ""; }
		else { settings.setCurrent("accnos", accnosfile); }

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

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { groupfile = ""; abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else {
			settings.setCurrent("counttable", countfile);
			CountTable ct;
			if (!ct.testGroups(countfile)) { LOG(LOGERROR) << "Your count file does not have any group information, aborting." << '\n'; abort = true; }
		}

		if ((groupfile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: group or count." << '\n'; abort = true;
		}


		if ((sharedfile == "") && (groupfile == "") && (countfile == "")) {
			//give priority to shared, then group
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else {
				groupfile = settings.getCurrent("group");
				if (groupfile != "") { LOG(INFO) << "Using " + groupfile + " as input file for the group parameter." << '\n'; }
				else {
					countfile = settings.getCurrent("counttable");
					if (countfile != "") { LOG(INFO) << "Using " + countfile + " as input file for the count parameter." << '\n'; }
					else {
						LOG(INFO) << "You have no current groupfile, countfile or sharedfile and one is required." << '\n'; abort = true;
					}
				}
			}
		}

		if ((accnosfile == "") && (Groups.size() == 0)) { Groups.push_back("all"); m->setGroups(Groups); }
	}

}
//**********************************************************************************************************************

int CountGroupsCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		//get groups you want to remove
		if (accnosfile != "") { m->readAccnos(accnosfile, Groups); m->setGroups(Groups); }

		if (groupfile != "") {
			map<string, string> variables;
			string thisOutputDir = outputDir;
			if (outputDir == "") { thisOutputDir += File::getPath(groupfile); }
			variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(groupfile));
			string outputFileName = getOutputFileName("summary", variables);
			outputNames.push_back(outputFileName); outputTypes["summary"].push_back(outputFileName);
			ofstream out;
			File::openOutputFile(outputFileName, out);

			GroupMap groupMap(groupfile);
			groupMap.readMap();

			//make sure groups are valid
			//takes care of user setting groupNames that are invalid or setting groups=all
			SharedUtil util;
			vector<string> nameGroups = groupMap.getNamesOfGroups();
			util.setGroups(Groups, nameGroups);

			int total = 0;
			for (int i = 0; i < Groups.size(); i++) {
				int num = groupMap.getNumSeqs(Groups[i]);
				total += num;
				LOG(INFO) << Groups[i] + " contains " + toString(num) + "." << '\n';
				out << Groups[i] << '\t' << num << endl;
			}
			out.close();
			LOG(INFO) << "\nTotal seqs: " + toString(total) + "." << '\n';
		}

		if (ctrlc_pressed) { return 0; }

		if (countfile != "") {
			map<string, string> variables;
			string thisOutputDir = outputDir;
			if (outputDir == "") { thisOutputDir += File::getPath(countfile); }
			variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(countfile));
			string outputFileName = getOutputFileName("summary", variables);
			outputNames.push_back(outputFileName); outputTypes["summary"].push_back(outputFileName);
			ofstream out;
			File::openOutputFile(outputFileName, out);

			CountTable ct;
			ct.readTable(countfile, true, false);

			//make sure groups are valid
			//takes care of user setting groupNames that are invalid or setting groups=all
			SharedUtil util;
			vector<string> nameGroups = ct.getNamesOfGroups();
			util.setGroups(Groups, nameGroups);

			int total = 0;
			for (int i = 0; i < Groups.size(); i++) {
				int num = ct.getGroupCount(Groups[i]);
				total += num;
				LOG(INFO) << Groups[i] + " contains " + toString(num) + "." << '\n';
				out << Groups[i] << '\t' << num << endl;
			}
			out.close();

			LOG(INFO) << "\nTotal seqs: " + toString(total) + "." << '\n';
		}

		if (ctrlc_pressed) { return 0; }

		if (sharedfile != "") {
			InputData input(sharedfile, "sharedfile");
			vector<SharedRAbundVector*> lookup = input.getSharedRAbundVectors();

			map<string, string> variables;
			string thisOutputDir = outputDir;
			if (outputDir == "") { thisOutputDir += File::getPath(sharedfile); }
			variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(sharedfile));
			string outputFileName = getOutputFileName("summary", variables);
			outputNames.push_back(outputFileName); outputTypes["summary"].push_back(outputFileName);
			ofstream out;
			File::openOutputFile(outputFileName, out);

			int total = 0;
			for (int i = 0; i < lookup.size(); i++) {
				int num = lookup[i]->getNumSeqs();
				total += num;
				LOG(INFO) << lookup[i]->getGroup() + " contains " + toString(num) + "." << '\n';
				out << lookup[i]->getGroup() << '\t' << num << endl;
				delete lookup[i];
			}
			out.close();

			LOG(INFO) << "\nTotal seqs: " + toString(total) + "." << '\n';
		}

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		return 0;
	}

	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CountGroupsCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************


