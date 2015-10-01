/*
 *  hclustercommand.cpp
 *  Mothur
 *
 *  Created by westcott on 10/13/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "hclustercommand.h"

 //**********************************************************************************************************************
vector<string> HClusterCommand::setParameters() {
	try {
		CommandParameter pphylip("phylip", "InputTypes", "", "", "PhylipColumn", "PhylipColumn", "none", "list-rabund-sabund", false, false, true); parameters.push_back(pphylip);
		CommandParameter pname("name", "InputTypes", "", "", "none", "none", "ColumnName", "", false, false, true); parameters.push_back(pname);
		CommandParameter pcolumn("column", "InputTypes", "", "", "PhylipColumn", "PhylipColumn", "ColumnName", "list-rabund-sabund", false, false, true); parameters.push_back(pcolumn);
		CommandParameter pcutoff("cutoff", "Number", "", "10", "", "", "", "", false, false, true); parameters.push_back(pcutoff);
		nkParameters.add(new NumberParameter("precision", -INFINITY, INFINITY, 100, false, false));
		CommandParameter pmethod("method", "Multiple", "furthest-nearest-average-weighted", "average", "", "", "", "", false, false); parameters.push_back(pmethod);
		nkParameters.add(new BooleanParameter("hard", true, false, false));
		nkParameters.add(new BooleanParameter("sorted", false, false, false));
		nkParameters.add(new BooleanParameter("showabund", true, false, false));
		nkParameters.add(new BooleanParameter("timing", false, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in HClusterCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string HClusterCommand::getHelpString() {
	try {
		string helpString = "The hcluster command parameter options are cutoff, precision, method, phylip, column, name, showabund, timing and sorted. Phylip or column and name are required, unless you have valid current files.\n"
			"The phylip and column parameter allow you to enter your distance file, and sorted indicates whether your column distance file is already sorted. \n"
			"The name parameter allows you to enter your name file and is required if your distance file is in column format. \n"
			"The hcluster command should be in the following format: \n"
			"hcluster(column=youDistanceFile, name=yourNameFile, method=yourMethod, cutoff=yourCutoff, precision=yourPrecision) \n"
			"The acceptable hcluster methods are furthest, nearest, weighted and average.\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in HClusterCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string HClusterCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "list") { pattern = "[filename],[clustertag],list"; }
	else if (type == "rabund") { pattern = "[filename],[clustertag],rabund"; }
	else if (type == "sabund") { pattern = "[filename],[clustertag],sabund"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}

//**********************************************************************************************************************
HClusterCommand::HClusterCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["rabund"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in HClusterCommand, HClusterCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
//This function checks to make sure the cluster command has no errors and then clusters based on the method chosen.
HClusterCommand::HClusterCommand(Settings& settings, string option) : Command(settings, option) {
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
			map<string, string>::iterator it;

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
			}

			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

			//check for required parameters
			phylipfile = validParameter.validFile(parameters, "phylip", true);
			if (phylipfile == "not open") { abort = true; }
			else if (phylipfile == "not found") { phylipfile = ""; }
			else { distfile = phylipfile;  format = "phylip"; 	settings.setCurrent("phylip", phylipfile); }

			columnfile = validParameter.validFile(parameters, "column", true);
			if (columnfile == "not open") { abort = true; }
			else if (columnfile == "not found") { columnfile = ""; }
			else { distfile = columnfile; format = "column";	settings.setCurrent("column", columnfile); }

			namefile = validParameter.validFile(parameters, "name", true);
			if (namefile == "not open") { abort = true; }
			else if (namefile == "not found") { namefile = ""; }
			else { settings.setCurrent("name", namefile); }

			if ((phylipfile == "") && (columnfile == "")) {
				//is there are current file available for either of these?
				//give priority to column, then phylip
				columnfile = settings.getCurrent("column");
				if (columnfile != "") { LOG(INFO) << "Using " + columnfile + " as input file for the column parameter." << '\n'; }
				else {
					phylipfile = settings.getCurrent("phylip");
					if (phylipfile != "") { LOG(INFO) << "Using " + phylipfile + " as input file for the phylip parameter." << '\n'; }
					else {
						LOG(INFO) << "No valid current files. You must provide a phylip or column file before you can use the hcluster command." << '\n';
						abort = true;
					}
				}
			}
			else if ((phylipfile != "") && (columnfile != "")) { LOG(INFO) << "When executing a hcluster command you must enter ONLY ONE of the following: phylip or column." << '\n'; abort = true; }

			if (columnfile != "") {
				if (namefile == "") {
					namefile = settings.getCurrent("name");
					if (namefile != "") { LOG(INFO) << "Using " + namefile + " as input file for the name parameter." << '\n'; }
					else {
						LOG(INFO) << "You need to provide a namefile if you are going to use the column format." << '\n';
						abort = true;
					}
				}
			}

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

			temp = validParameter.validFile(parameters, "cutoff", false);
			if (temp == "not found") { temp = "10"; }
			Utility::mothurConvert(temp, cutoff);
			cutoff += (5 / (precision * 10.0));

			method = validParameter.validFile(parameters, "method", false);
			if (method == "not found") { method = "average"; }

			if ((method == "furthest") || (method == "nearest") || (method == "average") || (method == "weighted")) {}
			else { LOG(INFO) << "Not a valid clustering method.  Valid clustering algorithms are furthest, nearest, average or weighted." << '\n'; abort = true; }

			showabund = validParameter.validFile(parameters, "showabund", false);
			if (showabund == "not found") { showabund = "T"; }

			sort = validParameter.validFile(parameters, "sorted", false);
			if (sort == "not found") { sort = "F"; }
			sorted = m->isTrue(sort);

			timing = validParameter.validFile(parameters, "timing", false);
			if (timing == "not found") { timing = "F"; }


			if (abort == false) {

				if (outputDir == "") { outputDir += File::getPath(distfile); }
				fileroot = outputDir + File::getRootName(File::getSimpleName(distfile));

				if (method == "furthest") { tag = "fn"; }
				else if (method == "nearest") { tag = "nn"; }
				else if (method == "weighted") { tag = "wn"; }
				else { tag = "an"; }

				map<string, string> variables;
				variables["[filename]"] = fileroot;
				variables["[clustertag]"] = tag;

				string sabundFileName = getOutputFileName("sabund", variables);
				string rabundFileName = getOutputFileName("rabund", variables);
				string listFileName = getOutputFileName("list", variables);

				File::openOutputFile(sabundFileName, sabundFile);
				File::openOutputFile(rabundFileName, rabundFile);
				File::openOutputFile(listFileName, listFile);

				outputNames.push_back(sabundFileName); outputTypes["sabund"].push_back(sabundFileName);
				outputNames.push_back(rabundFileName); outputTypes["rabund"].push_back(rabundFileName);
				outputNames.push_back(listFileName); outputTypes["list"].push_back(listFileName);
			}
		}
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in HClusterCommand, HClusterCommand";
		exit(1);
	}
}

//**********************************************************************************************************************

int HClusterCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	NameAssignment* nameMap = NULL;
	if (namefile != "") {
		nameMap = new NameAssignment(namefile);
		nameMap->readMap();
	}

	time_t estart = time(NULL);

	if (!sorted) {
		read = new ReadCluster(distfile, cutoff, outputDir, true);
		read->setFormat(format);
		read->read(nameMap);

		if (ctrlc_pressed) {
			delete read;
			sabundFile.close();
			rabundFile.close();
			listFile.close();
			for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
			return 0;
		}

		distfile = read->getOutputFile();

		list = read->getListVector();
		delete read;
	}
	else {
		list = new ListVector(nameMap->getListVector());
	}

	if (ctrlc_pressed) {
		sabundFile.close();
		rabundFile.close();
		listFile.close();
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
		return 0;
	}

	LOG(INFO) << "It took " + toString(time(NULL) - estart) + " seconds to sort. " << '\n';
	estart = time(NULL);

	//list vector made by read contains all sequence names
	if (list != NULL) {
		rabund = new RAbundVector(list->getRAbundVector());
	}
	else {
		LOG(INFO) << "Error: no list vector!" << '\n'; return 0;
	}

	list->printHeaders(listFile);

	float previousDist = 0.00000;
	float rndPreviousDist = 0.00000;
	oldRAbund = *rabund;
	oldList = *list;

	print_start = true;
	start = time(NULL);

	cluster = new HCluster(rabund, list, method, distfile, nameMap, cutoff);
	vector<seqDist> seqs; seqs.resize(1); // to start loop

	if (ctrlc_pressed) {
		delete cluster;
		sabundFile.close();
		rabundFile.close();
		listFile.close();
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
		return 0;
	}

	float saveCutoff = cutoff;

	while (seqs.size() != 0) {

		seqs = cluster->getSeqs();

		//to account for cutoff change in average neighbor
		if (seqs.size() != 0) {
			if (seqs[0].dist > cutoff) { break; }
		}

		if (ctrlc_pressed) {
			delete cluster;
			sabundFile.close();
			rabundFile.close();
			listFile.close();
			for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
			return 0;
		}

		for (int i = 0; i < seqs.size(); i++) {  //-1 means skip me

			if (seqs[i].seq1 != seqs[i].seq2) {
				cutoff = cluster->update(seqs[i].seq1, seqs[i].seq2, seqs[i].dist);

				if (ctrlc_pressed) {
					delete cluster;
					sabundFile.close();
					rabundFile.close();
					listFile.close();
					for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
					return 0;
				}


				float rndDist;
				if (hard) {
					rndDist = m->ceilDist(seqs[i].dist, precision);
				}
				else {
					rndDist = m->roundDist(seqs[i].dist, precision);
				}


				if ((previousDist <= 0.0000) && (seqs[i].dist != previousDist)) {
					printData("unique");
				}
				else if ((rndDist != rndPreviousDist)) {
					printData(toString(rndPreviousDist, length - 1));
				}

				previousDist = seqs[i].dist;
				rndPreviousDist = rndDist;
				oldRAbund = *rabund;
				oldList = *list;
			}
		}
	}

	if (ctrlc_pressed) {
		delete cluster;
		sabundFile.close();
		rabundFile.close();
		listFile.close();
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
		return 0;
	}

	if (previousDist <= 0.0000) {
		printData("unique");
	}
	else if (rndPreviousDist < cutoff) {
		printData(toString(rndPreviousDist, length - 1));
	}

	sabundFile.close();
	rabundFile.close();
	listFile.close();
	delete cluster;

	if (ctrlc_pressed) {
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
		return 0;
	}


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
	LOG(INFO) << '\n' << "It took " + toString(time(NULL) - estart) + " seconds to cluster. " << '\n';

	return 0;
}

//**********************************************************************************************************************

void HClusterCommand::printData(string label) {
	if (m->isTrue(timing)) {
		m->mothurOut("\tTime: " + toString(time(NULL) - start) + "\tsecs for " + toString(oldRAbund.getNumBins())
			+ "\tclusters. Updates: " + toString(loops)); m->mothurOutEndLine();
	}
	print_start = true;
	loops = 0;
	start = time(NULL);

	oldRAbund.setLabel(label);
	if (m->isTrue(showabund)) {
		oldRAbund.getSAbundVector().print(cout);
	}
	oldRAbund.print(rabundFile);
	oldRAbund.getSAbundVector().print(sabundFile);

	oldList.setLabel(label);
	oldList.print(listFile);


}
//**********************************************************************************************************************

