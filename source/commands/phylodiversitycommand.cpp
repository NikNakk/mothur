/*
 *  phylodiversitycommand.cpp
 *  Mothur
 *
 *  Created by westcott on 4/30/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "phylodiversitycommand.h"
#include "treereader.h"

 //**********************************************************************************************************************
vector<string> PhyloDiversityCommand::setParameters() {
	try {

		CommandParameter ptree("tree", "InputTypes", "", "", "none", "none", "none", "phylodiv", false, true, true); parameters.push_back(ptree);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "none", "", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "none", "none", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none", "", false, false, true); parameters.push_back(pgroup);
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new NumberParameter("iters", -INFINITY, INFINITY, 1000, false, false));
		nkParameters.add(new NumberParameter("freq", -INFINITY, INFINITY, 100, false, false));
		nkParameters.add(new ProcessorsParameter(settings));
		CommandParameter prarefy("rarefy", "Boolean", "", "F", "", "", "", "rarefy", false, false); parameters.push_back(prarefy);
		nkParameters.add(new NumberParameter("sampledepth", -INFINITY, INFINITY, 0, false, false));
		CommandParameter psummary("summary", "Boolean", "", "T", "", "", "", "summary", false, false); parameters.push_back(psummary);
		nkParameters.add(new BooleanParameter("collect", false, false, false));
		nkParameters.add(new BooleanParameter("scale", false, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PhyloDiversityCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string PhyloDiversityCommand::getHelpString() {
	try {
		string helpString = "The phylo.diversity command parameters are tree, group, name, count, groups, iters, freq, processors, scale, rarefy, collect and summary.  tree and group are required, unless you have valid current files.\n"
			"The groups parameter allows you to specify which of the groups in your groupfile you would like analyzed. The group names are separated by dashes. By default all groups are used.\n"
			"The iters parameter allows you to specify the number of randomizations to preform, by default iters=1000, if you set rarefy to true.\n"
			"The freq parameter is used indicate when to output your data, by default it is set to 100. But you can set it to a percentage of the number of sequence. For example freq=0.10, means 10%. \n"
			"The sampledepth parameter allows you to enter the number of sequences you want to sample.\n"
			"The scale parameter is used indicate that you want your output scaled to the number of sequences sampled, default = false. \n"
			"The rarefy parameter allows you to create a rarefaction curve. The default is false.\n"
			"The collect parameter allows you to create a collectors curve. The default is false.\n"
			"The summary parameter allows you to create a .summary file. The default is true.\n"
			"The processors parameter allows you to specify the number of processors to use. The default is 1.\n"
			"The phylo.diversity command should be in the following format: phylo.diversity(groups=yourGroups, rarefy=yourRarefy, iters=yourIters).\n"
			"Example phylo.diversity(groups=A-B-C, rarefy=T, iters=500).\n"
			"The phylo.diversity command output two files: .phylo.diversity and if rarefy=T, .rarefaction.\n"
			"Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PhyloDiversityCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string PhyloDiversityCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "phylodiv") { pattern = "[filename],[tag],phylodiv"; }
	else if (type == "rarefy") { pattern = "[filename],[tag],phylodiv.rarefaction"; }
	else if (type == "summary") { pattern = "[filename],[tag],phylodiv.summary"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}

//**********************************************************************************************************************
PhyloDiversityCommand::PhyloDiversityCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["phylodiv"] = tempOutNames;
		outputTypes["rarefy"] = tempOutNames;
		outputTypes["summary"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PhyloDiversityCommand, PhyloDiversityCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
PhyloDiversityCommand::PhyloDiversityCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();;

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
		outputTypes["phylodiv"] = tempOutNames;
		outputTypes["rarefy"] = tempOutNames;
		outputTypes["summary"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("tree");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["tree"] = inputDir + it->second; }
			}

			it = parameters.find("group");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["group"] = inputDir + it->second; }
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
		treefile = validParameter.validFile(parameters, "tree", true);
		if (treefile == "not open") { treefile = ""; abort = true; }
		else if (treefile == "not found") {
			//if there is a current design file, use it
			treefile = settings.getCurrent("tree");
			if (treefile != "") { LOG(INFO) << "Using " + treefile + " as input file for the tree parameter." << '\n'; }
			else { LOG(INFO) << "You have no current tree file and the tree parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("tree", treefile); }

		//check for required parameters
		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { groupfile = ""; abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { namefile = ""; abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { settings.setCurrent("name", namefile); }

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

		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(treefile); }

		string temp;
		temp = validParameter.validFile(parameters, "freq", false);			if (temp == "not found") { temp = "100"; }
		Utility::mothurConvert(temp, freq);

		temp = validParameter.validFile(parameters, "rarefy", false);			if (temp == "not found") { temp = "F"; }
		rarefy = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "sampledepth", false);		if (temp == "not found") { temp = "0"; }
		if (m->isNumeric1(temp)) {
			Utility::mothurConvert(temp, subsampleSize);
			if (subsampleSize == 0) { subsample = false; }
			else { subsample = true; }
		}
		else {
			subsample = false;
			LOG(LOGERROR) << "sampledepth must be numeric, aborting.\n" << '\n'; abort = true;
		}
		if (subsample) { rarefy = true; }

		temp = validParameter.validFile(parameters, "iters", false);			if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, iters);
		if (!rarefy) { iters = 1; }

		temp = validParameter.validFile(parameters, "summary", false);			if (temp == "not found") { temp = "T"; }
		summary = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "scale", false);			if (temp == "not found") { temp = "F"; }
		scale = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "collect", false);			if (temp == "not found") { temp = "F"; }
		collect = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else {
			Utility::split(groups, '-', Groups);
			m->setGroups(Groups);
		}

		if ((!collect) && (!rarefy) && (!summary)) { LOG(INFO) << "No outputs selected. You must set either collect, rarefy or summary to true, summary=T by default." << '\n'; abort = true; }

		if (countfile == "") {
			if (namefile == "") {
				vector<string> files; files.push_back(treefile);
				OptionParser::getNameFile(files);
			}
		}
	}

}
//**********************************************************************************************************************

int PhyloDiversityCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	int start = time(NULL);

	settings.setCurrent("tree", treefile);
	TreeReader* reader;
	if (countfile == "") { reader = new TreeReader(treefile, groupfile, namefile); }
	else { reader = new TreeReader(treefile, countfile); }
	vector<Tree*> trees = reader->getTrees();
	ct = trees[0]->getCountTable();
	delete reader;

	SharedUtil util;
	vector<string> mGroups = settings.getGroups();
	vector<string> tGroups = ct->getNamesOfGroups();
	util.setGroups(mGroups, tGroups, "phylo.diversity");	//sets the groups the user wants to analyze

	//incase the user had some mismatches between the tree and group files we don't want group xxx to be analyzed
	for (int i = 0; i < mGroups.size(); i++) { if (mGroups[i] == "xxx") { mGroups.erase(mGroups.begin() + i);  break; } }
	m->setGroups(mGroups);

	vector<string> outputNames;

	//for each of the users trees
	for (int i = 0; i < trees.size(); i++) {

		if (ctrlc_pressed) { delete ct; for (int j = 0; j < trees.size(); j++) { delete trees[j]; } for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } return 0; }

		ofstream outSum, outRare, outCollect;
		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(treefile));
		variables["[tag]"] = toString(i + 1);
		string outSumFile = getOutputFileName("summary", variables);
		string outRareFile = getOutputFileName("rarefy", variables);
		string outCollectFile = getOutputFileName("phylodiv", variables);

		if (summary) { File::openOutputFile(outSumFile, outSum); outputNames.push_back(outSumFile);		outputTypes["summary"].push_back(outSumFile); }
		if (rarefy) { File::openOutputFile(outRareFile, outRare); outputNames.push_back(outRareFile);	outputTypes["rarefy"].push_back(outRareFile); }
		if (collect) { File::openOutputFile(outCollectFile, outCollect); outputNames.push_back(outCollectFile);	 outputTypes["phylodiv"].push_back(outCollectFile); }

		int numLeafNodes = trees[i]->getNumLeaves();

		//create a vector containing indexes of leaf nodes, randomize it, select nodes to send to calculator
		vector<int> randomLeaf;
		for (int j = 0; j < numLeafNodes; j++) {
			if (m->inUsersGroups(trees[i]->tree[j].getGroup(), mGroups) == true) { //is this a node from the group the user selected.
				randomLeaf.push_back(j);
			}
		}

		numLeafNodes = randomLeaf.size();  //reset the number of leaf nodes you are using 

		//each group, each sampling, if no rarefy iters = 1;
		map<string, vector<float> > diversity;

		//each group, each sampling, if no rarefy iters = 1;
		map<string, vector<float> > sumDiversity;

		//find largest group total 
		int largestGroup = 0;
		for (int j = 0; j < mGroups.size(); j++) {
			int numSeqsThisGroup = ct->getGroupCount(mGroups[j]);
			if (numSeqsThisGroup > largestGroup) { largestGroup = numSeqsThisGroup; }

			//initialize diversity
			diversity[mGroups[j]].resize(numSeqsThisGroup + 1, 0.0);		//numSampled
																										//groupA		0.0			0.0
			//initialize sumDiversity
			sumDiversity[mGroups[j]].resize(numSeqsThisGroup + 1, 0.0);
		}

		//convert freq percentage to number
		if (subsample) { largestGroup = subsampleSize; }
		int increment = 100;
		if (freq < 1.0) {
			increment = largestGroup * freq;
		}
		else { increment = freq; }

		//initialize sampling spots
		set<int> numSampledList;
		for (int k = 1; k <= largestGroup; k++) { if ((k == 1) || (k % increment == 0)) { numSampledList.insert(k); } }
		if (largestGroup % increment != 0) { numSampledList.insert(largestGroup); }

		//add other groups ending points
		if (!subsample) {
			for (int j = 0; j < mGroups.size(); j++) {
				if (numSampledList.count(diversity[mGroups[j]].size() - 1) == 0) { numSampledList.insert(diversity[mGroups[j]].size() - 1); }
			}
		}

		if (rarefy) {
			vector<int> procIters;
			int numItersPerProcessor = iters / processors;

			//divide iters between processes
			for (int h = 0; h < processors; h++) {
				if (h == processors - 1) { numItersPerProcessor = iters - h * numItersPerProcessor; }
				procIters.push_back(numItersPerProcessor);
			}

			createProcesses(procIters, trees[i], diversity, sumDiversity, iters, increment, randomLeaf, numSampledList, outCollect, outSum);

		}
		else { //no need to paralellize if you dont want to rarefy
			driver(trees[i], diversity, sumDiversity, iters, increment, randomLeaf, numSampledList, outCollect, outSum, true);
		}

		if (rarefy) { printData(numSampledList, sumDiversity, outRare, iters); }
	}


	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to run phylo.diversity." << '\n';


	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";


	return 0;
}
//**********************************************************************************************************************
int PhyloDiversityCommand::createProcesses(vector<int>& procIters, Tree* t, map< string, vector<float> >& div, map<string, vector<float> >& sumDiv, int numIters, int increment, vector<int>& randomLeaf, set<int>& numSampledList, ofstream& outCollect, ofstream& outSum) {
	int process = 1;

	vector<int> processIDS;
	map< string, vector<float> >::iterator itSum;
	bool recalc = false;

#if defined (UNIX)

	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
			process++;
		}
		else if (pid == 0) {
			driver(t, div, sumDiv, procIters[process], increment, randomLeaf, numSampledList, outCollect, outSum, false);

			string outTemp = outputDir + m->mothurGetpid(process) + ".sumDiv.temp";
			ofstream out;
			File::openOutputFile(outTemp, out);

			//output the sumDIversity
			for (itSum = sumDiv.begin(); itSum != sumDiv.end(); itSum++) {
				out << itSum->first << '\t' << (itSum->second).size() << '\t';
				for (int k = 0; k < (itSum->second).size(); k++) {
					out << (itSum->second)[k] << '\t';
				}
				out << endl;
			}

			out.close();

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
				File::remove(outputDir + (toString(processIDS[i])) + ".sumDiv.temp");
			}
			recalc = true;
			break;
		}
	}

	if (recalc) {
		//test line, also set recalc to true.
		//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove(outputDir + (toString(processIDS[i])) + ".sumDiv.temp");}processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

		//divide iters between processes
		procIters.clear();
		int numItersPerProcessor = iters / processors;
		for (int h = 0; h < processors; h++) {
			if (h == processors - 1) { numItersPerProcessor = iters - h * numItersPerProcessor; }
			procIters.push_back(numItersPerProcessor);
		}

		processIDS.resize(0);
		process = 1;

		//loop through and create all the processes you want
		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				driver(t, div, sumDiv, procIters[process], increment, randomLeaf, numSampledList, outCollect, outSum, false);

				string outTemp = outputDir + m->mothurGetpid(process) + ".sumDiv.temp";
				ofstream out;
				File::openOutputFile(outTemp, out);

				//output the sumDIversity
				for (itSum = sumDiv.begin(); itSum != sumDiv.end(); itSum++) {
					out << itSum->first << '\t' << (itSum->second).size() << '\t';
					for (int k = 0; k < (itSum->second).size(); k++) {
						out << (itSum->second)[k] << '\t';
					}
					out << endl;
				}

				out.close();

				exit(0);
			}
			else {
				LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
				for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
				exit(0);
			}
		}
	}

	driver(t, div, sumDiv, procIters[0], increment, randomLeaf, numSampledList, outCollect, outSum, true);

	//force parent to wait until all the processes are done
	for (int i = 0;i < (processors - 1);i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	//get data created by processes
	for (int i = 0;i < (processors - 1);i++) {

		//input the sumDIversity
		string inTemp = outputDir + toString(processIDS[i]) + ".sumDiv.temp";
		ifstream in;
		File::openInputFile(inTemp, in);

		//output the sumDIversity
		for (int j = 0; j < sumDiv.size(); j++) {
			string group = "";
			int size = 0;

			in >> group >> size; File::gobble(in);

			for (int k = 0; k < size; k++) {
				float tempVal;
				in >> tempVal;

				sumDiv[group][k] += tempVal;
			}
			File::gobble(in);
		}

		in.close();
		File::remove(inTemp);
	}
#else

	//fill in functions
	vector<phylodivData*> pDataArray;
	vector<DWORD> dwThreadIdArray(processors - 1);
	vector<HANDLE> hThreadArray(processors - 1);
	vector<CountTable*> cts;
	vector<Tree*> trees;
	map<string, int> rootForGroup = getRootForGroups(t);

	//Create processor worker threads.
	for (int i = 1; i < processors; i++) {
		CountTable* copyCount = new CountTable();
		copyCount->copy(ct);
		Tree* copyTree = new Tree(copyCount);
		copyTree->getCopy(t);

		cts.push_back(copyCount);
		trees.push_back(copyTree);

		map<string, vector<float> > copydiv = div;
		map<string, vector<float> > copysumDiv = sumDiv;
		vector<int> copyrandomLeaf = randomLeaf;
		set<int> copynumSampledList = numSampledList;
		map<string, int> copyRootForGrouping = rootForGroup;

		phylodivData* temp = new phylodivData(m, procIters[i], copydiv, copysumDiv, copyTree, copyCount, increment, copyrandomLeaf, copynumSampledList, copyRootForGrouping, subsample, subsampleSize);
		pDataArray.push_back(temp);
		processIDS.push_back(i);

		hThreadArray[i - 1] = CreateThread(NULL, 0, MyPhyloDivThreadFunction, pDataArray[i - 1], 0, &dwThreadIdArray[i - 1]);
	}

	driver(t, div, sumDiv, procIters[0], increment, randomLeaf, numSampledList, outCollect, outSum, true);

	//Wait until all threads have terminated.
	WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

	//Close all thread handles and free memory allocations.
	for (int i = 0; i < pDataArray.size(); i++) {
		for (itSum = pDataArray[i]->sumDiv.begin(); itSum != pDataArray[i]->sumDiv.end(); itSum++) {
			for (int k = 0; k < (itSum->second).size(); k++) {
				sumDiv[itSum->first][k] += (itSum->second)[k];
			}
		}
		delete cts[i];
		delete trees[i];
		CloseHandle(hThreadArray[i]);
		delete pDataArray[i];
	}

#endif

	return 0;

}
//**********************************************************************************************************************
int PhyloDiversityCommand::driver(Tree* t, map< string, vector<float> >& div, map<string, vector<float> >& sumDiv, int numIters, int increment, vector<int>& randomLeaf, set<int>& numSampledList, ofstream& outCollect, ofstream& outSum, bool doSumCollect) {
	int numLeafNodes = randomLeaf.size();
	vector<string> mGroups = settings.getGroups();

	map<string, int> rootForGroup = getRootForGroups(t); //maps groupName to root node in tree. "root" for group may not be the trees root and we don't want to include the extra branches.


	for (int l = 0; l < numIters; l++) {
		random_shuffle(randomLeaf.begin(), randomLeaf.end());

		//initialize counts
		map<string, int> counts;
		vector< map<string, bool> > countedBranch;
		for (int i = 0; i < t->getNumNodes(); i++) {
			map<string, bool> temp;
			for (int j = 0; j < mGroups.size(); j++) { temp[mGroups[j]] = false; }
			countedBranch.push_back(temp);
		}

		for (int j = 0; j < mGroups.size(); j++) { counts[mGroups[j]] = 0; }

		map<string, int> metCount; bool allDone = false;
		for (int j = 0; j < mGroups.size(); j++) { counts[mGroups[j]] = false; }
		for (int k = 0; k < numLeafNodes; k++) {

			if (ctrlc_pressed) { return 0; }

			//calc branch length of randomLeaf k
			vector<float> br = calcBranchLength(t, randomLeaf[k], countedBranch, rootForGroup);

			//for each group in the groups update the total branch length accounting for the names file
			vector<string> groups = t->tree[randomLeaf[k]].getGroup();

			for (int j = 0; j < groups.size(); j++) {

				if (m->inUsersGroups(groups[j], mGroups)) {
					int numSeqsInGroupJ = 0;
					map<string, int>::iterator it;
					it = t->tree[randomLeaf[k]].pcount.find(groups[j]);
					if (it != t->tree[randomLeaf[k]].pcount.end()) { //this leaf node contains seqs from group j
						numSeqsInGroupJ = it->second;
					}

					if (numSeqsInGroupJ != 0) { div[groups[j]][(counts[groups[j]] + 1)] = div[groups[j]][counts[groups[j]]] + br[j]; }

					for (int s = (counts[groups[j]] + 2); s <= (counts[groups[j]] + numSeqsInGroupJ); s++) {
						div[groups[j]][s] = div[groups[j]][s - 1];  //update counts, but don't add in redundant branch lengths
					}
					counts[groups[j]] += numSeqsInGroupJ;
					if (subsample) {
						if (counts[groups[j]] >= subsampleSize) { metCount[groups[j]] = true; }
						bool allTrue = true;
						for (int h = 0; h < mGroups.size(); h++) {
							if (!metCount[mGroups[h]]) { allTrue = false; }
						}
						if (allTrue) { allDone = true; }
					}
					if (allDone) { j += groups.size(); k += numLeafNodes; }
				}
			}
		}

		//if you subsample then rarefy=t
		if (rarefy) {
			//add this diversity to the sum
			for (int j = 0; j < mGroups.size(); j++) {
				for (int g = 0; g < div[mGroups[j]].size(); g++) {
					sumDiv[mGroups[j]][g] += div[mGroups[j]][g];
				}
			}
		}

		if ((collect) && (l == 0) && doSumCollect) { printData(numSampledList, div, outCollect, 1); }
		if ((summary) && (l == 0) && doSumCollect) { printSumData(div, outSum, 1); }
	}

	return 0;

}

//**********************************************************************************************************************

void PhyloDiversityCommand::printSumData(map< string, vector<float> >& div, ofstream& out, int numIters) {

	out << "Groups\tnumSampled\tphyloDiversity" << endl;

	out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);

	vector<string> mGroups = settings.getGroups();
	int numSampled = 0;
	for (int j = 0; j < mGroups.size(); j++) {
		if (subsample) { numSampled = subsampleSize; }
		else { numSampled = (div[mGroups[j]].size() - 1); }

		out << mGroups[j] << '\t' << numSampled << '\t';

		float score;
		if (scale) { score = (div[mGroups[j]][numSampled] / (float)numIters) / (float)numSampled; }
		else { score = div[mGroups[j]][numSampled] / (float)numIters; }

		out << setprecision(4) << score << endl;
		//cout << mGroups[j] << '\t' << numSampled << '\t'<< setprecision(4) << score << endl;
	}

	out.close();

}
//**********************************************************************************************************************

void PhyloDiversityCommand::printData(set<int>& num, map< string, vector<float> >& div, ofstream& out, int numIters) {

	out << "numSampled";
	vector<string> mGroups = settings.getGroups();
	for (int i = 0; i < mGroups.size(); i++) { out << '\t' << mGroups[i]; }
	out << endl;

	out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);

	for (set<int>::iterator it = num.begin(); it != num.end(); it++) {
		int numSampled = *it;

		out << numSampled;

		for (int j = 0; j < mGroups.size(); j++) {
			if (numSampled < div[mGroups[j]].size()) {
				float score;
				if (scale) { score = (div[mGroups[j]][numSampled] / (float)numIters) / (float)numSampled; }
				else { score = div[mGroups[j]][numSampled] / (float)numIters; }

				out << '\t' << setprecision(4) << score;
			}
			else { out << "\tNA"; }
		}
		out << endl;
	}

	out.close();

}
//**********************************************************************************************************************
//need a vector of floats one branch length for every group the node represents.
vector<float> PhyloDiversityCommand::calcBranchLength(Tree* t, int leaf, vector< map<string, bool> >& counted, map<string, int> roots) {

	//calc the branch length
	//while you aren't at root
	vector<float> sums;
	int index = leaf;

	vector<string> groups = t->tree[leaf].getGroup();
	sums.resize(groups.size(), 0.0);


	//you are a leaf
	if (t->tree[index].getBranchLength() != -1) {
		for (int k = 0; k < groups.size(); k++) {
			sums[k] += abs(t->tree[index].getBranchLength());
		}
	}


	index = t->tree[index].getParent();

	//while you aren't at root
	while (t->tree[index].getParent() != -1) {

		if (ctrlc_pressed) { return sums; }

		for (int k = 0; k < groups.size(); k++) {

			if (index >= roots[groups[k]]) { counted[index][groups[k]] = true; } //if you are at this groups "root", then say we are done

			if (!counted[index][groups[k]]) { //if counted[index][groups[k] is true this groups has already added all br from here to root, so quit early
				if (t->tree[index].getBranchLength() != -1) {
					sums[k] += abs(t->tree[index].getBranchLength());
				}
				counted[index][groups[k]] = true;
			}
		}
		index = t->tree[index].getParent();
	}

	return sums;

}
//**********************************************************************************************************************
map<string, int> PhyloDiversityCommand::getRootForGroups(Tree* t) {
	map<string, int> roots; //maps group to root for group, may not be root of tree
	map<string, bool> done;

	//initialize root for all groups to -1
	for (int k = 0; k < (t->getCountTable())->getNamesOfGroups().size(); k++) { done[(t->getCountTable())->getNamesOfGroups()[k]] = false; }

	for (int i = 0; i < t->getNumLeaves(); i++) {

		vector<string> groups = t->tree[i].getGroup();

		int index = t->tree[i].getParent();

		for (int j = 0; j < groups.size(); j++) {

			if (done[groups[j]] == false) { //we haven't found the root for this group yet, initialize it
				done[groups[j]] = true;
				roots[groups[j]] = i; //set root to self to start
			}

			//while you aren't at root
			while (t->tree[index].getParent() != -1) {

				if (ctrlc_pressed) { return roots; }

				//do both your chidren have have descendants from the users groups? 
				int lc = t->tree[index].getLChild();
				int rc = t->tree[index].getRChild();

				int LpcountSize = 0;
				map<string, int>::iterator itGroup = t->tree[lc].pcount.find(groups[j]);
				if (itGroup != t->tree[lc].pcount.end()) { LpcountSize++; }

				int RpcountSize = 0;
				itGroup = t->tree[rc].pcount.find(groups[j]);
				if (itGroup != t->tree[rc].pcount.end()) { RpcountSize++; }

				if ((LpcountSize != 0) && (RpcountSize != 0)) { //possible root
					if (index > roots[groups[j]]) { roots[groups[j]] = index; }
				}
				else { ; }

				index = t->tree[index].getParent();
			}
			//}
		}
	}



	return roots;

}
//**********************************************************************************************************************



