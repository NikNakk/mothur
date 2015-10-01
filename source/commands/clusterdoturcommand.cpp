/*
 *  clusterdoturcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 10/27/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "clusterdoturcommand.h"
#include "clusterclassic.h"

 //**********************************************************************************************************************
vector<string> ClusterDoturCommand::setParameters() {
	try {
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "none", "none", "list", false, true, true); parameters.push_back(pphylip);
		CommandParameter pname("name", "InputTypes", "", "", "namecount", "none", "none", "rabund-sabund", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "namecount", "none", "none", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pcutoff("cutoff", "Number", "", "10", "", "", "", "", false, false, true); parameters.push_back(pcutoff);
		nkParameters.add(new NumberParameter("precision", -INFINITY, INFINITY, 100, false, false));
		CommandParameter pmethod("method", "Multiple", "furthest-nearest-average-weighted", "average", "", "", "", "", false, false); parameters.push_back(pmethod);
		nkParameters.add(new BooleanParameter("hard", true, false, false));
		nkParameters.add(new BooleanParameter("sim", false, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterDoturCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterDoturCommand::getHelpString() {
	try {
		string helpString = "The cluster.classic command clusters using the algorithm from dotur. \n"
			"The cluster.classic command parameter options are phylip, name, count, method, cuttoff, hard, sim, precision. Phylip is required, unless you have a valid current file.\n"
			"The cluster.classic command should be in the following format: \n"
			"cluster.classic(phylip=yourDistanceMatrix, method=yourMethod, cutoff=yourCutoff, precision=yourPrecision) \n"
			"The acceptable cluster methods are furthest, nearest, weighted and average.  If no method is provided then average is assumed.\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterDoturCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterDoturCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "list") { pattern = "[filename],[clustertag],list-[filename],[clustertag],[tag2],list"; }
	else if (type == "rabund") { pattern = "[filename],[clustertag],rabund"; }
	else if (type == "sabund") { pattern = "[filename],[clustertag],sabund"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ClusterDoturCommand::ClusterDoturCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["rabund"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterDoturCommand, ClusterCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
//This function checks to make sure the cluster command has no errors and then clusters based on the method chosen.
ClusterDoturCommand::ClusterDoturCommand(Settings& settings, string option) : Command(settings, option) {
	try {

		abort = false; calledHelp = false;

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
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {
					abort = true;
				}
			}

			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);
			if (inputDir == "not found") { inputDir = ""; }
			else {
				string path;
				it = parameters.find("phylip");
				//user has given a template file
				if (it != parameters.end()) {
					path = File::getPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") { parameters["phylip"] = inputDir + it->second; }
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

			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["list"] = tempOutNames;
			outputTypes["rabund"] = tempOutNames;
			outputTypes["sabund"] = tempOutNames;

			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

			//check for required parameters
			phylipfile = validParameter.validFile(parameters, "phylip", true);
			if (phylipfile == "not open") { abort = true; }
			else if (phylipfile == "not found") {
				phylipfile = settings.getCurrent("phylip");
				if (phylipfile != "") { LOG(INFO) << "Using " + phylipfile + " as input file for the phylip parameter." << '\n'; }
				else {
					LOG(INFO) << "You need to provide a phylip file with the cluster.classic command." << '\n';
					abort = true;
				}
			}
			else { settings.setCurrent("phylip", phylipfile); }


			//check for optional parameter and set defaults
			namefile = validParameter.validFile(parameters, "name", true);
			if (namefile == "not open") { abort = true; namefile = ""; }
			else if (namefile == "not found") { namefile = ""; }
			else { settings.setCurrent("name", namefile); }

			countfile = validParameter.validFile(parameters, "count", true);
			if (countfile == "not open") { abort = true; countfile = ""; }
			else if (countfile == "not found") { countfile = ""; }
			else { settings.setCurrent("counttable", countfile); }

			if ((countfile != "") && (namefile != "")) { LOG(INFO) << "When executing a cluster.classic command you must enter ONLY ONE of the following: count or name." << '\n'; abort = true; }

			string temp;
			temp = validParameter.validFile(parameters, "precision", false);
			if (temp == "not found") { temp = "100"; }
			//saves precision legnth for formatting below
			length = temp.length();
			Utility::mothurConvert(temp, precision);

			temp = validParameter.validFile(parameters, "cutoff", false);
			if (temp == "not found") { temp = "10"; }
			Utility::mothurConvert(temp, cutoff);
			cutoff += (5 / (precision * 10.0));

			temp = validParameter.validFile(parameters, "hard", false);			if (temp == "not found") { temp = "T"; }
			hard = m->isTrue(temp);

			temp = validParameter.validFile(parameters, "sim", false);				if (temp == "not found") { temp = "F"; }
			sim = m->isTrue(temp);

			method = validParameter.validFile(parameters, "method", false);
			if (method == "not found") { method = "average"; }

			if ((method == "furthest") || (method == "nearest") || (method == "average") || (method == "weighted")) {
				if (method == "furthest") { tag = "fn"; }
				else if (method == "nearest") { tag = "nn"; }
				else if (method == "average") { tag = "an"; }
				else if (method == "weighted") { tag = "wn"; }
			}
			else { LOG(INFO) << "Not a valid clustering method.  Valid clustering algorithms are furthest, nearest, average, weighted." << '\n'; abort = true; }
		}
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterDoturCommand, ClusterCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

int ClusterDoturCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }


	ClusterClassic* cluster = new ClusterClassic(cutoff, method, sim);

	NameAssignment* nameMap = NULL;
	CountTable* ct = NULL;
	map<string, int> counts;
	if (namefile != "") {
		nameMap = new NameAssignment(namefile);
		nameMap->readMap();
		cluster->readPhylipFile(phylipfile, nameMap);
		delete nameMap;
	}
	else if (countfile != "") {
		ct = new CountTable();
		ct->readTable(countfile, false, false);
		cluster->readPhylipFile(phylipfile, ct);
		counts = ct->getNameMap();
		delete ct;
	}
	else {
		cluster->readPhylipFile(phylipfile, nameMap);
	}
	tag = cluster->getTag();

	if (ctrlc_pressed) { delete cluster; return 0; }

	list = cluster->getListVector();
	rabund = cluster->getRAbundVector();

	if (outputDir == "") { outputDir += File::getPath(phylipfile); }
	fileroot = outputDir + File::getRootName(File::getSimpleName(phylipfile));

	map<string, string> variables;
	variables["[filename]"] = fileroot;
	variables["[clustertag]"] = tag;
	string sabundFileName = getOutputFileName("sabund", variables);
	string rabundFileName = getOutputFileName("rabund", variables);
	if (countfile != "") { variables["[tag2]"] = "unique_list"; }
	string listFileName = getOutputFileName("list", variables);

	if (countfile == "") {
		File::openOutputFile(sabundFileName, sabundFile);
		File::openOutputFile(rabundFileName, rabundFile);
		outputNames.push_back(sabundFileName); outputTypes["sabund"].push_back(sabundFileName);
		outputNames.push_back(rabundFileName); outputTypes["rabund"].push_back(rabundFileName);

	}
	File::openOutputFile(listFileName, listFile);
	outputNames.push_back(listFileName); outputTypes["list"].push_back(listFileName);
	list->printHeaders(listFile);

	float previousDist = 0.00000;
	float rndPreviousDist = 0.00000;
	oldRAbund = *rabund;
	oldList = *list;

	//double saveCutoff = cutoff;

	int estart = time(NULL);

	while ((cluster->getSmallDist() < cutoff) && (cluster->getNSeqs() > 1)) {
		if (ctrlc_pressed) {
			delete cluster; delete list; delete rabund; if (countfile == "") { rabundFile.close(); sabundFile.close();  File::remove((fileroot + tag + ".rabund")); File::remove((fileroot + tag + ".sabund")); }
			listFile.close(); File::remove((fileroot + tag + ".list")); outputTypes.clear();  return 0;
		}

		cluster->update(cutoff);

		float dist = cluster->getSmallDist();
		float rndDist;
		if (hard) {
			rndDist = m->ceilDist(dist, precision);
		}
		else {
			rndDist = m->roundDist(dist, precision);
		}

		if (previousDist <= 0.0000 && dist != previousDist) {
			printData("unique", counts);
		}
		else if (rndDist != rndPreviousDist) {
			printData(toString(rndPreviousDist, length - 1), counts);
		}

		previousDist = dist;
		rndPreviousDist = rndDist;
		oldRAbund = *rabund;
		oldList = *list;
	}

	if (previousDist <= 0.0000) {
		printData("unique", counts);
	}
	else if (rndPreviousDist < cutoff) {
		printData(toString(rndPreviousDist, length - 1), counts);
	}

	if (countfile == "") {
		sabundFile.close();
		rabundFile.close();
	}
	listFile.close();

	delete cluster;  delete list; delete rabund;

	//set list file as new current listfile
	string current = "";
	itTypes = outputTypes.find("list");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("list", current); }
	}

	//set rabund file as new current rabundfile
	itTypes = outputTypes.find("rabund");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("rabund", current); }
	}

	//set sabund file as new current sabundfile
	itTypes = outputTypes.find("sabund");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("sabund", current); }
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << '\n' << "It took " + toString(time(NULL) - estart) + " seconds to cluster" << '\n';

	return 0;
}

//**********************************************************************************************************************

void ClusterDoturCommand::printData(string label, map<string, int>& counts) {
	oldRAbund.setLabel(label);
	if (countfile == "") {
		oldRAbund.print(rabundFile);
		oldRAbund.getSAbundVector().print(sabundFile);
	}

	oldRAbund.getSAbundVector().print(cout);

	oldList.setLabel(label);

	if (countfile != "") {
		oldList.print(listFile, counts);
	}
	else {
		oldList.print(listFile);
	}

}
//**********************************************************************************************************************
