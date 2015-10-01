//
//  mergetaxsummarycommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 2/13/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#include "mergetaxsummarycommand.h"


//**********************************************************************************************************************
vector<string> MergeTaxSummaryCommand::setParameters() {
	try {
		CommandParameter pinput("input", "String", "", "", "", "", "", "", false, true, true); parameters.push_back(pinput);
		CommandParameter poutput("output", "String", "", "", "", "", "", "", false, true, true); parameters.push_back(poutput);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MergeTaxSummaryCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string MergeTaxSummaryCommand::getHelpString() {
	try {
		string helpString = "The merge.taxsummary command takes a list of tax.summary files separated by dashes and merges them into one file.";
		helpString += "The merge.taxsummary command parameters are input and output.";
		helpString += "Example merge.taxsummary(input=small.tax.summary-large.tax.summary, output=all.tax.summary)."
			"Note: No spaces between parameter labels (i.e. output), '=' and parameters (i.e.yourOutputFileName).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MergeTaxSummaryCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
MergeTaxSummaryCommand::MergeTaxSummaryCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["taxsummary"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MergeTaxSummaryCommand, MergeTaxSummaryCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

MergeTaxSummaryCommand::MergeTaxSummaryCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	if (option == "help") { help();  abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }
	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter;

		//check to make sure all parameters are valid for command
		for (map<string, string>::iterator it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["taxsummary"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }

		string fileList = validParameter.validFile(parameters, "input", false);
		if (fileList == "not found") { LOG(INFO) << "you must enter two or more file names" << '\n';  abort = true; }
		else { Utility::split(fileList, '-', fileNames); }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		string outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }


		numInputFiles = fileNames.size();
		ifstream testFile;
		if (numInputFiles == 0) {
			LOG(INFO) << "you must enter two or more file names and you entered " + toString(fileNames.size()) + " file names" << '\n';
			abort = true;
		}
		else {
			for (int i = 0;i < numInputFiles;i++) {
				if (inputDir != "") {
					string path = File::getPath(fileNames[i]);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") { fileNames[i] = inputDir + fileNames[i]; }
				}

				int ableToOpen;
				ifstream in;
				ableToOpen = File::openInputFile(fileNames[i], in, "noerror");
				in.close();

				//if you can't open it, try default location
				if (ableToOpen == 1) {
					if (settings.getDefaultPath() != "") { //default path is set
						string tryPath = settings.getDefaultPath() + File::getSimpleName(fileNames[i]);
						LOG(INFO) << "Unable to open " + fileNames[i] + ". Trying default " + tryPath << '\n';
						ifstream in2;
						ableToOpen = File::openInputFile(tryPath, in2, "noerror");
						in2.close();
						fileNames[i] = tryPath;
					}
				}

				//if you can't open it, try output location
				if (ableToOpen == 1) {
					if (settings.getOutputDir() != "") { //default path is set
						string tryPath = settings.getOutputDir() + File::getSimpleName(fileNames[i]);
						LOG(INFO) << "Unable to open " + fileNames[i] + ". Trying output directory " + tryPath << '\n';
						ifstream in2;
						ableToOpen = File::openInputFile(tryPath, in2, "noerror");
						in2.close();
						fileNames[i] = tryPath;
					}
				}



				if (ableToOpen == 1) {
					LOG(INFO) << "Unable to open " + fileNames[i] + ". It will be disregarded." << '\n';
					//erase from file list
					fileNames.erase(fileNames.begin() + i);
					i--;
				}
			}
		}

		outputFileName = validParameter.validFile(parameters, "output", false);
		if (outputFileName == "not found") { LOG(INFO) << "you must enter an output file name" << '\n';  abort = true; }
		else if (outputDir != "") { outputFileName = outputDir + File::getSimpleName(outputFileName); }

	}

}
//**********************************************************************************************************************

int MergeTaxSummaryCommand::execute() {
	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	outputFileName = m->getFullPathName(outputFileName);
	File::remove(outputFileName);

	vector<rawTaxNode> tree;
	tree.push_back(rawTaxNode("Root"));
	tree[0].rank = "0";
	bool hasGroups = true;
	set<string> groups;

	for (int i = 0; i < fileNames.size(); i++) {

		ifstream in;
		File::openInputFile(fileNames[i], in);
		string temp = File::getline(in);
		vector<string> headers = m->splitWhiteSpace(temp);

		vector<string> thisFilesGroups;
		if (headers.size() == 5) { hasGroups = false; }
		else { for (int j = 5; j < headers.size(); j++) { groups.insert(headers[j]); thisFilesGroups.push_back(headers[j]); } }

		int level, daugterLevels, total;
		float totalFloat;
		string rankId, tax;
		map<int, int> levelToCurrentNode;
		levelToCurrentNode[0] = 0;
		while (!in.eof()) {

			if (ctrlc_pressed) { return 0; }

			in >> level >> rankId >> tax >> daugterLevels >> totalFloat; File::gobble(in);

			if ((totalFloat < 1) && (totalFloat > 0)) {
				LOG(LOGERROR) << "cannot merge tax.summary files with relative abundances.\n"; ctrlc_pressed = true; in.close(); return 0;
			}
			else {
				total = int(totalFloat);
			}

			map<string, int> groupCounts;
			if (thisFilesGroups.size() != 0) {
				for (int j = 0; j < thisFilesGroups.size(); j++) {
					int tempNum; in >> tempNum; File::gobble(in);
					groupCounts[thisFilesGroups[j]] = tempNum;
				}
			}

			if (level == 0) {}
			else {
				map<int, int>::iterator itParent = levelToCurrentNode.find(level - 1);
				int parent = 0;
				if (itParent == levelToCurrentNode.end()) { LOG(LOGERROR) << "situation I didnt expect.\n"; }
				else { parent = itParent->second; }

				levelToCurrentNode[level] = addTaxToTree(tree, level, parent, tax, total, groupCounts);
			}
		}
		in.close();
	}

	if (!hasGroups && (groups.size() != 0)) { groups.clear();  LOG(WARNING) << "not all files contain group breakdown, ignoring group counts.\n"; }

	ofstream out;
	File::openOutputFile(outputFileName, out);
	print(out, tree, groups);

	if (ctrlc_pressed) { File::remove(outputFileName); return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	LOG(INFO) << outputFileName << '\n';	outputNames.push_back(outputFileName); outputTypes["taxsummary"].push_back(outputFileName);
	LOG(INFO) << "";

	return 0;
}
/**************************************************************************************************/

int MergeTaxSummaryCommand::addTaxToTree(vector<rawTaxNode>& tree, int level, int currentNode, string taxon, int total, map<string, int> groups) {
	map<string, int>::iterator childPointer;

	childPointer = tree[currentNode].children.find(taxon);
	int nodeToIncrement = 0;

	if (childPointer != tree[currentNode].children.end()) {	//if the node already exists, increment counts
		nodeToIncrement = childPointer->second;
		tree[nodeToIncrement].total += total;

		for (map<string, int>::iterator itGroups = groups.begin(); itGroups != groups.end(); itGroups++) {
			map<string, int>::iterator it = tree[nodeToIncrement].groupCount.find(itGroups->first);
			if (it == tree[nodeToIncrement].groupCount.end()) { tree[nodeToIncrement].groupCount[itGroups->first] = itGroups->second; }
			else { it->second += itGroups->second; }
		}
	}
	else {											//otherwise, create it
		tree.push_back(rawTaxNode(taxon));
		tree[currentNode].children[taxon] = tree.size() - 1;
		tree[tree.size() - 1].parent = currentNode;
		nodeToIncrement = tree.size() - 1;
		tree[nodeToIncrement].total = total;
		tree[nodeToIncrement].level = level;
		for (map<string, int>::iterator itGroups = groups.begin(); itGroups != groups.end(); itGroups++) {
			tree[nodeToIncrement].groupCount[itGroups->first] = itGroups->second;
		}
	}

	return nodeToIncrement;
}
/**************************************************************************************************/

int MergeTaxSummaryCommand::assignRank(int index, vector<rawTaxNode>& tree) {
	map<string, int>::iterator it;
	int counter = 1;

	for (it = tree[index].children.begin();it != tree[index].children.end();it++) {
		if (ctrlc_pressed) { return 0; }
		tree[it->second].rank = tree[index].rank + '.' + toString(counter);
		counter++;

		assignRank(it->second, tree);
	}

	return 0;
}
/**************************************************************************************************/

int MergeTaxSummaryCommand::print(ofstream& out, vector<rawTaxNode>& tree, set<string> groups) {

	assignRank(0, tree);
	vector<string> mGroups;
	//print labels
	out << "taxlevel\trankID\ttaxon\tdaughterlevels\ttotal";
	for (set<string>::iterator it = groups.begin(); it != groups.end(); it++) { out << '\t' << (*it); }
	out << endl;

	for (set<string>::iterator it2 = groups.begin(); it2 != groups.end(); it2++) { tree[0].groupCount[*it2] = 0; }

	map<string, int>::iterator it;
	for (it = tree[0].children.begin();it != tree[0].children.end();it++) {
		tree[0].total += tree[it->second].total;
		for (set<string>::iterator it2 = groups.begin(); it2 != groups.end(); it2++) {
			map<string, int>::iterator itGroups = tree[it->second].groupCount.find(*it2);
			if (itGroups != tree[it->second].groupCount.end()) {
				tree[0].groupCount[*it2] += itGroups->second;
			}
		}
	}


	//print root
	out << tree[0].level << "\t" << tree[0].rank << "\t" << tree[0].name << "\t" << tree[0].children.size() << "\t" << tree[0].total;

	for (set<string>::iterator it = groups.begin(); it != groups.end(); it++) {
		map<string, int>::iterator itGroups = tree[0].groupCount.find(*it);
		int num = 0;
		if (itGroups != tree[0].groupCount.end()) { num = itGroups->second; }
		out << '\t' << num;
	}
	out << endl;

	//print rest
	print(0, out, tree, groups);

	return 0;

}
/**************************************************************************************************/
int MergeTaxSummaryCommand::print(int i, ofstream& out, vector<rawTaxNode>& tree, set<string> groups) {
	map<string, int>::iterator it;
	for (it = tree[i].children.begin();it != tree[i].children.end();it++) {

		//print root
		out << tree[it->second].level << "\t" << tree[it->second].rank << "\t" << tree[it->second].name << "\t" << tree[it->second].children.size() << "\t" << tree[it->second].total;

		for (set<string>::iterator it2 = groups.begin(); it2 != groups.end(); it2++) {
			map<string, int>::iterator itGroups = tree[it->second].groupCount.find(*it2);
			int num = 0;
			if (itGroups != tree[it->second].groupCount.end()) { num = itGroups->second; }
			out << '\t' << num;
		}
		out << endl;

		print(it->second, out, tree, groups);
	}

	return 0;
}
//**********************************************************************************************************************


