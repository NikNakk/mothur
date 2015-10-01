/*
 *  mergefilecommand.cpp
 *  Mothur
 *
 *  Created by Pat Schloss on 6/14/09.
 *  Copyright 2009 Patrick D. Schloss. All rights reserved.
 *
 */

#include "mergefilecommand.h"

 //**********************************************************************************************************************
vector<string> MergeFileCommand::setParameters() {
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
		LOG(FATAL) << e.what() << " in MergeFileCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string MergeFileCommand::getHelpString() {
	try {
		string helpString = "The merge.file command takes a list of files separated by dashes and merges them into one file.";
		helpString += "The merge.file command parameters are input and output.";
		helpString += "Example merge.file(input=small.fasta-large.fasta, output=all.fasta)."
			"Note: No spaces between parameter labels (i.e. output), '=' and parameters (i.e.yourOutputFileName).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MergeFileCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
MergeFileCommand::MergeFileCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["merge"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MergeFileCommand, MergeFileCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

MergeFileCommand::MergeFileCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	if (option == "help") {
		help();
		abort = true; calledHelp = true;
	}
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
		outputTypes["merge"] = tempOutNames;

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

				if (File::openInputFile(fileNames[i], testFile)) { abort = true; }
				testFile.close();
			}
		}

		outputFileName = validParameter.validFile(parameters, "output", false);
		if (outputFileName == "not found") { LOG(INFO) << "you must enter an output file name" << '\n';  abort = true; }
		else if (outputDir != "") { outputFileName = outputDir + File::getSimpleName(outputFileName); }
	}

}
//**********************************************************************************************************************

int MergeFileCommand::execute() {
	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	File::remove(outputFileName);
	for (int i = 0;i < numInputFiles;i++) { File::appendFiles(fileNames[i], outputFileName); }

	if (ctrlc_pressed) { File::remove(outputFileName); return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	LOG(INFO) << outputFileName << '\n';	outputNames.push_back(outputFileName); outputTypes["merge"].push_back(outputFileName);
	LOG(INFO) << "";

	return 0;
}
//**********************************************************************************************************************
