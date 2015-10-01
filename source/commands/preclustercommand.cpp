/*
 *  preclustercommand.cpp
 *  Mothur
 *
 *  Created by westcott on 12/21/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "preclustercommand.h"
#include "deconvolutecommand.h"

 //**********************************************************************************************************************
vector<string> PreClusterCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "fasta-name", false, true, true); parameters.push_back(pfasta);
		nkParameters.add(new NameFileParameter(settings, "name", false, true, "NameCount"));
		nkParameters.add(new CountFileParameter(settings, "count", false, true, "NameCount-CountGroup"));
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none", "", false, false, true); parameters.push_back(pgroup);
		CommandParameter pdiffs("diffs", "Number", "", "1", "", "", "", "", false, false, true); parameters.push_back(pdiffs);
		nkParameters.add(new ProcessorsParameter(settings));
		CommandParameter palign("align", "Multiple", "needleman-gotoh-blast-noalign", "needleman", "", "", "", "", false, false); parameters.push_back(palign);
		nkParameters.add(new NumberParameter("match", -INFINITY, INFINITY, 1.0, false, false));
		nkParameters.add(new NumberParameter("mismatch", -INFINITY, INFINITY, -1.0, false, false));
		nkParameters.add(new NumberParameter("gapopen", -INFINITY, INFINITY, -2.0, false, false));
		nkParameters.add(new NumberParameter("gapextend", -INFINITY, INFINITY, -1.0, false, false));

		nkParameters.add(new BooleanParameter("topdown", true, false, false));

		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PreClusterCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string PreClusterCommand::getHelpString() {
	try {
		string helpString = "The pre.cluster command groups sequences that are within a given number of base mismatches.\n"
			"The pre.cluster command outputs a new fasta and name file.\n"
			"The pre.cluster command parameters are fasta, name, group, count, topdown, processors and diffs. The fasta parameter is required. \n"
			"The name parameter allows you to give a list of seqs that are identical. This file is 2 columns, first column is name or representative sequence, second column is a list of its identical sequences separated by commas.\n"
			"The group parameter allows you to provide a group file so you can cluster by group. \n"
			"The count parameter allows you to provide a count file so you can cluster by group. \n"
			"The diffs parameter allows you to specify maximum number of mismatched bases allowed between sequences in a grouping. The default is 1.\n"
			"The topdown parameter allows you to specify whether to cluster from largest abundance to smallest or smallest to largest.  Default=T, meaning largest to smallest.\n"
			"The align parameter allows you to specify the alignment method to use.  Your options are: gotoh, needleman, blast and noalign. The default is needleman.\n"
			"The match parameter allows you to specify the bonus for having the same base. The default is 1.0.\n"
			"The mistmatch parameter allows you to specify the penalty for having different bases.  The default is -1.0.\n"
			"The gapopen parameter allows you to specify the penalty for opening a gap in an alignment. The default is -2.0.\n"
			"The gapextend parameter allows you to specify the penalty for extending a gap in an alignment.  The default is -1.0.\n"
			"The pre.cluster command should be in the following format: \n"
			"pre.cluster(fasta=yourFastaFile, names=yourNamesFile, diffs=yourMaxDiffs) \n"
			"Example pre.cluster(fasta=amazon.fasta, diffs=2).\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PreClusterCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string PreClusterCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],precluster,[extension]"; }
	else if (type == "name") { pattern = "[filename],precluster.names"; }
	else if (type == "count") { pattern = "[filename],precluster.count_table"; }
	else if (type == "map") { pattern = "[filename],precluster.map"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
PreClusterCommand::PreClusterCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["map"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PreClusterCommand, PreClusterCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

PreClusterCommand::PreClusterCommand(Settings& settings, string option) : Command(settings, option) {
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
		for (map<string, string>::iterator it2 = parameters.begin(); it2 != parameters.end(); it2++) {
			if (validParameter.isValidParameter(it2->first, myArray, it2->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["map"] = tempOutNames;
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

		//check for required parameters
		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not found") {
			fastafile = settings.getCurrent("fasta");
			if (fastafile != "") { LOG(INFO) << "Using " + fastafile + " as input file for the fasta parameter." << '\n'; }
			else { LOG(INFO) << "You have no current fastafile and the fasta parameter is required." << '\n'; abort = true; }
		}
		else if (fastafile == "not open") { abort = true; }
		else { settings.setCurrent("fasta", fastafile); }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = "";
			outputDir += File::getPath(fastafile); //if user entered a file with a path then preserve it	
		}

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not found") { namefile = ""; }
		else if (namefile == "not open") { namefile = ""; abort = true; }
		else { settings.setCurrent("name", namefile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not found") { groupfile = "";  bygroup = false; }
		else if (groupfile == "not open") { abort = true; groupfile = ""; }
		else { settings.setCurrent("group", groupfile); bygroup = true; }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not found") { countfile = ""; }
		else if (countfile == "not open") { abort = true; countfile = ""; }
		else {
			settings.setCurrent("counttable", countfile);
			ct.readTable(countfile, true, false);
			if (ct.hasGroupInfo()) { bygroup = true; }
			else { bygroup = false; }
		}

		if ((namefile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: name or count." << '\n'; abort = true;
		}

		if ((groupfile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: group or count." << '\n'; abort = true;
		}


		string temp = validParameter.validFile(parameters, "diffs", false);		if (temp == "not found") { temp = "1"; }
		Utility::mothurConvert(temp, diffs);

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "topdown", false);		if (temp == "not found") { temp = "T"; }
		topdown = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "match", false);		if (temp == "not found") { temp = "1.0"; }
		Utility::mothurConvert(temp, match);

		temp = validParameter.validFile(parameters, "mismatch", false);		if (temp == "not found") { temp = "-1.0"; }
		Utility::mothurConvert(temp, misMatch);
		if (misMatch > 0) { LOG(LOGERROR) << "mismatch must be negative.\n"; abort = true; }

		temp = validParameter.validFile(parameters, "gapopen", false);		if (temp == "not found") { temp = "-2.0"; }
		Utility::mothurConvert(temp, gapOpen);
		if (gapOpen > 0) { LOG(LOGERROR) << "gapopen must be negative.\n"; abort = true; }

		temp = validParameter.validFile(parameters, "gapextend", false);	if (temp == "not found") { temp = "-1.0"; }
		Utility::mothurConvert(temp, gapExtend);
		if (gapExtend > 0) { LOG(LOGERROR) << "gapextend must be negative.\n"; abort = true; }

		align = validParameter.validFile(parameters, "align", false);		if (align == "not found") { align = "needleman"; }

		method = "unaligned";

		if (countfile == "") {
			if (namefile == "") {
				vector<string> files; files.push_back(fastafile);
				OptionParser::getNameFile(files);
			}
		}
	}

}
//**********************************************************************************************************************

int PreClusterCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	int start = time(NULL);

	if (align == "gotoh") { alignment = new GotohOverlap(gapOpen, gapExtend, match, misMatch, 1000); }
	else if (align == "needleman") { alignment = new NeedlemanOverlap(gapOpen, match, misMatch, 1000); }
	else if (align == "blast") { alignment = new BlastAlignment(gapOpen, gapExtend, match, misMatch); }
	else if (align == "noalign") { alignment = new NoAlign(); }
	else {
		LOG(INFO) << align + " is not a valid alignment option. I will run the command using needleman.";
		LOG(INFO) << "";
		alignment = new NeedlemanOverlap(gapOpen, match, misMatch, 1000);
	}

	string fileroot = outputDir + File::getRootName(File::getSimpleName(fastafile));
	map<string, string> variables;
	variables["[filename]"] = fileroot;
	string newNamesFile = getOutputFileName("name", variables);
	string newCountFile = getOutputFileName("count", variables);
	string newMapFile = getOutputFileName("map", variables); //add group name if by group
	variables["[extension]"] = m->getExtension(fastafile);
	string newFastaFile = getOutputFileName("fasta", variables);
	outputNames.push_back(newFastaFile); outputTypes["fasta"].push_back(newFastaFile);
	if (countfile == "") { outputNames.push_back(newNamesFile); outputTypes["name"].push_back(newNamesFile); }
	else { outputNames.push_back(newCountFile); outputTypes["count"].push_back(newCountFile); }

	if (bygroup) {
		//clear out old files
		ofstream outFasta; File::openOutputFile(newFastaFile, outFasta); outFasta.close();
		ofstream outNames; File::openOutputFile(newNamesFile, outNames);  outNames.close();
		newMapFile = fileroot + "precluster.";

		//parse fasta and name file by group
		vector<string> groups;
		if (countfile != "") {
			cparser = new SequenceCountParser(countfile, fastafile);
			groups = cparser->getNamesOfGroups();
		}
		else {
			if (namefile != "") { parser = new SequenceParser(groupfile, fastafile, namefile); }
			else { parser = new SequenceParser(groupfile, fastafile); }
			groups = parser->getNamesOfGroups();
		}

		if (processors == 1) { driverGroups(newFastaFile, newNamesFile, newMapFile, 0, groups.size(), groups); }
		else { createProcessesGroups(newFastaFile, newNamesFile, newMapFile, groups); }

		if (countfile != "") {
			mergeGroupCounts(newCountFile, newNamesFile, newFastaFile);
			delete cparser;
		}
		else {
			delete parser;
			//run unique.seqs for deconvolute results
			string inputString = "fasta=" + newFastaFile;
			if (namefile != "") { inputString += ", name=" + newNamesFile; }
			LOG(INFO) << '\n' << "/******************************************/" << '\n';
			LOG(INFO) << "Running command: unique.seqs(" + inputString + ")" << '\n';
			m->mothurCalling = true;

			Command* uniqueCommand = new DeconvoluteCommand(inputString);
			uniqueCommand->execute();

			map<string, vector<string> > filenames = uniqueCommand->getOutputFiles();

			delete uniqueCommand;
			m->mothurCalling = false;
			LOG(INFO) << "/******************************************/" << '\n';

			m->renameFile(filenames["fasta"][0], newFastaFile);
			m->renameFile(filenames["name"][0], newNamesFile);
		}
		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }	 delete alignment; return 0; }
		LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to run pre.cluster." << '\n';

	}
	else {
		if (processors != 1) { LOG(INFO) << "When using running without group information mothur can only use 1 processor, continuing." << '\n'; processors = 1; }
		if (namefile != "") { readNameFile(); }

		//reads fasta file and return number of seqs
		int numSeqs = readFASTA(); //fills alignSeqs and makes all seqs active

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } delete alignment; return 0; }

		if (numSeqs == 0) { LOG(INFO) << "Error reading fasta file...please correct." << '\n'; delete alignment; return 0; }
		if (diffs > length) { LOG(INFO) << "Error: diffs is greater than your sequence length." << '\n'; delete alignment; return 0; }

		int count = process(newMapFile);
		outputNames.push_back(newMapFile); outputTypes["map"].push_back(newMapFile);

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } delete alignment; return 0; }

		LOG(INFO) << "Total number of sequences before precluster was " + toString(alignSeqs.size()) + "." << '\n';
		LOG(INFO) << "pre.cluster removed " + toString(count) + " sequences." << '\n' << '\n';
		if (countfile != "") { newNamesFile = newCountFile; }
		printData(newFastaFile, newNamesFile, "");

		LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to cluster " + toString(numSeqs) + " sequences." << '\n';
	}

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } delete alignment; return 0; }

	delete alignment;

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

	itTypes = outputTypes.find("count");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
	}

	return 0;

}
/**************************************************************************************************/
int PreClusterCommand::createProcessesGroups(string newFName, string newNName, string newMFile, vector<string> groups) {

	vector<int> processIDS;
	int process = 1;
	int num = 0;
	bool recalc = false;

	//sanity check
	if (groups.size() < processors) { processors = groups.size(); }

	//divide the groups between the processors
	vector<linePair> lines;
	int remainingPairs = groups.size();
	int startIndex = 0;
	for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
		int numPairs = remainingPairs; //case for last processor
		if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
		lines.push_back(linePair(startIndex, (startIndex + numPairs))); //startIndex, endIndex
		startIndex = startIndex + numPairs;
		remainingPairs = remainingPairs - numPairs;
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
			num = driverGroups(newFName + m->mothurGetpid(process) + ".temp", newNName + m->mothurGetpid(process) + ".temp", newMFile, lines[process].start, lines[process].end, groups);

			string tempFile = m->mothurGetpid(process) + ".outputNames.temp";
			ofstream outTemp;
			File::openOutputFile(tempFile, outTemp);

			outTemp << outputNames.size();
			for (int i = 0; i < outputNames.size(); i++) { outTemp << outputNames[i] << endl; }
			outTemp.close();

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
				File::remove((toString(processIDS[i]) + ".outputNames.temp"));
			}
			recalc = true;
			break;
		}
	}

	if (recalc) {
		//test line, also set recalc to true.
		//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove((toString(processIDS[i]) + ".outputNames.temp"));}processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

		lines.clear();
		num = 0;
		processIDS.resize(0);
		process = 1;

		int remainingPairs = groups.size();
		int startIndex = 0;
		for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
			int numPairs = remainingPairs; //case for last processor
			if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
			lines.push_back(linePair(startIndex, (startIndex + numPairs))); //startIndex, endIndex
			startIndex = startIndex + numPairs;
			remainingPairs = remainingPairs - numPairs;
		}

		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				outputNames.clear();
				num = driverGroups(newFName + m->mothurGetpid(process) + ".temp", newNName + m->mothurGetpid(process) + ".temp", newMFile, lines[process].start, lines[process].end, groups);

				string tempFile = m->mothurGetpid(process) + ".outputNames.temp";
				ofstream outTemp;
				File::openOutputFile(tempFile, outTemp);

				outTemp << outputNames.size();
				for (int i = 0; i < outputNames.size(); i++) { outTemp << outputNames[i] << endl; }
				outTemp.close();

				exit(0);
			}
			else {
				LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
				for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
				exit(0);
			}
		}
	}


	//do my part
	num = driverGroups(newFName, newNName, newMFile, lines[0].start, lines[0].end, groups);

	//force parent to wait until all the processes are done
	for (int i = 0;i < processIDS.size();i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	for (int i = 0; i < processIDS.size(); i++) {
		string tempFile = toString(processIDS[i]) + ".outputNames.temp";
		ifstream intemp;
		File::openInputFile(tempFile, intemp);

		int num;
		intemp >> num;
		for (int k = 0; k < num; k++) {
			string name = "";
			intemp >> name; File::gobble(intemp);

			outputNames.push_back(name); outputTypes["map"].push_back(name);
		}
		intemp.close();
		File::remove(tempFile);
	}
#else

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Windows version shared memory, so be careful when passing variables through the preClusterData struct. 
	//Above fork() will clone, so memory is separate, but that's not the case with windows, 
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<preClusterData*> pDataArray;
	vector<DWORD> dwThreadIdArray(processors - 1);
	vector<HANDLE> hThreadArray(processors - 1);

	//Create processor worker threads.
	for (int i = 1; i < processors; i++) {
		// Allocate memory for thread data.
		string extension = toString(i) + ".temp";

		preClusterData* tempPreCluster = new preClusterData(fastafile, namefile, groupfile, countfile, (newFName + extension), (newNName + extension), newMFile, groups, m, lines[i].start, lines[i].end, diffs, topdown, i, method, align, match, misMatch, gapOpen, gapExtend);
		pDataArray.push_back(tempPreCluster);
		processIDS.push_back(i);

		//MySeqSumThreadFunction is in header. It must be global or static to work with the threads.
		//default security attributes, thread function name, argument to thread function, use default creation flags, returns the thread identifier
		hThreadArray[i - 1] = CreateThread(NULL, 0, MyPreclusterThreadFunction, pDataArray[i - 1], 0, &dwThreadIdArray[i - 1]);
	}


	//using the main process as a worker saves time and memory
	num = driverGroups(newFName, newNName, newMFile, lines[0].start, lines[0].end, groups);

	//Wait until all threads have terminated.
	WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

	//Close all thread handles and free memory allocations.
	for (int i = 0; i < pDataArray.size(); i++) {
		if (pDataArray[i]->count != (pDataArray[i]->end - pDataArray[i]->start)) {
			LOG(LOGERROR) << "process " + toString(i) + " only processed " + toString(pDataArray[i]->count) + " of " + toString(pDataArray[i]->end - pDataArray[i]->start) + " groups assigned to it, quitting. \n"; ctrlc_pressed = true;
		}
		for (int j = 0; j < pDataArray[i]->mapFileNames.size(); j++) {
			outputNames.push_back(pDataArray[i]->mapFileNames[j]); outputTypes["map"].push_back(pDataArray[i]->mapFileNames[j]);
		}
		CloseHandle(hThreadArray[i]);
		delete pDataArray[i];
	}

#endif		

	//append output files
	for (int i = 0;i < processIDS.size();i++) {
		//newFName = m->getFullPathName(".\\" + newFName);
		//newNName = m->getFullPathName(".\\" + newNName);

		File::appendFiles((newFName + toString(processIDS[i]) + ".temp"), newFName);
		File::remove((newFName + toString(processIDS[i]) + ".temp"));

		File::appendFiles((newNName + toString(processIDS[i]) + ".temp"), newNName);
		File::remove((newNName + toString(processIDS[i]) + ".temp"));
	}

	return num;

}
/**************************************************************************************************/
int PreClusterCommand::driverGroups(string newFFile, string newNFile, string newMFile, int start, int end, vector<string> groups) {

	int numSeqs = 0;

	//precluster each group
	for (int i = start; i < end; i++) {

		start = time(NULL);

		if (ctrlc_pressed) { return 0; }

		LOG(INFO) << '\n' << "Processing group " + groups[i] + ":" << '\n';

		map<string, string> thisNameMap;
		vector<Sequence> thisSeqs;
		if (groupfile != "") {
			thisSeqs = parser->getSeqs(groups[i]);
		}
		else if (countfile != "") {
			thisSeqs = cparser->getSeqs(groups[i]);
		}
		if (namefile != "") { thisNameMap = parser->getNameMap(groups[i]); }

		//fill alignSeqs with this groups info.
		numSeqs = loadSeqs(thisNameMap, thisSeqs, groups[i]);

		if (ctrlc_pressed) { return 0; }

		if (method == "aligned") { if (diffs > length) { LOG(INFO) << "Error: diffs is greater than your sequence length." << '\n'; ctrlc_pressed = true; return 0; } }

		int count = process(newMFile + groups[i] + ".map");
		outputNames.push_back(newMFile + groups[i] + ".map"); outputTypes["map"].push_back(newMFile + groups[i] + ".map");

		if (ctrlc_pressed) { return 0; }

		LOG(INFO) << "Total number of sequences before pre.cluster was " + toString(alignSeqs.size()) + "." << '\n';
		LOG(INFO) << "pre.cluster removed " + toString(count) + " sequences." << '\n' << '\n';
		printData(newFFile, newNFile, groups[i]);

		LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to cluster " + toString(numSeqs) + " sequences." << '\n';

	}

	return numSeqs;
}
/**************************************************************************************************/
int PreClusterCommand::process(string newMapFile) {
	ofstream out;
	File::openOutputFile(newMapFile, out);

	//sort seqs by number of identical seqs
	if (topdown) { sort(alignSeqs.begin(), alignSeqs.end(), comparePriorityTopDown); }
	else { sort(alignSeqs.begin(), alignSeqs.end(), comparePriorityDownTop); }

	int count = 0;
	int numSeqs = alignSeqs.size();

	if (topdown) {
		//think about running through twice...
		for (int i = 0; i < numSeqs; i++) {

			if (alignSeqs[i].active) {  //this sequence has not been merged yet

				string chunk = alignSeqs[i].seq.getName() + "\t" + toString(alignSeqs[i].numIdentical) + "\t" + toString(0) + "\t" + alignSeqs[i].seq.getAligned() + "\n";

				//try to merge it with all smaller seqs
				for (int j = i + 1; j < numSeqs; j++) {

					if (ctrlc_pressed) { out.close(); return 0; }

					if (alignSeqs[j].active) {  //this sequence has not been merged yet
						//are you within "diff" bases
						int mismatch = calcMisMatches(alignSeqs[i].seq.getAligned(), alignSeqs[j].seq.getAligned());

						if (mismatch <= diffs) {
							//merge
							alignSeqs[i].names += ',' + alignSeqs[j].names;
							alignSeqs[i].numIdentical += alignSeqs[j].numIdentical;

							chunk += alignSeqs[j].seq.getName() + "\t" + toString(alignSeqs[j].numIdentical) + "\t" + toString(mismatch) + "\t" + alignSeqs[j].seq.getAligned() + "\n";

							alignSeqs[j].active = 0;
							alignSeqs[j].numIdentical = 0;
							count++;
						}
					}//end if j active
				}//end for loop j

				//remove from active list 
				alignSeqs[i].active = 0;

				out << "ideal_seq_" << (i + 1) << '\t' << alignSeqs[i].numIdentical << endl << chunk << endl;

			}//end if active i
			if (i % 100 == 0) { LOG(SCREENONLY) << toString(i) + "\t" + toString(numSeqs - count) + "\t" + toString(count) + "\n"; }
		}
	}
	else {
		map<int, string> mapFile;
		map<int, int> originalCount;
		map<int, int>::iterator itCount;
		for (int i = 0; i < numSeqs; i++) { mapFile[i] = ""; originalCount[i] = alignSeqs[i].numIdentical; }

		//think about running through twice...
		for (int i = 0; i < numSeqs; i++) {

			//try to merge it into larger seqs
			for (int j = i + 1; j < numSeqs; j++) {

				if (ctrlc_pressed) { out.close(); return 0; }

				if (originalCount[j] > originalCount[i]) {  //this sequence is more abundant than I am
					//are you within "diff" bases
					int mismatch = calcMisMatches(alignSeqs[i].seq.getAligned(), alignSeqs[j].seq.getAligned());

					if (mismatch <= diffs) {
						//merge
						alignSeqs[j].names += ',' + alignSeqs[i].names;
						alignSeqs[j].numIdentical += alignSeqs[i].numIdentical;

						mapFile[j] = alignSeqs[i].seq.getName() + "\t" + toString(alignSeqs[i].numIdentical) + "\t" + toString(mismatch) + "\t" + alignSeqs[i].seq.getAligned() + "\n" + mapFile[i];
						alignSeqs[i].numIdentical = 0;
						originalCount.erase(i);
						mapFile[i] = "";
						count++;
						j += numSeqs; //exit search, we merged this one in.
					}
				}//end abundance check
			}//end for loop j

			if (i % 100 == 0) { LOG(SCREENONLY) << toString(i) + "\t" + toString(numSeqs - count) + "\t" + toString(count) + "\n"; }
		}

		for (int i = 0; i < numSeqs; i++) {
			if (alignSeqs[i].numIdentical != 0) {
				out << "ideal_seq_" << (i + 1) << '\t' << alignSeqs[i].numIdentical << endl << alignSeqs[i].seq.getName() + "\t" + toString(alignSeqs[i].numIdentical) + "\t" + toString(0) + "\t" + alignSeqs[i].seq.getAligned() + "\n" << mapFile[i] << endl;
			}
		}

	}
	out.close();

	if (numSeqs % 100 != 0) { LOG(INFO) << toString(numSeqs) + "\t" + toString(numSeqs - count) + "\t" + toString(count) << '\n'; }

	return count;

}
/**************************************************************************************************/
int PreClusterCommand::readFASTA() {
	//ifstream inNames;
	ifstream inFasta;

	File::openInputFile(fastafile, inFasta);
	set<int> lengths;

	while (!inFasta.eof()) {

		if (ctrlc_pressed) { inFasta.close(); return 0; }

		Sequence seq(inFasta);  File::gobble(inFasta);

		if (seq.getName() != "") {  //can get "" if commented line is at end of fasta file
			if (namefile != "") {
				itSize = sizes.find(seq.getName());

				if (itSize == sizes.end()) { LOG(INFO) << seq.getName() + " is not in your names file, please correct." << '\n'; exit(1); }
				else {
					seqPNode tempNode(itSize->second, seq, names[seq.getName()]);
					alignSeqs.push_back(tempNode);
					lengths.insert(seq.getAligned().length());
				}
			}
			else { //no names file, you are identical to yourself 
				int numRep = 1;
				if (countfile != "") { numRep = ct.getNumSeqs(seq.getName()); }
				seqPNode tempNode(numRep, seq, seq.getName());
				alignSeqs.push_back(tempNode);
				lengths.insert(seq.getAligned().length());
			}
		}
	}
	inFasta.close();

	if (lengths.size() > 1) { method = "unaligned"; }
	else if (lengths.size() == 1) { method = "aligned"; }

	length = *(lengths.begin());

	return alignSeqs.size();
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in PreClusterCommand, readFASTA";
	exit(1);
}
}
/**************************************************************************************************/
int PreClusterCommand::loadSeqs(map<string, string>& thisName, vector<Sequence>& thisSeqs, string group) {
	try {
		set<int> lengths;
		alignSeqs.clear();
		map<string, string>::iterator it;
		bool error = false;
		map<string, int> thisCount;
		if (countfile != "") { thisCount = cparser->getCountTable(group); }

		for (int i = 0; i < thisSeqs.size(); i++) {

			if (ctrlc_pressed) { return 0; }

			if (namefile != "") {
				it = thisName.find(thisSeqs[i].getName());

				//should never be true since parser checks for this
				if (it == thisName.end()) { LOG(INFO) << thisSeqs[i].getName() + " is not in your names file, please correct." << '\n'; error = true; }
				else {
					//get number of reps
					int numReps = 1;
					for (int j = 0;j < (it->second).length();j++) {
						if ((it->second)[j] == ',') { numReps++; }
					}

					seqPNode tempNode(numReps, thisSeqs[i], it->second);
					alignSeqs.push_back(tempNode);
					lengths.insert(thisSeqs[i].getAligned().length());
				}
			}
			else { //no names file, you are identical to yourself 
				int numRep = 1;
				if (countfile != "") {
					map<string, int>::iterator it2 = thisCount.find(thisSeqs[i].getName());

					//should never be true since parser checks for this
					if (it2 == thisCount.end()) { LOG(INFO) << thisSeqs[i].getName() + " is not in your count file, please correct." << '\n'; error = true; }
					else { numRep = it2->second; }
				}
				seqPNode tempNode(numRep, thisSeqs[i], thisSeqs[i].getName());
				alignSeqs.push_back(tempNode);
				lengths.insert(thisSeqs[i].getAligned().length());
			}
		}

		if (lengths.size() > 1) { method = "unaligned"; }
		else if (lengths.size() == 1) { method = "aligned"; }

		length = *(lengths.begin());

		//sanity check
		if (error) { ctrlc_pressed = true; }

		thisSeqs.clear();

		return alignSeqs.size();
	}

	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PreClusterCommand, loadSeqs";
		exit(1);
	}
}

/**************************************************************************************************/

int PreClusterCommand::calcMisMatches(string seq1, string seq2) {
	try {
		int numBad = 0;

		if (method == "unaligned") {
			//align to eachother
			Sequence seqI("seq1", seq1);
			Sequence seqJ("seq2", seq2);

			//align seq2 to seq1 - less abundant to more abundant
			alignment->align(seqJ.getUnaligned(), seqI.getUnaligned());
			seq2 = alignment->getSeqAAln();
			seq1 = alignment->getSeqBAln();

			//chop gap ends
			int startPos = 0;
			int endPos = seq2.length() - 1;
			for (int i = 0; i < seq2.length(); i++) { if (isalpha(seq2[i])) { startPos = i; break; } }
			for (int i = seq2.length() - 1; i >= 0; i--) { if (isalpha(seq2[i])) { endPos = i; break; } }

			//count number of diffs
			for (int i = startPos; i <= endPos; i++) {
				if (seq2[i] != seq1[i]) { numBad++; }
				if (numBad > diffs) { return length; } //to far to cluster
			}

		}
		else {
			//count diffs
			for (int i = 0; i < seq1.length(); i++) {
				//do they match
				if (seq1[i] != seq2[i]) { numBad++; }
				if (numBad > diffs) { return length; } //to far to cluster
			}
		}
		return numBad;
	}
	/**************************************************************************************************/

	int PreClusterCommand::mergeGroupCounts(string newcount, string newname, string newfasta) {
		ifstream inNames;
		File::openInputFile(newname, inNames);

		string group, first, second;
		set<string> uniqueNames;
		while (!inNames.eof()) {
			if (ctrlc_pressed) { break; }
			inNames >> group; File::gobble(inNames);
			inNames >> first; File::gobble(inNames);
			inNames >> second; File::gobble(inNames);

			vector<string> names;
			m->splitAtComma(second, names);

			uniqueNames.insert(first);

			int total = ct.getGroupCount(first, group);
			for (int i = 1; i < names.size(); i++) {
				total += ct.getGroupCount(names[i], group);
				ct.setAbund(names[i], group, 0);
			}
			ct.setAbund(first, group, total);
		}
		inNames.close();

		vector<string> namesOfSeqs = ct.getNamesOfSeqs();
		for (int i = 0; i < namesOfSeqs.size(); i++) {
			if (ct.getNumSeqs(namesOfSeqs[i]) == 0) {
				ct.remove(namesOfSeqs[i]);
			}
		}

		ct.printTable(newcount);
		File::remove(newname);

		if (bygroup) { //if by group, must remove the duplicate seqs that are named the same
			ifstream in;
			File::openInputFile(newfasta, in);

			ofstream out;
			File::openOutputFile(newfasta + "temp", out);

			int count = 0;
			set<string> already;
			while (!in.eof()) {
				if (ctrlc_pressed) { break; }

				Sequence seq(in); File::gobble(in);

				if (seq.getName() != "") {
					count++;
					if (already.count(seq.getName()) == 0) {
						seq.printSequence(out);
						already.insert(seq.getName());
					}
				}
			}
			in.close();
			out.close();
			File::remove(newfasta);
			m->renameFile(newfasta + "temp", newfasta);
		}
		return 0;

	}

	/**************************************************************************************************/

	void PreClusterCommand::printData(string newfasta, string newname, string group) {
		ofstream outFasta;
		ofstream outNames;

		if (bygroup) {
			File::openOutputFileAppend(newfasta, outFasta);
			File::openOutputFileAppend(newname, outNames);
		}
		else {
			File::openOutputFile(newfasta, outFasta);
			File::openOutputFile(newname, outNames);
		}

		if ((countfile != "") && (group == "")) { outNames << "Representative_Sequence\ttotal\n"; }
		for (int i = 0; i < alignSeqs.size(); i++) {
			if (alignSeqs[i].numIdentical != 0) {
				alignSeqs[i].seq.printSequence(outFasta);
				if (countfile != "") {
					if (group != "") { outNames << group << '\t' << alignSeqs[i].seq.getName() << '\t' << alignSeqs[i].names << endl; }
					else { outNames << alignSeqs[i].seq.getName() << '\t' << alignSeqs[i].numIdentical << endl; }
				}
				else { outNames << alignSeqs[i].seq.getName() << '\t' << alignSeqs[i].names << endl; }
			}
		}

		outFasta.close();
		outNames.close();

	}
	/**************************************************************************************************/

	void PreClusterCommand::readNameFile() {
		ifstream in;
		File::openInputFile(namefile, in);
		string firstCol, secondCol;

		while (!in.eof()) {
			in >> firstCol >> secondCol; File::gobble(in);

			m->checkName(firstCol);
			m->checkName(secondCol);
			int size = m->getNumNames(secondCol);

			names[firstCol] = secondCol;
			sizes[firstCol] = size;
		}
		in.close();
	}

	/**************************************************************************************************/


