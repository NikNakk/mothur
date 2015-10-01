/*
 *  clustersplitcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/19/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "clustersplitcommand.h"


 //**********************************************************************************************************************
vector<string> ClusterSplitCommand::setParameters() {
	try {
		CommandParameter pfile("file", "InputTypes", "", "", "PhylipColumnFasta", "PhylipColumnFasta", "none", "", false, false, true); parameters.push_back(pfile);
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "none", "none", "FastaTaxName", "", false, false, true); parameters.push_back(ptaxonomy);
		CommandParameter pphylip("phylip", "InputTypes", "", "", "PhylipColumnFasta", "PhylipColumnFasta", "none", "list", false, false, true); parameters.push_back(pphylip);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "PhylipColumnFasta", "PhylipColumnFasta", "FastaTaxName", "list", false, false, true); parameters.push_back(pfasta);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "ColumnName-FastaTaxName", "rabund-sabund", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount", "none", "", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pcolumn("column", "InputTypes", "", "", "PhylipColumnFasta", "PhylipColumnFasta", "ColumnName", "list", false, false, true); parameters.push_back(pcolumn);
		CommandParameter ptaxlevel("taxlevel", "Number", "", "3", "", "", "", "", false, false, true); parameters.push_back(ptaxlevel);
		nkParameters.add(new MultipleParameter("splitmethod", vector<string>{"classify", "fasta", "distance"}, "distance", false, false, true));
		nkParameters.add(new BooleanParameter("large", false, false, false));
		nkParameters.add(new BooleanParameter("showabund", true, false, false));
		nkParameters.add(new BooleanParameter("cluster", true, false, false));
		nkParameters.add(new BooleanParameter("timing", false, false, false));
		nkParameters.add(new ProcessorsParameter(settings));
		CommandParameter pcutoff("cutoff", "Number", "", "0.25", "", "", "", "", false, false, true); parameters.push_back(pcutoff);
		nkParameters.add(new NumberParameter("precision", -INFINITY, INFINITY, 100, false, false));
		CommandParameter pmethod("method", "Multiple", "furthest-nearest-average-weighted", "average", "", "", "", "", false, false); parameters.push_back(pmethod);
		nkParameters.add(new BooleanParameter("hard", true, false, false));
		nkParameters.add(new BooleanParameter("islist", false, false, false));
		nkParameters.add(new BooleanParameter("classic", false, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterSplitCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterSplitCommand::getHelpString() {
	try {
		string helpString = "The cluster.split command parameter options are file, fasta, phylip, column, name, count, cutoff, precision, method, splitmethod, taxonomy, taxlevel, showabund, timing, hard, large, cluster, processors. Fasta or Phylip or column and name are required.\n"
			"The cluster.split command can split your files in 3 ways. Splitting by distance file, by classification, or by classification also using a fasta file. \n"
			"For the distance file method, you need only provide your distance file and mothur will split the file into distinct groups. \n"
			"For the classification method, you need to provide your distance file and taxonomy file, and set the splitmethod to classify.  \n"
			"You will also need to set the taxlevel you want to split by. mothur will split the sequences into distinct taxonomy groups, and split the distance file based on those groups. \n"
			"For the classification method using a fasta file, you need to provide your fasta file, names file and taxonomy file.  \n"
			"You will also need to set the taxlevel you want to split by. mothur will split the sequence into distinct taxonomy groups, and create distance files for each grouping. \n"
			"The file option allows you to enter your file containing your list of column and names/count files as well as the singleton file.  This file is mothur generated, when you run cluster.split() with the cluster=f parameter.  This can be helpful when you have a large dataset that you may be able to use all your processors for the splitting step, but have to reduce them for the cluster step due to RAM constraints. For example: cluster.split(fasta=yourFasta, taxonomy=yourTax, count=yourCount, taxlevel=3, cluster=f, processors=8) then cluster.split(file=yourFile, processors=4).  This allows your to maximize your processors during the splitting step.  Also, if you are unsure if the cluster step will have RAM issue with multiple processors, you can avoid running the first part of the command multiple times.\n"
			"The phylip and column parameter allow you to enter your distance file. \n"
			"The fasta parameter allows you to enter your aligned fasta file. \n"
			"The name parameter allows you to enter your name file. \n"
			"The count parameter allows you to enter your count file. \n A count or name file is required if your distance file is in column format"
			"The cluster parameter allows you to indicate whether you want to run the clustering or just split the distance matrix, default=t"
			"The cutoff parameter allow you to set the distance you want to cluster to, default is 0.25. \n"
			"The precision parameter allows you specify the precision of the precision of the distances outputted, default=100, meaning 2 decimal places. \n"
			"The method allows you to specify what clustering algorithm you want to use, default=average, option furthest, nearest, or average. \n"
			"The splitmethod parameter allows you to specify how you want to split your distance file before you cluster, default=distance, options distance, classify or fasta. \n"
			"The taxonomy parameter allows you to enter the taxonomy file for your sequences, this is only valid if you are using splitmethod=classify. Be sure your taxonomy file does not include the probability scores. \n"
			"The taxlevel parameter allows you to specify the taxonomy level you want to use to split the distance file, default=3, meaning use the first taxon in each list. \n"
			"The large parameter allows you to indicate that your distance matrix is too large to fit in RAM.  The default value is false.\n"
			"The classic parameter allows you to indicate that you want to run your files with cluster.classic.  It is only valid with splitmethod=fasta. Default=f.\n"
			"The cluster.split command should be in the following format: \n"
			"cluster.split(column=youDistanceFile, name=yourNameFile, method=yourMethod, cutoff=yourCutoff, precision=yourPrecision, splitmethod=yourSplitmethod, taxonomy=yourTaxonomyfile, taxlevel=yourtaxlevel) \n"
			"Example: cluster.split(column=abrecovery.dist, name=abrecovery.names, method=furthest, cutoff=0.10, precision=1000, splitmethod=classify, taxonomy=abrecovery.silva.slv.taxonomy, taxlevel=5) \n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterSplitCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterSplitCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "list") { pattern = "[filename],[clustertag],list-[filename],[clustertag],[tag2],list"; }
	else if (type == "rabund") { pattern = "[filename],[clustertag],rabund"; }
	else if (type == "sabund") { pattern = "[filename],[clustertag],sabund"; }
	else if (type == "column") { pattern = "[filename],dist"; }
	else if (type == "file") { pattern = "[filename],file"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ClusterSplitCommand::ClusterSplitCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["rabund"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
		outputTypes["column"] = tempOutNames;
		outputTypes["file"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterSplitCommand, ClusterSplitCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
//This function checks to make sure the cluster command has no errors and then clusters based on the method chosen.
ClusterSplitCommand::ClusterSplitCommand(Settings& settings, string option) : Command(settings, option) {
	try {
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterSplitCommand, ClusterSplitCommand";
		exit(1);
	}
}

//**********************************************************************************************************************

int ClusterSplitCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	time_t estart;
	vector<string> listFileNames;
	vector< map<string, string> > distName;
	set<string> labels;
	string singletonName = "";

	double saveCutoff = cutoff;

	if (file != "") {
		deleteFiles = false; estart = time(NULL);
		singletonName = readFile(distName);

		if (isList) {

			//set list file as new current listfile
			string current = "";
			itTypes = outputTypes.find("list");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("list", current); }
			}

			LOG(INFO) << '\n' << "Output File Names: " << '\n';
			for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
			LOG(INFO) << "";

			return 0;
		}

	}
	else {

		//****************** file prep work ******************************//

			//if user gave a phylip file convert to column file
		if (format == "phylip") {
			estart = time(NULL);
			LOG(INFO) << "Converting to column format..." << '\n';

			ReadCluster* convert = new ReadCluster(distfile, cutoff, outputDir, false);

			NameAssignment* nameMap = NULL;
			convert->setFormat("phylip");
			convert->read(nameMap);

			if (ctrlc_pressed) { delete convert;  return 0; }

			distfile = convert->getOutputFile();

			//if no names file given with phylip file, create it
			ListVector* listToMakeNameFile = convert->getListVector();
			if ((namefile == "") && (countfile == "")) {  //you need to make a namefile for split matrix
				ofstream out;
				namefile = phylipfile + ".names";
				File::openOutputFile(namefile, out);
				for (int i = 0; i < listToMakeNameFile->getNumBins(); i++) {
					string bin = listToMakeNameFile->get(i);
					out << bin << '\t' << bin << endl;
				}
				out.close();
			}
			delete listToMakeNameFile;
			delete convert;

			LOG(INFO) << "It took " + toString(time(NULL) - estart) + " seconds to convert the distance file." << '\n';
		}
		if (ctrlc_pressed) { return 0; }

		estart = time(NULL);
		LOG(INFO) << "Splitting the file..." << '\n';

		//split matrix into non-overlapping groups
		SplitMatrix* split;
		if (splitmethod == "distance") { split = new SplitMatrix(distfile, namefile, countfile, taxFile, cutoff, splitmethod, large); }
		else if (splitmethod == "classify") { split = new SplitMatrix(distfile, namefile, countfile, taxFile, taxLevelCutoff, splitmethod, large); }
		else if (splitmethod == "fasta") { split = new SplitMatrix(fastafile, namefile, countfile, taxFile, taxLevelCutoff, cutoff, splitmethod, processors, classic, outputDir); }
		else { LOG(INFO) << "Not a valid splitting method.  Valid splitting algorithms are distance, classify or fasta." << '\n'; return 0; }
		split->split();

		if (ctrlc_pressed) { delete split; return 0; }

		singletonName = split->getSingletonNames();
		distName = split->getDistanceFiles();  //returns map of distance files -> namefile sorted by distance file size
		delete split;

		if (app.isDebug) { LOG(DEBUG) << "distName.size() = " + toString(distName.size()) + ".\n"; }

		//output a merged distance file
		//if (splitmethod == "fasta")		{ createMergedDistanceFile(distName); }

		if (ctrlc_pressed) { return 0; }

		LOG(INFO) << "It took " + toString(time(NULL) - estart) + " seconds to split the distance file." << '\n';
		estart = time(NULL);

		if (!runCluster) {
			string filename = printFile(singletonName, distName);

			LOG(INFO) << '\n' << "Output File Names: " << '\n';
			LOG(INFO) << '\n' << filename << '\n';
			for (int i = 0; i < distName.size(); i++) { LOG(INFO) << distName[i].begin()->first << '\n'; LOG(INFO) << distName[i].begin()->second << '\n'; }
			LOG(INFO) << "";

			return 0;
		}
		deleteFiles = true;

	}
	//****************** break up files between processes and cluster each file set ******************************//
	///////////////////// WINDOWS CAN ONLY USE 1 PROCESSORS ACCESS VIOLATION UNRESOLVED ///////////////////////
	//sanity check
	if (processors > distName.size()) { processors = distName.size(); }

#if defined (UNIX)
	if (processors == 1) {
		listFileNames = cluster(distName, labels); //clusters individual files and returns names of list files
	}
	else {
		listFileNames = createProcesses(distName, labels);
	}
#else
	listFileNames = cluster(distName, labels); //clusters individual files and returns names of list files
#endif

	if (ctrlc_pressed) { for (int i = 0; i < listFileNames.size(); i++) { File::remove(listFileNames[i]); } return 0; }

	if (saveCutoff != cutoff) { LOG(INFO) << "Cutoff was " + toString(saveCutoff) + " changed cutoff to " + toString(cutoff) << '\n'; }

	LOG(INFO) << "It took " + toString(time(NULL) - estart) + " seconds to cluster" << '\n';

	//****************** merge list file and create rabund and sabund files ******************************//
	estart = time(NULL);
	LOG(INFO) << "Merging the clustered files..." << '\n';

	ListVector* listSingle;
	map<float, int> labelBins = completeListFile(listFileNames, singletonName, labels, listSingle); //returns map of label to numBins

	if (ctrlc_pressed) { if (listSingle != NULL) { delete listSingle; } for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	mergeLists(listFileNames, labelBins, listSingle);

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	//delete after all are complete incase a crash happens
	if (!deleteFiles) { for (int i = 0; i < distName.size(); i++) { File::remove(distName[i].begin()->first); File::remove(distName[i].begin()->second); } }

	LOG(INFO) << "It took " + toString(time(NULL) - estart) + " seconds to merge." << '\n';

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

	return 0;
}
//**********************************************************************************************************************
map<float, int> ClusterSplitCommand::completeListFile(vector<string> listNames, string singleton, set<string>& userLabels, ListVector*& listSingle) {

	map<float, int> labelBin;
	vector<float> orderFloat;
	int numSingleBins;

	//read in singletons
	if (singleton != "none") {

		ifstream in;
		File::openInputFile(singleton, in);

		string firstCol, secondCol;
		listSingle = new ListVector();

		if (countfile != "") { File::getline(in); File::gobble(in); }

		while (!in.eof()) {
			in >> firstCol >> secondCol; File::getline(in); File::gobble(in);
			if (countfile == "") { listSingle->push_back(secondCol); }
			else { listSingle->push_back(firstCol); }
		}

		in.close();
		File::remove(singleton);

		numSingleBins = listSingle->getNumBins();
	}
	else { listSingle = NULL; numSingleBins = 0; }

	//go through users set and make them floats so we can sort them 
	for (set<string>::iterator it = userLabels.begin(); it != userLabels.end(); ++it) {
		float temp = -10.0;

		if ((*it != "unique") && (convertTestFloat(*it, temp) == true)) { convert(*it, temp); }
		else if (*it == "unique") { temp = -1.0; }

		if (temp <= cutoff) {
			orderFloat.push_back(temp);
			labelBin[temp] = numSingleBins; //initialize numbins 
		}
	}

	//sort order
	sort(orderFloat.begin(), orderFloat.end());
	userLabels.clear();

	//get the list info from each file
	for (int k = 0; k < listNames.size(); k++) {

		if (ctrlc_pressed) {
			if (listSingle != NULL) { delete listSingle; listSingle = NULL; File::remove(singleton); }
			for (int i = 0; i < listNames.size(); i++) { File::remove(listNames[i]); }
			return labelBin;
		}

		InputData* input = new InputData(listNames[k], "list");
		ListVector* list = input->getListVector();
		string lastLabel = list->getLabel();

		string filledInList = listNames[k] + "filledInTemp";
		ofstream outFilled;
		File::openOutputFile(filledInList, outFilled);

		//for each label needed
		for (int l = 0; l < orderFloat.size(); l++) {

			string thisLabel;
			if (orderFloat[l] == -1) { thisLabel = "unique"; }
			else { thisLabel = toString(orderFloat[l], length - 1); }

			//this file has reached the end
			if (list == NULL) {
				list = input->getListVector(lastLabel, true);
			}
			else {	//do you have the distance, or do you need to fill in

				float labelFloat;
				if (list->getLabel() == "unique") { labelFloat = -1.0; }
				else { convert(list->getLabel(), labelFloat); }

				//check for missing labels
				if (labelFloat > orderFloat[l]) { //you are missing the label, get the next smallest one
					//if its bigger get last label, otherwise keep it
					delete list;
					list = input->getListVector(lastLabel, true);  //get last list vector to use, you actually want to move back in the file
				}
				lastLabel = list->getLabel();
			}

			//print to new file
			list->setLabel(thisLabel);
			list->print(outFilled);

			//update labelBin
			labelBin[orderFloat[l]] += list->getNumBins();

			delete list;

			list = input->getListVector();
		}

		if (list != NULL) { delete list; }
		delete input;

		outFilled.close();
		File::remove(listNames[k]);
		rename(filledInList.c_str(), listNames[k].c_str());
	}

	return labelBin;
}
//**********************************************************************************************************************
int ClusterSplitCommand::mergeLists(vector<string> listNames, map<float, int> userLabels, ListVector* listSingle) {
	if (outputDir == "") { outputDir += File::getPath(distfile); }
	fileroot = outputDir + File::getRootName(File::getSimpleName(distfile));

	map<string, string> variables;
	variables["[filename]"] = fileroot;
	variables["[clustertag]"] = tag;
	string sabundFileName = getOutputFileName("sabund", variables);
	string rabundFileName = getOutputFileName("rabund", variables);
	if (countfile != "") { variables["[tag2]"] = "unique_list"; }
	string listFileName = getOutputFileName("list", variables);

	map<string, int> counts;
	if (countfile == "") {
		File::openOutputFile(sabundFileName, outSabund);
		File::openOutputFile(rabundFileName, outRabund);
		outputNames.push_back(sabundFileName); outputTypes["sabund"].push_back(sabundFileName);
		outputNames.push_back(rabundFileName); outputTypes["rabund"].push_back(rabundFileName);

	}
	else {
		if (file == "") {
			CountTable ct;
			ct.readTable(countfile, false, false);
			counts = ct.getNameMap();
		}
	}

	File::openOutputFile(listFileName, outList);
	outputNames.push_back(listFileName); outputTypes["list"].push_back(listFileName);

	map<float, int>::iterator itLabel;

	//clears out junk for autocompleting of list files above.  Perhaps there is a beter way to handle this from within the data structure?
	m->printedListHeaders = false;

	//for each label needed
	for (itLabel = userLabels.begin(); itLabel != userLabels.end(); itLabel++) {

		string thisLabel;
		if (itLabel->first == -1) { thisLabel = "unique"; }
		else { thisLabel = toString(itLabel->first, length - 1); }

		//outList << thisLabel << '\t' << itLabel->second << '\t';

		RAbundVector* rabund = NULL;
		ListVector completeList;
		completeList.setLabel(thisLabel);

		if (countfile == "") {
			rabund = new RAbundVector();
			rabund->setLabel(thisLabel);
		}

		//add in singletons
		if (listSingle != NULL) {
			for (int j = 0; j < listSingle->getNumBins(); j++) {
				//outList << listSingle->get(j) << '\t';
				completeList.push_back(listSingle->get(j));
				if (countfile == "") { rabund->push_back(m->getNumNames(listSingle->get(j))); }
			}
		}

		//get the list info from each file
		for (int k = 0; k < listNames.size(); k++) {

			if (ctrlc_pressed) { if (listSingle != NULL) { delete listSingle; } for (int i = 0; i < listNames.size(); i++) { File::remove(listNames[i]); } if (rabund != NULL) { delete rabund; } return 0; }

			InputData* input = new InputData(listNames[k], "list");
			ListVector* list = input->getListVector(thisLabel);

			//this file has reached the end
			if (list == NULL) { LOG(INFO) << "Error merging listvectors in file " + listNames[k] << '\n'; }
			else {
				for (int j = 0; j < list->getNumBins(); j++) {
					//outList << list->get(j) << '\t';
					completeList.push_back(list->get(j));
					if (countfile == "") { rabund->push_back(m->getNumNames(list->get(j))); }
				}
				delete list;
			}
			delete input;
		}

		if (countfile == "") {
			SAbundVector sabund = rabund->getSAbundVector();
			sabund.print(outSabund);
			rabund->print(outRabund);
		}
		//outList << endl;
		if (!m->printedListHeaders) { m->listBinLabelsInFile.clear(); completeList.printHeaders(outList); }
		if (countfile == "") { completeList.print(outList); }
		else if ((file == "") && (countfile != "")) { completeList.print(outList, counts); }
		else { completeList.print(outList); }

		if (rabund != NULL) { delete rabund; }
	}

	outList.close();
	if (countfile == "") {
		outRabund.close();
		outSabund.close();
	}
	if (listSingle != NULL) { delete listSingle; }

	for (int i = 0; i < listNames.size(); i++) { File::remove(listNames[i]); }

	return 0;
}

//**********************************************************************************************************************

void ClusterSplitCommand::printData(ListVector* oldList) {
	string label = oldList->getLabel();
	RAbundVector oldRAbund = oldList->getRAbundVector();

	oldRAbund.setLabel(label);
	if (m->isTrue(showabund)) {
		oldRAbund.getSAbundVector().print(cout);
	}
	oldRAbund.print(outRabund);
	oldRAbund.getSAbundVector().print(outSabund);

	oldList->print(outList);
}
//**********************************************************************************************************************
vector<string>  ClusterSplitCommand::createProcesses(vector< map<string, string> > distName, set<string>& labels) {
	deleteFiles = false; //so if we need to recalc the processors the files are still there
	bool recalc = false;
	vector<string> listFiles;
	vector < vector < map<string, string> > > dividedNames; //distNames[1] = vector of filenames for process 1...
	dividedNames.resize(processors);

	//for each file group figure out which process will complete it
	//want to divide the load intelligently so the big files are spread between processes
	for (int i = 0; i < distName.size(); i++) {
		//cout << i << endl;
		int processToAssign = (i + 1) % processors;
		if (processToAssign == 0) { processToAssign = processors; }

		dividedNames[(processToAssign - 1)].push_back(distName[i]);
		if ((processToAssign - 1) == 1) { LOG(INFO) << distName[i].begin()->first + "\n"; }
	}

	//now lets reverse the order of ever other process, so we balance big files running with little ones
	for (int i = 0; i < processors; i++) {
		//cout << i << endl;
		int remainder = ((i + 1) % processors);
		if (remainder) { reverse(dividedNames[i].begin(), dividedNames[i].end()); }
	}

	if (ctrlc_pressed) { return listFiles; }

#if defined (UNIX)
	int process = 1;
	processIDS.clear();

	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
			process++;
		}
		else if (pid == 0) {
			set<string> labels;
			vector<string> listFileNames = cluster(dividedNames[process], labels);

			//write out names to file
			string filename = m->mothurGetpid(process) + ".temp";
			ofstream out;
			File::openOutputFile(filename, out);
			out << tag << endl;
			for (int j = 0; j < listFileNames.size(); j++) { out << listFileNames[j] << endl; }
			out.close();

			//print out labels
			ofstream outLabels;
			filename = m->mothurGetpid(process) + ".temp.labels";
			File::openOutputFile(filename, outLabels);

			outLabels << cutoff << endl;
			for (set<string>::iterator it = labels.begin(); it != labels.end(); it++) {
				outLabels << (*it) << endl;
			}
			outLabels.close();

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
			for (int i = 0;i < processIDS.size();i++) {
				File::remove((toString(processIDS[i]) + ".temp"));
				File::remove((toString(processIDS[i]) + ".temp.labels"));
			}
			ctrlc_pressed = false;
			recalc = true;
			break;
		}

	}

	if (recalc) {
		//test line, also set recalc to true.
		//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove((toString(processIDS[i]) + ".temp"));File::remove((toString(processIDS[i]) + ".temp.labels"));} processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

		listFiles.clear();
		dividedNames.clear(); //distNames[1] = vector of filenames for process 1...
		dividedNames.resize(processors);

		//for each file group figure out which process will complete it
		//want to divide the load intelligently so the big files are spread between processes
		for (int i = 0; i < distName.size(); i++) {
			//cout << i << endl;
			int processToAssign = (i + 1) % processors;
			if (processToAssign == 0) { processToAssign = processors; }

			dividedNames[(processToAssign - 1)].push_back(distName[i]);
			if ((processToAssign - 1) == 1) { LOG(INFO) << distName[i].begin()->first + "\n"; }
		}

		//now lets reverse the order of ever other process, so we balance big files running with little ones
		for (int i = 0; i < processors; i++) {
			//cout << i << endl;
			int remainder = ((i + 1) % processors);
			if (remainder) { reverse(dividedNames[i].begin(), dividedNames[i].end()); }
		}

		processIDS.resize(0);
		process = 1;

		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				set<string> labels;
				vector<string> listFileNames = cluster(dividedNames[process], labels);

				//write out names to file
				string filename = m->mothurGetpid(process) + ".temp";
				ofstream out;
				File::openOutputFile(filename, out);
				out << tag << endl;
				for (int j = 0; j < listFileNames.size(); j++) { out << listFileNames[j] << endl; }
				out.close();

				//print out labels
				ofstream outLabels;
				filename = m->mothurGetpid(process) + ".temp.labels";
				File::openOutputFile(filename, outLabels);

				outLabels << cutoff << endl;
				for (set<string>::iterator it = labels.begin(); it != labels.end(); it++) {
					outLabels << (*it) << endl;
				}
				outLabels.close();

				exit(0);
			}
			else {
				LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
				for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
				exit(0);
			}
		}
	}


	//do your part
	listFiles = cluster(dividedNames[0], labels);

	//force parent to wait until all the processes are done
	for (int i = 0;i < processIDS.size();i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	//get list of list file names from each process
	for (int i = 0;i < processIDS.size();i++) {
		string filename = toString(processIDS[i]) + ".temp";
		ifstream in;
		File::openInputFile(filename, in);

		in >> tag; File::gobble(in);

		while (!in.eof()) {
			string tempName;
			in >> tempName; File::gobble(in);
			listFiles.push_back(tempName);
		}
		in.close();
		File::remove((toString(processIDS[i]) + ".temp"));

		//get labels
		filename = toString(processIDS[i]) + ".temp.labels";
		ifstream in2;
		File::openInputFile(filename, in2);

		float tempCutoff;
		in2 >> tempCutoff; File::gobble(in2);
		if (tempCutoff < cutoff) { cutoff = tempCutoff; }

		while (!in2.eof()) {
			string tempName;
			in2 >> tempName; File::gobble(in2);
			if (labels.count(tempName) == 0) { labels.insert(tempName); }
		}
		in2.close();
		File::remove((toString(processIDS[i]) + ".temp.labels"));
	}

	deleteFiles = true;

	//delete the temp files now that we are done
	for (int i = 0; i < distName.size(); i++) {
		string thisNamefile = distName[i].begin()->second;
		string thisDistFile = distName[i].begin()->first;
		File::remove(thisNamefile);
		File::remove(thisDistFile);
	}

#else
#endif

	return listFiles;

}
//**********************************************************************************************************************

vector<string> ClusterSplitCommand::cluster(vector< map<string, string> > distNames, set<string>& labels) {

	vector<string> listFileNames;
	double smallestCutoff = cutoff;

	//cluster each distance file
	for (int i = 0; i < distNames.size(); i++) {

		string thisNamefile = distNames[i].begin()->second;
		string thisDistFile = distNames[i].begin()->first;

		string listFileName = "";
		if (classic) { listFileName = clusterClassicFile(thisDistFile, thisNamefile, labels, smallestCutoff); }
		else { listFileName = clusterFile(thisDistFile, thisNamefile, labels, smallestCutoff); }

		if (ctrlc_pressed) { //clean up
			for (int i = 0; i < listFileNames.size(); i++) { File::remove(listFileNames[i]); }
			listFileNames.clear(); return listFileNames;
		}

		listFileNames.push_back(listFileName);
	}

	cutoff = smallestCutoff;

	return listFileNames;



}
//**********************************************************************************************************************
string ClusterSplitCommand::clusterClassicFile(string thisDistFile, string thisNamefile, set<string>& labels, double& smallestCutoff) {
	string listFileName = "";

	ListVector* list = NULL;
	ListVector oldList;
	RAbundVector* rabund = NULL;

	LOG(INFO) << '\n' << "Reading " + thisDistFile << '\n';

	//reads phylip file storing data in 2D vector, also fills list and rabund
	bool sim = false;
	ClusterClassic* cluster = new ClusterClassic(cutoff, method, sim);

	NameAssignment* nameMap = NULL;
	CountTable* ct = NULL;
	if (namefile != "") {
		nameMap = new NameAssignment(thisNamefile);
		nameMap->readMap();
		cluster->readPhylipFile(thisDistFile, nameMap);
	}
	else if (countfile != "") {
		ct = new CountTable();
		ct->readTable(thisNamefile, false, false);
		cluster->readPhylipFile(thisDistFile, ct);
	}
	tag = cluster->getTag();

	if (ctrlc_pressed) {
		if (namefile != "") { delete nameMap; }
		else { delete ct; } delete cluster; return 0;
	}

	list = cluster->getListVector();
	rabund = cluster->getRAbundVector();

	if (outputDir == "") { outputDir += File::getPath(thisDistFile); }
	fileroot = outputDir + File::getRootName(File::getSimpleName(thisDistFile));
	listFileName = fileroot + tag + ".list";

	ofstream listFile;
	File::openOutputFile(fileroot + tag + ".list", listFile);

	float previousDist = 0.00000;
	float rndPreviousDist = 0.00000;
	oldList = *list;

	LOG(INFO) << '\n' << "Clustering " + thisDistFile << '\n';

	while ((cluster->getSmallDist() < cutoff) && (cluster->getNSeqs() > 1)) {
		if (ctrlc_pressed) {
			delete cluster; delete list; delete rabund; listFile.close();  if (namefile != "") { delete nameMap; }
			else { delete ct; } return listFileName;
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
			oldList.setLabel("unique");
			oldList.print(listFile);
			if (labels.count("unique") == 0) { labels.insert("unique"); }
		}
		else if (rndDist != rndPreviousDist) {
			oldList.setLabel(toString(rndPreviousDist, length - 1));
			oldList.print(listFile);
			if (labels.count(toString(rndPreviousDist, length - 1)) == 0) { labels.insert(toString(rndPreviousDist, length - 1)); }
		}


		previousDist = dist;
		rndPreviousDist = rndDist;
		oldList = *list;
	}

	if (previousDist <= 0.0000) {
		oldList.setLabel("unique");
		oldList.print(listFile);
		if (labels.count("unique") == 0) { labels.insert("unique"); }
	}
	else if (rndPreviousDist < cutoff) {
		oldList.setLabel(toString(rndPreviousDist, length - 1));
		oldList.print(listFile);
		if (labels.count(toString(rndPreviousDist, length - 1)) == 0) { labels.insert(toString(rndPreviousDist, length - 1)); }
	}


	listFile.close();

	delete cluster;  delete list; delete rabund;
	if (namefile != "") { delete nameMap; }
	else { delete ct; }

	if (deleteFiles) {
		File::remove(thisDistFile);
		File::remove(thisNamefile);
	}
	return listFileName;

}

//**********************************************************************************************************************
string ClusterSplitCommand::clusterFile(string thisDistFile, string thisNamefile, set<string>& labels, double& smallestCutoff) {
	string listFileName = "";

	Cluster* cluster = NULL;
	SparseDistanceMatrix* matrix = NULL;
	ListVector* list = NULL;
	ListVector oldList;
	RAbundVector* rabund = NULL;

	if (ctrlc_pressed) { return listFileName; }

	LOG(INFO) << '\n' << "Reading " + thisDistFile << '\n';

	ReadMatrix* read = new ReadColumnMatrix(thisDistFile);
	read->setCutoff(cutoff);

	NameAssignment* nameMap = NULL;
	CountTable* ct = NULL;
	if (namefile != "") {
		nameMap = new NameAssignment(thisNamefile);
		nameMap->readMap();
		read->read(nameMap);
	}
	else if (countfile != "") {
		ct = new CountTable();
		ct->readTable(thisNamefile, false, false);
		read->read(ct);
	}
	else { read->read(nameMap); }

	list = read->getListVector();
	oldList = *list;
	matrix = read->getDMatrix();

	if (countfile != "") {
		rabund = new RAbundVector();
		createRabund(ct, list, rabund); //creates an rabund that includes the counts for the unique list
		delete ct;
	}
	else { rabund = new RAbundVector(list->getRAbundVector()); }

	delete read;  read = NULL;
	if (namefile != "") { delete nameMap; nameMap = NULL; }

	LOG(INFO) << '\n' << "Clustering " + thisDistFile << '\n';

	//create cluster
	float adjust = -1.0;
	if (method == "furthest") { cluster = new CompleteLinkage(rabund, list, matrix, cutoff, method, adjust); }
	else if (method == "nearest") { cluster = new SingleLinkage(rabund, list, matrix, cutoff, method, adjust); }
	else if (method == "average") { cluster = new AverageLinkage(rabund, list, matrix, cutoff, method, adjust); }
	tag = cluster->getTag();

	if (outputDir == "") { outputDir += File::getPath(thisDistFile); }
	fileroot = outputDir + File::getRootName(File::getSimpleName(thisDistFile));

	ofstream listFile;
	File::openOutputFile(fileroot + tag + ".list", listFile);

	listFileName = fileroot + tag + ".list";

	float previousDist = 0.00000;
	float rndPreviousDist = 0.00000;

	oldList = *list;

	print_start = true;
	start = time(NULL);
	double saveCutoff = cutoff;

	while (matrix->getSmallDist() < cutoff && matrix->getNNodes() > 0) {

		if (ctrlc_pressed) { //clean up
			delete matrix; delete list;	delete cluster; delete rabund;
			listFile.close();
			File::remove(listFileName);
			return listFileName;
		}

		cluster->update(saveCutoff);

		float dist = matrix->getSmallDist();
		float rndDist;
		if (hard) {
			rndDist = m->ceilDist(dist, precision);
		}
		else {
			rndDist = m->roundDist(dist, precision);
		}

		if (previousDist <= 0.0000 && dist != previousDist) {
			oldList.setLabel("unique");
			oldList.print(listFile);
			if (labels.count("unique") == 0) { labels.insert("unique"); }
		}
		else if (rndDist != rndPreviousDist) {
			oldList.setLabel(toString(rndPreviousDist, length - 1));
			oldList.print(listFile);
			if (labels.count(toString(rndPreviousDist, length - 1)) == 0) { labels.insert(toString(rndPreviousDist, length - 1)); }
		}

		previousDist = dist;
		rndPreviousDist = rndDist;
		oldList = *list;
	}


	if (previousDist <= 0.0000) {
		oldList.setLabel("unique");
		oldList.print(listFile);
		if (labels.count("unique") == 0) { labels.insert("unique"); }
	}
	else if (rndPreviousDist < cutoff) {
		oldList.setLabel(toString(rndPreviousDist, length - 1));
		oldList.print(listFile);
		if (labels.count(toString(rndPreviousDist, length - 1)) == 0) { labels.insert(toString(rndPreviousDist, length - 1)); }
	}

	delete matrix; delete list;	delete cluster; delete rabund;
	matrix = NULL; list = NULL; cluster = NULL; rabund = NULL;
	listFile.close();

	if (ctrlc_pressed) { //clean up
		File::remove(listFileName);
		return listFileName;
	}

	if (deleteFiles) {
		File::remove(thisDistFile);
		File::remove(thisNamefile);
	}

	if (saveCutoff != cutoff) {
		if (hard) { saveCutoff = m->ceilDist(saveCutoff, precision); }
		else { saveCutoff = m->roundDist(saveCutoff, precision); }

		LOG(INFO) << "Cutoff was " + toString(cutoff) + " changed cutoff to " + toString(saveCutoff) << '\n';
	}

	if (saveCutoff < smallestCutoff) { smallestCutoff = saveCutoff; }

	return listFileName;

}
//**********************************************************************************************************************

int ClusterSplitCommand::createMergedDistanceFile(vector< map<string, string> > distNames) {
	try {

		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir = File::getPath(fastafile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(fastafile));
		string outputFileName = getOutputFileName("column", variables);
		File::remove(outputFileName);


		for (int i = 0; i < distNames.size(); i++) {
			if (ctrlc_pressed) { return 0; }

			string thisDistFile = distNames[i].begin()->first;

			File::appendFiles(thisDistFile, outputFileName);
		}

		outputTypes["column"].push_back(outputFileName); outputNames.push_back(outputFileName);

		return 0;


	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterSplitCommand, createMergedDistanceFile";
		exit(1);
	}
}
//**********************************************************************************************************************
int ClusterSplitCommand::createRabund(CountTable*& ct, ListVector*& list, RAbundVector*& rabund) {
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
string ClusterSplitCommand::printFile(string singleton, vector< map<string, string> >& distName) {
	ofstream out;
	map<string, string> variables;
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir = File::getPath(distfile); }
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(distfile));
	string outputFileName = getOutputFileName("file", variables);
	File::openOutputFile(outputFileName, out);
	outputTypes["file"].push_back(outputFileName); outputNames.push_back(outputFileName);
	settings.setCurrent("file", outputFileName);

	out << singleton << endl;
	if (namefile != "") { out << "name" << endl; }
	else if (countfile != "") { out << "count" << endl; }
	else { out << "unknown" << endl; }

	for (int i = 0; i < distName.size(); i++) { out << distName[i].begin()->first << '\t' << distName[i].begin()->second << endl; }
	out.close();

	return outputFileName;

}
//**********************************************************************************************************************
string ClusterSplitCommand::readFile(vector< map<string, string> >& distName) {

	string singleton, thiscolumn, thisname, type;

	ifstream in;
	File::openInputFile(file, in);

	in >> singleton; File::gobble(in);

	string path = File::getPath(singleton);
	if (path == "") { singleton = inputDir + singleton; }

	in >> type; File::gobble(in);

	if (type == "name") { namefile = "name"; }
	else if (type == "count") { countfile = "count"; }
	else { LOG(LOGERROR) << "unknown file type. Are the files in column 2 of the file name files or count files? Please change unknown to name or count.\n"; ctrlc_pressed = true; }

	if (isList) {

		vector<string> listFileNames;
		string thisListFileName = "";
		set<string> listLabels;

		while (!in.eof()) {
			if (ctrlc_pressed) { break; }

			in >> thisListFileName; File::gobble(in);

			string path = File::getPath(thisListFileName);
			if (path == "") { thisListFileName = inputDir + thisListFileName; }

			getLabels(thisListFileName, listLabels);
			listFileNames.push_back(thisListFileName);
		}

		ListVector* listSingle;
		map<float, int> labelBins = completeListFile(listFileNames, singleton, listLabels, listSingle);

		mergeLists(listFileNames, labelBins, listSingle);

	}
	else {

		while (!in.eof()) {
			if (ctrlc_pressed) { break; }

			in >> thiscolumn; File::gobble(in);
			in >> thisname; File::gobble(in);

			map<string, string> temp;
			temp[thiscolumn] = thisname;
			distName.push_back(temp);
		}
	}

	in.close();

	return singleton;

}
//**********************************************************************************************************************
int ClusterSplitCommand::getLabels(string file, set<string>& listLabels) {
	ifstream in;
	File::openInputFile(file, in);

	//read headers
	File::getline(in); File::gobble(in);

	string label;
	while (!in.eof()) {
		if (ctrlc_pressed) { break; }

		in >> label; File::getline(in); File::gobble(in);

		listLabels.insert(label);
	}

	in.close();

	return 0;

}
//**********************************************************************************************************************
