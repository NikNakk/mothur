/*
 *  reversecommand.cpp
 *  Mothur
 *
 *  Created by Pat Schloss on 6/6/09.
 *  Copyright 2009 Patrick D. Schloss. All rights reserved.
 *
 */

#include "reversecommand.h"
#include "sequence.hpp"
#include "qualityscores.h"

 //**********************************************************************************************************************
vector<string> ReverseSeqsCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "fastaQual", "none", "fasta", false, false, true); parameters.push_back(pfasta);
		CommandParameter pqfile("qfile", "InputTypes", "", "", "none", "fastaQual", "none", "qfile", false, false, true); parameters.push_back(pqfile);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ReverseSeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ReverseSeqsCommand::getHelpString() {
	try {
		string helpString = "The reverse.seqs command reads a fastafile and outputs a fasta file containing the reverse compliment.\n"
			"The reverse.seqs command parameters fasta or qfile are required.\n"
			"The reverse.seqs command should be in the following format: \n"
			"reverse.seqs(fasta=yourFastaFile) \n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ReverseSeqsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ReverseSeqsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],rc,[extension]"; }
	else if (type == "qfile") { pattern = "[filename],rc,[extension]"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ReverseSeqsCommand::ReverseSeqsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ReverseSeqsCommand, ReverseSeqsCommand";
		exit(1);
	}
}
//***************************************************************************************************************

ReverseSeqsCommand::ReverseSeqsCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["fasta"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;

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
		fastaFileName = validParameter.validFile(parameters, "fasta", true);
		if (fastaFileName == "not open") { abort = true; }
		else if (fastaFileName == "not found") { fastaFileName = ""; }// LOG(INFO) << "fasta is a required parameter for the reverse.seqs command." << '\n'; abort = true;  }	
		else { settings.setCurrent("fasta", fastaFileName); }

		qualFileName = validParameter.validFile(parameters, "qfile", true);
		if (qualFileName == "not open") { abort = true; }
		else if (qualFileName == "not found") { qualFileName = ""; }//LOG(INFO) << "fasta is a required parameter for the reverse.seqs command." << '\n'; abort = true;  }	
		else { settings.setCurrent("qual", qualFileName); }

		if ((fastaFileName == "") && (qualFileName == "")) {
			fastaFileName = settings.getCurrent("fasta");
			if (fastaFileName != "") { LOG(INFO) << "Using " + fastaFileName + " as input file for the fasta parameter." << '\n'; }
			else {
				qualFileName = settings.getCurrent("qual");
				if (qualFileName != "") { LOG(INFO) << "Using " + qualFileName + " as input file for the qfile parameter." << '\n'; }
				else {
					LOG(INFO) << "You have no current files for fasta or qfile, and fasta or qfile is a required parameter for the reverse.seqs command." << '\n';
					abort = true;
				}
			}
		}

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = "";
		}

	}
}
//***************************************************************************************************************


int ReverseSeqsCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		string fastaReverseFileName;

		if (fastaFileName != "") {
			ifstream inFASTA;
			File::openInputFile(fastaFileName, inFASTA);

			ofstream outFASTA;
			string tempOutputDir = outputDir;
			if (outputDir == "") { tempOutputDir += File::getPath(fastaFileName); } //if user entered a file with a path then preserve it
			map<string, string> variables;
			variables["[filename]"] = tempOutputDir + File::getRootName(File::getSimpleName(fastaFileName));
			variables["[extension]"] = m->getExtension(fastaFileName);
			fastaReverseFileName = getOutputFileName("fasta", variables);
			File::openOutputFile(fastaReverseFileName, outFASTA);

			while (!inFASTA.eof()) {
				if (ctrlc_pressed) { inFASTA.close();  outFASTA.close(); File::remove(fastaReverseFileName); return 0; }

				Sequence currSeq(inFASTA);  File::gobble(inFASTA);
				if (currSeq.getName() != "") {
					currSeq.reverseComplement();
					currSeq.printSequence(outFASTA);
				}
			}
			inFASTA.close();
			outFASTA.close();
			outputNames.push_back(fastaReverseFileName); outputTypes["fasta"].push_back(fastaReverseFileName);
		}

		string qualReverseFileName;

		if (qualFileName != "") {
			QualityScores currQual;

			ifstream inQual;
			File::openInputFile(qualFileName, inQual);

			ofstream outQual;
			string tempOutputDir = outputDir;
			if (outputDir == "") { tempOutputDir += File::getPath(qualFileName); } //if user entered a file with a path then preserve it
			map<string, string> variables;
			variables["[filename]"] = tempOutputDir + File::getRootName(File::getSimpleName(qualFileName));
			variables["[extension]"] = m->getExtension(qualFileName);
			string qualReverseFileName = getOutputFileName("qfile", variables);
			File::openOutputFile(qualReverseFileName, outQual);

			while (!inQual.eof()) {
				if (ctrlc_pressed) { inQual.close();  outQual.close(); File::remove(qualReverseFileName); return 0; }
				currQual = QualityScores(inQual);  File::gobble(inQual);
				currQual.flipQScores();
				currQual.printQScores(outQual);
			}
			inQual.close();
			outQual.close();
			outputNames.push_back(qualReverseFileName); outputTypes["qfile"].push_back(qualReverseFileName);
		}

		if (ctrlc_pressed) { File::remove(qualReverseFileName); File::remove(fastaReverseFileName); return 0; }

		//set fasta file as new current fastafile
		string current = "";
		itTypes = outputTypes.find("fasta");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("fasta", current); }
		}

		itTypes = outputTypes.find("qfile");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("qual", current); }
		}


		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0;i < outputNames.size();i++) {
			LOG(INFO) << outputNames[i];
			LOG(INFO) << "";
		}



		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ReverseSeqsCommand, execute";
		exit(1);
	}
}

//***************************************************************************************************************
