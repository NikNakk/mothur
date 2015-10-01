/*
 *  summarysharedcommand.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "summarysharedcommand.h"
#include "subsample.h"

 //**********************************************************************************************************************
vector<string> SummarySharedCommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "none", "none", "none", "summary", false, true, true); parameters.push_back(pshared);
		nkParameters.add(new StringParameter("label", "", false, false));
		CommandParameter psubsample("subsample", "String", "", "", "", "", "", "phylip", false, false); parameters.push_back(psubsample);
		CommandParameter pdistance("distance", "Boolean", "", "F", "", "", "", "phylip", false, false); parameters.push_back(pdistance);
		nkParameters.add(new MultipleParameter("calc", vector<string>{"sharedchao", "sharedsobs", "sharedace", "jabund", "sorabund", "jclass", "sorclass", "jest", "sorest", "thetayc", "thetan-kstest-whittaker-sharednseqs-ochiai-anderberg-kulczynski-kulczynskicody-lennon-morisitahorn-braycurtis-odum-canberra-structeuclidean-structchord-hellinger-manhattan-structpearson-soergel-spearman-structkulczynski-speciesprofile-structchi2-hamming-gower-memchi2-memchord-memeuclidean-mempearson-jsd-rjsd"}, "sharedsobs-sharedchao-sharedace-jabund-sorabund-jclass-sorclass-jest-sorest-thetayc-thetan", true, false, true));
		CommandParameter poutput("output", "Multiple", "lt-square", "lt", "", "", "", "", false, false); parameters.push_back(poutput);
		nkParameters.add(new BooleanParameter("all", false, false, false));
		nkParameters.add(new NumberParameter("iters", -INFINITY, INFINITY, 1000, false, false));
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SummarySharedCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string SummarySharedCommand::getHelpString() {
	try {
		string helpString = "";
		ValidCalculators validCalculator
			"The summary.shared command parameters are shared, label, calc, distance, processors, subsample, iters and all.  shared is required if there is no current sharedfile.\n"
			"The summary.shared command should be in the following format: \n"
			"summary.shared(label=yourLabel, calc=yourEstimators, groups=yourGroups).\n"
			"Example summary.shared(label=unique-.01-.03, groups=B-C, calc=sharedchao-sharedace-jabund-sorensonabund-jclass-sorclass-jest-sorest-thetayc-thetan).\n";
		helpString += validCalculator.printCalc("sharedsummary")
			"The iters parameter allows you to choose the number of times you would like to run the subsample.\n"
			"The subsample parameter allows you to enter the size pergroup of the sample or you can set subsample=T and mothur will use the size of your smallest group.\n"
			"The output parameter allows you to specify format of your distance matrix. Options are lt, and square. The default is lt.\n"
			"The default value for calc is sharedsobs-sharedchao-sharedace-jabund-sorensonabund-jclass-sorclass-jest-sorest-thetayc-thetan\n"
			"The default value for groups is all the groups in your groupfile.\n"
			"The distance parameter allows you to indicate you would like a distance file created for each calculator for each label, default=f.\n"
			"The label parameter is used to analyze specific labels in your input.\n"
			"The all parameter is used to specify if you want the estimate of all your groups together.  This estimate can only be made for sharedsobs and sharedchao calculators. The default is false.\n"
			"If you use sharedchao and run into memory issues, set all to false. \n"
			"The groups parameter allows you to specify which of the groups in your groupfile you would like analyzed.  You must enter at least 2 valid groups.\n"
			"Note: No spaces between parameter labels (i.e. label), '=' and parameters (i.e.yourLabel).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SummarySharedCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string SummarySharedCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "summary") { pattern = "[filename],summary-[filename],[tag],summary"; }
	else if (type == "phylip") { pattern = "[filename],[calc],[distance],[outputtag],[tag2],dist"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
SummarySharedCommand::SummarySharedCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["summary"] = tempOutNames;
		outputTypes["phylip"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SummarySharedCommand, SummarySharedCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

SummarySharedCommand::SummarySharedCommand(Settings& settings, string option) : Command(settings, option) {
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
		for (map<string, string>::iterator it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["summary"] = tempOutNames;
		outputTypes["phylip"] = tempOutNames;

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
		if (calc == "not found") { calc = "sharedsobs-sharedchao-sharedace-jabund-sorabund-jclass-sorclass-jest-sorest-thetayc-thetan"; }
		else {
			if (calc == "default") { calc = "sharedsobs-sharedchao-sharedace-jabund-sorabund-jclass-sorclass-jest-sorest-thetayc-thetan"; }
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
			m->setGroups(Groups);
		}

		string temp = validParameter.validFile(parameters, "all", false);				if (temp == "not found") { temp = "false"; }
		all = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "iters", false);			if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, iters);

		output = validParameter.validFile(parameters, "output", false);
		if (output == "not found") { output = "lt"; }
		else { createPhylip = true; }
		if ((output != "lt") && (output != "square")) { LOG(INFO) << output + " is not a valid output form. Options are lt and square. I will use lt." << '\n'; output = "lt"; }

		temp = validParameter.validFile(parameters, "subsample", false);		if (temp == "not found") { temp = "F"; }
		if (m->isNumeric1(temp)) { Utility::mothurConvert(temp, subsampleSize); subsample = true; }
		else {
			if (m->isTrue(temp)) { subsample = true; subsampleSize = -1; }  //we will set it to smallest group later 
			else { subsample = false; }
		}

		if (subsample == false) { iters = 0; }

		temp = validParameter.validFile(parameters, "distance", false);					if (temp == "not found") { temp = "false"; }
		createPhylip = m->isTrue(temp);
		if (subsample) { createPhylip = true; }

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		if (abort == false) {

			ValidCalculators validCalculator;
			int i;

			for (i = 0; i < Estimators.size(); i++) {
				if (validCalculator.isValidCalculator("sharedsummary", Estimators[i]) == true) {
					if (Estimators[i] == "sharedsobs") {
						sumCalculators.push_back(new SharedSobsCS());
					}
					else if (Estimators[i] == "sharedchao") {
						sumCalculators.push_back(new SharedChao1());
					}
					else if (Estimators[i] == "sharedace") {
						sumCalculators.push_back(new SharedAce());
					}
					else if (Estimators[i] == "jabund") {
						sumCalculators.push_back(new JAbund());
					}
					else if (Estimators[i] == "sorabund") {
						sumCalculators.push_back(new SorAbund());
					}
					else if (Estimators[i] == "jclass") {
						sumCalculators.push_back(new Jclass());
					}
					else if (Estimators[i] == "sorclass") {
						sumCalculators.push_back(new SorClass());
					}
					else if (Estimators[i] == "jest") {
						sumCalculators.push_back(new Jest());
					}
					else if (Estimators[i] == "sorest") {
						sumCalculators.push_back(new SorEst());
					}
					else if (Estimators[i] == "thetayc") {
						sumCalculators.push_back(new ThetaYC());
					}
					else if (Estimators[i] == "thetan") {
						sumCalculators.push_back(new ThetaN());
					}
					else if (Estimators[i] == "kstest") {
						sumCalculators.push_back(new KSTest());
					}
					else if (Estimators[i] == "sharednseqs") {
						sumCalculators.push_back(new SharedNSeqs());
					}
					else if (Estimators[i] == "ochiai") {
						sumCalculators.push_back(new Ochiai());
					}
					else if (Estimators[i] == "anderberg") {
						sumCalculators.push_back(new Anderberg());
					}
					else if (Estimators[i] == "kulczynski") {
						sumCalculators.push_back(new Kulczynski());
					}
					else if (Estimators[i] == "kulczynskicody") {
						sumCalculators.push_back(new KulczynskiCody());
					}
					else if (Estimators[i] == "lennon") {
						sumCalculators.push_back(new Lennon());
					}
					else if (Estimators[i] == "morisitahorn") {
						sumCalculators.push_back(new MorHorn());
					}
					else if (Estimators[i] == "braycurtis") {
						sumCalculators.push_back(new BrayCurtis());
					}
					else if (Estimators[i] == "whittaker") {
						sumCalculators.push_back(new Whittaker());
					}
					else if (Estimators[i] == "odum") {
						sumCalculators.push_back(new Odum());
					}
					else if (Estimators[i] == "canberra") {
						sumCalculators.push_back(new Canberra());
					}
					else if (Estimators[i] == "structeuclidean") {
						sumCalculators.push_back(new StructEuclidean());
					}
					else if (Estimators[i] == "structchord") {
						sumCalculators.push_back(new StructChord());
					}
					else if (Estimators[i] == "hellinger") {
						sumCalculators.push_back(new Hellinger());
					}
					else if (Estimators[i] == "manhattan") {
						sumCalculators.push_back(new Manhattan());
					}
					else if (Estimators[i] == "structpearson") {
						sumCalculators.push_back(new StructPearson());
					}
					else if (Estimators[i] == "soergel") {
						sumCalculators.push_back(new Soergel());
					}
					else if (Estimators[i] == "spearman") {
						sumCalculators.push_back(new Spearman());
					}
					else if (Estimators[i] == "structkulczynski") {
						sumCalculators.push_back(new StructKulczynski());
					}
					else if (Estimators[i] == "speciesprofile") {
						sumCalculators.push_back(new SpeciesProfile());
					}
					else if (Estimators[i] == "hamming") {
						sumCalculators.push_back(new Hamming());
					}
					else if (Estimators[i] == "structchi2") {
						sumCalculators.push_back(new StructChi2());
					}
					else if (Estimators[i] == "gower") {
						sumCalculators.push_back(new Gower());
					}
					else if (Estimators[i] == "memchi2") {
						sumCalculators.push_back(new MemChi2());
					}
					else if (Estimators[i] == "memchord") {
						sumCalculators.push_back(new MemChord());
					}
					else if (Estimators[i] == "memeuclidean") {
						sumCalculators.push_back(new MemEuclidean());
					}
					else if (Estimators[i] == "mempearson") {
						sumCalculators.push_back(new MemPearson());
					}
					else if (Estimators[i] == "jsd") {
						sumCalculators.push_back(new JSD());
					}
					else if (Estimators[i] == "rjsd") {
						sumCalculators.push_back(new RJSD());
					}
				}
			}

			mult = false;
		}
	}
}
//**********************************************************************************************************************

int SummarySharedCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	ofstream outputFileHandle, outAll;
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(sharedfile));
	string outputFileName = getOutputFileName("summary", variables);

	//if the users entered no valid calculators don't execute command
	if (sumCalculators.size() == 0) { return 0; }
	//check if any calcs can do multiples
	else {
		if (all) {
			for (int i = 0; i < sumCalculators.size(); i++) {
				if (sumCalculators[i]->getMultiple() == true) { mult = true; }
			}
		}
	}

	input = new InputData(sharedfile, "sharedfile");
	lookup = input->getSharedRAbundVectors();
	string lastLabel = lookup[0]->getLabel();

	/******************************************************/
	//output headings for files
	/******************************************************/
	//output estimator names as column headers
	File::openOutputFile(outputFileName, outputFileHandle);
	outputFileHandle << "label" << '\t' << "comparison" << '\t';
	for (int i = 0;i < sumCalculators.size();i++) {
		outputFileHandle << '\t' << sumCalculators[i]->getName();
		if (sumCalculators[i]->getCols() == 3) { outputFileHandle << "\t" << sumCalculators[i]->getName() << "_lci\t" << sumCalculators[i]->getName() << "_hci"; }
	}
	outputFileHandle << endl;
	outputFileHandle.close();

	//create file and put column headers for multiple groups file
	variables["[tag]"] = "multiple";
	string outAllFileName = getOutputFileName("summary", variables);
	if (mult == true) {
		File::openOutputFile(outAllFileName, outAll);
		outputNames.push_back(outAllFileName);

		outAll << "label" << '\t' << "comparison" << '\t';
		for (int i = 0;i < sumCalculators.size();i++) {
			if (sumCalculators[i]->getMultiple() == true) {
				outAll << '\t' << sumCalculators[i]->getName();
			}
		}
		outAll << endl;
		outAll.close();
	}

	if (lookup.size() < 2) {
		LOG(INFO) << "I cannot run the command without at least 2 valid groups.";
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }

		//close files and clean up
		File::remove(outputFileName);
		if (mult == true) { File::remove(outAllFileName); }
		return 0;
		//if you only have 2 groups you don't need a .sharedmultiple file
	}
	else if ((lookup.size() == 2) && (mult == true)) {
		mult = false;
		File::remove(outAllFileName);
		outputNames.pop_back();
	}

	if (ctrlc_pressed) {
		if (mult) { File::remove(outAllFileName); }
		File::remove(outputFileName);
		delete input;
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		for (int i = 0;i < sumCalculators.size();i++) { delete sumCalculators[i]; }
		m->clearGroups();
		return 0;
	}
	/******************************************************/
	if (subsample) {
		if (subsampleSize == -1) { //user has not set size, set size = smallest samples size
			subsampleSize = lookup[0]->getNumSeqs();
			for (int i = 1; i < lookup.size(); i++) {
				int thisSize = lookup[i]->getNumSeqs();

				if (thisSize < subsampleSize) { subsampleSize = thisSize; }
			}
		}
		else {
			m->clearGroups();
			Groups.clear();
			vector<SharedRAbundVector*> temp;
			for (int i = 0; i < lookup.size(); i++) {
				if (lookup[i]->getNumSeqs() < subsampleSize) {
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

		if (lookup.size() < 2) { LOG(INFO) << "You have not provided enough valid groups.  I cannot run the command." << '\n'; ctrlc_pressed = true; delete input; return 0; }
	}


	/******************************************************/
	//comparison breakup to be used by different processes later
	numGroups = lookup.size();
	lines.resize(processors);
	for (int i = 0; i < processors; i++) {
		lines[i].start = int(sqrt(float(i) / float(processors)) * numGroups);
		lines[i].end = int(sqrt(float(i + 1) / float(processors)) * numGroups);
	}
	/******************************************************/

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	//as long as you are not at the end of the file or done wih the lines you want
	while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
		if (ctrlc_pressed) {
			if (mult) { File::remove(outAllFileName); }
			File::remove(outputFileName);
			delete input;
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			for (int i = 0;i < sumCalculators.size();i++) { delete sumCalculators[i]; }
			m->clearGroups();
			return 0;
		}


		if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {
			LOG(INFO) << lookup[0]->getLabel() << '\n';
			process(lookup, outputFileName, outAllFileName);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());
		}

		if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = lookup[0]->getLabel();

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input->getSharedRAbundVectors(lastLabel);

			LOG(INFO) << lookup[0]->getLabel() << '\n';
			process(lookup, outputFileName, outAllFileName);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());

			//restore real lastlabel to save below
			lookup[0]->setLabel(saveLabel);
		}

		lastLabel = lookup[0]->getLabel();

		//get next line to process
		//prevent memory leak
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		lookup = input->getSharedRAbundVectors();
	}

	if (ctrlc_pressed) {
		if (mult) { File::remove(outAllFileName); }
		File::remove(outputFileName);
		delete input;
		for (int i = 0;i < sumCalculators.size();i++) { delete sumCalculators[i]; }
		m->clearGroups();
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

	//run last label if you need to
	if (needToRun == true) {
		for (int i = 0; i < lookup.size(); i++) { if (lookup[i] != NULL) { delete lookup[i]; } }
		lookup = input->getSharedRAbundVectors(lastLabel);

		LOG(INFO) << lookup[0]->getLabel() << '\n';
		process(lookup, outputFileName, outAllFileName);
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
	}


	//reset groups parameter
	m->clearGroups();

	for (int i = 0;i < sumCalculators.size();i++) { delete sumCalculators[i]; }
	delete input;

	if (ctrlc_pressed) {
		File::remove(outAllFileName);
		File::remove(outputFileName);
		return 0;
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	LOG(INFO) << outputFileName << '\n';
	if (mult) { LOG(INFO) << outAllFileName << '\n';	outputTypes["summary"].push_back(outAllFileName); }
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; } outputTypes["summary"].push_back(outputFileName);
	LOG(INFO) << "";

	return 0;
}
/***********************************************************/
int SummarySharedCommand::printSims(ostream& out, vector< vector<double> >& simMatrix) {

	out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);

	//output num seqs
	out << simMatrix.size() << endl;

	if (output == "lt") {
		for (int b = 0; b < simMatrix.size(); b++) {
			out << lookup[b]->getGroup();
			for (int n = 0; n < b; n++) {
				if (ctrlc_pressed) { return 0; }
				out << '\t' << simMatrix[b][n];
			}
			out << endl;
		}
	}
	else {
		for (int b = 0; b < simMatrix.size(); m++) {
			out << lookup[b]->getGroup();
			for (int n = 0; n < simMatrix[b].size(); n++) {
				if (ctrlc_pressed) { return 0; }
				out << '\t' << simMatrix[b][n];
			}
			out << endl;
		}
	}

	return 0;
}
/***********************************************************/
int SummarySharedCommand::process(vector<SharedRAbundVector*> thisLookup, string sumFileName, string sumAllFileName) {
	vector< vector< vector<seqDist> > > calcDistsTotals;  //each iter, one for each calc, then each groupCombos dists. this will be used to make .dist files
	vector< vector<seqDist>  > calcDists; calcDists.resize(sumCalculators.size());

	for (int thisIter = 0; thisIter < iters + 1; thisIter++) {

		vector<SharedRAbundVector*> thisItersLookup = thisLookup;

		if (subsample && (thisIter != 0)) { //we want the summary results for the whole dataset, then the subsampling
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
		}


		if (processors == 1) {
			driver(thisItersLookup, 0, numGroups, sumFileName + ".temp", sumAllFileName + ".temp", calcDists);
			File::appendFiles((sumFileName + ".temp"), sumFileName);
			File::remove((sumFileName + ".temp"));
			if (mult) {
				File::appendFiles((sumAllFileName + ".temp"), sumAllFileName);
				File::remove((sumAllFileName + ".temp"));
			}
		}
		else {

			int process = 1;
			vector<int> processIDS;
			bool recalc = false;

#if defined (UNIX)
			//loop through and create all the processes you want
			while (process != processors) {
				pid_t pid = fork();

				if (pid > 0) {
					processIDS.push_back(pid);
					process++;
				}
				else if (pid == 0) {
					driver(thisItersLookup, lines[process].start, lines[process].end, sumFileName + m->mothurGetpid(process) + ".temp", sumAllFileName + m->mothurGetpid(process) + ".temp", calcDists);

					//only do this if you want a distance file
					if (createPhylip) {
						string tempdistFileName = File::getRootName(File::getSimpleName(sumFileName)) + m->mothurGetpid(process) + ".dist";
						ofstream outtemp;
						File::openOutputFile(tempdistFileName, outtemp);

						for (int i = 0; i < calcDists.size(); i++) {
							outtemp << calcDists[i].size() << endl;

							for (int j = 0; j < calcDists[i].size(); j++) {
								outtemp << calcDists[i][j].seq1 << '\t' << calcDists[i][j].seq2 << '\t' << calcDists[i][j].dist << endl;
							}
						}
						outtemp.close();
					}

					exit(0);
				}
				else {
					LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(process) + "\n"; processors = process;
					for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
					//wait to die
					for (int i = 0;i < processIDS.size();i++) {
						int temp = processIDS[i];
						wait(&temp);
					}
					ctrlc_pressed = false;
					for (int i = 0;i < processIDS.size();i++) {
						File::remove(sumFileName + (toString(processIDS[i]) + ".temp"));
						File::remove(sumAllFileName + (toString(processIDS[i]) + ".temp"));
						if (createPhylip) { File::remove(File::getRootName(File::getSimpleName(sumFileName)) + (toString(processIDS[i]) + ".dist")); }
					}
					recalc = true;
					break;
				}
			}

			if (recalc) {
				//test line, also set recalc to true.
				//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove(sumFileName + (toString(processIDS[i]) + ".temp"));File::remove(sumAllFileName + (toString(processIDS[i]) + ".temp"));if (createPhylip) { File::remove(File::getRootName(File::getSimpleName(sumFileName)) + (toString(processIDS[i]) + ".dist")); }}processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

				/******************************************************/
				//comparison breakup to be used by different processes later
				lines.clear();
				numGroups = thisLookup.size();
				lines.resize(processors);
				for (int i = 0; i < processors; i++) {
					lines[i].start = int(sqrt(float(i) / float(processors)) * numGroups);
					lines[i].end = int(sqrt(float(i + 1) / float(processors)) * numGroups);
				}
				/******************************************************/

				calcDists.clear(); calcDists.resize(sumCalculators.size());
				processIDS.resize(0);
				process = 1;

				//loop through and create all the processes you want
				while (process != processors) {
					pid_t pid = fork();

					if (pid > 0) {
						processIDS.push_back(pid);
						process++;
					}
					else if (pid == 0) {
						driver(thisItersLookup, lines[process].start, lines[process].end, sumFileName + m->mothurGetpid(process) + ".temp", sumAllFileName + m->mothurGetpid(process) + ".temp", calcDists);

						//only do this if you want a distance file
						if (createPhylip) {
							string tempdistFileName = File::getRootName(File::getSimpleName(sumFileName)) + m->mothurGetpid(process) + ".dist";
							ofstream outtemp;
							File::openOutputFile(tempdistFileName, outtemp);

							for (int i = 0; i < calcDists.size(); i++) {
								outtemp << calcDists[i].size() << endl;

								for (int j = 0; j < calcDists[i].size(); j++) {
									outtemp << calcDists[i][j].seq1 << '\t' << calcDists[i][j].seq2 << '\t' << calcDists[i][j].dist << endl;
								}
							}
							outtemp.close();
						}

						exit(0);
					}
					else {
						LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
						for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
						exit(0);
					}
				}

			}

			//parent do your part
			driver(thisItersLookup, lines[0].start, lines[0].end, sumFileName + m->mothurGetpid(process) + ".temp", sumAllFileName + m->mothurGetpid(process) + ".temp", calcDists);
			File::appendFiles((sumFileName + m->mothurGetpid(process) + ".temp"), sumFileName);
			File::remove((sumFileName + m->mothurGetpid(process) + ".temp"));
			if (mult) { File::appendFiles((sumAllFileName + m->mothurGetpid(process) + ".temp"), sumAllFileName); }

			//force parent to wait until all the processes are done
			for (int i = 0; i < processIDS.size(); i++) {
				int temp = processIDS[i];
				wait(&temp);
			}

			for (int i = 0; i < processIDS.size(); i++) {
				File::appendFiles((sumFileName + toString(processIDS[i]) + ".temp"), sumFileName);
				File::remove((sumFileName + toString(processIDS[i]) + ".temp"));
				if (mult) { File::remove((sumAllFileName + toString(processIDS[i]) + ".temp")); }

				if (createPhylip) {
					string tempdistFileName = File::getRootName(File::getSimpleName(sumFileName)) + toString(processIDS[i]) + ".dist";
					ifstream intemp;
					File::openInputFile(tempdistFileName, intemp);

					for (int k = 0; k < calcDists.size(); k++) {
						int size = 0;
						intemp >> size; File::gobble(intemp);

						for (int j = 0; j < size; j++) {
							int seq1 = 0;
							int seq2 = 0;
							float dist = 1.0;

							intemp >> seq1 >> seq2 >> dist;   File::gobble(intemp);

							seqDist tempDist(seq1, seq2, dist);
							calcDists[k].push_back(tempDist);
						}
					}
					intemp.close();
					File::remove(tempdistFileName);
				}
			}
#else
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			//Windows version shared memory, so be careful when passing variables through the summarySharedData struct. 
			//Above fork() will clone, so memory is separate, but that's not the case with windows, 
			//Taking advantage of shared memory to pass results vectors.
			//////////////////////////////////////////////////////////////////////////////////////////////////////

			vector<summarySharedData*> pDataArray;
			vector<DWORD> dwThreadIdArray(processors - 1);
			vector<HANDLE> hThreadArray(processors - 1);

			//Create processor worker threads.
			for (int i = 1; i < processors; i++) {

				//make copy of lookup so we don't get access violations
				vector<SharedRAbundVector*> newLookup;
				for (int k = 0; k < thisLookup.size(); k++) {
					SharedRAbundVector* temp = new SharedRAbundVector();
					temp->setLabel(thisLookup[k]->getLabel());
					temp->setGroup(thisLookup[k]->getGroup());
					newLookup.push_back(temp);
				}


				//for each bin
				for (int k = 0; k < thisItersLookup[0]->getNumBins(); k++) {
					if (ctrlc_pressed) { for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; } return 0; }
					for (int j = 0; j < thisItersLookup.size(); j++) { newLookup[j]->push_back(thisItersLookup[j]->getAbundance(k), thisItersLookup[j]->getGroup()); }
				}

				// Allocate memory for thread data.
				summarySharedData* tempSum = new summarySharedData((sumFileName + toString(i) + ".temp"), m, lines[i].start, lines[i].end, Estimators, newLookup);
				pDataArray.push_back(tempSum);
				processIDS.push_back(i);

				hThreadArray[i - 1] = CreateThread(NULL, 0, MySummarySharedThreadFunction, pDataArray[i - 1], 0, &dwThreadIdArray[i - 1]);
			}

			//parent do your part
			driver(thisItersLookup, lines[0].start, lines[0].end, sumFileName + "0.temp", sumAllFileName + "0.temp", calcDists);
			File::appendFiles((sumFileName + "0.temp"), sumFileName);
			File::remove((sumFileName + "0.temp"));
			if (mult) { File::appendFiles((sumAllFileName + "0.temp"), sumAllFileName); }

			//Wait until all threads have terminated.
			WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

			//Close all thread handles and free memory allocations.
			for (int i = 0; i < pDataArray.size(); i++) {
				if (pDataArray[i]->count != (pDataArray[i]->end - pDataArray[i]->start)) {
					LOG(LOGERROR) << "process " + toString(i) + " only processed " + toString(pDataArray[i]->count) + " of " + toString(pDataArray[i]->end - pDataArray[i]->start) + " groups assigned to it, quitting. \n"; ctrlc_pressed = true;
				}
				File::appendFiles((sumFileName + toString(processIDS[i]) + ".temp"), sumFileName);
				File::remove((sumFileName + toString(processIDS[i]) + ".temp"));

				for (int j = 0; j < pDataArray[i]->thisLookup.size(); j++) { delete pDataArray[i]->thisLookup[j]; }

				if (createPhylip) {
					for (int k = 0; k < calcDists.size(); k++) {
						int size = pDataArray[i]->calcDists[k].size();
						for (int j = 0; j < size; j++) { calcDists[k].push_back(pDataArray[i]->calcDists[k][j]); }
					}
				}

				CloseHandle(hThreadArray[i]);
				delete pDataArray[i];
			}

#endif
		}

		if (subsample && (thisIter != 0)) { //we want the summary results for the whole dataset, then the subsampling

			calcDistsTotals.push_back(calcDists);
			//clean up memory
			for (int i = 0; i < thisItersLookup.size(); i++) { delete thisItersLookup[i]; }
			thisItersLookup.clear();
		}
		else {
			if (createPhylip) {
				for (int i = 0; i < calcDists.size(); i++) {
					if (ctrlc_pressed) { break; }

					//initialize matrix
					vector< vector<double> > matrix; //square matrix to represent the distance
					matrix.resize(thisLookup.size());
					for (int k = 0; k < thisLookup.size(); k++) { matrix[k].resize(thisLookup.size(), 0.0); }

					for (int j = 0; j < calcDists[i].size(); j++) {
						int row = calcDists[i][j].seq1;
						int column = calcDists[i][j].seq2;
						double dist = calcDists[i][j].dist;

						matrix[row][column] = dist;
						matrix[column][row] = dist;
					}

					map<string, string> variables;
					variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(sharedfile));
					variables["[calc]"] = sumCalculators[i]->getName();
					variables["[distance]"] = thisLookup[0]->getLabel();
					variables["[outputtag]"] = output;
					variables["[tag2]"] = "";
					string distFileName = getOutputFileName("phylip", variables);
					outputNames.push_back(distFileName); outputTypes["phylip"].push_back(distFileName);
					ofstream outDist;
					File::openOutputFile(distFileName, outDist);
					outDist.setf(ios::fixed, ios::floatfield); outDist.setf(ios::showpoint);

					printSims(outDist, matrix);

					outDist.close();
				}
			}
		}
		for (int i = 0; i < calcDists.size(); i++) { calcDists[i].clear(); }
	}

	if (iters != 0) {
		//we need to find the average distance and standard deviation for each groups distance
		vector< vector<seqDist>  > calcAverages = m->getAverages(calcDistsTotals);

		//find standard deviation
		vector< vector<seqDist>  > stdDev = m->getStandardDeviation(calcDistsTotals, calcAverages);

		//print results
		for (int i = 0; i < calcDists.size(); i++) {
			vector< vector<double> > matrix; //square matrix to represent the distance
			matrix.resize(thisLookup.size());
			for (int k = 0; k < thisLookup.size(); k++) { matrix[k].resize(thisLookup.size(), 0.0); }

			vector< vector<double> > stdmatrix; //square matrix to represent the stdDev
			stdmatrix.resize(thisLookup.size());
			for (int k = 0; k < thisLookup.size(); k++) { stdmatrix[k].resize(thisLookup.size(), 0.0); }


			for (int j = 0; j < calcAverages[i].size(); j++) {
				int row = calcAverages[i][j].seq1;
				int column = calcAverages[i][j].seq2;
				float dist = calcAverages[i][j].dist;
				float stdDist = stdDev[i][j].dist;

				matrix[row][column] = dist;
				matrix[column][row] = dist;
				stdmatrix[row][column] = stdDist;
				stdmatrix[column][row] = stdDist;
			}

			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(sharedfile));
			variables["[calc]"] = sumCalculators[i]->getName();
			variables["[distance]"] = thisLookup[0]->getLabel();
			variables["[outputtag]"] = output;
			variables["[tag2]"] = "ave";
			string distFileName = getOutputFileName("phylip", variables);
			outputNames.push_back(distFileName); outputTypes["phylip"].push_back(distFileName);
			ofstream outAve;
			File::openOutputFile(distFileName, outAve);
			outAve.setf(ios::fixed, ios::floatfield); outAve.setf(ios::showpoint);

			printSims(outAve, matrix);

			outAve.close();

			variables["[tag2]"] = "std";
			distFileName = getOutputFileName("phylip", variables);
			outputNames.push_back(distFileName); outputTypes["phylip"].push_back(distFileName);
			ofstream outSTD;
			File::openOutputFile(distFileName, outSTD);
			outSTD.setf(ios::fixed, ios::floatfield); outSTD.setf(ios::showpoint);

			printSims(outSTD, stdmatrix);

			outSTD.close();

		}
	}

	return 0;
}
/**************************************************************************************************/
int SummarySharedCommand::driver(vector<SharedRAbundVector*> thisLookup, int start, int end, string sumFile, string sumAllFile, vector< vector<seqDist> >& calcDists) {
	try {

		//loop through calculators and add to file all for all calcs that can do mutiple groups
		if (mult == true) {
			ofstream outAll;
			File::openOutputFile(sumAllFile, outAll);

			//output label
			outAll << thisLookup[0]->getLabel() << '\t';

			//output groups names
			string outNames = "";
			for (int j = 0; j < thisLookup.size(); j++) {
				outNames += thisLookup[j]->getGroup() + "-";
			}
			outNames = outNames.substr(0, outNames.length() - 1); //rip off extra '-';
			outAll << outNames << '\t';

			for (int i = 0;i < sumCalculators.size();i++) {
				if (sumCalculators[i]->getMultiple() == true) {
					sumCalculators[i]->getValues(thisLookup);

					if (ctrlc_pressed) { outAll.close(); return 1; }

					outAll << '\t';
					sumCalculators[i]->print(outAll);
				}
			}
			outAll << endl;
			outAll.close();
		}

		ofstream outputFileHandle;
		File::openOutputFile(sumFile, outputFileHandle);

		vector<SharedRAbundVector*> subset;
		for (int k = start; k < end; k++) { // pass cdd each set of groups to compare

			for (int l = 0; l < k; l++) {

				outputFileHandle << thisLookup[0]->getLabel() << '\t';

				subset.clear(); //clear out old pair of sharedrabunds
				//add new pair of sharedrabunds
				subset.push_back(thisLookup[k]); subset.push_back(thisLookup[l]);

				//sort groups to be alphanumeric
				if (thisLookup[k]->getGroup() > thisLookup[l]->getGroup()) {
					outputFileHandle << (thisLookup[l]->getGroup() + '\t' + thisLookup[k]->getGroup()) << '\t'; //print out groups
				}
				else {
					outputFileHandle << (thisLookup[k]->getGroup() + '\t' + thisLookup[l]->getGroup()) << '\t'; //print out groups
				}

				for (int i = 0;i < sumCalculators.size();i++) {

					//if this calc needs all groups to calculate the pair load all groups
					if (sumCalculators[i]->getNeedsAll()) {
						//load subset with rest of lookup for those calcs that need everyone to calc for a pair
						for (int w = 0; w < thisLookup.size(); w++) {
							if ((w != k) && (w != l)) { subset.push_back(thisLookup[w]); }
						}
					}

					vector<double> tempdata = sumCalculators[i]->getValues(subset); //saves the calculator outputs

					if (ctrlc_pressed) { outputFileHandle.close(); return 1; }

					outputFileHandle << '\t';
					sumCalculators[i]->print(outputFileHandle);

					seqDist temp(l, k, tempdata[0]);
					calcDists[i].push_back(temp);
				}
				outputFileHandle << endl;
			}
		}

		outputFileHandle.close();

		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SummarySharedCommand, driver";
		exit(1);
	}
}
/**************************************************************************************************/


