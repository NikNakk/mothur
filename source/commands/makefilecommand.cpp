//
//  makefilecommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 6/24/15.
//  Copyright (c) 2015 Schloss Lab. All rights reserved.
//

#include "makefilecommand.h"

//**********************************************************************************************************************
vector<string> MakeFileCommand::setParameters() {
	CommandParameter ptype("type", "Multiple", "fastq-gz", "fastq", "", "", "", "", false, false); parameters.push_back(ptype);
	nkParameters.add(new MultipleParameter("numcols", vector<string>{"2", "3"}, "3", false, false, true));
	nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
	nkParameters.add(new StringParameter("inputdir", "", false, false));
	nkParameters.add(new StringParameter("outputdir", "", false, false));

	vector<string> myArray;
	for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
	return myArray;
}
//**********************************************************************************************************************
string MakeFileCommand::getHelpString() {
	string helpString = "The make.file command takes a input directory and creates a file file containing the fastq or gz files in the directory.\n"
		"The make.fastq command parameters are inputdir, numcols and type.  inputdir is required.\n"
		"May create more than one file. Mothur will attempt to match paired files. \n"
		"The type parameter allows you to set the type of files to look for. Options are fastq or gz.  Default=fastq. \n"
		"The numcols parameter allows you to set number of columns you mothur to make in the file.  Default=3, meaning groupName forwardFastq reverseFastq. The groupName is made from the beginning part of the forwardFastq file. Everything up to the first '_' or if no '_' is found then the root of the forwardFastq filename.\n"
		"The make.file command should be in the following format: \n"
		"make.file(inputdir=yourInputDirectory). \n"
		"Example make.group(inputdir=fastqFiles)\n"
		"Note: No spaces between parameter labels (i.e. inputdir), '=' and parameters (i.e. yourInputDirectory).\n";
	return helpString;
}
//**********************************************************************************************************************
string MakeFileCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "file") { pattern = "[filename],[tag],file"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
MakeFileCommand::MakeFileCommand(Settings& settings) : Command(settings) {
	abort = true; calledHelp = true;
	setParameters();
	vector<string> tempOutNames;
	outputTypes["file"] = tempOutNames;
}

//**********************************************************************************************************************

MakeFileCommand::MakeFileCommand(Settings& settings, string option) : Command(settings, option) {

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
		outputTypes["file"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter
		inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = "";	LOG(LOGERROR) << "The inputdir parameter is required, aborting." << '\n'; abort = true; }
		else {
			if (m->dirCheck(inputDir)) {} // all set
			else { abort = true; }
		}

		//if the user changes the output directory command factory will send this info to us in the output parameter
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = inputDir; }


		//if the user changes the input directory command factory will send this info to us in the output parameter
		typeFile = validParameter.validFile(parameters, "type", false);
		if (typeFile == "not found") { typeFile = "fastq"; }

		if ((typeFile != "fastq") && (typeFile != "gz")) { LOG(INFO) << typeFile + " is not a valid type. Options are fastq or gz. I will use fastq." << '\n'; typeFile = "fastq"; }

		string temp = validParameter.validFile(parameters, "numcols", false);		if (temp == "not found") { temp = "3"; }
		if ((temp != "2") && (temp != "3")) { LOG(INFO) << temp + " is not a valid numcols. Options are 2 or 3. I will use 3." << '\n'; temp = "3"; }
		Utility::mothurConvert(temp, numCols);

	}
}
//**********************************************************************************************************************

int MakeFileCommand::execute() {
	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//find all .fastq files
	string tempFile = inputDir + "fileList.temp";
	string findCommand = "find \"" + inputDir.substr(0, inputDir.length() - 1) + "\" -maxdepth 1 -name \"*." + typeFile + "\"  > \"" + tempFile + "\"";
	if (app.isDebug) { LOG(INFO) << findCommand + "\n"; }
	system(findCommand.c_str());

	//read in list of files
	vector<string> fastqFiles;
	m->readAccnos(tempFile, fastqFiles, "no error");
	File::remove(tempFile);

	if (fastqFiles.size() == 0) { LOG(WARNING) << "Unable to find any " + typeFile + " files in your directory.\n"; }
	else {

		//sort into alpha order to put pairs togther if they exist
		sort(fastqFiles.begin(), fastqFiles.end());

		vector< vector<string> > paired;
		vector<string> singles;
		string lastFile = "";
		for (int i = 0; i < fastqFiles.size() - 1; i++) {

			if (app.isDebug) { LOG(DEBUG) << "File " + toString(i) + " = " + fastqFiles[i] + ".\n"; }

			if (ctrlc_pressed) { break; }

			string simpleName1 = File::getRootName(File::getSimpleName(fastqFiles[i]));
			string simpleName2 = File::getRootName(File::getSimpleName(fastqFiles[i + 1]));

			//possible pair
			if (simpleName1.length() == simpleName2.length()) {
				int numDiffs = 0;
				for (int j = 0; j < simpleName1.length(); j++) {
					if (numDiffs > 1) { break; }
					else if (simpleName1[j] != simpleName2[j]) { numDiffs++; }
				}
				if (numDiffs > 1) { singles.push_back(fastqFiles[i]); lastFile = fastqFiles[i]; }
				else { //only one diff = paired files
					int pos = simpleName1.find("R1");
					int pos2 = simpleName2.find("R2");
					if ((pos != string::npos) && (pos2 != string::npos)) {
						vector<string> temp;
						if (numCols == 3) {
							string groupName = "noGroup" + toString(i);
							int posUnderscore = fastqFiles[i].find_first_of('_');
							if (posUnderscore == string::npos) { groupName = File::getSimpleName(File::getRootName(fastqFiles[i])); }
							else { groupName = File::getSimpleName(fastqFiles[i].substr(0, posUnderscore)); }
							temp.push_back(groupName);
						}
						temp.push_back(fastqFiles[i]); temp.push_back(fastqFiles[i + 1]); lastFile = fastqFiles[i + 1];
						paired.push_back(temp);
						i++;
					}
					else {
						singles.push_back(fastqFiles[i]); lastFile = fastqFiles[i];
					}
				}
			}
			else {
				singles.push_back(fastqFiles[i]); lastFile = fastqFiles[i];
			}
		}
		if (lastFile != fastqFiles[fastqFiles.size() - 1]) { singles.push_back(fastqFiles[fastqFiles.size() - 1]); }

		if (singles.size() != 0) {
			map<string, string> variables;
			variables["[filename]"] = outputDir + "fileList.";
			variables["[tag]"] = "single";
			string filename = getOutputFileName("file", variables);
			ofstream out;
			File::openOutputFile(filename, out);
			outputNames.push_back(filename); outputTypes["file"].push_back(filename);
			settings.setCurrent("file", filename);

			for (int i = 0; i < singles.size(); i++) {
				out << singles[i] << endl;
			}
			out.close();
		}

		if (paired.size() != 0) {
			map<string, string> variables;
			variables["[filename]"] = outputDir + "fileList.";
			variables["[tag]"] = "paired";
			string filename = getOutputFileName("file", variables);
			ofstream out;
			File::openOutputFile(filename, out);
			outputNames.push_back(filename); outputTypes["file"].push_back(filename);
			settings.setCurrent("file", filename);

			for (int i = 0; i < paired.size(); i++) {
				for (int j = 0; j < paired[i].size(); j++) {
					out << paired[i][j] << '\t';
				}
				out << endl;
			}
			out.close();
		}

	}
	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
//**********************************************************************************************************************


