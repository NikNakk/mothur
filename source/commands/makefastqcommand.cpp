/*
 *  makefastqcommand.cpp
 *  mothur
 *
 *  Created by westcott on 2/14/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "makefastqcommand.h"
#include "sequence.hpp"
#include "qualityscores.h"

 //**********************************************************************************************************************
vector<string> MakeFastQCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "fastq", false, true, true); parameters.push_back(pfasta);
		CommandParameter pqfile("qfile", "InputTypes", "", "", "none", "none", "none", "fastq", false, true, true); parameters.push_back(pqfile);
		CommandParameter pformat("format", "Multiple", "sanger-illumina-illumina1.8+", "sanger", "", "", "", "", false, false); parameters.push_back(pformat);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MakeFastQCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string MakeFastQCommand::getHelpString() {
	try {
		string helpString = "The make.fastq command reads a fasta and quality file and creates a fastq file.\n"
			"The make.fastq command parameters are fasta, qfile and format.  fasta and qfile are required.\n"
			"The format parameter is used to indicate whether your sequences are sanger, illumina1.8+ or illumina, default=sanger.\n"
			"The make.fastq command should be in the following format: make.fastq(qfile=yourQualityFile, fasta=yourFasta).\n"
			"Example make.fastq(fasta=amazon.fasta, qfile=amazon.qual).\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MakeFastQCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string MakeFastQCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fastq") { pattern = "[filename],fastq"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
MakeFastQCommand::MakeFastQCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fastq"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MakeFastQCommand, MakeFastQCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
MakeFastQCommand::MakeFastQCommand(Settings& settings, string option) : Command(settings, option) {
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
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["fastq"] = tempOutNames;


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

			it = parameters.find("qfile");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["qfile"] = inputDir + it->second; }
			}
		}


		//check for required parameters
		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not open") { abort = true; fastafile = ""; }
		else if (fastafile == "not found") {
			fastafile = settings.getCurrent("fasta");
			if (fastafile != "") { LOG(INFO) << "Using " + fastafile + " as input file for the fasta parameter." << '\n'; }
			else { LOG(INFO) << "You have no current fastafile and the fasta parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("fasta", fastafile); }

		qualfile = validParameter.validFile(parameters, "qfile", true);
		if (qualfile == "not open") { abort = true; qualfile = ""; }
		else if (qualfile == "not found") {
			qualfile = settings.getCurrent("qual");
			if (qualfile != "") { LOG(INFO) << "Using " + qualfile + " as input file for the qfile parameter." << '\n'; }
			else { LOG(INFO) << "You have no current qualfile and the qfile parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("qual", qualfile); }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(fastafile); }

		format = validParameter.validFile(parameters, "format", false);		if (format == "not found") { format = "sanger"; }

		if ((format != "sanger") && (format != "illumina") && (format != "illumina1.8+")) {
			LOG(INFO) << format + " is not a valid format. Your format choices are sanger, illumina1.8+ and illumina, aborting." << '\n';
			abort = true;
		}


	}

}
//**********************************************************************************************************************

int MakeFastQCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastafile));
	string outputFile = getOutputFileName("fastq", variables);
	outputNames.push_back(outputFile); outputTypes["fastq"].push_back(outputFile);

	ofstream out;
	File::openOutputFile(outputFile, out);

	ifstream qFile;
	File::openInputFile(qualfile, qFile);

	ifstream fFile;
	File::openInputFile(fastafile, fFile);

	while (!fFile.eof() && !qFile.eof()) {

		if (ctrlc_pressed) { break; }

		Sequence currSeq(fFile); File::gobble(fFile);
		QualityScores currQual(qFile);  File::gobble(qFile);

		if (currSeq.getName() != currQual.getName()) { LOG(LOGERROR) << "mismatch between fasta and quality files. Found " + currSeq.getName() + " in fasta file and " + currQual.getName() + " in quality file." << '\n'; ctrlc_pressed = true; }
		else {
			//print sequence
			out << '@' << currSeq.getName() << endl << currSeq.getAligned() << endl;

			string qualityString = convertQual(currQual.getQualityScores());

			//print quality info
			out << '+' << currQual.getName() << endl << qualityString << endl;
		}

	}

	fFile.close();
	qFile.close();
	out.close();

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;

}
//**********************************************************************************************************************
string MakeFastQCommand::convertQual(vector<int> qual) {
	string qualScores;

	for (int i = 0; i < qual.size(); i++) {
		int controlChar = int('!');
		if (format == "illumina") { controlChar = int('@'); }

		int temp = qual[i] + controlChar;
		char qualChar = (char)temp;

		qualScores += qualChar;
	}

	return qualScores;
}
//**********************************************************************************************************************





