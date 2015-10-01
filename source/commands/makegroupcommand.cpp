/*
 *  makegroupcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/7/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "makegroupcommand.h"
#include "sequence.hpp"


 //**********************************************************************************************************************
vector<string> MakeGroupCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "group", false, true, true); parameters.push_back(pfasta);
		CommandParameter pgroups("groups", "String", "", "", "", "", "", "", false, false, true); parameters.push_back(pgroups);
		nkParameters.add(new StringParameter("output", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MakeGroupCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string MakeGroupCommand::getHelpString() {
	try {
		string helpString = "The make.group command reads a fasta file or series of fasta files and creates a groupfile.\n"
			"The make.group command parameters are fasta, groups and output. Fasta and group are required.\n"
			"The output parameter allows you to specify the name of groupfile created. \n"
			"The make.group command should be in the following format: \n"
			"make.group(fasta=yourFastaFiles, groups=yourGroups). \n"
			"Example make.group(fasta=seqs1.fasta-seq2.fasta-seqs3.fasta, groups=A-B-C)\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFiles).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MakeGroupCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string MakeGroupCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "group") { pattern = "[filename],groups"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
MakeGroupCommand::MakeGroupCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["group"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MakeGroupCommand, MakeGroupCommand";
		exit(1);
	}
}

//**********************************************************************************************************************

MakeGroupCommand::MakeGroupCommand(Settings& settings, string option) : Command(settings, option) {

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
		outputTypes["group"] = tempOutNames;

		//if the user changes the output directory command factory will send this info to us in the output parameter
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }

		fastaFileName = validParameter.validFile(parameters, "fasta", false);
		if (fastaFileName == "not found") { 				//if there is a current fasta file, use it
			string filename = settings.getCurrent("fasta");
			if (filename != "") { fastaFileNames.push_back(filename); LOG(INFO) << "Using " + filename + " as input file for the fasta parameter." << '\n'; }
			else { LOG(INFO) << "You have no current fastafile and the fasta parameter is required." << '\n'; abort = true; }
		}
		else {
			Utility::split(fastaFileName, '-', fastaFileNames);

			//go through files and make sure they are good, if not, then disregard them
			for (int i = 0; i < fastaFileNames.size(); i++) {

				bool ignore = false;
				if (fastaFileNames[i] == "current") {
					fastaFileNames[i] = settings.getCurrent("fasta");
					if (fastaFileNames[i] != "") {
						LOG(INFO) << "Using " + fastaFileNames[i] + " as input file for the fasta parameter where you had given current." << '\n';
						filename += File::getRootName(File::getSimpleName(fastaFileNames[i]));
					}
					else {
						LOG(INFO) << "You have no current fastafile, ignoring current." << '\n'; ignore = true;
						//erase from file list
						fastaFileNames.erase(fastaFileNames.begin() + i);
						i--;
					}
				}

				if (!ignore) {
					if (inputDir != "") {
						string path = File::getPath(fastaFileNames[i]);
						//if the user has not given a path then, add inputdir. else leave path alone.
						if (path == "") { fastaFileNames[i] = inputDir + fastaFileNames[i]; }
					}

					ifstream in;
					int ableToOpen = File::openInputFile(fastaFileNames[i], in, "noerror");

					//if you can't open it, try default location
					if (ableToOpen == 1) {
						if (settings.getDefaultPath() != "") { //default path is set
							string tryPath = settings.getDefaultPath() + File::getSimpleName(fastaFileNames[i]);
							LOG(INFO) << "Unable to open " + fastaFileNames[i] + ". Trying default " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							fastaFileNames[i] = tryPath;
						}
					}

					//if you can't open it, try default location
					if (ableToOpen == 1) {
						if (settings.getOutputDir() != "") { //default path is set
							string tryPath = settings.getOutputDir() + File::getSimpleName(fastaFileNames[i]);
							LOG(INFO) << "Unable to open " + fastaFileNames[i] + ". Trying output directory " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							fastaFileNames[i] = tryPath;
						}
					}
					in.close();

					if (ableToOpen == 1) {
						LOG(INFO) << "Unable to open " + fastaFileNames[i] + ". It will be disregarded." << '\n';
						//erase from file list
						fastaFileNames.erase(fastaFileNames.begin() + i);
						i--;
					}
					else { filename += File::getRootName(File::getSimpleName(fastaFileNames[i]));  settings.setCurrent("fasta", fastaFileNames[i]); }
				}
			}

			//prevent giantic file name
			map<string, string> variables;
			variables["[filename]"] = filename;
			if (fastaFileNames.size() > 3) { variables["[filename]"] = outputDir + "merge"; }
			filename = getOutputFileName("group", variables);

			//make sure there is at least one valid file left
			if (fastaFileNames.size() == 0) { LOG(INFO) << "no valid files." << '\n'; abort = true; }
		}

		output = validParameter.validFile(parameters, "output", false);
		if (output == "not found") { output = ""; }
		else { filename = output; }

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { LOG(INFO) << "groups is a required parameter for the make.group command." << '\n'; abort = true; }
		else { Utility::split(groups, '-', groupsNames); }

		if (groupsNames.size() != fastaFileNames.size()) { LOG(INFO) << "You do not have the same number of valid fastfile files as groups.  This could be because we could not open a fastafile." << '\n'; abort = true; }
	}
}
//**********************************************************************************************************************

int MakeGroupCommand::execute() {
	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	if (outputDir == "") { outputDir = File::getPath(fastaFileNames[0]); }

	filename = outputDir + filename;

	ofstream out;
	File::openOutputFile(filename, out);

	for (int i = 0; i < fastaFileNames.size(); i++) {

		if (ctrlc_pressed) { outputTypes.clear(); out.close(); File::remove(filename); return 0; }

		ifstream in;
		File::openInputFile(fastaFileNames[i], in);

		while (!in.eof()) {

			Sequence seq(in, "no align"); File::gobble(in);

			if (ctrlc_pressed) { outputTypes.clear();  in.close(); out.close(); File::remove(filename); return 0; }

			if (seq.getName() != "") { out << seq.getName() << '\t' << groupsNames[i] << endl; }
		}
		in.close();
	}

	out.close();

	LOG(INFO) << '\n' << "Output File Names: " + filename << '\n'; outputNames.push_back(filename); outputTypes["group"].push_back(filename);
	LOG(INFO) << "";

	//set group file as new current groupfile
	string current = "";
	itTypes = outputTypes.find("group");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("group", current); }
	}

	return 0;
}
//**********************************************************************************************************************


