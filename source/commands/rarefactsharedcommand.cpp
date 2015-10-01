/*
 *  rarefactsharedcommand.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/6/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "rarefactsharedcommand.h"
#include "sharedsobs.h"
#include "sharednseqs.h"
#include "sharedutilities.h"
#include "subsample.h"


 //**********************************************************************************************************************
vector<string> RareFactSharedCommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "none", "none", "none", "", false, true, true); parameters.push_back(pshared);
		CommandParameter pdesign("design", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pdesign);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("freq", -INFINITY, INFINITY, 100, false, false));
		nkParameters.add(new NumberParameter("iters", -INFINITY, INFINITY, 1000, false, false));
		nkParameters.add(new MultipleParameter("calc", vector<string>{"sharednseqs", "sharedobserved"}, "sharedobserved", true, false, true));
		nkParameters.add(new NumberParameter("subsampleiters", -INFINITY, INFINITY, 1000, false, false));
		nkParameters.add(new StringParameter("subsample", "", false, false));
		nkParameters.add(new BooleanParameter("jumble", true, false, false));
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new StringParameter("sets", "", false, false));
		nkParameters.add(new BooleanParameter("groupmode", true, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RareFactSharedCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string RareFactSharedCommand::getHelpString() {
	try {
		string helpString = "";
		ValidCalculators validCalculator
			"The rarefaction.shared command parameters are shared, design, label, iters, groups, sets, jumble, groupmode and calc.  shared is required if there is no current sharedfile. \n"
			"The design parameter allows you to assign your groups to sets. If provided mothur will run rarefaction.shared on a per set basis. \n"
			"The sets parameter allows you to specify which of the sets in your designfile you would like to analyze. The set names are separated by dashes. THe default is all sets in the designfile.\n"
			"The rarefaction command should be in the following format: \n"
			"rarefaction.shared(label=yourLabel, iters=yourIters, calc=yourEstimators, jumble=yourJumble, groups=yourGroups).\n"
			"The freq parameter is used indicate when to output your data, by default it is set to 100. But you can set it to a percentage of the number of sequence. For example freq=0.10, means 10%. \n"
			"Example rarefaction.shared(label=unique-0.01-0.03,  iters=10000, groups=B-C, jumble=T, calc=sharedobserved).\n"
			"The default values for iters is 1000, freq is 100, and calc is sharedobserved which calculates the shared rarefaction curve for the observed richness.\n"
			"The subsampleiters parameter allows you to choose the number of times you would like to run the subsample.\n"
			"The subsample parameter allows you to enter the size pergroup of the sample or you can set subsample=T and mothur will use the size of your smallest group.\n"
			"The default value for groups is all the groups in your groupfile, and jumble is true.\n";
		helpString += validCalculator.printCalc("sharedrarefaction")
			"The label parameter is used to analyze specific labels in your input.\n"
			"The groups parameter allows you to specify which of the groups in your groupfile you would like analyzed.  You must enter at least 2 valid groups.\n"
			"Note: No spaces between parameter labels (i.e. freq), '=' and parameters (i.e.yourFreq).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RareFactSharedCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string RareFactSharedCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "sharedrarefaction") { pattern = "[filename],shared.rarefaction"; }
	else if (type == "sharedr_nseqs") { pattern = "[filename],shared.r_nseqs"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
RareFactSharedCommand::RareFactSharedCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["sharedrarefaction"] = tempOutNames;
		outputTypes["sharedr_nseqs"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RareFactSharedCommand, RareFactSharedCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

RareFactSharedCommand::RareFactSharedCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	allLines = 1;

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
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["sharedrarefaction"] = tempOutNames;
		outputTypes["sharedr_nseqs"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
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

		}

		//get shared file
		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = ""; abort = true; }
		else if (sharedfile == "not found") {
			//if there is a current shared file, use it
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else { LOG(INFO) << "You have no current sharedfile and the shared parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("shared", sharedfile); }

		designfile = validParameter.validFile(parameters, "design", true);
		if (designfile == "not open") { abort = true; designfile = ""; }
		else if (designfile == "not found") { designfile = ""; }
		else { settings.setCurrent("design", designfile); }


		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(sharedfile); }


		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}


		calc = validParameter.validFile(parameters, "calc", false);
		if (calc == "not found") { calc = "sharedobserved"; }
		else {
			if (calc == "default") { calc = "sharedobserved"; }
		}
		Utility::split(calc, '-', Estimators);
		if (m->inUsersGroups("citation", Estimators)) {
			ValidCalculators validCalc; validCalc.printCitations(Estimators);
			//remove citation from list of calcs
			for (int i = 0; i < Estimators.size(); i++) { if (Estimators[i] == "citation") { Estimators.erase(Estimators.begin() + i); break; } }
		}

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else {
			Utility::split(groups, '-', Groups);
		}
		m->setGroups(Groups);

		string sets = validParameter.validFile(parameters, "sets", false);
		if (sets == "not found") { sets = ""; }
		else {
			Utility::split(sets, '-', Sets);
		}

		string temp;
		temp = validParameter.validFile(parameters, "freq", false);			if (temp == "not found") { temp = "100"; }
		Utility::mothurConvert(temp, freq);

		temp = validParameter.validFile(parameters, "iters", false);			if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, nIters);

		temp = validParameter.validFile(parameters, "jumble", false);			if (temp == "not found") { temp = "T"; }
		if (m->isTrue(temp)) { jumble = true; }
		else { jumble = false; }
		m->jumble = jumble;

		temp = validParameter.validFile(parameters, "groupmode", false);		if (temp == "not found") { temp = "T"; }
		groupMode = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "subsampleiters", false);			if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, iters);

		temp = validParameter.validFile(parameters, "subsample", false);		if (temp == "not found") { temp = "F"; }
		if (m->isNumeric1(temp)) { Utility::mothurConvert(temp, subsampleSize); subsample = true; }
		else {
			if (m->isTrue(temp)) { subsample = true; subsampleSize = -1; }  //we will set it to smallest group later 
			else { subsample = false; }
		}

		if (subsample == false) { iters = 1; }

	}

}
//**********************************************************************************************************************

int RareFactSharedCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	DesignMap designMap;
	if (designfile == "") { //fake out designMap to run with process
		process(designMap, "");
	}
	else {
		designMap.read(designfile);

		//fill Sets - checks for "all" and for any typo groups
		SharedUtil util;
		vector<string> nameSets = designMap.getCategory();
		util.setGroups(Sets, nameSets);

		for (int i = 0; i < Sets.size(); i++) {
			process(designMap, Sets[i]);
		}

		if (groupMode) { outputNames = createGroupFile(outputNames); }
	}

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
//**********************************************************************************************************************

int RareFactSharedCommand::process(DesignMap& designMap, string thisSet) {
	Rarefact* rCurve;
	vector<Display*> rDisplays;

	InputData input(sharedfile, "sharedfile");
	lookup = input.getSharedRAbundVectors();
	if (lookup.size() < 2) {
		LOG(INFO) << "I cannot run the command without at least 2 valid groups.";
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		return 0;
	}

	string fileNameRoot = outputDir + File::getRootName(File::getSimpleName(sharedfile));

	vector<string> newGroups = settings.getGroups();
	if (thisSet != "") {  //make groups only filled with groups from this set so that's all inputdata will read
		vector<string> thisSets; thisSets.push_back(thisSet);
		newGroups = designMap.getNamesGroups(thisSets);
		fileNameRoot += thisSet + ".";
	}

	vector<SharedRAbundVector*> subset;
	if (thisSet == "") { subset.clear(); subset = lookup; }
	else {//fill subset with this sets groups
		subset.clear();
		for (int i = 0; i < lookup.size(); i++) {
			if (m->inUsersGroups(lookup[i]->getGroup(), newGroups)) {
				subset.push_back(lookup[i]);
			}
		}
	}

	/******************************************************/
	if (subsample) {
		if (subsampleSize == -1) { //user has not set size, set size = smallest samples size
			subsampleSize = subset[0]->getNumSeqs();
			for (int i = 1; i < subset.size(); i++) {
				int thisSize = subset[i]->getNumSeqs();

				if (thisSize < subsampleSize) { subsampleSize = thisSize; }
			}
		}
		else {
			newGroups.clear();
			vector<SharedRAbundVector*> temp;
			for (int i = 0; i < subset.size(); i++) {
				if (subset[i]->getNumSeqs() < subsampleSize) {
					LOG(INFO) << subset[i]->getGroup() + " contains " + toString(subset[i]->getNumSeqs()) + ". Eliminating." << '\n';
					delete subset[i];
				}
				else {
					newGroups.push_back(subset[i]->getGroup());
					temp.push_back(subset[i]);
				}
			}
			subset = temp;
		}

		if (subset.size() < 2) { LOG(INFO) << "You have not provided enough valid groups.  I cannot run the command." << '\n'; ctrlc_pressed = true; return 0; }
	}
	/******************************************************/

	map<string, string> variables;
	variables["[filename]"] = fileNameRoot;
	ValidCalculators validCalculator;
	for (int i = 0; i < Estimators.size(); i++) {
		if (validCalculator.isValidCalculator("sharedrarefaction", Estimators[i]) == true) {
			if (Estimators[i] == "sharedobserved") {
				rDisplays.push_back(new RareDisplay(new SharedSobs(), new SharedThreeColumnFile(getOutputFileName("sharedrarefaction", variables), "")));
				outputNames.push_back(getOutputFileName("sharedrarefaction", variables)); outputTypes["sharedrarefaction"].push_back(getOutputFileName("sharedrarefaction", variables));
			}
			else if (Estimators[i] == "sharednseqs") {
				rDisplays.push_back(new RareDisplay(new SharedNSeqs(), new SharedThreeColumnFile(getOutputFileName("sharedr_nseqs", variables), "")));
				outputNames.push_back(getOutputFileName("sharedr_nseqs", variables)); outputTypes["sharedr_nseqs"].push_back(getOutputFileName("sharedr_nseqs", variables));
			}
		}
		file2Group[outputNames.size() - 1] = thisSet;
	}

	//if the users entered no valid calculators don't execute command
	if (rDisplays.size() == 0) { for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }  return 0; }

	if (ctrlc_pressed) {
		for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		return 0;
	}


	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	string lastLabel = subset[0]->getLabel();
	set<string> processedLabels;
	set<string> userLabels = labels;

	//as long as you are not at the end of the file or done wih the lines you want
	while ((subset[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
		if (ctrlc_pressed) {
			for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }
			for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			return 0;
		}

		if (allLines == 1 || labels.count(subset[0]->getLabel()) == 1) {
			LOG(INFO) << subset[0]->getLabel() + '\t' + thisSet << '\n';
			rCurve = new Rarefact(subset, rDisplays);
			rCurve->getSharedCurve(freq, nIters);
			delete rCurve;

			if (subsample) { subsampleLookup(subset, fileNameRoot); }

			processedLabels.insert(subset[0]->getLabel());
			userLabels.erase(subset[0]->getLabel());
		}

		if ((Utility::anyLabelsToProcess(subset[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = subset[0]->getLabel();

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input.getSharedRAbundVectors(lastLabel);

			if (thisSet == "") { subset.clear(); subset = lookup; }
			else {//fill subset with this sets groups
				subset.clear();
				for (int i = 0; i < lookup.size(); i++) {
					if (m->inUsersGroups(lookup[i]->getGroup(), newGroups)) {
						subset.push_back(lookup[i]);
					}
				}
			}

			LOG(INFO) << subset[0]->getLabel() + '\t' + thisSet << '\n';
			rCurve = new Rarefact(subset, rDisplays);
			rCurve->getSharedCurve(freq, nIters);
			delete rCurve;

			if (subsample) { subsampleLookup(subset, fileNameRoot); }

			processedLabels.insert(subset[0]->getLabel());
			userLabels.erase(subset[0]->getLabel());

			//restore real lastlabel to save below
			subset[0]->setLabel(saveLabel);
		}


		lastLabel = subset[0]->getLabel();

		//get next line to process
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		lookup = input.getSharedRAbundVectors();

		if (lookup[0] != NULL) {
			if (thisSet == "") { subset.clear(); subset = lookup; }
			else {//fill subset with this sets groups
				subset.clear();
				for (int i = 0; i < lookup.size(); i++) {
					if (m->inUsersGroups(lookup[i]->getGroup(), newGroups)) {
						subset.push_back(lookup[i]);
					}
				}
			}
		}
		else { subset.clear(); subset.push_back(NULL); }

	}

	if (ctrlc_pressed) {
		for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }
		return 0;
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

	if (ctrlc_pressed) {
		for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }
		return 0;
	}

	//run last label if you need to
	if (needToRun == true) {
		for (int i = 0; i < lookup.size(); i++) { if (lookup[i] != NULL) { delete lookup[i]; } }
		lookup = input.getSharedRAbundVectors(lastLabel);

		if (thisSet == "") { subset.clear(); subset = lookup; }
		else {//fill subset with this sets groups
			subset.clear();
			for (int i = 0; i < lookup.size(); i++) {
				if (m->inUsersGroups(lookup[i]->getGroup(), newGroups)) {
					subset.push_back(lookup[i]);
				}
			}
		}

		LOG(INFO) << subset[0]->getLabel() + '\t' + thisSet << '\n';
		rCurve = new Rarefact(subset, rDisplays);
		rCurve->getSharedCurve(freq, nIters);
		delete rCurve;

		if (subsample) { subsampleLookup(subset, fileNameRoot); }

		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
	}

	for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }


	return 0;
}
//**********************************************************************************************************************
int RareFactSharedCommand::subsampleLookup(vector<SharedRAbundVector*>& thisLookup, string fileNameRoot) {

	map<string, vector<string> > filenames;
	for (int thisIter = 0; thisIter < iters; thisIter++) {

		vector<SharedRAbundVector*> thisItersLookup = thisLookup;

		//we want the summary results for the whole dataset, then the subsampling
		SubSample sample;
		vector<string> tempLabels; //dont need since we arent printing the sampled sharedRabunds

		//make copy of lookup so we don't get access violations
		vector<SharedRAbundVector*> newLookup;
		for (int k = 0; k < thisItersLookup.size(); k++) {
			SharedRAbundVector* temp = new SharedRAbundVector();
			temp->setLabel(thisItersLookup[k]->getLabel());
			temp->setGroup(thisItersLookup[k]->getGroup());
			newLookup.push_back(temp);
		}

		//for each bin
		for (int k = 0; k < thisItersLookup[0]->getNumBins(); k++) {
			if (ctrlc_pressed) { for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; } return 0; }
			for (int j = 0; j < thisItersLookup.size(); j++) { newLookup[j]->push_back(thisItersLookup[j]->getAbundance(k), thisItersLookup[j]->getGroup()); }
		}

		tempLabels = sample.getSample(newLookup, subsampleSize);
		thisItersLookup = newLookup;


		Rarefact* rCurve;
		vector<Display*> rDisplays;

		string thisfileNameRoot = fileNameRoot + toString(thisIter);

		map<string, string> variables;
		variables["[filename]"] = thisfileNameRoot;

		ValidCalculators validCalculator;
		for (int i = 0; i < Estimators.size(); i++) {
			if (validCalculator.isValidCalculator("sharedrarefaction", Estimators[i]) == true) {
				if (Estimators[i] == "sharedobserved") {
					rDisplays.push_back(new RareDisplay(new SharedSobs(), new SharedThreeColumnFile(getOutputFileName("sharedrarefaction", variables), "")));
					filenames["sharedrarefaction"].push_back(getOutputFileName("sharedrarefaction", variables));
				}
				else if (Estimators[i] == "sharednseqs") {
					rDisplays.push_back(new RareDisplay(new SharedNSeqs(), new SharedThreeColumnFile(getOutputFileName("sharedr_nseqs", variables), "")));
					filenames["sharedr_nseqs"].push_back(getOutputFileName("sharedr_nseqs", variables));
				}
			}
		}

		rCurve = new Rarefact(thisItersLookup, rDisplays);
		rCurve->getSharedCurve(freq, nIters);
		delete rCurve;

		//clean up memory
		for (int i = 0; i < thisItersLookup.size(); i++) { delete thisItersLookup[i]; }
		thisItersLookup.clear();
		for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }
	}

	//create std and ave outputs
	vector< vector< vector< double > > > results; //iter -> numSampled -> data
	for (map<string, vector<string> >::iterator it = filenames.begin(); it != filenames.end(); it++) {
		vector<string> thisTypesFiles = it->second;
		vector<string> columnHeaders;
		for (int i = 0; i < thisTypesFiles.size(); i++) {
			ifstream in;
			File::openInputFile(thisTypesFiles[i], in);

			string headers = File::getline(in); File::gobble(in);
			columnHeaders = m->splitWhiteSpace(headers);
			int numCols = columnHeaders.size();

			vector<vector<double> > thisFilesLines;
			while (!in.eof()) {
				if (ctrlc_pressed) { break; }
				vector<double> data; data.resize(numCols, 0);
				//read numSampled line
				for (int j = 0; j < numCols; j++) { in >> data[j]; File::gobble(in); }
				thisFilesLines.push_back(data);
			}
			in.close();
			results.push_back(thisFilesLines);
			File::remove(thisTypesFiles[i]);
		}

		if (!ctrlc_pressed) {
			//process results
			map<string, string> variables; variables["[filename]"] = fileNameRoot + "ave-std." + thisLookup[0]->getLabel() + ".";

			string outputFile = getOutputFileName(it->first, variables);
			ofstream out;
			File::openOutputFile(outputFile, out);
			outputNames.push_back(outputFile); outputTypes[it->first].push_back(outputFile);

			out << columnHeaders[0] << '\t' << "method";
			for (int i = 1; i < columnHeaders.size(); i++) { out << '\t' << columnHeaders[i]; }
			out << endl;

			vector< vector<double> > aveResults; aveResults.resize(results[0].size());
			for (int i = 0; i < aveResults.size(); i++) { aveResults[i].resize(results[0][i].size(), 0.0); }

			for (int thisIter = 0; thisIter < iters; thisIter++) { //sum all groups dists for each calculator
				for (int i = 0; i < aveResults.size(); i++) {  //initialize sums to zero.
					aveResults[i][0] = results[thisIter][i][0];
					for (int j = 1; j < aveResults[i].size(); j++) {
						aveResults[i][j] += results[thisIter][i][j];
					}
				}
			}

			for (int i = 0; i < aveResults.size(); i++) {  //finds average.
				for (int j = 1; j < aveResults[i].size(); j++) {
					aveResults[i][j] /= (float)iters;
				}
			}

			//standard deviation
			vector< vector<double> > stdResults; stdResults.resize(results[0].size());
			for (int i = 0; i < stdResults.size(); i++) { stdResults[i].resize(results[0][i].size(), 0.0); }

			for (int thisIter = 0; thisIter < iters; thisIter++) { //compute the difference of each dist from the mean, and square the result of each
				for (int i = 0; i < stdResults.size(); i++) {
					stdResults[i][0] = aveResults[i][0];
					for (int j = 1; j < stdResults[i].size(); j++) {
						stdResults[i][j] += ((results[thisIter][i][j] - aveResults[i][j]) * (results[thisIter][i][j] - aveResults[i][j]));
					}
				}
			}

			for (int i = 0; i < stdResults.size(); i++) {  //finds average.
				out << aveResults[i][0] << '\t' << "ave";
				for (int j = 1; j < aveResults[i].size(); j++) { out << '\t' << aveResults[i][j]; }
				out << endl;
				out << stdResults[i][0] << '\t' << "std";
				for (int j = 1; j < stdResults[i].size(); j++) {
					stdResults[i][j] /= (float)iters;
					stdResults[i][j] = sqrt(stdResults[i][j]);
					out << '\t' << stdResults[i][j];
				}
				out << endl;
			}
			out.close();
		}
	}


	return 0;
}
//**********************************************************************************************************************
vector<string> RareFactSharedCommand::createGroupFile(vector<string>& outputNames) {

	vector<string> newFileNames;

	//find different types of files
	map<string, map<string, string> > typesFiles;
	map<string, vector< vector<string> > > fileLabels; //combofile name to labels. each label is a vector because it may be unique lci hci.
	vector<string> groupNames;
	for (int i = 0; i < outputNames.size(); i++) {

		string extension = m->getExtension(outputNames[i]);
		string combineFileName = outputDir + File::getRootName(File::getSimpleName(sharedfile)) + "groups" + extension;
		File::remove(combineFileName); //remove old file

		ifstream in;
		File::openInputFile(outputNames[i], in);

		string labels = File::getline(in);

		istringstream iss(labels, istringstream::in);
		string newLabel = ""; vector<string> theseLabels;
		while (!iss.eof()) { iss >> newLabel; File::gobble(iss); theseLabels.push_back(newLabel); }
		vector< vector<string> > allLabels;
		vector<string> thisSet; thisSet.push_back(theseLabels[0]); allLabels.push_back(thisSet); thisSet.clear(); //makes "numSampled" its own grouping
		for (int j = 1; j < theseLabels.size() - 1; j++) {
			if (theseLabels[j + 1] == "lci") {
				thisSet.push_back(theseLabels[j]);
				thisSet.push_back(theseLabels[j + 1]);
				thisSet.push_back(theseLabels[j + 2]);
				j++; j++;
			}
			else { //no lci or hci for this calc.
				thisSet.push_back(theseLabels[j]);
			}
			allLabels.push_back(thisSet);
			thisSet.clear();
		}
		fileLabels[combineFileName] = allLabels;

		map<string, map<string, string> >::iterator itfind = typesFiles.find(extension);
		if (itfind != typesFiles.end()) {
			(itfind->second)[outputNames[i]] = file2Group[i];
		}
		else {
			map<string, string> temp;
			temp[outputNames[i]] = file2Group[i];
			typesFiles[extension] = temp;
		}
		if (!(m->inUsersGroups(file2Group[i], groupNames))) { groupNames.push_back(file2Group[i]); }
	}

	//for each type create a combo file

	for (map<string, map<string, string> >::iterator it = typesFiles.begin(); it != typesFiles.end(); it++) {

		ofstream out;
		string combineFileName = outputDir + File::getRootName(File::getSimpleName(sharedfile)) + "groups" + it->first;
		File::openOutputFileAppend(combineFileName, out);
		newFileNames.push_back(combineFileName);
		map<string, string> thisTypesFiles = it->second; //it->second maps filename to group
		set<int> numSampledSet;

		//open each type summary file
		map<string, map<int, vector< vector<string> > > > files; //maps file name to lines in file
		int maxLines = 0;
		for (map<string, string>::iterator itFileNameGroup = thisTypesFiles.begin(); itFileNameGroup != thisTypesFiles.end(); itFileNameGroup++) {

			string thisfilename = itFileNameGroup->first;
			string group = itFileNameGroup->second;

			ifstream temp;
			File::openInputFile(thisfilename, temp);

			//read through first line - labels
			File::getline(temp);	File::gobble(temp);

			map<int, vector< vector<string> > > thisFilesLines;
			while (!temp.eof()) {
				int numSampled = 0;
				temp >> numSampled; File::gobble(temp);

				vector< vector<string> > theseReads;
				vector<string> thisSet; thisSet.push_back(toString(numSampled)); theseReads.push_back(thisSet); thisSet.clear();
				for (int k = 1; k < fileLabels[combineFileName].size(); k++) { //output thing like 0.03-A lci-A hci-A
					vector<string> reads;
					string next = "";
					for (int l = 0; l < fileLabels[combineFileName][k].size(); l++) { //output modified labels
						temp >> next; File::gobble(temp);
						reads.push_back(next);
					}
					theseReads.push_back(reads);
				}
				thisFilesLines[numSampled] = theseReads;
				File::gobble(temp);

				numSampledSet.insert(numSampled);
			}

			files[group] = thisFilesLines;

			//save longest file for below
			if (maxLines < thisFilesLines.size()) { maxLines = thisFilesLines.size(); }

			temp.close();
			File::remove(thisfilename);
		}

		//output new labels line
		out << fileLabels[combineFileName][0][0];
		for (int k = 1; k < fileLabels[combineFileName].size(); k++) { //output thing like 0.03-A lci-A hci-A
			for (int n = 0; n < groupNames.size(); n++) { // for each group
				for (int l = 0; l < fileLabels[combineFileName][k].size(); l++) { //output modified labels
					out << '\t' << fileLabels[combineFileName][k][l] << '-' << groupNames[n];
				}
			}
		}
		out << endl;

		//for each label
		for (set<int>::iterator itNumSampled = numSampledSet.begin(); itNumSampled != numSampledSet.end(); itNumSampled++) {

			out << (*itNumSampled);

			if (ctrlc_pressed) { break; }

			for (int k = 1; k < fileLabels[combineFileName].size(); k++) { //each chunk
				//grab data for each group
				for (map<string, map<int, vector< vector<string> > > >::iterator itFileNameGroup = files.begin(); itFileNameGroup != files.end(); itFileNameGroup++) {

					string group = itFileNameGroup->first;

					map<int, vector< vector<string> > >::iterator itLine = files[group].find(*itNumSampled);
					if (itLine != files[group].end()) {
						for (int l = 0; l < (itLine->second)[k].size(); l++) {
							out << '\t' << (itLine->second)[k][l];

						}
					}
					else {
						for (int l = 0; l < fileLabels[combineFileName][k].size(); l++) {
							out << "\tNA";
						}
					}
				}
			}
			out << endl;
		}
		out.close();
	}

	//return combine file name
	return newFileNames;

}
//**********************************************************************************************************************
