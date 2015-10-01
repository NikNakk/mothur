/*
 *  listseqscommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 7/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "listseqscommand.h"
#include "sequence.hpp"
#include "listvector.hpp"
#include "counttable.h"


 //**********************************************************************************************************************
vector<string> ListSeqsCommand::setParameters() {
	try {
		CommandParameter pfastq("fastq", "InputTypes", "", "", "FNGLT", "FNGLT", "none", "accnos", false, false, true); parameters.push_back(pfastq);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "FNGLT", "FNGLT", "none", "accnos", false, false, true); parameters.push_back(pfasta);
		CommandParameter pname("name", "InputTypes", "", "", "FNGLT", "FNGLT", "none", "accnos", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "FNGLT", "FNGLT", "none", "accnos", false, false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "FNGLT", "FNGLT", "none", "accnos", false, false, true); parameters.push_back(pgroup);
		CommandParameter plist("list", "InputTypes", "", "", "FNGLT", "FNGLT", "none", "accnos", false, false, true); parameters.push_back(plist);
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "FNGLT", "FNGLT", "none", "accnos", false, false, true); parameters.push_back(ptaxonomy);
		CommandParameter palignreport("alignreport", "InputTypes", "", "", "FNGLT", "FNGLT", "none", "accnos", false, false); parameters.push_back(palignreport);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ListSeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ListSeqsCommand::getHelpString() {
	try {
		string helpString = "The list.seqs command reads a fasta, name, group, count, list, taxonomy, fastq or alignreport file and outputs a .accnos file containing sequence names.\n"
			"The list.seqs command parameters are fasta, name, group, count, list, taxonomy, fastq and alignreport.  You must provide one of these parameters.\n"
			"The list.seqs command should be in the following format: list.seqs(fasta=yourFasta).\n"
			"Example list.seqs(fasta=amazon.fasta).\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ListSeqsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ListSeqsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "accnos") { pattern = "[filename],accnos"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ListSeqsCommand::ListSeqsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["accnos"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ListSeqsCommand, ListSeqsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

ListSeqsCommand::ListSeqsCommand(Settings& settings, string option) : Command(settings, option) {
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
		for (map<string, string>::iterator it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["accnos"] = tempOutNames;

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("alignreport");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["alignreport"] = inputDir + it->second; }
			}

			it = parameters.find("fasta");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fasta"] = inputDir + it->second; }
			}

			it = parameters.find("list");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["list"] = inputDir + it->second; }
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

			it = parameters.find("taxonomy");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["taxonomy"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}

			it = parameters.find("fastq");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fastq"] = inputDir + it->second; }
			}
		}

		//check for required parameters
		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not open") { abort = true; }
		else if (fastafile == "not found") { fastafile = ""; }
		else { settings.setCurrent("fasta", fastafile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { settings.setCurrent("name", namefile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		alignfile = validParameter.validFile(parameters, "alignreport", true);
		if (alignfile == "not open") { abort = true; }
		else if (alignfile == "not found") { alignfile = ""; }

		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { settings.setCurrent("list", listfile); }

		taxfile = validParameter.validFile(parameters, "taxonomy", true);
		if (taxfile == "not open") { abort = true; }
		else if (taxfile == "not found") { taxfile = ""; }
		else { settings.setCurrent("taxonomy", taxfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		fastqfile = validParameter.validFile(parameters, "fastq", true);
		if (fastqfile == "not open") { abort = true; }
		else if (fastqfile == "not found") { fastqfile = ""; }

		if ((fastqfile == "") && (countfile == "") && (fastafile == "") && (namefile == "") && (listfile == "") && (groupfile == "") && (alignfile == "") && (taxfile == "")) { LOG(INFO) << "You must provide a file." << '\n'; abort = true; }

		int okay = 1;
		if (outputDir != "") { okay++; }
		if (inputDir != "") { okay++; }

		if (parameters.size() > okay) { LOG(INFO) << "You may only enter one file." << '\n'; abort = true; }
	}

}
//**********************************************************************************************************************

int ListSeqsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//read functions fill names vector
	if (fastafile != "") { inputFileName = fastafile;	readFasta(); }
	else if (fastqfile != "") { inputFileName = fastqfile;	readFastq(); }
	else if (namefile != "") { inputFileName = namefile;	readName(); }
	else if (groupfile != "") { inputFileName = groupfile;	readGroup(); }
	else if (alignfile != "") { inputFileName = alignfile;	readAlign(); }
	else if (listfile != "") { inputFileName = listfile;	readList(); }
	else if (taxfile != "") { inputFileName = taxfile;	readTax(); }
	else if (countfile != "") { inputFileName = countfile;	readCount(); }

	if (ctrlc_pressed) { outputTypes.clear();  return 0; }

	//sort in alphabetical order
	sort(names.begin(), names.end());

	if (outputDir == "") { outputDir += File::getPath(inputFileName); }

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputFileName));
	string outputFileName = getOutputFileName("accnos", variables);

	ofstream out;
	File::openOutputFile(outputFileName, out);
	outputNames.push_back(outputFileName); outputTypes["accnos"].push_back(outputFileName);

	//output to .accnos file
	for (int i = 0; i < names.size(); i++) {

		if (ctrlc_pressed) { outputTypes.clear(); out.close(); File::remove(outputFileName); return 0; }

		out << names[i] << endl;
	}
	out.close();

	if (ctrlc_pressed) { outputTypes.clear();  File::remove(outputFileName); return 0; }

	settings.setCurrent("accnos", outputFileName);

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	LOG(INFO) << outputFileName << '\n';
	LOG(INFO) << "";

	//set accnos file as new current accnosfile
	string current = "";
	itTypes = outputTypes.find("accnos");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("accnos", current); }
	}

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in ListSeqsCommand, execute";
	exit(1);
}
}
//**********************************************************************************************************************
int ListSeqsCommand::readFastq() {
	try {

		ifstream in;
		File::openInputFile(fastqfile, in);
		string name;

		//ofstream out;
		//string newFastaName = outputDir + File::getRootName(File::getSimpleName(fastafile)) + "numsAdded.fasta";
		//File::openOutputFile(newFastaName, out);
		int count = 1;
		//string lastName = "";

		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); return 0; }

			//read sequence name
			string name = File::getline(in); File::gobble(in);

			if (name[0] == '@') {
				vector<string> splits = m->splitWhiteSpace(name);
				name = splits[0];
				name = name.substr(1);
				m->checkName(name);
				names.push_back(name);
				//get rest of lines
				name = File::getline(in); File::gobble(in);
				name = File::getline(in); File::gobble(in);
				name = File::getline(in); File::gobble(in);
			}

			File::gobble(in);
			if (app.isDebug) { count++; cout << "[DEBUG]: count = " + toString(count) + ", name = " + name + "\n"; }
		}
		in.close();
		//out.close();

		return 0;

	}

	//**********************************************************************************************************************
	int ListSeqsCommand::readFasta() {

		ifstream in;
		File::openInputFile(fastafile, in);
		string name;

		//ofstream out;
		//string newFastaName = outputDir + File::getRootName(File::getSimpleName(fastafile)) + "numsAdded.fasta";
		//File::openOutputFile(newFastaName, out);
		int count = 1;
		//string lastName = "";

		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); return 0; }

			Sequence currSeq(in);
			name = currSeq.getName();

			if (name != "") { names.push_back(name); }

			File::gobble(in);
			if (app.isDebug) { count++; cout << "[DEBUG]: count = " + toString(count) + ", name = " + currSeq.getName() + "\n"; }
		}
		in.close();
		//out.close();

		return 0;

	}
	//**********************************************************************************************************************
	int ListSeqsCommand::readList() {
		ifstream in;
		File::openInputFile(listfile, in);

		if (!in.eof()) {
			//read in list vector
			ListVector list(in);

			//for each bin
			for (int i = 0; i < list.getNumBins(); i++) {
				string binnames = list.get(i);

				if (ctrlc_pressed) { in.close(); return 0; }

				m->splitAtComma(binnames, names);
			}
		}
		in.close();

		return 0;

	}

	//**********************************************************************************************************************
	int ListSeqsCommand::readName() {

		ifstream in;
		File::openInputFile(namefile, in);
		string name, firstCol, secondCol;

		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); return 0; }

			in >> firstCol;	File::gobble(in);
			in >> secondCol;

			//parse second column saving each name
			m->splitAtComma(secondCol, names);

			File::gobble(in);
		}
		in.close();
		return 0;

	}

	//**********************************************************************************************************************
	int ListSeqsCommand::readGroup() {

		ifstream in;
		File::openInputFile(groupfile, in);
		string name, group;

		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); return 0; }

			in >> name;	File::gobble(in);			//read from first column
			in >> group;			//read from second column

			names.push_back(name);

			File::gobble(in);
		}
		in.close();
		return 0;

	}
	//**********************************************************************************************************************
	int ListSeqsCommand::readCount() {
		CountTable ct;
		ct.readTable(countfile, false, false);

		if (ctrlc_pressed) { return 0; }

		names = ct.getNamesOfSeqs();

		return 0;

	}
	//**********************************************************************************************************************
	//alignreport file has a column header line then all other lines contain 16 columns.  we just want the first column since that contains the name
	int ListSeqsCommand::readAlign() {

		ifstream in;
		File::openInputFile(alignfile, in);
		string name, junk;

		//read column headers
		for (int i = 0; i < 16; i++) {
			if (!in.eof()) { in >> junk; }
			else { break; }
		}
		//File::getline(in);

		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); return 0; }

			in >> name;				//read from first column
			//File::getline(in);
			//read rest
			for (int i = 0; i < 15; i++) {
				if (!in.eof()) { in >> junk; }
				else { break; }
			}

			names.push_back(name);

			File::gobble(in);
		}
		in.close();

		return 0;


	}
	//**********************************************************************************************************************
	int ListSeqsCommand::readTax() {

		ifstream in;
		File::openInputFile(taxfile, in);
		string name, firstCol, secondCol;

		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); return 0; }

			in >> firstCol;
			in >> secondCol;

			names.push_back(firstCol);

			File::gobble(in);

		}
		in.close();

		return 0;

	}
	//**********************************************************************************************************************
