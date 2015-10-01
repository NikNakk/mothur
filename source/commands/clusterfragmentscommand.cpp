/*
 *  ryanscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 9/23/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "clusterfragmentscommand.h"
#include "needlemanoverlap.hpp"

 //**********************************************************************************************************************
 //sort by unaligned
inline bool comparePriority(seqRNode first, seqRNode second) {
	bool better = false;

	if (first.length > second.length) {
		better = true;
	}
	else if (first.length == second.length) {
		if (first.numIdentical > second.numIdentical) {
			better = true;
		}
	}

	return better;
}
//**********************************************************************************************************************
vector<string> ClusterFragmentsCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "fasta-name", false, true, true); parameters.push_back(pfasta);
		nkParameters.add(new NameFileParameter(settings, "name", false, true, "NameCount"));
		CommandParameter pcount("count", "InputTypes", "", "", "namecount", "none", "none", "count", false, false, true); parameters.push_back(pcount);
		nkParameters.add(new NumberParameter("diffs", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("percent", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterFragmentsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterFragmentsCommand::getHelpString() {
	try {
		string helpString = "The cluster.fragments command groups sequences that are part of a larger sequence.\n"
			"The cluster.fragments command outputs a new fasta and name or count file.\n"
			"The cluster.fragments command parameters are fasta, name, count, diffs and percent. The fasta parameter is required, unless you have a valid current file. \n"
			"The names parameter allows you to give a list of seqs that are identical. This file is 2 columns, first column is name or representative sequence, second column is a list of its identical sequences separated by commas.\n"
			"The diffs parameter allows you to set the number of differences allowed, default=0. \n"
			"The percent parameter allows you to set percentage of differences allowed, default=0. percent=2 means if the number of difference is less than or equal to two percent of the length of the fragment, then cluster.\n"
			"You may use diffs and percent at the same time to say something like: If the number or differences is greater than 1 or more than 2% of the fragment length, don't merge. \n"
			"The cluster.fragments command should be in the following format: \n"
			"cluster.fragments(fasta=yourFastaFile, names=yourNamesFile) \n"
			"Example cluster.fragments(fasta=amazon.fasta).\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterFragmentsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterFragmentsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],fragclust.fasta"; }
	else if (type == "name") { pattern = "[filename],fragclust.names"; }
	else if (type == "count") { pattern = "[filename],fragclust.count_table"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ClusterFragmentsCommand::ClusterFragmentsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClusterFragmentsCommand, ClusterFragmentsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
ClusterFragmentsCommand::ClusterFragmentsCommand(Settings& settings, string option) : Command(settings, option) {
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
		else if (fastafile == "not open") { fastafile = ""; abort = true; }
		else { settings.setCurrent("fasta", fastafile); }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(fastafile); }

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not found") { namefile = ""; }
		else if (namefile == "not open") { namefile = ""; abort = true; }
		else { readNameFile(); settings.setCurrent("name", namefile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { abort = true; countfile = ""; }
		else if (countfile == "not found") { countfile = ""; }
		else { ct.readTable(countfile, true, false); settings.setCurrent("counttable", countfile); }

		if ((countfile != "") && (namefile != "")) { LOG(INFO) << "When executing a cluster.fragments command you must enter ONLY ONE of the following: count or name." << '\n'; abort = true; }

		string temp;
		temp = validParameter.validFile(parameters, "diffs", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, diffs);

		temp = validParameter.validFile(parameters, "percent", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, percent);

		if (countfile == "") {
			if (namefile == "") {
				vector<string> files; files.push_back(fastafile);
				OptionParser::getNameFile(files);
			}
		}

	}

}
//**********************************************************************************************************************
int ClusterFragmentsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	int start = time(NULL);

	//reads fasta file and return number of seqs
	int numSeqs = readFASTA(); //fills alignSeqs and makes all seqs active

	if (ctrlc_pressed) { return 0; }

	if (numSeqs == 0) { LOG(INFO) << "Error reading fasta file...please correct." << '\n'; return 0; }

	//sort seqs by length of unaligned sequence
	sort(alignSeqs.begin(), alignSeqs.end(), comparePriority);

	int count = 0;

	//think about running through twice...
	for (int i = 0; i < numSeqs; i++) {

		if (alignSeqs[i].active) {  //this sequence has not been merged yet

			string iBases = alignSeqs[i].seq.getUnaligned();

			//try to merge it with all smaller seqs
			for (int j = i + 1; j < numSeqs; j++) {

				if (ctrlc_pressed) { return 0; }

				if (alignSeqs[j].active) {  //this sequence has not been merged yet

					string jBases = alignSeqs[j].seq.getUnaligned();

					if (isFragment(iBases, jBases)) {
						if (countfile != "") {
							ct.mergeCounts(alignSeqs[i].names, alignSeqs[j].names);
						}
						else {
							//merge
							alignSeqs[i].names += ',' + alignSeqs[j].names;
							alignSeqs[i].numIdentical += alignSeqs[j].numIdentical;
						}
						alignSeqs[j].active = 0;
						alignSeqs[j].numIdentical = 0;
						count++;
					}
				}//end if j active
			}//end if i != j

			//remove from active list 
			alignSeqs[i].active = 0;

		}//end if active i
		if (i % 100 == 0) { LOG(SCREENONLY) << toString(i) + "\t" + toString(numSeqs - count) + "\t" + toString(count) + "\n"; }
	}

	if (numSeqs % 100 != 0) { LOG(SCREENONLY) << toString(numSeqs) + "\t" + toString(numSeqs - count) + "\t" + toString(count) + "\n"; }


	string fileroot = outputDir + File::getRootName(File::getSimpleName(fastafile));
	map<string, string> variables;
	variables["[filename]"] = fileroot;
	string newFastaFile = getOutputFileName("fasta", variables);
	string newNamesFile = getOutputFileName("name", variables);
	if (countfile != "") { newNamesFile = getOutputFileName("count", variables); }

	if (ctrlc_pressed) { return 0; }

	LOG(INFO) << '\n' << "Total number of sequences before cluster.fragments was " + toString(alignSeqs.size()) + "." << '\n';
	LOG(INFO) << "cluster.fragments removed " + toString(count) + " sequences." << '\n' << '\n';

	printData(newFastaFile, newNamesFile);

	LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to cluster " + toString(numSeqs) + " sequences." << '\n';

	if (ctrlc_pressed) { File::remove(newFastaFile); File::remove(newNamesFile); return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	LOG(INFO) << newFastaFile << '\n';
	LOG(INFO) << newNamesFile << '\n';
	outputNames.push_back(newFastaFile);  outputNames.push_back(newNamesFile); outputTypes["fasta"].push_back(newFastaFile); outputTypes["name"].push_back(newNamesFile);
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
//***************************************************************************************************************
bool ClusterFragmentsCommand::isFragment(string seq1, string seq2) {
	bool fragment = false;

	//exact match
	int pos = seq1.find(seq2);
	if (pos != string::npos) { return true; }
	//no match, no diffs wanted
	else if ((diffs == 0) && (percent == 0)) { return false; }
	else { //try aligning and see if you can find it

		//find number of acceptable differences for this sequence fragment
		int totalDiffs = 0;
		if (diffs == 0) { //you didnt set diffs you want a percentage
			totalDiffs = floor((seq2.length() * (percent / 100.0)));
		}
		else if (percent == 0) { //you didn't set percent you want diffs
			totalDiffs = diffs;
		}
		else if ((percent != 0) && (diffs != 0)) { //you want both, set total diffs to smaller of 2
			totalDiffs = diffs;
			int percentDiff = floor((seq2.length() * (percent / 100.0)));
			if (percentDiff < totalDiffs) { totalDiffs = percentDiff; }
		}

		Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (seq1.length() + totalDiffs + 1));

		//use needleman to align 
		alignment->align(seq2, seq1);
		string tempSeq2 = alignment->getSeqAAln();
		string temp = alignment->getSeqBAln();

		delete alignment;

		//chop gap ends
		int startPos = 0;
		int endPos = tempSeq2.length() - 1;
		for (int i = 0; i < tempSeq2.length(); i++) { if (isalpha(tempSeq2[i])) { startPos = i; break; } }
		for (int i = tempSeq2.length() - 1; i >= 0; i--) { if (isalpha(tempSeq2[i])) { endPos = i; break; } }

		//count number of diffs
		int numDiffs = 0;
		for (int i = startPos; i <= endPos; i++) {
			if (tempSeq2[i] != temp[i]) { numDiffs++; }
		}

		if (numDiffs <= totalDiffs) { fragment = true; }

	}

	return fragment;

}
/**************************************************************************************************/
int ClusterFragmentsCommand::readFASTA() {

	ifstream inFasta;
	File::openInputFile(fastafile, inFasta);

	while (!inFasta.eof()) {

		if (ctrlc_pressed) { inFasta.close(); return 0; }

		Sequence seq(inFasta);  File::gobble(inFasta);

		if (seq.getName() != "") {  //can get "" if commented line is at end of fasta file
			if (namefile != "") {
				itSize = sizes.find(seq.getName());

				if (itSize == sizes.end()) { LOG(INFO) << seq.getName() + " is not in your names file, please correct." << '\n'; exit(1); }
				else {
					seqRNode tempNode(itSize->second, seq, names[seq.getName()], seq.getUnaligned().length());
					alignSeqs.push_back(tempNode);
				}
			}
			else if (countfile != "") {
				seqRNode tempNode(ct.getNumSeqs(seq.getName()), seq, seq.getName(), seq.getUnaligned().length());
				alignSeqs.push_back(tempNode);
			}
			else { //no names file, you are identical to yourself 
				seqRNode tempNode(1, seq, seq.getName(), seq.getUnaligned().length());
				alignSeqs.push_back(tempNode);
			}
		}
	}

	inFasta.close();
	return alignSeqs.size();
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in ClusterFragmentsCommand, readFASTA";
	exit(1);
}
}
/**************************************************************************************************/
void ClusterFragmentsCommand::printData(string newfasta, string newname) {
	try {
		ofstream outFasta;
		ofstream outNames;

		File::openOutputFile(newfasta, outFasta);
		if (countfile == "") { File::openOutputFile(newname, outNames); }

		for (int i = 0; i < alignSeqs.size(); i++) {
			if (alignSeqs[i].numIdentical != 0) {
				alignSeqs[i].seq.printSequence(outFasta);
				if (countfile == "") { outNames << alignSeqs[i].seq.getName() << '\t' << alignSeqs[i].names << endl; }
			}
		}

		outFasta.close();
		if (countfile == "") { outNames.close(); }
		else { ct.printTable(newname); }
	}
	/**************************************************************************************************/

	void ClusterFragmentsCommand::readNameFile() {
		ifstream in;
		File::openInputFile(namefile, in);
		string firstCol, secondCol;

		while (!in.eof()) {
			in >> firstCol >> secondCol; File::gobble(in);
			names[firstCol] = secondCol;
			int size = 1;

			for (int i = 0;i < secondCol.size();i++) {
				if (secondCol[i] == ',') { size++; }
			}
			sizes[firstCol] = size;
		}
		in.close();
	}
	/**************************************************************************************************/

