//
//  getmetacommunitycommand.cpp
//  Mothur
//
//  Created by SarahsWork on 4/9/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#include "getmetacommunitycommand.h"
#include "communitytype.h"
#include "kmeans.h"
#include "validcalculator.h"
#include "subsample.h"

//**********************************************************************************************************************
vector<string> GetMetaCommunityCommand::setParameters() {
	CommandParameter pshared("shared", "InputTypes", "", "", "none", "none", "none", "outputType", false, true); parameters.push_back(pshared);
	nkParameters.add(new StringParameter("groups", "", false, false));
	nkParameters.add(new StringParameter("label", "", false, false));
	nkParameters.add(new MultipleParameter("calc", vector<string>{"sharedsobs", "sharedchao", "sharedace", "jabund", "sorabund", "jclass", "sorclass", "jest", "sorest", "thetayc", "thetan-kstest-sharednseqs-ochiai-anderberg-kulczynski-kulczynskicody-lennon-morisitahorn-braycurtis-whittaker-odum-canberra-structeuclidean-structchord-hellinger-manhattan-structpearson-soergel-spearman-structkulczynski-speciesprofile-hamming-structchi2-gower-memchi2-memchord-memeuclidean-mempearson-jsd-rjsd"}, "rjsd", false, false, true));
	nkParameters.add(new StringParameter("subsample", "", false, false));
	nkParameters.add(new NumberParameter("iters", -INFINITY, INFINITY, 1000, false, false));
	CommandParameter pminpartitions("minpartitions", "Number", "", "5", "", "", "", "", false, false, true); parameters.push_back(pminpartitions);
	CommandParameter pmaxpartitions("maxpartitions", "Number", "", "100", "", "", "", "", false, false, true); parameters.push_back(pmaxpartitions);
	CommandParameter poptimizegap("optimizegap", "Number", "", "3", "", "", "", "", false, false, true); parameters.push_back(poptimizegap);
	nkParameters.add(new ProcessorsParameter(settings));
	nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
	nkParameters.add(new StringParameter("inputdir", "", false, false));
	nkParameters.add(new StringParameter("outputdir", "", false, false));
	nkParameters.add(new MultipleParameter("method", vector<string>{"dmm", "kmeans", "pam"}, "dmm", false, false, true));

	vector<string> myArray;
	for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
	return myArray;
}
//**********************************************************************************************************************
string GetMetaCommunityCommand::getHelpString() {
	string helpString = "The get.communitytype command parameters are shared, method, label, groups, minpartitions, maxpartitions, optimizegap and processors. The shared file is required. \n"
		"The label parameter is used to analyze specific labels in your input. labels are separated by dashes.\n"
		"The groups parameter allows you to specify which of the groups in your shared file you would like analyzed.  Group names are separated by dashes.\n"
		"The method parameter allows you to select the method you would like to use.  Options are dmm, kmeans and pam. Default=dmm.\n"
		"The calc parameter allows you to select the calculator you would like to use to calculate the distance matrix used by the pam and kmeans method. By default the rjsd calculator is used.\n"
		"The iters parameter allows you to choose the number of times you would like to run the subsample while calculating the distance matrix for the pam and kmeans method.\n"
		"The subsample parameter allows you to enter the size pergroup of the sample or you can set subsample=T and mothur will use the size of your smallest group while calculating the distance matrix for the pam and kmeans methods.\n"
		"The minpartitions parameter is used to .... Default=5.\n"
		"The maxpartitions parameter is used to .... Default=10.\n"
		"The optimizegap parameter is used to .... Default=3.\n"
		"The processors parameter allows you to specify number of processors to use.  The default is 1.\n"
		"The get.communitytype command should be in the following format: get.communitytype(shared=yourSharedFile).\n";
	return helpString;
}
//**********************************************************************************************************************
string GetMetaCommunityCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fit") { pattern = "[filename],[distance],[method],mix.fit"; }
	else if (type == "relabund") { pattern = "[filename],[distance],[method],[tag],mix.relabund"; }
	else if (type == "design") { pattern = "[filename],[distance],[method],mix.design"; }
	else if (type == "matrix") { pattern = "[filename],[distance],[method],[tag],mix.posterior"; }
	else if (type == "parameters") { pattern = "[filename],[distance],[method],mix.parameters"; }
	else if (type == "summary") { pattern = "[filename],[distance],[method],mix.summary"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
GetMetaCommunityCommand::GetMetaCommunityCommand(Settings& settings) : Command(settings) {
	abort = true; calledHelp = true;
	setParameters();
	vector<string> tempOutNames;
	outputTypes["fit"] = tempOutNames;
	outputTypes["relabund"] = tempOutNames;
	outputTypes["matrix"] = tempOutNames;
	outputTypes["design"] = tempOutNames;
	outputTypes["parameters"] = tempOutNames;
	outputTypes["summary"] = tempOutNames;
}
//**********************************************************************************************************************
GetMetaCommunityCommand::GetMetaCommunityCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	allLines = true;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		//valid paramters for this command
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
		outputTypes["fit"] = tempOutNames;
		outputTypes["relabund"] = tempOutNames;
		outputTypes["matrix"] = tempOutNames;
		outputTypes["design"] = tempOutNames;
		outputTypes["parameters"] = tempOutNames;
		outputTypes["summary"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("shared");
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				if (path == "") { parameters["shared"] = inputDir + it->second; }
			}
		}

		//get shared file, it is required
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
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = File::getPath(sharedfile); //if user entered a file with a path then preserve it
		}

		string temp = validParameter.validFile(parameters, "minpartitions", false);	if (temp == "not found") { temp = "5"; }
		Utility::mothurConvert(temp, minpartitions);

		temp = validParameter.validFile(parameters, "maxpartitions", false);        if (temp == "not found") { temp = "10"; }
		Utility::mothurConvert(temp, maxpartitions);

		temp = validParameter.validFile(parameters, "optimizegap", false);          if (temp == "not found") { temp = "3"; }
		Utility::mothurConvert(temp, optimizegap);

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		//set processors to 1 until we figure out whats going on with this command.
		temp = "1";
		//settings.setProcessors(temp);
		LOG(INFO) << "Using 1 processor\n";
		Utility::mothurConvert(temp, processors);

		string groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else { Utility::split(groups, '-', Groups); }
		m->setGroups(Groups);

		string label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		method = validParameter.validFile(parameters, "method", false);
		if (method == "not found") { method = "dmm"; }

		if ((method == "dmm") || (method == "kmeans") || (method == "pam")) {}
		else { LOG(LOGERROR) << "" + method + " is not a valid method.  Valid algorithms are dmm, kmeans and pam." << '\n'; abort = true; }

		calc = validParameter.validFile(parameters, "calc", false);
		if (calc == "not found") { calc = "rjsd"; }
		else {
			if (calc == "default") { calc = "rjsd"; }
		}
		Utility::split(calc, '-', Estimators);
		if (m->inUsersGroups("citation", Estimators)) {
			ValidCalculators validCalc; validCalc.printCitations(Estimators);
			//remove citation from list of calcs
			for (int i = 0; i < Estimators.size(); i++) { if (Estimators[i] == "citation") { Estimators.erase(Estimators.begin() + i); break; } }
		}
		if (Estimators.size() != 1) { abort = true; LOG(LOGERROR) << "only one calculator is allowed.\n"; }

		temp = validParameter.validFile(parameters, "iters", false);			if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, iters);

		temp = validParameter.validFile(parameters, "subsample", false);		if (temp == "not found") { temp = "F"; }
		if (m->isNumeric1(temp)) { Utility::mothurConvert(temp, subsampleSize); subsample = true; }
		else {
			if (m->isTrue(temp)) { subsample = true; subsampleSize = -1; }  //we will set it to smallest group later
			else { subsample = false; }
		}

		if (subsample == false) { iters = 0; }
	}

}
//**********************************************************************************************************************

int GetMetaCommunityCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	InputData input(sharedfile, "sharedfile");
	vector<SharedRAbundVector*> lookup = input.getSharedRAbundVectors();
	string lastLabel = lookup[0]->getLabel();

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

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

		if (lookup.size() < 2) { LOG(INFO) << "You have not provided enough valid groups.  I cannot run the command." << '\n'; ctrlc_pressed = true;  return 0; }
	}


	//as long as you are not at the end of the file or done wih the lines you want
	while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

		if (ctrlc_pressed) { for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }  return 0; }

		if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

			LOG(INFO) << lookup[0]->getLabel() << '\n';

			createProcesses(lookup);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());
		}

		if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = lookup[0]->getLabel();

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input.getSharedRAbundVectors(lastLabel);
			LOG(INFO) << lookup[0]->getLabel() << '\n';

			createProcesses(lookup);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());

			//restore real lastlabel to save below
			lookup[0]->setLabel(saveLabel);
		}

		lastLabel = lookup[0]->getLabel();
		//prevent memory leak
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }

		if (ctrlc_pressed) { return 0; }

		//get next line to process
		lookup = input.getSharedRAbundVectors();
	}

	if (ctrlc_pressed) { return 0; }

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

		createProcesses(lookup);

		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
	}

	//output files created by command
	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";
	return 0;

}
//**********************************************************************************************************************
int GetMetaCommunityCommand::createProcesses(vector<SharedRAbundVector*>& thislookup) {

	//#if defined (UNIX)
   // #else
	//until bug is resolved
	processors = 1; //qFinderDMM not thread safe
	//#endif

	vector<int> processIDS;
	int process = 1;
	int num = 0;
	int minPartition = 0;

	//sanity check
	if (maxpartitions < processors) { processors = maxpartitions; }

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(sharedfile));
	variables["[distance]"] = thislookup[0]->getLabel();
	variables["[method]"] = method;
	string outputFileName = getOutputFileName("fit", variables);
	outputNames.push_back(outputFileName); outputTypes["fit"].push_back(outputFileName);

	//divide the partitions between the processors
	vector< vector<int> > dividedPartitions;
	vector< vector<string> > rels, matrix;
	vector<string> doneFlags;
	dividedPartitions.resize(processors);
	rels.resize(processors);
	matrix.resize(processors);

	//for each file group figure out which process will complete it
	//want to divide the load intelligently so the big files are spread between processes
	for (int i = 1; i <= maxpartitions; i++) {
		//cout << i << endl;
		int processToAssign = (i + 1) % processors;
		if (processToAssign == 0) { processToAssign = processors; }

		if (app.isDebug) { LOG(DEBUG) << "assigning " + toString(i) + " to process " + toString(processToAssign - 1) + "\n"; }
		dividedPartitions[(processToAssign - 1)].push_back(i);

		variables["[tag]"] = toString(i);
		string relName = getOutputFileName("relabund", variables);
		string mName = getOutputFileName("matrix", variables);
		rels[(processToAssign - 1)].push_back(relName);
		matrix[(processToAssign - 1)].push_back(mName);
	}

	for (int i = 0; i < processors; i++) { //read from everyone elses, just write to yours
		string tempDoneFile = File::getRootName(File::getSimpleName(sharedfile)) + toString(i) + ".done.temp";
		doneFlags.push_back(tempDoneFile);
		ofstream out;
		File::openOutputFile(tempDoneFile, out); //clear out 
		out.close();
	}


#if defined (UNIX)

	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
			process++;
		}
		else if (pid == 0) {
			outputNames.clear();
			num = processDriver(thislookup, dividedPartitions[process], (outputFileName + m->mothurGetpid(process)), rels[process], matrix[process], doneFlags, process);

			//pass numSeqs to parent
			ofstream out;
			string tempFile = m->mothurGetpid(process) + ".outputNames.temp";
			File::openOutputFile(tempFile, out);
			out << num << endl;
			out << outputNames.size() << endl;
			for (int i = 0; i < outputNames.size(); i++) { out << outputNames[i] << endl; }
			out.close();

			exit(0);
		}
		else {
			LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
			for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
			exit(0);
		}
	}

	//do my part
	if (method == "dmm") { LOG(INFO) << "K\tNLE\t\tlogDet\tBIC\t\tAIC\t\tLaplace\n"; }
	else {
		LOG(INFO) << "K\tCH";
		for (int i = 0; i < thislookup.size(); i++) { LOG(INFO) << '\t' + thislookup[i]->getGroup(); }
		LOG(INFO) << "\n";
	}
	minPartition = processDriver(thislookup, dividedPartitions[0], outputFileName, rels[0], matrix[0], doneFlags, 0);

	//force parent to wait until all the processes are done
	for (int i = 0;i < processIDS.size();i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	vector<string> tempOutputNames = outputNames;
	for (int i = 0;i < processIDS.size();i++) {
		ifstream in;
		string tempFile = toString(processIDS[i]) + ".outputNames.temp";
		File::openInputFile(tempFile, in);
		if (!in.eof()) {
			int tempNum = 0;
			in >> tempNum; File::gobble(in);
			if (tempNum < minPartition) { minPartition = tempNum; }
			in >> tempNum; File::gobble(in);
			for (int i = 0; i < tempNum; i++) {
				string tempName = "";
				in >> tempName; File::gobble(in);
				tempOutputNames.push_back(tempName);
			}
		}
		in.close(); File::remove(tempFile);

		File::appendFilesWithoutHeaders(outputFileName + toString(processIDS[i]), outputFileName);
		File::remove(outputFileName + toString(processIDS[i]));
	}

	if (processors > 1) {
		outputNames.clear();
		for (int i = 0; i < tempOutputNames.size(); i++) { //remove files if needed
			string name = tempOutputNames[i];
			vector<string> parts;
			m->splitAtChar(name, parts, '.');
			bool keep = true;
			if (((parts[parts.size() - 1] == "relabund") || (parts[parts.size() - 1] == "posterior")) && (parts[parts.size() - 2] == "mix")) {
				string tempNum = parts[parts.size() - 3];
				int num;  Utility::mothurConvert(tempNum, num);
				//if (num > minPartition) {
				 //   File::remove(tempOutputNames[i]);
				//    keep = false; if (app.isDebug) { LOG(DEBUG) << "removing " + tempOutputNames[i] + ".\n"; }
				//}
			}
			if (keep) { outputNames.push_back(tempOutputNames[i]); }
		}

		//reorder fit file
		ifstream in;
		File::openInputFile(outputFileName, in);
		string headers = File::getline(in); File::gobble(in);

		map<int, string> file;
		while (!in.eof()) {
			string numString, line;
			int num;
			in >> numString; line = File::getline(in); File::gobble(in);
			Utility::mothurConvert(numString, num);
			file[num] = line;
		}
		in.close();
		ofstream out;
		File::openOutputFile(outputFileName, out);
		out << headers << endl;
		for (map<int, string>::iterator it = file.begin(); it != file.end(); it++) {
			out << it->first << '\t' << it->second << endl;
			if (app.isDebug) { LOG(DEBUG) << "printing: " + toString(it->first) + '\t' + it->second + ".\n"; }
		}
		out.close();
	}

#else
	LOG(INFO) << "K\tNLE\t\tlogDet\tBIC\t\tAIC\t\tLaplace\n";
	minPartition = processDriver(thislookup, dividedPartitions[0], outputFileName, rels[0], matrix[0], doneFlags, 0);
#endif
	for (int i = 0; i < processors; i++) { //read from everyone elses, just write to yours
		string tempDoneFile = File::getRootName(File::getSimpleName(sharedfile)) + toString(i) + ".done.temp";
		File::remove(tempDoneFile);
	}

	if (ctrlc_pressed) { return 0; }

	if (app.isDebug) { LOG(DEBUG) << "minPartition = " + toString(minPartition) + "\n"; }

	//run generate Summary function for smallest minPartition
	variables["[tag]"] = toString(minPartition);
	vector<double> piValues = generateDesignFile(minPartition, variables);
	if (method == "dmm") { generateSummaryFile(minPartition, variables, piValues); } //pam doesn't make a relabund file

	return 0;

}
//**********************************************************************************************************************
int GetMetaCommunityCommand::processDriver(vector<SharedRAbundVector*>& thislookup, vector<int>& parts, string outputFileName, vector<string> relabunds, vector<string> matrix, vector<string> doneFlags, int processID) {

	double minLaplace = 1e10;
	int minPartition = 1;
	vector<double> minSilhouettes; minSilhouettes.resize(thislookup.size(), 0);

	ofstream fitData, silData;
	if (method == "dmm") {
		File::openOutputFile(outputFileName, fitData);
		fitData.setf(ios::fixed, ios::floatfield);
		fitData.setf(ios::showpoint);
		fitData << "K\tNLE\tlogDet\tBIC\tAIC\tLaplace" << endl;
	}
	else if ((method == "pam") || (method == "kmeans")) { //because ch is looking of maximal value
		minLaplace = 0;
		File::openOutputFile(outputFileName, silData);
		silData.setf(ios::fixed, ios::floatfield);
		silData.setf(ios::showpoint);
		silData << "K\tCH";
		for (int i = 0; i < thislookup.size(); i++) { silData << '\t' << thislookup[i]->getGroup(); }
		silData << endl;
	}

	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::showpoint);

	vector< vector<int> > sharedMatrix;
	vector<string> thisGroups;
	for (int i = 0; i < thislookup.size(); i++) { sharedMatrix.push_back(thislookup[i]->getAbundances()); thisGroups.push_back(thislookup[i]->getGroup()); }

	vector< vector<double> > dists; //do we want to output this matrix??
	if ((method == "pam") || (method == "kmeans")) { dists = generateDistanceMatrix(thislookup); }

	if (app.isDebug) {
		LOG(DEBUG) << "dists = \n";
		for (int i = 0; i < dists.size(); i++) {
			if (ctrlc_pressed) { break; }
			LOG(DEBUG) << "i = " + toString(i) + '\t';
			for (int j = 0; j < i; j++) { LOG(INFO) << toString(dists[i][j]) + "\t"; }
			LOG(INFO) << "\n";
		}
	}

	for (int i = 0;i < parts.size();i++) {

		int numPartitions = parts[i];

		if (app.isDebug) { LOG(DEBUG) << "running partition " + toString(numPartitions) + "\n"; }

		if (ctrlc_pressed) { break; }

		//check to see if anyone else is done
		for (int j = 0; j < doneFlags.size(); j++) {
			if (!File::isBlank(doneFlags[j])) { //another process has finished
				//are they done at a lower partition?
				ifstream in;
				File::openInputFile(doneFlags[j], in);
				int tempNum;
				in >> tempNum; in.close();
				if (tempNum < numPartitions) { break; } //quit, because someone else has finished
			}
		}

		CommunityTypeFinder* finder = NULL;
		if (method == "dmm") { finder = new qFinderDMM(sharedMatrix, numPartitions); }
		else if (method == "kmeans") { finder = new KMeans(sharedMatrix, numPartitions); }
		else if (method == "pam") { finder = new Pam(sharedMatrix, dists, numPartitions); }
		else {
			if (i == 0) { LOG(INFO) << method + " is not a valid method option. I will run the command using dmm.\n"; }
			finder = new qFinderDMM(sharedMatrix, numPartitions);
		}

		string relabund = relabunds[i];
		string matrixName = matrix[i];
		outputNames.push_back(matrixName); outputTypes["matrix"].push_back(matrixName);

		finder->printZMatrix(matrixName, thisGroups);

		double chi; vector<double> silhouettes;
		if (method == "dmm") {
			double laplace = finder->getLaplace();
			if (laplace < minLaplace) {
				minPartition = numPartitions;
				minLaplace = laplace;
			}
		}
		else {
			chi = finder->calcCHIndex(dists);
			silhouettes = finder->calcSilhouettes(dists);
			if (chi > minLaplace) { //save partition with maximum ch index score
				minPartition = numPartitions;
				minLaplace = chi;
				minSilhouettes = silhouettes;
			}
		}

		if (method == "dmm") {
			finder->printFitData(cout, minLaplace);
			finder->printFitData(fitData);
			finder->printRelAbund(relabund, settings.currentSharedBinLabels);
			outputNames.push_back(relabund); outputTypes["relabund"].push_back(relabund);
		}
		else if ((method == "pam") || (method == "kmeans")) { //print silouettes and ch values
			finder->printSilData(cout, chi, silhouettes);
			finder->printSilData(silData, chi, silhouettes);
			if (method == "kmeans") {
				finder->printRelAbund(relabund, settings.currentSharedBinLabels);
				outputNames.push_back(relabund); outputTypes["relabund"].push_back(relabund);
			}
		}
		delete finder;

		if (optimizegap != -1 && (numPartitions - minPartition) >= optimizegap && numPartitions >= minpartitions) {
			string tempDoneFile = File::getRootName(File::getSimpleName(sharedfile)) + toString(processID) + ".done.temp";
			ofstream outDone;
			File::openOutputFile(tempDoneFile, outDone);
			outDone << minPartition << endl;
			outDone.close();
			break;
		}
	}
	if (method == "dmm") { fitData.close(); }

	if (ctrlc_pressed) { return 0; }

	return minPartition;
}
/**************************************************************************************************/

vector<double> GetMetaCommunityCommand::generateDesignFile(int numPartitions, map<string, string> variables) {
	vector<double> piValues(numPartitions, 0);

	ifstream postFile;
	variables["[tag]"] = toString(numPartitions);
	string input = getOutputFileName("matrix", variables);
	File::openInputFile(input, postFile);//((fileRoot + toString(numPartitions) + "mix.posterior").c_str()); //matrix file
	variables.erase("[tag]");
	string outputFileName = getOutputFileName("design", variables);
	ofstream designFile;
	File::openOutputFile(outputFileName, designFile);
	outputNames.push_back(outputFileName); outputTypes["design"].push_back(outputFileName);


	vector<string> titles(numPartitions);

	for (int i = 0;i < numPartitions;i++) { postFile >> titles[i]; }

	double posterior;
	string sampleName;
	int numSamples = 0;

	while (postFile) {

		if (ctrlc_pressed) { break; }

		double maxPosterior = 0.0000;
		int maxPartition = -1;

		postFile >> sampleName;

		for (int i = 0;i < numPartitions;i++) {

			postFile >> posterior;
			if (posterior > maxPosterior) {
				maxPosterior = posterior;
				maxPartition = i;
			}
			piValues[i] += posterior;

		}

		designFile << sampleName << '\t' << titles[maxPartition] << endl;

		numSamples++;
		File::gobble(postFile);
	}
	for (int i = 0;i < numPartitions;i++) {
		piValues[i] /= (double)numSamples;
	}


	postFile.close();
	designFile.close();

	return piValues;
}

/**************************************************************************************************/

inline bool summaryFunction(summaryData i, summaryData j) { return i.difference > j.difference; }

/**************************************************************************************************/
int GetMetaCommunityCommand::generateSummaryFile(int numPartitions, map<string, string> v, vector<double> piValues) {
	vector<summaryData> summary;

	vector<double> pMean(numPartitions, 0);
	vector<double> pLCI(numPartitions, 0);
	vector<double> pUCI(numPartitions, 0);

	string name, header;
	double mean, lci, uci;

	ifstream referenceFile;
	map<string, string> variables;
	variables["[filename]"] = v["[filename]"];
	variables["[distance]"] = v["[distance]"];
	variables["[method]"] = method;
	variables["[tag]"] = "1";
	string reference = getOutputFileName("relabund", variables);
	File::openInputFile(reference, referenceFile); //((fileRoot + label + ".1mix.relabund").c_str());
	variables["[tag]"] = toString(numPartitions);
	string partFile = getOutputFileName("relabund", variables);
	ifstream partitionFile;
	File::openInputFile(partFile, partitionFile); //((fileRoot + toString(numPartitions) + "mix.relabund").c_str());

	header = File::getline(referenceFile);
	header = File::getline(partitionFile);
	stringstream head(header);
	string dummy, label;
	head >> dummy;
	vector<string> thetaValues(numPartitions, "");
	for (int i = 0;i < numPartitions;i++) {
		head >> label >> dummy >> dummy;
		thetaValues[i] = label.substr(label.find_last_of('_') + 1);
	}


	vector<double> partitionDiff(numPartitions, 0.0000);

	while (referenceFile) {

		if (ctrlc_pressed) { break; }

		referenceFile >> name >> mean >> lci >> uci;

		summaryData tempData;
		tempData.name = name;
		tempData.refMean = mean;

		double difference = 0.0000;

		partitionFile >> name;
		for (int j = 0;j < numPartitions;j++) {
			partitionFile >> pMean[j] >> pLCI[j] >> pUCI[j];
			difference += abs(mean - pMean[j]);
			partitionDiff[j] += abs(mean - pMean[j]);;
		}

		tempData.partMean = pMean;
		tempData.partLCI = pLCI;
		tempData.partUCI = pUCI;
		tempData.difference = difference;
		summary.push_back(tempData);

		File::gobble(referenceFile);
		File::gobble(partitionFile);
	}
	referenceFile.close();
	partitionFile.close();

	if (ctrlc_pressed) { return 0; }

	int numOTUs = (int)summary.size();

	sort(summary.begin(), summary.end(), summaryFunction);

	variables.erase("[tag]");
	string outputFileName = getOutputFileName("parameters", variables);
	outputNames.push_back(outputFileName); outputTypes["parameters"].push_back(outputFileName);

	ofstream parameterFile;
	File::openOutputFile(outputFileName, parameterFile); //((fileRoot + "mix.parameters").c_str());
	parameterFile.setf(ios::fixed, ios::floatfield);
	parameterFile.setf(ios::showpoint);

	double totalDifference = 0.0000;
	parameterFile << "Part\tDif2Ref_i\ttheta_i\tpi_i\n";
	for (int i = 0;i < numPartitions;i++) {
		if (ctrlc_pressed) { break; }
		parameterFile << i + 1 << '\t' << setprecision(2) << partitionDiff[i] << '\t' << thetaValues[i] << '\t' << piValues[i] << endl;
		totalDifference += partitionDiff[i];
	}
	parameterFile.close();

	if (ctrlc_pressed) { return 0; }

	string summaryFileName = getOutputFileName("summary", variables);
	outputNames.push_back(summaryFileName); outputTypes["summary"].push_back(summaryFileName);

	ofstream summaryFile;
	File::openOutputFile(summaryFileName, summaryFile); //((fileRoot + "mix.summary").c_str());
	summaryFile.setf(ios::fixed, ios::floatfield);
	summaryFile.setf(ios::showpoint);


	summaryFile << "OTU\tP0.mean";
	for (int i = 0;i < numPartitions;i++) {
		summaryFile << "\tP" << i + 1 << ".mean\tP" << i + 1 << ".lci\tP" << i + 1 << ".uci";
	}
	summaryFile << "\tDifference\tCumFraction" << endl;

	double cumDiff = 0.0000;

	for (int i = 0;i < numOTUs;i++) {
		if (ctrlc_pressed) { break; }
		summaryFile << summary[i].name << setprecision(2) << '\t' << summary[i].refMean;
		for (int j = 0;j < numPartitions;j++) {
			summaryFile << '\t' << summary[i].partMean[j] << '\t' << summary[i].partLCI[j] << '\t' << summary[i].partUCI[j];
		}

		cumDiff += summary[i].difference / totalDifference;
		summaryFile << '\t' << summary[i].difference << '\t' << cumDiff << endl;
	}
	summaryFile.close();

	return 0;


}
//**********************************************************************************************************************
vector<vector<double> > GetMetaCommunityCommand::generateDistanceMatrix(vector<SharedRAbundVector*>& thisLookup) {
	vector<vector<double> > results;

	Calculator* matrixCalculator;
	ValidCalculators validCalculator;
	int i = 0;

	if (validCalculator.isValidCalculator("matrix", Estimators[i]) == true) {
		if (Estimators[i] == "sharedsobs") {
			matrixCalculator = new SharedSobsCS();
		}
		else if (Estimators[i] == "sharedchao") {
			matrixCalculator = new SharedChao1();
		}
		else if (Estimators[i] == "sharedace") {
			matrixCalculator = new SharedAce();
		}
		else if (Estimators[i] == "jabund") {
			matrixCalculator = new JAbund();
		}
		else if (Estimators[i] == "sorabund") {
			matrixCalculator = new SorAbund();
		}
		else if (Estimators[i] == "jclass") {
			matrixCalculator = new Jclass();
		}
		else if (Estimators[i] == "sorclass") {
			matrixCalculator = new SorClass();
		}
		else if (Estimators[i] == "jest") {
			matrixCalculator = new Jest();
		}
		else if (Estimators[i] == "sorest") {
			matrixCalculator = new SorEst();
		}
		else if (Estimators[i] == "thetayc") {
			matrixCalculator = new ThetaYC();
		}
		else if (Estimators[i] == "thetan") {
			matrixCalculator = new ThetaN();
		}
		else if (Estimators[i] == "kstest") {
			matrixCalculator = new KSTest();
		}
		else if (Estimators[i] == "sharednseqs") {
			matrixCalculator = new SharedNSeqs();
		}
		else if (Estimators[i] == "ochiai") {
			matrixCalculator = new Ochiai();
		}
		else if (Estimators[i] == "anderberg") {
			matrixCalculator = new Anderberg();
		}
		else if (Estimators[i] == "kulczynski") {
			matrixCalculator = new Kulczynski();
		}
		else if (Estimators[i] == "kulczynskicody") {
			matrixCalculator = new KulczynskiCody();
		}
		else if (Estimators[i] == "lennon") {
			matrixCalculator = new Lennon();
		}
		else if (Estimators[i] == "morisitahorn") {
			matrixCalculator = new MorHorn();
		}
		else if (Estimators[i] == "braycurtis") {
			matrixCalculator = new BrayCurtis();
		}
		else if (Estimators[i] == "whittaker") {
			matrixCalculator = new Whittaker();
		}
		else if (Estimators[i] == "odum") {
			matrixCalculator = new Odum();
		}
		else if (Estimators[i] == "canberra") {
			matrixCalculator = new Canberra();
		}
		else if (Estimators[i] == "structeuclidean") {
			matrixCalculator = new StructEuclidean();
		}
		else if (Estimators[i] == "structchord") {
			matrixCalculator = new StructChord();
		}
		else if (Estimators[i] == "hellinger") {
			matrixCalculator = new Hellinger();
		}
		else if (Estimators[i] == "manhattan") {
			matrixCalculator = new Manhattan();
		}
		else if (Estimators[i] == "structpearson") {
			matrixCalculator = new StructPearson();
		}
		else if (Estimators[i] == "soergel") {
			matrixCalculator = new Soergel();
		}
		else if (Estimators[i] == "spearman") {
			matrixCalculator = new Spearman();
		}
		else if (Estimators[i] == "structkulczynski") {
			matrixCalculator = new StructKulczynski();
		}
		else if (Estimators[i] == "speciesprofile") {
			matrixCalculator = new SpeciesProfile();
		}
		else if (Estimators[i] == "hamming") {
			matrixCalculator = new Hamming();
		}
		else if (Estimators[i] == "structchi2") {
			matrixCalculator = new StructChi2();
		}
		else if (Estimators[i] == "gower") {
			matrixCalculator = new Gower();
		}
		else if (Estimators[i] == "memchi2") {
			matrixCalculator = new MemChi2();
		}
		else if (Estimators[i] == "memchord") {
			matrixCalculator = new MemChord();
		}
		else if (Estimators[i] == "memeuclidean") {
			matrixCalculator = new MemEuclidean();
		}
		else if (Estimators[i] == "mempearson") {
			matrixCalculator = new MemPearson();
		}
		else if (Estimators[i] == "jsd") {
			matrixCalculator = new JSD();
		}
		else if (Estimators[i] == "rjsd") {
			matrixCalculator = new RJSD();
		}
		else {
			LOG(LOGERROR) << "" + Estimators[i] + " is not a valid calculator, please correct.\n"; ctrlc_pressed = true; return results;
		}
	}

	//calc distances
	vector< vector< vector<seqDist> > > calcDistsTotals;  //each iter, then each groupCombos dists. this will be used to make .dist files
	vector< vector<seqDist> > calcDists; calcDists.resize(1);

	for (int thisIter = 0; thisIter < iters + 1; thisIter++) {

		vector<SharedRAbundVector*> thisItersLookup = thisLookup;

		if (subsample && (thisIter != 0)) {
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
				if (ctrlc_pressed) { for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; } return results; }
				for (int j = 0; j < thisItersLookup.size(); j++) { newLookup[j]->push_back(thisItersLookup[j]->getAbundance(k), thisItersLookup[j]->getGroup()); }
			}

			tempLabels = sample.getSample(newLookup, subsampleSize);
			thisItersLookup = newLookup;
		}


		driver(thisItersLookup, calcDists, matrixCalculator);

		if (subsample && (thisIter != 0)) {
			if ((thisIter) % 100 == 0) { LOG(SCREENONLY) << toString(thisIter) + "\n"; }
			calcDistsTotals.push_back(calcDists);
			for (int i = 0; i < calcDists.size(); i++) {
				for (int j = 0; j < calcDists[i].size(); j++) {
					if (app.isDebug) { LOG(DEBUG) << "Results: iter = " + toString(thisIter) + ", " + thisLookup[calcDists[i][j].seq1]->getGroup() + " - " + thisLookup[calcDists[i][j].seq2]->getGroup() + " distance = " + toString(calcDists[i][j].dist) + ".\n"; }
				}
			}
			//clean up memory
			for (int i = 0; i < thisItersLookup.size(); i++) { delete thisItersLookup[i]; }
			thisItersLookup.clear();
		}
		else { //print results for whole dataset
			for (int i = 0; i < calcDists.size(); i++) {
				if (ctrlc_pressed) { break; }

				//initialize matrix
				results.resize(thisLookup.size());
				for (int k = 0; k < thisLookup.size(); k++) { results[k].resize(thisLookup.size(), 0.0); }

				for (int j = 0; j < calcDists[i].size(); j++) {
					int row = calcDists[i][j].seq1;
					int column = calcDists[i][j].seq2;
					double dist = calcDists[i][j].dist;

					results[row][column] = dist;
					results[column][row] = dist;
				}
			}
		}
		for (int i = 0; i < calcDists.size(); i++) { calcDists[i].clear(); }
	}

	if (iters != 0) {
		//we need to find the average distance and standard deviation for each groups distance
		vector< vector<seqDist>  > calcAverages = m->getAverages(calcDistsTotals, "average");

		//print results
		for (int i = 0; i < calcDists.size(); i++) {
			results.resize(thisLookup.size());
			for (int k = 0; k < thisLookup.size(); k++) { results[k].resize(thisLookup.size(), 0.0); }

			for (int j = 0; j < calcAverages[i].size(); j++) {
				int row = calcAverages[i][j].seq1;
				int column = calcAverages[i][j].seq2;
				float dist = calcAverages[i][j].dist;

				results[row][column] = dist;
				results[column][row] = dist;
			}
		}
	}


	return results;
}
/**************************************************************************************************/
int GetMetaCommunityCommand::driver(vector<SharedRAbundVector*> thisLookup, vector< vector<seqDist> >& calcDists, Calculator* matrixCalculator) {
	vector<SharedRAbundVector*> subset;

	for (int k = 0; k < thisLookup.size(); k++) { // pass cdd each set of groups to compare

		for (int l = 0; l < k; l++) {

			if (k != l) { //we dont need to similiarity of a groups to itself
				subset.clear(); //clear out old pair of sharedrabunds
				//add new pair of sharedrabunds
				subset.push_back(thisLookup[k]); subset.push_back(thisLookup[l]);



				//if this calc needs all groups to calculate the pair load all groups
				if (matrixCalculator->getNeedsAll()) {
					//load subset with rest of lookup for those calcs that need everyone to calc for a pair
					for (int w = 0; w < thisLookup.size(); w++) {
						if ((w != k) && (w != l)) { subset.push_back(thisLookup[w]); }
					}
				}

				vector<double> tempdata = matrixCalculator->getValues(subset); //saves the calculator outputs

				if (ctrlc_pressed) { return 1; }

				seqDist temp(l, k, tempdata[0]);
				//cout << l << '\t' << k << '\t' <<  tempdata[0] << endl;
				calcDists[0].push_back(temp);
			}

		}
	}

	return 0;
}
//**********************************************************************************************************************

