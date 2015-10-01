/*
 *  clustercommand.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "clustercommand.h"
#include "readphylip.h"
#include "readcolumn.h"
#include "readmatrix.hpp"
#include "clusterdoturcommand.h"


 //**********************************************************************************************************************
vector<string> ClusterCommand::setParameters() {
	try {
		CommandParameter pphylip("phylip", "InputTypes", "", "", "PhylipColumn", "PhylipColumn", "none", "list", false, false, true); parameters.push_back(pphylip);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "ColumnName", "rabund-sabund", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount", "none", "none", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pcolumn("column", "InputTypes", "", "", "PhylipColumn", "PhylipColumn", "ColumnName", "list", false, false, true); parameters.push_back(pcolumn);
		CommandParameter pcutoff("cutoff", "Number", "", "10", "", "", "", "", false, false, true); parameters.push_back(pcutoff);
		nkParameters.add(new NumberParameter("precision", -INFINITY, INFINITY, 100, false, false));
		nkParameters.add(new MultipleParameter("method", vector<string>{"furthest", "nearest", "average", "weighted"}, "average", false, false, true));
		nkParameters.add(new BooleanParameter("showabund", true, false, false));
		nkParameters.add(new BooleanParameter("timing", false, false, false));
		nkParameters.add(new BooleanParameter("sim", false, false, false));
		nkParameters.add(new BooleanParameter("hard", true, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		//nkParameters.add(new StringParameter("adjust", "F", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterCommand::getHelpString() {
	try {
		string helpString = "The cluster command parameter options are phylip, column, name, count, method, cuttoff, hard, precision, sim, showabund and timing. Phylip or column and name are required, unless you have a valid current file.\n";
		//helpString += "The adjust parameter is used to handle missing distances.  If you set a cutoff, adjust=f by default.  If not, adjust=t by default. Adjust=f, means ignore missing distances and adjust cutoff as needed with the average neighbor method.  Adjust=t, will treat missing distances as 1.0. You can also set the value the missing distances should be set to, adjust=0.5 would give missing distances a value of 0.5.\n"
		"The cluster command should be in the following format: \n"
			"cluster(method=yourMethod, cutoff=yourCutoff, precision=yourPrecision) \n"
			"The acceptable cluster methods are furthest, nearest, average and weighted.  If no method is provided then average is assumed.\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "list") { pattern = "[filename],[clustertag],list-[filename],[clustertag],[tag2],list"; }
	else if (type == "rabund") { pattern = "[filename],[clustertag],rabund"; }
	else if (type == "sabund") { pattern = "[filename],[clustertag],sabund"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ClusterCommand::ClusterCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["rabund"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterCommand, ClusterCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
//This function checks to make sure the cluster command has no errors and then clusters based on the method chosen.
ClusterCommand::ClusterCommand(Settings& settings, string option) : Command(settings, option) {
	try {
		abort = false; calledHelp = false;

		//allow user to run help
		if (option == "help") { help(); abort = true; calledHelp = true; }
		else if (option == "citation") { citation(); abort = true; calledHelp = true; }

		else {
			vector<string> myArray = setParameters();

			OptionParser parser(option);
			map<string, string> parameters = parser.getParameters();
			map<string, string>::iterator it;

			ValidParameters validParameter;

			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) {
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {
					abort = true;
				}
			}

			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["list"] = tempOutNames;
			outputTypes["rabund"] = tempOutNames;
			outputTypes["sabund"] = tempOutNames;

			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

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

				it = parameters.find("column");
				//user has given a template file
				if (it != parameters.end()) {
					path = File::getPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") { parameters["column"] = inputDir + it->second; }
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

			//check for required parameters
			phylipfile = validParameter.validFile(parameters, "phylip", true);
			if (phylipfile == "not open") { phylipfile = ""; abort = true; }
			else if (phylipfile == "not found") { phylipfile = ""; }
			else { distfile = phylipfile;  format = "phylip"; 	settings.setCurrent("phylip", phylipfile); }

			columnfile = validParameter.validFile(parameters, "column", true);
			if (columnfile == "not open") { columnfile = ""; abort = true; }
			else if (columnfile == "not found") { columnfile = ""; }
			else { distfile = columnfile; format = "column"; settings.setCurrent("column", columnfile); }

			namefile = validParameter.validFile(parameters, "name", true);
			if (namefile == "not open") { abort = true; }
			else if (namefile == "not found") { namefile = ""; }
			else { settings.setCurrent("name", namefile); }

			countfile = validParameter.validFile(parameters, "count", true);
			if (countfile == "not open") { abort = true; countfile = ""; }
			else if (countfile == "not found") { countfile = ""; }
			else { settings.setCurrent("counttable", countfile); }

			if ((phylipfile == "") && (columnfile == "")) {
				//is there are current file available for either of these?
				//give priority to column, then phylip
				columnfile = settings.getCurrent("column");
				if (columnfile != "") { distfile = columnfile; format = "column"; LOG(INFO) << "Using " + columnfile + " as input file for the column parameter." << '\n'; }
				else {
					phylipfile = settings.getCurrent("phylip");
					if (phylipfile != "") { distfile = phylipfile;  format = "phylip"; LOG(INFO) << "Using " + phylipfile + " as input file for the phylip parameter." << '\n'; }
					else {
						LOG(INFO) << "No valid current files. You must provide a phylip or column file before you can use the cluster command." << '\n';
						abort = true;
					}
				}
			}
			else if ((phylipfile != "") && (columnfile != "")) { LOG(INFO) << "When executing a cluster command you must enter ONLY ONE of the following: phylip or column." << '\n'; abort = true; }

			if (columnfile != "") {
				if ((namefile == "") && (countfile == "")) {
					namefile = settings.getCurrent("name");
					if (namefile != "") { LOG(INFO) << "Using " + namefile + " as input file for the name parameter." << '\n'; }
					else {
						countfile = settings.getCurrent("counttable");
						if (countfile != "") { LOG(INFO) << "Using " + countfile + " as input file for the count parameter." << '\n'; }
						else {
							LOG(INFO) << "You need to provide a namefile or countfile if you are going to use the column format." << '\n';
							abort = true;
						}
					}
				}
			}

			if ((countfile != "") && (namefile != "")) { LOG(INFO) << "When executing a cluster command you must enter ONLY ONE of the following: count or name." << '\n'; abort = true; }

			//check for optional parameter and set defaults
			// ...at some point should added some additional type checking...
			//get user cutoff and precision or use defaults
			string temp;
			temp = validParameter.validFile(parameters, "precision", false);
			if (temp == "not found") { temp = "100"; }
			//saves precision legnth for formatting below
			length = temp.length();
			Utility::mothurConvert(temp, precision);

			temp = validParameter.validFile(parameters, "hard", false);			if (temp == "not found") { temp = "T"; }
			hard = m->isTrue(temp);

			temp = validParameter.validFile(parameters, "sim", false);				if (temp == "not found") { temp = "F"; }
			sim = m->isTrue(temp);

			//bool cutoffSet = false;
			temp = validParameter.validFile(parameters, "cutoff", false);
			if (temp == "not found") { temp = "10"; }
			//else { cutoffSet = true; }
			Utility::mothurConvert(temp, cutoff);
			cutoff += (5 / (precision * 10.0));

			//temp = validParameter.validFile(parameters, "adjust", false);				if (temp == "not found") { temp = "F"; }
			//if (m->isNumeric1(temp))    { Utility::mothurConvert(temp, adjust);   }
			//else if (m->isTrue(temp))   { adjust = 1.0;                     }
			//else                        { adjust = -1.0;                    }
			adjust = -1.0;

			method = validParameter.validFile(parameters, "method", false);
			if (method == "not found") { method = "average"; }

			if ((method == "furthest") || (method == "nearest") || (method == "average") || (method == "weighted")) {}
			else { LOG(INFO) << "Not a valid clustering method.  Valid clustering algorithms are furthest, nearest, average, and weighted." << '\n'; abort = true; }

			showabund = validParameter.validFile(parameters, "showabund", false);
			if (showabund == "not found") { showabund = "T"; }

			timing = validParameter.validFile(parameters, "timing", false);
			if (timing == "not found") { timing = "F"; }

		}
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterCommand, ClusterCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
ClusterCommand::~ClusterCommand() {}
//**********************************************************************************************************************

int ClusterCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//phylip file given and cutoff not given - use cluster.classic because it uses less memory and is faster
	if ((format == "phylip") && (cutoff > 10.0)) {
		LOG(INFO) << '\n' << "You are using a phylip file and no cutoff.  I will run cluster.classic to save memory and time." << '\n';

		//run unique.seqs for deconvolute results
		string inputString = "phylip=" + distfile;
		if (namefile != "") { inputString += ", name=" + namefile; }
		else if (countfile != "") { inputString += ", count=" + countfile; }
		inputString += ", precision=" + toString(precision);
		inputString += ", method=" + method;
		if (hard) { inputString += ", hard=T"; }
		else { inputString += ", hard=F"; }
		if (sim) { inputString += ", sim=T"; }
		else { inputString += ", sim=F"; }


		LOG(INFO) << '\n' << "/------------------------------------------------------------/" << '\n';
		LOG(INFO) << "Running command: cluster.classic(" + inputString + ")" << '\n';

		Command* clusterClassicCommand = new ClusterDoturCommand(inputString);
		clusterClassicCommand->execute();
		delete clusterClassicCommand;

		LOG(INFO) << "/------------------------------------------------------------/" << '\n';

		return 0;
	}

	ReadMatrix* read;
	if (format == "column") { read = new ReadColumnMatrix(columnfile, sim); }	//sim indicates whether its a similarity matrix
	else if (format == "phylip") { read = new ReadPhylipMatrix(phylipfile, sim); }

	read->setCutoff(cutoff);

	NameAssignment* nameMap = NULL;
	CountTable* ct = NULL;
	map<string, int> counts;
	if (namefile != "") {
		nameMap = new NameAssignment(namefile);
		nameMap->readMap();
		read->read(nameMap);
	}
	else if (countfile != "") {
		ct = new CountTable();
		ct->readTable(countfile, false, false);
		read->read(ct);
		counts = ct->getNameMap();
	}
	else { read->read(nameMap); }

	list = read->getListVector();
	matrix = read->getDMatrix();

	if (countfile != "") {
		rabund = new RAbundVector();
		createRabund(ct, list, rabund); //creates an rabund that includes the counts for the unique list
		delete ct;
	}
	else { rabund = new RAbundVector(list->getRAbundVector()); }
	delete read;

	if (ctrlc_pressed) { //clean up
		delete list; delete matrix; delete rabund; if (countfile == "") { rabundFile.close(); sabundFile.close();  File::remove((fileroot + tag + ".rabund")); File::remove((fileroot + tag + ".sabund")); }
		listFile.close(); File::remove((fileroot + tag + ".list")); outputTypes.clear(); return 0;
	}

	//create cluster
	if (method == "furthest") { cluster = new CompleteLinkage(rabund, list, matrix, cutoff, method, adjust); }
	else if (method == "nearest") { cluster = new SingleLinkage(rabund, list, matrix, cutoff, method, adjust); }
	else if (method == "average") { cluster = new AverageLinkage(rabund, list, matrix, cutoff, method, adjust); }
	else if (method == "weighted") { cluster = new WeightedLinkage(rabund, list, matrix, cutoff, method, adjust); }
	tag = cluster->getTag();

	if (outputDir == "") { outputDir += File::getPath(distfile); }
	fileroot = outputDir + File::getRootName(File::getSimpleName(distfile));

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

	time_t estart = time(NULL);
	float previousDist = 0.00000;
	float rndPreviousDist = 0.00000;
	oldRAbund = *rabund;
	oldList = *list;

	print_start = true;
	start = time(NULL);
	loops = 0;
	double saveCutoff = cutoff;

	while (matrix->getSmallDist() < cutoff && matrix->getNNodes() > 0) {

		if (ctrlc_pressed) { //clean up
			delete list; delete matrix; delete rabund; delete cluster;
			if (countfile == "") { rabundFile.close(); sabundFile.close();  File::remove((fileroot + tag + ".rabund")); File::remove((fileroot + tag + ".sabund")); }
			listFile.close(); File::remove((fileroot + tag + ".list")); outputTypes.clear(); return 0;
		}

		if (print_start && m->isTrue(timing)) {
			m->mothurOut("Clustering (" + tag + ") dist " + toString(matrix->getSmallDist()) + "/"
				+ toString(m->roundDist(matrix->getSmallDist(), precision))
				+ "\t(precision: " + toString(precision) + ", Nodes: " + toString(matrix->getNNodes()) + ")");
			cout.flush();
			print_start = false;
		}

		loops++;

		cluster->update(cutoff);

		float dist = matrix->getSmallDist();
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

	if (print_start && m->isTrue(timing)) {
		m->mothurOut("Clustering (" + tag + ") for distance " + toString(previousDist) + "/" + toString(rndPreviousDist)
			+ "\t(precision: " + toString(precision) + ", Nodes: " + toString(matrix->getNNodes()) + ")");
		cout.flush();
		print_start = false;
	}

	if (previousDist <= 0.0000) {
		printData("unique", counts);
	}
	else if (rndPreviousDist < cutoff) {
		printData(toString(rndPreviousDist, length - 1), counts);
	}

	delete matrix;
	delete list;
	delete rabund;
	delete cluster;
	if (countfile == "") {
		sabundFile.close();
		rabundFile.close();
	}
	listFile.close();

	if (saveCutoff != cutoff) {
		if (hard) { saveCutoff = m->ceilDist(saveCutoff, precision); }
		else { saveCutoff = m->roundDist(saveCutoff, precision); }

		LOG(INFO) << "changed cutoff to " + toString(cutoff) << '\n';
	}

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
	LOG(INFO) << "";


	//if (m->isTrue(timing)) {
	LOG(INFO) << "It took " + toString(time(NULL) - estart) + " seconds to cluster" << '\n';
	//}


	return 0;
}

//**********************************************************************************************************************

void ClusterCommand::printData(string label, map<string, int>& counts) {
	if (m->isTrue(timing)) {
		m->mothurOut("\tTime: " + toString(time(NULL) - start) + "\tsecs for " + toString(oldRAbund.getNumBins())
			+ "\tclusters. Updates: " + toString(loops)); m->mothurOutEndLine();
	}
	print_start = true;
	loops = 0;
	start = time(NULL);

	oldRAbund.setLabel(label);
	if (countfile == "") {
		oldRAbund.print(rabundFile);
		oldRAbund.getSAbundVector().print(sabundFile);
	}

	if (m->isTrue(showabund)) {
		oldRAbund.getSAbundVector().print(cout);
	}

	oldList.setLabel(label);
	if (countfile != "") {
		oldList.print(listFile, counts);
	}
	else {
		oldList.print(listFile);
	}


}
//**********************************************************************************************************************

int ClusterCommand::createRabund(CountTable*& ct, ListVector*& list, RAbundVector*& rabund) {
	rabund->setLabel(list->getLabel());
	for (int i = 0; i < list->getNumBins(); i++) {
		if (ctrlc_pressed) { break; }
		vector<string> binNames;
		string bin = list->get(i);
		m->splitAtComma(bin, binNames);
		int total = 0;
		for (int j = 0; j < binNames.size(); j++) { total += ct->getNumSeqs(binNames[j]); }
		rabund->push_back(total);
	}
	return 0;

}
//**********************************************************************************************************************
