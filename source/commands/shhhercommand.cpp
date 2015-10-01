/*
 *  shhher.cpp
 *  Mothur
 *
 *  Created by Pat Schloss on 12/27/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "shhhercommand.h"

 //**********************************************************************************************************************
vector<string> ShhherCommand::setParameters() {
	try {
		CommandParameter pflow("flow", "InputTypes", "", "", "none", "fileflow", "none", "fasta-name-group-counts-qfile", false, false, true); parameters.push_back(pflow);
		CommandParameter pfile("file", "InputTypes", "", "", "none", "fileflow", "none", "fasta-name-group-counts-qfile", false, false, true); parameters.push_back(pfile);
		CommandParameter plookup("lookup", "InputTypes", "", "", "none", "none", "none", "", false, false, true); parameters.push_back(plookup);
		nkParameters.add(new NumberParameter("cutoff", -INFINITY, INFINITY, 0.01, false, false));
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new NumberParameter("maxiter", -INFINITY, INFINITY, 1000, false, false));
		nkParameters.add(new NumberParameter("large", -INFINITY, INFINITY, -1, false, false));
		nkParameters.add(new NumberParameter("sigma", -INFINITY, INFINITY, 60, false, false));
		nkParameters.add(new NumberParameter("mindelta", -INFINITY, INFINITY, 0.000001, false, false));
		nkParameters.add(new MultipleParameter("order", vector<string>{"A", "B", "I"}, "A", false, false, true));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ShhherCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ShhherCommand::getHelpString() {
	try {
		string helpString = "The shhh.flows command reads a file containing flowgrams and creates a file of corrected sequences.\n"
			"The shhh.flows command parameters are flow, file, lookup, cutoff, processors, large, maxiter, sigma, mindelta and order.\n"
			"The flow parameter is used to input your flow file.\n"
			"The file parameter is used to input the *flow.files file created by trim.flows.\n"
			"The lookup parameter is used specify the lookup file you would like to use. http://www.mothur.org/wiki/Lookup_files.\n"
			"The order parameter options are A, B or I.  Default=A. A = TACG and B = TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC and I = TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC.\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ShhherCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ShhherCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],shhh.fasta"; }
	else if (type == "name") { pattern = "[filename],shhh.names"; }
	else if (type == "group") { pattern = "[filename],shhh.groups"; }
	else if (type == "counts") { pattern = "[filename],shhh.counts"; }
	else if (type == "qfile") { pattern = "[filename],shhh.qual"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************

ShhherCommand::ShhherCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["counts"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ShhherCommand, ShhherCommand";
		exit(1);
	}
}

//**********************************************************************************************************************

ShhherCommand::ShhherCommand(Settings& settings, string option) : Command(settings, option) {

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
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["counts"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;


		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("flow");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["flow"] = inputDir + it->second; }
			}

			it = parameters.find("lookup");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["lookup"] = inputDir + it->second; }
			}

			it = parameters.find("file");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["file"] = inputDir + it->second; }
			}
		}

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//check for required parameters
		flowFileName = validParameter.validFile(parameters, "flow", true);
		flowFilesFileName = validParameter.validFile(parameters, "file", true);
		if (flowFileName == "not found" && flowFilesFileName == "not found") {
			LOG(INFO) << "values for either flow or file must be provided for the shhh.flows command.";
			LOG(INFO) << "";
			abort = true;
		}
		else if (flowFileName == "not open" || flowFilesFileName == "not open") { abort = true; }

		if (flowFileName != "not found") {
			compositeFASTAFileName = "";
			compositeNamesFileName = "";
		}
		else {
			ofstream temp;

			string thisoutputDir = outputDir;
			if (outputDir == "") { thisoutputDir = File::getPath(flowFilesFileName); } //if user entered a file with a path then preserve it

			//we want to rip off .files, and also .flow if its there
			string fileroot = File::getRootName(File::getSimpleName(flowFilesFileName));
			if (fileroot[fileroot.length() - 1] == '.') { fileroot = fileroot.substr(0, fileroot.length() - 1); } //rip off dot
			string extension = m->getExtension(fileroot);
			if (extension == ".flow") { fileroot = File::getRootName(fileroot); }
			else { fileroot += "."; } //add back if needed

			compositeFASTAFileName = thisoutputDir + fileroot + "shhh.fasta";
			File::openOutputFile(compositeFASTAFileName, temp);
			temp.close();

			compositeNamesFileName = thisoutputDir + fileroot + "shhh.names";
			File::openOutputFile(compositeNamesFileName, temp);
			temp.close();
		}

		if (flowFilesFileName != "not found") {
			string fName;

			ifstream flowFilesFile;
			File::openInputFile(flowFilesFileName, flowFilesFile);
			while (flowFilesFile) {
				fName = File::getline(flowFilesFile);

				//test if file is valid
				ifstream in;
				int ableToOpen = File::openInputFile(fName, in, "noerror");
				in.close();
				if (ableToOpen == 1) {
					if (inputDir != "") { //default path is set
						string tryPath = inputDir + fName;
						LOG(INFO) << "Unable to open " + fName + ". Trying input directory " + tryPath << '\n';
						ifstream in2;
						ableToOpen = File::openInputFile(tryPath, in2, "noerror");
						in2.close();
						fName = tryPath;
					}
				}

				if (ableToOpen == 1) {
					if (settings.getDefaultPath() != "") { //default path is set
						string tryPath = settings.getDefaultPath() + File::getSimpleName(fName);
						LOG(INFO) << "Unable to open " + fName + ". Trying default " + tryPath << '\n';
						ifstream in2;
						ableToOpen = File::openInputFile(tryPath, in2, "noerror");
						in2.close();
						fName = tryPath;
					}
				}

				//if you can't open it its not in current working directory or inputDir, try mothur excutable location
				if (ableToOpen == 1) {
					string exepath = m->argv;
					string tempPath = exepath;
					for (int i = 0; i < exepath.length(); i++) { tempPath[i] = tolower(exepath[i]); }
					exepath = exepath.substr(0, (tempPath.find_last_of('m')));

					string tryPath = m->getFullPathName(exepath) + File::getSimpleName(fName);
					LOG(INFO) << "Unable to open " + fName + ". Trying mothur's executable location " + tryPath << '\n';
					ifstream in2;
					ableToOpen = File::openInputFile(tryPath, in2, "noerror");
					in2.close();
					fName = tryPath;
				}

				if (ableToOpen == 1) { LOG(INFO) << "Unable to open " + fName + ". Disregarding. " << '\n'; }
				else { flowFileVector.push_back(fName); }
				File::gobble(flowFilesFile);
			}
			flowFilesFile.close();
			if (flowFileVector.size() == 0) { LOG(LOGERROR) << "no valid files." << '\n'; abort = true; }
		}
		else {
			if (outputDir == "") { outputDir = File::getPath(flowFileName); }
			flowFileVector.push_back(flowFileName);
		}

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		string temp;
		temp = validParameter.validFile(parameters, "lookup", true);
		if (temp == "not found") {
			string path = m->argv;
			string tempPath = path;
			for (int i = 0; i < path.length(); i++) { tempPath[i] = tolower(path[i]); }
			path = path.substr(0, (tempPath.find_last_of('m')));

#if defined (UNIX)
			path += "lookupFiles/";
#else
			path += "lookupFiles\\";
#endif
			lookupFileName = m->getFullPathName(path) + "LookUp_Titanium.pat";

			int ableToOpen;
			ifstream in;
			ableToOpen = File::openInputFile(lookupFileName, in, "noerror");
			in.close();

			//if you can't open it, try input location
			if (ableToOpen == 1) {
				if (inputDir != "") { //default path is set
					string tryPath = inputDir + File::getSimpleName(lookupFileName);
					LOG(INFO) << "Unable to open " + lookupFileName + ". Trying input directory " + tryPath << '\n';
					ifstream in2;
					ableToOpen = File::openInputFile(tryPath, in2, "noerror");
					in2.close();
					lookupFileName = tryPath;
				}
			}

			//if you can't open it, try default location
			if (ableToOpen == 1) {
				if (settings.getDefaultPath() != "") { //default path is set
					string tryPath = settings.getDefaultPath() + File::getSimpleName(lookupFileName);
					LOG(INFO) << "Unable to open " + lookupFileName + ". Trying default " + tryPath << '\n';
					ifstream in2;
					ableToOpen = File::openInputFile(tryPath, in2, "noerror");
					in2.close();
					lookupFileName = tryPath;
				}
			}

			//if you can't open it its not in current working directory or inputDir, try mothur excutable location
			if (ableToOpen == 1) {
				string exepath = m->argv;
				string tempPath = exepath;
				for (int i = 0; i < exepath.length(); i++) { tempPath[i] = tolower(exepath[i]); }
				exepath = exepath.substr(0, (tempPath.find_last_of('m')));

				string tryPath = m->getFullPathName(exepath) + File::getSimpleName(lookupFileName);
				LOG(INFO) << "Unable to open " + lookupFileName + ". Trying mothur's executable location " + tryPath << '\n';
				ifstream in2;
				ableToOpen = File::openInputFile(tryPath, in2, "noerror");
				in2.close();
				lookupFileName = tryPath;
			}

			if (ableToOpen == 1) { LOG(INFO) << "Unable to open " + lookupFileName + "." << '\n'; abort = true; }
		}
		else if (temp == "not open") {

			lookupFileName = validParameter.validFile(parameters, "lookup", false);

			//if you can't open it its not inputDir, try mothur excutable location
			string exepath = m->argv;
			string tempPath = exepath;
			for (int i = 0; i < exepath.length(); i++) { tempPath[i] = tolower(exepath[i]); }
			exepath = exepath.substr(0, (tempPath.find_last_of('m')));

			string tryPath = m->getFullPathName(exepath) + File::getSimpleName(lookupFileName);
			LOG(INFO) << "Unable to open " + lookupFileName + ". Trying mothur's executable location " + tryPath << '\n';
			ifstream in2;
			int ableToOpen = File::openInputFile(tryPath, in2, "noerror");
			in2.close();
			lookupFileName = tryPath;

			if (ableToOpen == 1) { LOG(INFO) << "Unable to open " + lookupFileName + "." << '\n'; abort = true; }
		}
		else { lookupFileName = temp; }

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "cutoff", false);	if (temp == "not found") { temp = "0.01"; }
		Utility::mothurConvert(temp, cutoff);

		temp = validParameter.validFile(parameters, "mindelta", false);	if (temp == "not found") { temp = "0.000001"; }
		Utility::mothurConvert(temp, minDelta);

		temp = validParameter.validFile(parameters, "maxiter", false);	if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, maxIters);

		temp = validParameter.validFile(parameters, "large", false);	if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, largeSize);
		if (largeSize != 0) { large = true; }
		else { large = false; }
		if (largeSize < 0) { LOG(INFO) << "The value of the large cannot be negative.\n"; }

		temp = validParameter.validFile(parameters, "sigma", false);if (temp == "not found") { temp = "60"; }
		Utility::mothurConvert(temp, sigma);

		temp = validParameter.validFile(parameters, "order", false);  if (temp == "not found") { temp = "A"; }
		if (temp.length() > 1) {
			LOG(LOGERROR) << "" + temp + " is not a valid option for order. order options are A, B, or I. A = TACG, B = TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC, and I = TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC.\n";  abort = true;
		}
		else {
			if (toupper(temp[0]) == 'A') { flowOrder = "TACG"; }
			else if (toupper(temp[0]) == 'B') {
				flowOrder = "TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC";
			}
			else if (toupper(temp[0]) == 'I') {
				flowOrder = "TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC";
			}
			else {
				LOG(LOGERROR) << "" + temp + " is not a valid option for order. order options are A, B, or I. A = TACG, B = TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC, and I = TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC.\n";  abort = true;
			}
		}


	}

}
//**********************************************************************************************************************

int ShhherCommand::execute() {
	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	getSingleLookUp();	if (ctrlc_pressed) { return 0; }
	getJointLookUp();	if (ctrlc_pressed) { return 0; }

	int numFiles = flowFileVector.size();

	if (numFiles < processors) { processors = numFiles; }

#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux) || (__linux__) || (__unix__) || (__unix)
	if (processors == 1) { driver(flowFileVector, compositeFASTAFileName, compositeNamesFileName); }
	else { createProcesses(flowFileVector); } //each processor processes one file
#else
	driver(flowFileVector, compositeFASTAFileName, compositeNamesFileName);
#endif

	if (compositeFASTAFileName != "") {
		outputNames.push_back(compositeFASTAFileName); outputTypes["fasta"].push_back(compositeFASTAFileName);
		outputNames.push_back(compositeNamesFileName); outputTypes["name"].push_back(compositeNamesFileName);
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
//********************************************************************************************************************
//sorts biggest to smallest
inline bool compareFileSizes(string left, string right) {

	FILE * pFile;
	long leftsize = 0;

	//get num bytes in file
	string filename = left;
	pFile = fopen(filename.c_str(), "rb");
	string error = "Error opening " + filename;
	if (pFile == NULL) perror(error.c_str());
	else {
		fseek(pFile, 0, SEEK_END);
		leftsize = ftell(pFile);
		fclose(pFile);
	}

	FILE * pFile2;
	long rightsize = 0;

	//get num bytes in file
	filename = right;
	pFile2 = fopen(filename.c_str(), "rb");
	error = "Error opening " + filename;
	if (pFile2 == NULL) perror(error.c_str());
	else {
		fseek(pFile2, 0, SEEK_END);
		rightsize = ftell(pFile2);
		fclose(pFile2);
	}

	return (leftsize > rightsize);
}
/**************************************************************************************************/

int ShhherCommand::createProcesses(vector<string> filenames) {
	vector<int> processIDS;
	int process = 1;
	int num = 0;
	bool recalc = false;

	//sanity check
	if (filenames.size() < processors) { processors = filenames.size(); }

	//sort file names by size to divide load better
	sort(filenames.begin(), filenames.end(), compareFileSizes);

	vector < vector <string> > dividedFiles; //dividedFiles[1] = vector of filenames for process 1...
	dividedFiles.resize(processors);

	//for each file, figure out which process will complete it
	//want to divide the load intelligently so the big files are spread between processes
	for (int i = 0; i < filenames.size(); i++) {
		int processToAssign = (i + 1) % processors;
		if (processToAssign == 0) { processToAssign = processors; }

		dividedFiles[(processToAssign - 1)].push_back(filenames[i]);
	}

	//now lets reverse the order of ever other process, so we balance big files running with little ones
	for (int i = 0; i < processors; i++) {
		int remainder = ((i + 1) % processors);
		if (remainder) { reverse(dividedFiles[i].begin(), dividedFiles[i].end()); }
	}


#if defined (UNIX)		

	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
			process++;
		}
		else if (pid == 0) {
			num = driver(dividedFiles[process], compositeFASTAFileName + m->mothurGetpid(process) + ".temp", compositeNamesFileName + m->mothurGetpid(process) + ".temp");

			//pass numSeqs to parent
			ofstream out;
			string tempFile = compositeFASTAFileName + m->mothurGetpid(process) + ".num.temp";
			File::openOutputFile(tempFile, out);
			out << num << endl;
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
				File::remove(compositeNamesFileName + (toString(processIDS[i]) + ".temp"));
				File::remove(compositeFASTAFileName + (toString(processIDS[i]) + ".temp"));
				File::remove(compositeFASTAFileName + (toString(processIDS[i]) + ".num.temp"));
			}
			recalc = true;
			break;

		}
	}

	if (recalc) {
		//test line, also set recalc to true.
		//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove(compositeNamesFileName + (toString(processIDS[i]) + ".temp"));File::remove(compositeFASTAFileName + (toString(processIDS[i]) + ".temp"));File::remove(compositeFASTAFileName + (toString(processIDS[i]) + ".num.temp"));}processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

		dividedFiles.clear(); //dividedFiles[1] = vector of filenames for process 1...
		dividedFiles.resize(processors);

		//for each file, figure out which process will complete it
		//want to divide the load intelligently so the big files are spread between processes
		for (int i = 0; i < filenames.size(); i++) {
			int processToAssign = (i + 1) % processors;
			if (processToAssign == 0) { processToAssign = processors; }

			dividedFiles[(processToAssign - 1)].push_back(filenames[i]);
		}

		//now lets reverse the order of ever other process, so we balance big files running with little ones
		for (int i = 0; i < processors; i++) {
			int remainder = ((i + 1) % processors);
			if (remainder) { reverse(dividedFiles[i].begin(), dividedFiles[i].end()); }
		}

		num = 0;
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
				num = driver(dividedFiles[process], compositeFASTAFileName + m->mothurGetpid(process) + ".temp", compositeNamesFileName + m->mothurGetpid(process) + ".temp");

				//pass numSeqs to parent
				ofstream out;
				string tempFile = compositeFASTAFileName + m->mothurGetpid(process) + ".num.temp";
				File::openOutputFile(tempFile, out);
				out << num << endl;
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

	//do my part
	driver(dividedFiles[0], compositeFASTAFileName, compositeNamesFileName);

	//force parent to wait until all the processes are done
	for (int i = 0;i < processIDS.size();i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

#else

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////// NOT WORKING, ACCESS VIOLATION ON READ OF FLOWGRAMS IN THREAD /////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Windows version shared memory, so be careful when passing variables through the shhhFlowsData struct. 
	//Above fork() will clone, so memory is separate, but that's not the case with windows, 
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	vector<shhhFlowsData*> pDataArray;
	vector<DWORD> dwThreadIdArray(processors-1);
	vector<HANDLE> hThreadArray(processors-1);

	//Create processor worker threads.
	for( int i=0; i<processors-1; i++ ){
		// Allocate memory for thread data.
		string extension = "";
		if (i != 0) { extension = toString(i) + ".temp"; }

		shhhFlowsData* tempFlow = new shhhFlowsData(filenames, (compositeFASTAFileName + extension), (compositeNamesFileName + extension), outputDir, flowOrder, jointLookUp, singleLookUp, m, lines[i].start, lines[i].end, cutoff, sigma, minDelta, maxIters, i);
		pDataArray.push_back(tempFlow);
		processIDS.push_back(i);

		hThreadArray[i] = CreateThread(NULL, 0, ShhhFlowsThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]);
	}

	//using the main process as a worker saves time and memory
	//do my part
	driver(filenames, compositeFASTAFileName, compositeNamesFileName, lines[processors-1].start, lines[processors-1].end);

	//Wait until all threads have terminated.
	WaitForMultipleObjects(processors-1, &(hThreadArray[0]), TRUE, INFINITE);

	//Close all thread handles and free memory allocations.
	for(int i=0; i < pDataArray.size(); i++){
		for(int j=0; j < pDataArray[i]->outputNames.size(); j++){ outputNames.push_back(pDataArray[i]->outputNames[j]); }
		CloseHandle(hThreadArray[i]);
		delete pDataArray[i];
	}
	*/
#endif

	for (int i = 0;i < processIDS.size();i++) {
		ifstream in;
		string tempFile = compositeFASTAFileName + toString(processIDS[i]) + ".num.temp";
		File::openInputFile(tempFile, in);
		if (!in.eof()) {
			int tempNum = 0;
			in >> tempNum;
			if (tempNum != dividedFiles[i + 1].size()) {
				LOG(LOGERROR) << "main process expected " + toString(processIDS[i]) + " to complete " + toString(dividedFiles[i + 1].size()) + " files, and it only reported completing " + toString(tempNum) + ". This will cause file mismatches.  The flow files may be too large to process with multiple processors. \n";
			}
		}
		in.close(); File::remove(tempFile);

		if (compositeFASTAFileName != "") {
			File::appendFiles((compositeFASTAFileName + toString(processIDS[i]) + ".temp"), compositeFASTAFileName);
			File::appendFiles((compositeNamesFileName + toString(processIDS[i]) + ".temp"), compositeNamesFileName);
			File::remove((compositeFASTAFileName + toString(processIDS[i]) + ".temp"));
			File::remove((compositeNamesFileName + toString(processIDS[i]) + ".temp"));
		}
	}

	return 0;

}
/**************************************************************************************************/

vector<string> ShhherCommand::parseFlowFiles(string filename) {
	vector<string> files;
	int count = 0;

	ifstream in;
	File::openInputFile(filename, in);

	int thisNumFLows = 0;
	in >> thisNumFLows; File::gobble(in);

	while (!in.eof()) {
		if (ctrlc_pressed) { break; }

		ofstream out;
		string outputFileName = filename + toString(count) + ".temp";
		File::openOutputFile(outputFileName, out);
		out << thisNumFLows << endl;
		files.push_back(outputFileName);

		int numLinesWrote = 0;
		for (int i = 0; i < largeSize; i++) {
			if (in.eof()) { break; }
			string line = File::getline(in); File::gobble(in);
			out << line << endl;
			numLinesWrote++;
		}
		out.close();

		if (numLinesWrote == 0) { File::remove(outputFileName); files.pop_back(); }
		count++;
	}
	in.close();

	if (ctrlc_pressed) { for (int i = 0; i < files.size(); i++) { File::remove(files[i]); }  files.clear(); }

	LOG(INFO) << "\nDivided " + filename + " into " + toString(files.size()) + " files.\n\n";

	return files;
}
/**************************************************************************************************/

int ShhherCommand::driver(vector<string> filenames, string thisCompositeFASTAFileName, string thisCompositeNamesFileName) {

	int numCompleted = 0;

	for (int i = 0;i < filenames.size();i++) {

		if (ctrlc_pressed) { break; }

		vector<string> theseFlowFileNames; theseFlowFileNames.push_back(filenames[i]);
		if (large) { theseFlowFileNames = parseFlowFiles(filenames[i]); }

		if (ctrlc_pressed) { break; }

		double begClock = clock();
		unsigned long long begTime;

		string fileNameForOutput = filenames[i];

		for (int g = 0; g < theseFlowFileNames.size(); g++) {

			string flowFileName = theseFlowFileNames[g];
			LOG(INFO) << "\n>>>>>\tProcessing " + flowFileName + " (file " + toString(i + 1) + " of " + toString(filenames.size()) + ")\t<<<<<\n";
			LOG(INFO) << "Reading flowgrams...\n";

			vector<string> seqNameVector;
			vector<int> lengths;
			vector<short> flowDataIntI;
			vector<double> flowDataPrI;
			map<string, int> nameMap;
			vector<short> uniqueFlowgrams;
			vector<int> uniqueCount;
			vector<int> mapSeqToUnique;
			vector<int> mapUniqueToSeq;
			vector<int> uniqueLengths;
			int numFlowCells;

			if (app.isDebug) { LOG(DEBUG) << "About to read flowgrams.\n"; }
			int numSeqs = getFlowData(flowFileName, seqNameVector, lengths, flowDataIntI, nameMap, numFlowCells);

			if (ctrlc_pressed) { break; }

			LOG(INFO) << "Identifying unique flowgrams...\n";
			int numUniques = getUniques(numSeqs, numFlowCells, uniqueFlowgrams, uniqueCount, uniqueLengths, mapSeqToUnique, mapUniqueToSeq, lengths, flowDataPrI, flowDataIntI);

			if (ctrlc_pressed) { break; }

			LOG(INFO) << "Calculating distances between flowgrams...\n";
			string distFileName = flowFileName.substr(0, flowFileName.find_last_of('.')) + ".shhh.dist";
			begTime = time(NULL);


			flowDistParentFork(numFlowCells, distFileName, numUniques, mapUniqueToSeq, mapSeqToUnique, lengths, flowDataPrI, flowDataIntI);

			LOG(INFO) << '\n' << "Total time: " + toString(time(NULL) - begTime) + '\t' + toString((clock() - begClock) / CLOCKS_PER_SEC) + '\n';


			string namesFileName = flowFileName.substr(0, flowFileName.find_last_of('.')) + ".shhh.names";
			createNamesFile(numSeqs, numUniques, namesFileName, seqNameVector, mapSeqToUnique, mapUniqueToSeq);

			if (ctrlc_pressed) { break; }

			LOG(INFO) << "\nClustering flowgrams...\n";
			string listFileName = flowFileName.substr(0, flowFileName.find_last_of('.')) + ".shhh.list";
			cluster(listFileName, distFileName, namesFileName);

			if (ctrlc_pressed) { break; }

			vector<int> otuData;
			vector<int> cumNumSeqs;
			vector<int> nSeqsPerOTU;
			vector<vector<int> > aaP;	//tMaster->aanP:	each row is a different otu / each col contains the sequence indices
			vector<vector<int> > aaI;	//tMaster->aanI:	that are in each otu - can't differentiate between aaP and aaI 
			vector<int> seqNumber;		//tMaster->anP:		the sequence id number sorted by OTU
			vector<int> seqIndex;		//tMaster->anI;		the index that corresponds to seqNumber


			int numOTUs = getOTUData(numSeqs, listFileName, otuData, cumNumSeqs, nSeqsPerOTU, aaP, aaI, seqNumber, seqIndex, nameMap);

			if (ctrlc_pressed) { break; }

			File::remove(distFileName);
			File::remove(namesFileName);
			File::remove(listFileName);

			vector<double> dist;		//adDist - distance of sequences to centroids
			vector<short> change;		//did the centroid sequence change? 0 = no; 1 = yes
			vector<int> centroids;		//the representative flowgram for each cluster m
			vector<double> weight;
			vector<double> singleTau;	//tMaster->adTau:	1-D Tau vector (1xnumSeqs)
			vector<int> nSeqsBreaks;
			vector<int> nOTUsBreaks;

			if (app.isDebug) { LOG(DEBUG) << "numSeqs = " + toString(numSeqs) + " numOTUS = " + toString(numOTUs) + " about to alloc a dist vector with size = " + toString((numSeqs * numOTUs)) + ".\n"; }

			dist.assign(numSeqs * numOTUs, 0);
			change.assign(numOTUs, 1);
			centroids.assign(numOTUs, -1);
			weight.assign(numOTUs, 0);
			singleTau.assign(numSeqs, 1.0);

			nSeqsBreaks.assign(2, 0);
			nOTUsBreaks.assign(2, 0);

			nSeqsBreaks[0] = 0;
			nSeqsBreaks[1] = numSeqs;
			nOTUsBreaks[1] = numOTUs;

			if (app.isDebug) { LOG(DEBUG) << "done allocating memory, about to denoise.\n"; }

			if (ctrlc_pressed) { break; }

			double maxDelta = 0;
			int iter = 0;

			begClock = clock();
			begTime = time(NULL);

			LOG(INFO) << "\nDenoising flowgrams...\n";
			LOG(INFO) << "iter\tmaxDelta\tnLL\t\tcycletime\n";

			while ((maxIters == 0 && maxDelta > minDelta) || iter < MIN_ITER || (maxDelta > minDelta && iter < maxIters)) {

				if (ctrlc_pressed) { break; }

				double cycClock = clock();
				unsigned long long cycTime = time(NULL);
				fill(numOTUs, seqNumber, seqIndex, cumNumSeqs, nSeqsPerOTU, aaP, aaI);

				if (ctrlc_pressed) { break; }

				calcCentroidsDriver(numOTUs, cumNumSeqs, nSeqsPerOTU, seqIndex, change, centroids, singleTau, mapSeqToUnique, uniqueFlowgrams, flowDataIntI, lengths, numFlowCells, seqNumber);

				if (ctrlc_pressed) { break; }

				maxDelta = getNewWeights(numOTUs, cumNumSeqs, nSeqsPerOTU, singleTau, seqNumber, weight);

				if (ctrlc_pressed) { break; }

				double nLL = getLikelihood(numSeqs, numOTUs, nSeqsPerOTU, seqNumber, cumNumSeqs, seqIndex, dist, weight);

				if (ctrlc_pressed) { break; }

				checkCentroids(numOTUs, centroids, weight);

				if (ctrlc_pressed) { break; }

				calcNewDistances(numSeqs, numOTUs, nSeqsPerOTU, dist, weight, change, centroids, aaP, singleTau, aaI, seqNumber, seqIndex, uniqueFlowgrams, flowDataIntI, numFlowCells, lengths);

				if (ctrlc_pressed) { break; }

				iter++;

				LOG(INFO) << toString(iter) + '\t' + toString(maxDelta) + '\t' + toString(nLL) + '\t' + toString(time(NULL) - cycTime) + '\t' + toString((clock() - cycClock) / (double)CLOCKS_PER_SEC) + '\n';

			}

			if (ctrlc_pressed) { break; }

			LOG(INFO) << "\nFinalizing...\n";
			fill(numOTUs, seqNumber, seqIndex, cumNumSeqs, nSeqsPerOTU, aaP, aaI);

			if (app.isDebug) { LOG(DEBUG) << "done fill().\n"; }

			if (ctrlc_pressed) { break; }

			setOTUs(numOTUs, numSeqs, seqNumber, seqIndex, cumNumSeqs, nSeqsPerOTU, otuData, singleTau, dist, aaP, aaI);

			if (app.isDebug) { LOG(DEBUG) << "done setOTUs().\n"; }

			if (ctrlc_pressed) { break; }

			vector<int> otuCounts(numOTUs, 0);
			for (int j = 0;j < numSeqs;j++) { otuCounts[otuData[j]]++; }

			calcCentroidsDriver(numOTUs, cumNumSeqs, nSeqsPerOTU, seqIndex, change, centroids, singleTau, mapSeqToUnique, uniqueFlowgrams, flowDataIntI, lengths, numFlowCells, seqNumber);

			if (app.isDebug) { LOG(DEBUG) << "done calcCentroidsDriver().\n"; }

			if (ctrlc_pressed) { break; }

			if ((large) && (g == 0)) { flowFileName = filenames[i]; theseFlowFileNames[0] = filenames[i]; }
			string thisOutputDir = outputDir;
			if (outputDir == "") { thisOutputDir = File::getPath(flowFileName); }
			map<string, string> variables;
			variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(flowFileName));
			string qualityFileName = getOutputFileName("qfile", variables);
			string fastaFileName = getOutputFileName("fasta", variables);
			string nameFileName = getOutputFileName("name", variables);
			string otuCountsFileName = getOutputFileName("counts", variables);
			string fileRoot = File::getRootName(File::getSimpleName(flowFileName));
			int pos = fileRoot.find_first_of('.');
			string fileGroup = fileRoot;
			if (pos != string::npos) { fileGroup = fileRoot.substr(pos + 1, (fileRoot.length() - 1 - (pos + 1))); }
			string groupFileName = getOutputFileName("group", variables);


			writeQualities(numOTUs, numFlowCells, qualityFileName, otuCounts, nSeqsPerOTU, seqNumber, singleTau, flowDataIntI, uniqueFlowgrams, cumNumSeqs, mapUniqueToSeq, seqNameVector, centroids, aaI); if (ctrlc_pressed) { break; }
			writeSequences(thisCompositeFASTAFileName, numOTUs, numFlowCells, fastaFileName, otuCounts, uniqueFlowgrams, seqNameVector, aaI, centroids);if (ctrlc_pressed) { break; }
			writeNames(thisCompositeNamesFileName, numOTUs, nameFileName, otuCounts, seqNameVector, aaI, nSeqsPerOTU);				if (ctrlc_pressed) { break; }
			writeClusters(otuCountsFileName, numOTUs, numFlowCells, otuCounts, centroids, uniqueFlowgrams, seqNameVector, aaI, nSeqsPerOTU, lengths, flowDataIntI);			if (ctrlc_pressed) { break; }
			writeGroups(groupFileName, fileGroup, numSeqs, seqNameVector);						if (ctrlc_pressed) { break; }

			if (large) {
				if (g > 0) {
					variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(theseFlowFileNames[0]));
					File::appendFiles(qualityFileName, getOutputFileName("qfile", variables));
					File::remove(qualityFileName);
					File::appendFiles(fastaFileName, getOutputFileName("fasta", variables));
					File::remove(fastaFileName);
					File::appendFiles(nameFileName, getOutputFileName("name", variables));
					File::remove(nameFileName);
					File::appendFiles(otuCountsFileName, getOutputFileName("counts", variables));
					File::remove(otuCountsFileName);
					File::appendFiles(groupFileName, getOutputFileName("group", variables));
					File::remove(groupFileName);
				}
				File::remove(theseFlowFileNames[g]);
			}
		}

		numCompleted++;
		LOG(INFO) << "Total time to process " + fileNameForOutput + ":\t" + toString(time(NULL) - begTime) + '\t' + toString((clock() - begClock) / (double)CLOCKS_PER_SEC) + '\n';
	}

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	return numCompleted;

}
catch (exception& e) {
	LOG(FATAL) << e.what() << " in ShhherCommand, driver";
	exit(1);
}
}

/**************************************************************************************************/
int ShhherCommand::getFlowData(string filename, vector<string>& thisSeqNameVector, vector<int>& thisLengths, vector<short>& thisFlowDataIntI, map<string, int>& thisNameMap, int& numFlowCells) {
	try {

		ifstream flowFile;

		File::openInputFile(filename, flowFile);

		string seqName;
		int currentNumFlowCells;
		float intensity;
		thisSeqNameVector.clear();
		thisLengths.clear();
		thisFlowDataIntI.clear();
		thisNameMap.clear();

		string numFlowTest;
		flowFile >> numFlowTest;

		if (!m->isContainingOnlyDigits(numFlowTest)) { LOG(LOGERROR) << "expected a number and got " + numFlowTest + ", quitting. Did you use the flow parameter instead of the file parameter?" << '\n'; exit(1); }
		else { convert(numFlowTest, numFlowCells); }

		if (app.isDebug) { LOG(DEBUG) << "numFlowCells = " + toString(numFlowCells) + ".\n"; }
		int index = 0;//pcluster
		while (!flowFile.eof()) {

			if (ctrlc_pressed) { break; }

			flowFile >> seqName >> currentNumFlowCells;

			thisLengths.push_back(currentNumFlowCells);

			thisSeqNameVector.push_back(seqName);
			thisNameMap[seqName] = index++;//pcluster

			if (app.isDebug) { LOG(DEBUG) << "seqName = " + seqName + " length = " + toString(currentNumFlowCells) + " index = " + toString(index) + "\n"; }

			for (int i = 0;i < numFlowCells;i++) {
				flowFile >> intensity;
				if (intensity > 9.99) { intensity = 9.99; }
				int intI = int(100 * intensity + 0.0001);
				thisFlowDataIntI.push_back(intI);
			}
			File::gobble(flowFile);
		}
		flowFile.close();

		int numSeqs = thisSeqNameVector.size();

		for (int i = 0;i < numSeqs;i++) {

			if (ctrlc_pressed) { break; }

			int iNumFlowCells = i * numFlowCells;
			for (int j = thisLengths[i];j < numFlowCells;j++) {
				thisFlowDataIntI[iNumFlowCells + j] = 0;
			}
		}

		return numSeqs;

	}
	/**************************************************************************************************/

	int ShhherCommand::flowDistParentFork(int numFlowCells, string distFileName, int stopSeq, vector<int>& mapUniqueToSeq, vector<int>& mapSeqToUnique, vector<int>& lengths, vector<double>& flowDataPrI, vector<short>& flowDataIntI) {
		try {

			ostringstream outStream;
			outStream.setf(ios::fixed, ios::floatfield);
			outStream.setf(ios::dec, ios::basefield);
			outStream.setf(ios::showpoint);
			outStream.precision(6);

			int begTime = time(NULL);
			double begClock = clock();

			for (int i = 0;i < stopSeq;i++) {

				if (ctrlc_pressed) { break; }

				for (int j = 0;j < i;j++) {
					float flowDistance = calcPairwiseDist(numFlowCells, mapUniqueToSeq[i], mapUniqueToSeq[j], mapSeqToUnique, lengths, flowDataPrI, flowDataIntI);

					if (flowDistance < 1e-6) {
						outStream << mapUniqueToSeq[i] << '\t' << mapUniqueToSeq[j] << '\t' << 0.000000 << endl;
					}
					else if (flowDistance <= cutoff) {
						outStream << mapUniqueToSeq[i] << '\t' << mapUniqueToSeq[j] << '\t' << flowDistance << endl;
					}
				}
				if (i % 100 == 0) {
					LOG(SCREENONLY) << toString(i) + "\t" + toString(time(NULL) - begTime);
					LOG(SCREENONLY) << "\t" + toString((clock() - begClock) / CLOCKS_PER_SEC) + "\n";
				}
			}

			ofstream distFile(distFileName.c_str());
			distFile << outStream.str();
			distFile.close();

			if (ctrlc_pressed) {}
			else {
				LOG(SCREENONLY) << toString(stopSeq - 1) + "\t" + toString(time(NULL) - begTime);
				LOG(SCREENONLY) << "\t" + toString((clock() - begClock) / CLOCKS_PER_SEC) + "\n";
			}

			return 0;
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, flowDistParentFork";
			exit(1);
		}
	}
	/**************************************************************************************************/

	float ShhherCommand::calcPairwiseDist(int numFlowCells, int seqA, int seqB, vector<int>& mapSeqToUnique, vector<int>& lengths, vector<double>& flowDataPrI, vector<short>& flowDataIntI) {
		try {
			int minLength = lengths[mapSeqToUnique[seqA]];
			if (lengths[seqB] < minLength) { minLength = lengths[mapSeqToUnique[seqB]]; }

			int ANumFlowCells = seqA * numFlowCells;
			int BNumFlowCells = seqB * numFlowCells;

			float dist = 0;

			for (int i = 0;i < minLength;i++) {

				if (ctrlc_pressed) { break; }

				int flowAIntI = flowDataIntI[ANumFlowCells + i];
				float flowAPrI = flowDataPrI[ANumFlowCells + i];

				int flowBIntI = flowDataIntI[BNumFlowCells + i];
				float flowBPrI = flowDataPrI[BNumFlowCells + i];
				dist += jointLookUp[flowAIntI * NUMBINS + flowBIntI] - flowAPrI - flowBPrI;
			}

			dist /= (float)minLength;
			return dist;
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, calcPairwiseDist";
			exit(1);
		}
	}

	/**************************************************************************************************/

	int ShhherCommand::getUniques(int numSeqs, int numFlowCells, vector<short>& uniqueFlowgrams, vector<int>& uniqueCount, vector<int>& uniqueLengths, vector<int>& mapSeqToUnique, vector<int>& mapUniqueToSeq, vector<int>& lengths, vector<double>& flowDataPrI, vector<short>& flowDataIntI) {
		try {
			int numUniques = 0;
			uniqueFlowgrams.assign(numFlowCells * numSeqs, -1);
			uniqueCount.assign(numSeqs, 0);							//	anWeights
			uniqueLengths.assign(numSeqs, 0);
			mapSeqToUnique.assign(numSeqs, -1);
			mapUniqueToSeq.assign(numSeqs, -1);

			vector<short> uniqueFlowDataIntI(numFlowCells * numSeqs, -1);

			for (int i = 0;i < numSeqs;i++) {

				if (ctrlc_pressed) { break; }

				int index = 0;

				vector<short> current(numFlowCells);
				for (int j = 0;j < numFlowCells;j++) {
					current[j] = short(((flowDataIntI[i * numFlowCells + j] + 50.0) / 100.0));
				}

				for (int j = 0;j < numUniques;j++) {
					int offset = j * numFlowCells;
					bool toEnd = 1;

					int shorterLength;
					if (lengths[i] < uniqueLengths[j]) { shorterLength = lengths[i]; }
					else { shorterLength = uniqueLengths[j]; }

					for (int k = 0;k < shorterLength;k++) {
						if (current[k] != uniqueFlowgrams[offset + k]) {
							toEnd = 0;
							break;
						}
					}

					if (toEnd) {
						mapSeqToUnique[i] = j;
						uniqueCount[j]++;
						index = j;
						if (lengths[i] > uniqueLengths[j]) { uniqueLengths[j] = lengths[i]; }
						break;
					}
					index++;
				}

				if (index == numUniques) {
					uniqueLengths[numUniques] = lengths[i];
					uniqueCount[numUniques] = 1;
					mapSeqToUnique[i] = numUniques;//anMap
					mapUniqueToSeq[numUniques] = i;//anF

					for (int k = 0;k < numFlowCells;k++) {
						uniqueFlowgrams[numUniques * numFlowCells + k] = current[k];
						uniqueFlowDataIntI[numUniques * numFlowCells + k] = flowDataIntI[i * numFlowCells + k];
					}

					numUniques++;
				}
			}
			uniqueFlowDataIntI.resize(numFlowCells * numUniques);
			uniqueLengths.resize(numUniques);

			flowDataPrI.resize(numSeqs * numFlowCells, 0);
			for (int i = 0;i < flowDataPrI.size();i++) { if (ctrlc_pressed) { break; } flowDataPrI[i] = getProbIntensity(flowDataIntI[i]); }

			return numUniques;
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, getUniques";
			exit(1);
		}
	}
	/**************************************************************************************************/
	int ShhherCommand::createNamesFile(int numSeqs, int numUniques, string filename, vector<string>& seqNameVector, vector<int>& mapSeqToUnique, vector<int>& mapUniqueToSeq) {
		try {

			vector<string> duplicateNames(numUniques, "");
			for (int i = 0;i < numSeqs;i++) {
				duplicateNames[mapSeqToUnique[i]] += seqNameVector[i] + ',';
			}

			ofstream nameFile;
			File::openOutputFile(filename, nameFile);

			for (int i = 0;i < numUniques;i++) {

				if (ctrlc_pressed) { break; }

				//			nameFile << seqNameVector[mapUniqueToSeq[i]] << '\t' << duplicateNames[i].substr(0, duplicateNames[i].find_last_of(',')) << endl;
				nameFile << mapUniqueToSeq[i] << '\t' << duplicateNames[i].substr(0, duplicateNames[i].find_last_of(',')) << endl;
			}

			nameFile.close();

			return 0;
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, createNamesFile";
			exit(1);
		}
	}
	//**********************************************************************************************************************

	int ShhherCommand::cluster(string filename, string distFileName, string namesFileName) {

		ReadMatrix* read = new ReadColumnMatrix(distFileName);
		read->setCutoff(cutoff);

		NameAssignment* clusterNameMap = new NameAssignment(namesFileName);
		clusterNameMap->readMap();
		read->read(clusterNameMap);

		ListVector* list = read->getListVector();
		SparseDistanceMatrix* matrix = read->getDMatrix();

		delete read;
		delete clusterNameMap;

		RAbundVector* rabund = new RAbundVector(list->getRAbundVector());

		float adjust = -1.0;
		Cluster* cluster = new CompleteLinkage(rabund, list, matrix, cutoff, "furthest", adjust);
		string tag = cluster->getTag();

		double clusterCutoff = cutoff;
		while (matrix->getSmallDist() <= clusterCutoff && matrix->getNNodes() > 0) {

			if (ctrlc_pressed) { break; }

			cluster->update(clusterCutoff);
		}

		list->setLabel(toString(cutoff));

		ofstream listFile;
		File::openOutputFile(filename, listFile);
		list->print(listFile);
		listFile.close();

		delete matrix;	delete cluster;	delete rabund; delete list;

		return 0;
	}
	/**************************************************************************************************/

	int ShhherCommand::getOTUData(int numSeqs, string fileName, vector<int>& otuData,
		vector<int>& cumNumSeqs,
		vector<int>& nSeqsPerOTU,
		vector<vector<int> >& aaP,	//tMaster->aanP:	each row is a different otu / each col contains the sequence indices
		vector<vector<int> >& aaI,	//tMaster->aanI:	that are in each otu - can't differentiate between aaP and aaI 
		vector<int>& seqNumber,		//tMaster->anP:		the sequence id number sorted by OTU
		vector<int>& seqIndex,
		map<string, int>& nameMap) {

		ifstream listFile;
		File::openInputFile(fileName, listFile);
		string label;
		int numOTUs;

		listFile >> label >> numOTUs;

		if (app.isDebug) { LOG(DEBUG) << "Getting OTU Data...\n"; }

		otuData.assign(numSeqs, 0);
		cumNumSeqs.assign(numOTUs, 0);
		nSeqsPerOTU.assign(numOTUs, 0);
		aaP.clear();aaP.resize(numOTUs);

		seqNumber.clear();
		aaI.clear();
		seqIndex.clear();

		string singleOTU = "";

		for (int i = 0;i < numOTUs;i++) {

			if (ctrlc_pressed) { break; }
			if (app.isDebug) { LOG(DEBUG) << "processing OTU " + toString(i) + ".\n"; }

			listFile >> singleOTU;

			istringstream otuString(singleOTU);

			while (otuString) {

				string seqName = "";

				for (int j = 0;j < singleOTU.length();j++) {
					char letter = otuString.get();

					if (letter != ',') {
						seqName += letter;
					}
					else {
						map<string, int>::iterator nmIt = nameMap.find(seqName);
						int index = nmIt->second;

						nameMap.erase(nmIt);

						otuData[index] = i;
						nSeqsPerOTU[i]++;
						aaP[i].push_back(index);
						seqName = "";
					}
				}

				map<string, int>::iterator nmIt = nameMap.find(seqName);

				int index = nmIt->second;
				nameMap.erase(nmIt);

				otuData[index] = i;
				nSeqsPerOTU[i]++;
				aaP[i].push_back(index);

				otuString.get();
			}

			sort(aaP[i].begin(), aaP[i].end());
			for (int j = 0;j < nSeqsPerOTU[i];j++) {
				seqNumber.push_back(aaP[i][j]);
			}
			for (int j = nSeqsPerOTU[i];j < numSeqs;j++) {
				aaP[i].push_back(0);
			}


		}

		for (int i = 1;i < numOTUs;i++) {
			cumNumSeqs[i] = cumNumSeqs[i - 1] + nSeqsPerOTU[i - 1];
		}
		aaI = aaP;
		seqIndex = seqNumber;

		listFile.close();

		return numOTUs;

	}
	/**************************************************************************************************/

	int ShhherCommand::calcCentroidsDriver(int numOTUs,
		vector<int>& cumNumSeqs,
		vector<int>& nSeqsPerOTU,
		vector<int>& seqIndex,
		vector<short>& change,		//did the centroid sequence change? 0 = no; 1 = yes
		vector<int>& centroids,		//the representative flowgram for each cluster m
		vector<double>& singleTau,	//tMaster->adTau:	1-D Tau vector (1xnumSeqs)
		vector<int>& mapSeqToUnique,
		vector<short>& uniqueFlowgrams,
		vector<short>& flowDataIntI,
		vector<int>& lengths,
		int numFlowCells,
		vector<int>& seqNumber) {

		//this function gets the most likely homopolymer length at a flow position for a group of sequences
		//within an otu

		try {

			for (int i = 0;i<numOTUs;i++) {

				if (ctrlc_pressed) { break; }

				double count = 0;
				int position = 0;
				int minFlowGram = 100000000;
				double minFlowValue = 1e8;
				change[i] = 0; //FALSE

				for (int j = 0;j < nSeqsPerOTU[i];j++) {
					count += singleTau[seqNumber[cumNumSeqs[i] + j]];
				}

				if (nSeqsPerOTU[i] > 0 && count > MIN_COUNT) {
					vector<double> adF(nSeqsPerOTU[i]);
					vector<int> anL(nSeqsPerOTU[i]);

					for (int j = 0;j < nSeqsPerOTU[i];j++) {
						int index = cumNumSeqs[i] + j;
						int nI = seqIndex[index];
						int nIU = mapSeqToUnique[nI];

						int k;
						for (k = 0;k < position;k++) {
							if (nIU == anL[k]) {
								break;
							}
						}
						if (k == position) {
							anL[position] = nIU;
							adF[position] = 0.0000;
							position++;
						}
					}

					for (int j = 0;j < nSeqsPerOTU[i];j++) {
						int index = cumNumSeqs[i] + j;
						int nI = seqIndex[index];

						double tauValue = singleTau[seqNumber[index]];

						for (int k = 0;k < position;k++) {
							double dist = getDistToCentroid(anL[k], nI, lengths[nI], uniqueFlowgrams, flowDataIntI, numFlowCells);
							adF[k] += dist * tauValue;
						}
					}

					for (int j = 0;j < position;j++) {
						if (adF[j] < minFlowValue) {
							minFlowGram = j;
							minFlowValue = adF[j];
						}
					}

					if (centroids[i] != anL[minFlowGram]) {
						change[i] = 1;
						centroids[i] = anL[minFlowGram];
					}
				}
				else if (centroids[i] != -1) {
					change[i] = 1;
					centroids[i] = -1;
				}
			}

			return 0;
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, calcCentroidsDriver";
			exit(1);
		}
	}
	/**************************************************************************************************/

	double ShhherCommand::getDistToCentroid(int cent, int flow, int length, vector<short>& uniqueFlowgrams,
		vector<short>& flowDataIntI, int numFlowCells) {
		try {

			int flowAValue = cent * numFlowCells;
			int flowBValue = flow * numFlowCells;

			double dist = 0;

			for (int i = 0;i < length;i++) {
				dist += singleLookUp[uniqueFlowgrams[flowAValue] * NUMBINS + flowDataIntI[flowBValue]];
				flowAValue++;
				flowBValue++;
			}

			return dist / (double)length;
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, getDistToCentroid";
			exit(1);
		}
	}
	/**************************************************************************************************/

	double ShhherCommand::getNewWeights(int numOTUs, vector<int>& cumNumSeqs, vector<int>& nSeqsPerOTU, vector<double>& singleTau, vector<int>& seqNumber, vector<double>& weight) {
		try {

			double maxChange = 0;

			for (int i = 0;i < numOTUs;i++) {

				if (ctrlc_pressed) { break; }

				double difference = weight[i];
				weight[i] = 0;

				for (int j = 0;j < nSeqsPerOTU[i];j++) {
					int index = cumNumSeqs[i] + j;
					double tauValue = singleTau[seqNumber[index]];
					weight[i] += tauValue;
				}

				difference = fabs(weight[i] - difference);
				if (difference > maxChange) { maxChange = difference; }
			}
			return maxChange;
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, getNewWeights";
			exit(1);
		}
	}

	/**************************************************************************************************/

	double ShhherCommand::getLikelihood(int numSeqs, int numOTUs, vector<int>& nSeqsPerOTU, vector<int>& seqNumber, vector<int>& cumNumSeqs, vector<int>& seqIndex, vector<double>& dist, vector<double>& weight) {

		try {

			vector<long double> P(numSeqs, 0);
			int effNumOTUs = 0;

			for (int i = 0;i < numOTUs;i++) {
				if (weight[i] > MIN_WEIGHT) {
					effNumOTUs++;
				}
			}

			string hold;
			for (int i = 0;i < numOTUs;i++) {

				if (ctrlc_pressed) { break; }

				for (int j = 0;j < nSeqsPerOTU[i];j++) {
					int index = cumNumSeqs[i] + j;
					int nI = seqIndex[index];
					double singleDist = dist[seqNumber[index]];

					P[nI] += weight[i] * exp(-singleDist * sigma);
				}
			}
			double nLL = 0.00;
			for (int i = 0;i < numSeqs;i++) {
				if (P[i] == 0) { P[i] = DBL_EPSILON; }

				nLL += -log(P[i]);
			}

			nLL = nLL - (double)numSeqs * log(sigma);

			return nLL;
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, getNewWeights";
			exit(1);
		}
	}

	/**************************************************************************************************/

	int ShhherCommand::checkCentroids(int numOTUs, vector<int>& centroids, vector<double>& weight) {
		try {
			vector<int> unique(numOTUs, 1);

			for (int i = 0;i < numOTUs;i++) {
				if (centroids[i] == -1 || weight[i] < MIN_WEIGHT) {
					unique[i] = -1;
				}
			}

			for (int i = 0;i < numOTUs;i++) {

				if (ctrlc_pressed) { break; }

				if (unique[i] == 1) {
					for (int j = i + 1;j < numOTUs;j++) {
						if (unique[j] == 1) {

							if (centroids[j] == centroids[i]) {
								unique[j] = 0;
								centroids[j] = -1;

								weight[i] += weight[j];
								weight[j] = 0.0;
							}
						}
					}
				}
			}

			return 0;
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, checkCentroids";
			exit(1);
		}
	}
	/**************************************************************************************************/

	void ShhherCommand::calcNewDistances(int numSeqs, int numOTUs, vector<int>& nSeqsPerOTU, vector<double>& dist,
		vector<double>& weight, vector<short>& change, vector<int>& centroids,
		vector<vector<int> >& aaP, vector<double>& singleTau, vector<vector<int> >& aaI,
		vector<int>& seqNumber, vector<int>& seqIndex,
		vector<short>& uniqueFlowgrams,
		vector<short>& flowDataIntI, int numFlowCells, vector<int>& lengths) {

		try {

			int total = 0;
			vector<double> newTau(numOTUs, 0);
			vector<double> norms(numSeqs, 0);
			nSeqsPerOTU.assign(numOTUs, 0);

			for (int i = 0;i < numSeqs;i++) {

				if (ctrlc_pressed) { break; }

				int indexOffset = i * numOTUs;

				double offset = 1e8;

				for (int j = 0;j < numOTUs;j++) {

					if (weight[j] > MIN_WEIGHT && change[j] == 1) {
						dist[indexOffset + j] = getDistToCentroid(centroids[j], i, lengths[i], uniqueFlowgrams, flowDataIntI, numFlowCells);
					}

					if (weight[j] > MIN_WEIGHT && dist[indexOffset + j] < offset) {
						offset = dist[indexOffset + j];
					}
				}

				for (int j = 0;j < numOTUs;j++) {
					if (weight[j] > MIN_WEIGHT) {
						newTau[j] = exp(sigma * (-dist[indexOffset + j] + offset)) * weight[j];
						norms[i] += newTau[j];
					}
					else {
						newTau[j] = 0.0;
					}
				}

				for (int j = 0;j < numOTUs;j++) {
					newTau[j] /= norms[i];
				}

				for (int j = 0;j < numOTUs;j++) {
					if (newTau[j] > MIN_TAU) {

						int oldTotal = total;

						total++;

						singleTau.resize(total, 0);
						seqNumber.resize(total, 0);
						seqIndex.resize(total, 0);

						singleTau[oldTotal] = newTau[j];

						aaP[j][nSeqsPerOTU[j]] = oldTotal;
						aaI[j][nSeqsPerOTU[j]] = i;
						nSeqsPerOTU[j]++;
					}
				}

			}

		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, calcNewDistances";
			exit(1);
		}
	}
	/**************************************************************************************************/

	int ShhherCommand::fill(int numOTUs, vector<int>& seqNumber, vector<int>& seqIndex, vector<int>& cumNumSeqs, vector<int>& nSeqsPerOTU, vector<vector<int> >& aaP, vector<vector<int> >& aaI) {
		int index = 0;
		for (int i = 0;i < numOTUs;i++) {

			if (ctrlc_pressed) { return 0; }

			cumNumSeqs[i] = index;
			for (int j = 0;j < nSeqsPerOTU[i];j++) {
				seqNumber[index] = aaP[i][j];
				seqIndex[index] = aaI[i][j];

				index++;
			}
		}

		return 0;
	}
	/**************************************************************************************************/

	void ShhherCommand::setOTUs(int numOTUs, int numSeqs, vector<int>& seqNumber, vector<int>& seqIndex, vector<int>& cumNumSeqs, vector<int>& nSeqsPerOTU,
		vector<int>& otuData, vector<double>& singleTau, vector<double>& dist, vector<vector<int> >& aaP, vector<vector<int> >& aaI) {

		try {
			vector<double> bigTauMatrix(numOTUs * numSeqs, 0.0000);

			for (int i = 0;i < numOTUs;i++) {

				if (ctrlc_pressed) { break; }

				for (int j = 0;j < nSeqsPerOTU[i];j++) {
					int index = cumNumSeqs[i] + j;
					double tauValue = singleTau[seqNumber[index]];
					int sIndex = seqIndex[index];
					bigTauMatrix[sIndex * numOTUs + i] = tauValue;
				}
			}

			for (int i = 0;i < numSeqs;i++) {
				double maxTau = -1.0000;
				int maxOTU = -1;
				for (int j = 0;j<numOTUs;j++) {
					if (bigTauMatrix[i * numOTUs + j] > maxTau) {
						maxTau = bigTauMatrix[i * numOTUs + j];
						maxOTU = j;
					}
				}

				otuData[i] = maxOTU;
			}

			nSeqsPerOTU.assign(numOTUs, 0);

			for (int i = 0;i < numSeqs;i++) {
				int index = otuData[i];

				singleTau[i] = 1.0000;
				dist[i] = 0.0000;

				aaP[index][nSeqsPerOTU[index]] = i;
				aaI[index][nSeqsPerOTU[index]] = i;

				nSeqsPerOTU[index]++;
			}

			fill(numOTUs, seqNumber, seqIndex, cumNumSeqs, nSeqsPerOTU, aaP, aaI);
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, setOTUs";
			exit(1);
		}
	}
	/**************************************************************************************************/

	void ShhherCommand::writeQualities(int numOTUs, int numFlowCells, string qualityFileName, vector<int> otuCounts, vector<int>& nSeqsPerOTU, vector<int>& seqNumber,
		vector<double>& singleTau, vector<short>& flowDataIntI, vector<short>& uniqueFlowgrams, vector<int>& cumNumSeqs,
		vector<int>& mapUniqueToSeq, vector<string>& seqNameVector, vector<int>& centroids, vector<vector<int> >& aaI) {

		try {

			ofstream qualityFile;
			File::openOutputFile(qualityFileName, qualityFile);

			qualityFile.setf(ios::fixed, ios::floatfield);
			qualityFile.setf(ios::showpoint);
			qualityFile << setprecision(6);

			vector<vector<int> > qualities(numOTUs);
			vector<double> pr(HOMOPS, 0);


			for (int i = 0;i < numOTUs;i++) {

				if (ctrlc_pressed) { break; }

				int index = 0;

				if (nSeqsPerOTU[i] > 0) {

					while (index < numFlowCells) {

						double maxPrValue = 1e8;
						short maxPrIndex = -1;
						double count = 0.0000;

						pr.assign(HOMOPS, 0);

						for (int j = 0;j < nSeqsPerOTU[i];j++) {
							int lIndex = cumNumSeqs[i] + j;
							double tauValue = singleTau[seqNumber[lIndex]];
							int sequenceIndex = aaI[i][j];
							short intensity = flowDataIntI[sequenceIndex * numFlowCells + index];

							count += tauValue;

							for (int s = 0;s < HOMOPS;s++) {
								pr[s] += tauValue * singleLookUp[s * NUMBINS + intensity];
							}
						}

						maxPrIndex = uniqueFlowgrams[centroids[i] * numFlowCells + index];
						maxPrValue = pr[maxPrIndex];

						if (count > MIN_COUNT) {
							double U = 0.0000;
							double norm = 0.0000;

							for (int s = 0;s < HOMOPS;s++) {
								norm += exp(-(pr[s] - maxPrValue));
							}

							for (int s = 1;s <= maxPrIndex;s++) {
								int value = 0;
								double temp = 0.0000;

								U += exp(-(pr[s - 1] - maxPrValue)) / norm;

								if (U > 0.00) {
									temp = log10(U);
								}
								else {
									temp = -10.1;
								}
								temp = floor(-10 * temp);
								value = (int)floor(temp);
								if (value > 100) { value = 100; }

								qualities[i].push_back((int)value);
							}
						}//end if

						index++;

					}//end while

				}//end if


				if (otuCounts[i] > 0) {
					qualityFile << '>' << seqNameVector[mapUniqueToSeq[i]] << endl;
					//need to get past the first four bases
					for (int j = 4; j < qualities[i].size(); j++) { qualityFile << qualities[i][j] << ' '; }
					qualityFile << endl;
				}
			}//end for
			qualityFile.close();
			outputNames.push_back(qualityFileName); outputTypes["qfile"].push_back(qualityFileName);

		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, writeQualities";
			exit(1);
		}
	}

	/**************************************************************************************************/

	void ShhherCommand::writeSequences(string thisCompositeFASTAFileName, int numOTUs, int numFlowCells, string fastaFileName, vector<int> otuCounts, vector<short>& uniqueFlowgrams, vector<string>& seqNameVector, vector<vector<int> >& aaI, vector<int>& centroids) {

		ofstream fastaFile;
		File::openOutputFile(fastaFileName, fastaFile);

		vector<string> names(numOTUs, "");

		for (int i = 0;i < numOTUs;i++) {

			if (ctrlc_pressed) { break; }

			int index = centroids[i];

			if (otuCounts[i] > 0) {
				fastaFile << '>' << seqNameVector[aaI[i][0]] << endl;

				string newSeq = "";

				for (int j = 0;j < numFlowCells;j++) {

					char base = flowOrder[j % flowOrder.length()];
					for (int k = 0;k < uniqueFlowgrams[index * numFlowCells + j];k++) {
						newSeq += base;
					}
				}

				if (newSeq.length() >= 4) { fastaFile << newSeq.substr(4) << endl; }
				else { fastaFile << "NNNN" << endl; }
			}
		}
		fastaFile.close();

		outputNames.push_back(fastaFileName); outputTypes["fasta"].push_back(fastaFileName);

		if (thisCompositeFASTAFileName != "") {
			File::appendFiles(fastaFileName, thisCompositeFASTAFileName);
		}
	}

	/**************************************************************************************************/

	void ShhherCommand::writeNames(string thisCompositeNamesFileName, int numOTUs, string nameFileName, vector<int> otuCounts, vector<string>& seqNameVector, vector<vector<int> >& aaI, vector<int>& nSeqsPerOTU) {

		ofstream nameFile;
		File::openOutputFile(nameFileName, nameFile);

		for (int i = 0;i < numOTUs;i++) {

			if (ctrlc_pressed) { break; }

			if (otuCounts[i] > 0) {
				nameFile << seqNameVector[aaI[i][0]] << '\t' << seqNameVector[aaI[i][0]];

				for (int j = 1;j < nSeqsPerOTU[i];j++) {
					nameFile << ',' << seqNameVector[aaI[i][j]];
				}

				nameFile << endl;
			}
		}
		nameFile.close();
		outputNames.push_back(nameFileName); outputTypes["name"].push_back(nameFileName);


		if (thisCompositeNamesFileName != "") {
			File::appendFiles(nameFileName, thisCompositeNamesFileName);
		}
	}

	/**************************************************************************************************/

	void ShhherCommand::writeGroups(string groupFileName, string fileRoot, int numSeqs, vector<string>& seqNameVector) {
		ofstream groupFile;
		File::openOutputFile(groupFileName, groupFile);

		for (int i = 0;i < numSeqs;i++) {
			if (ctrlc_pressed) { break; }
			groupFile << seqNameVector[i] << '\t' << fileRoot << endl;
		}
		groupFile.close();
		outputNames.push_back(groupFileName); outputTypes["group"].push_back(groupFileName);

	}

	/**************************************************************************************************/

	void ShhherCommand::writeClusters(string otuCountsFileName, int numOTUs, int numFlowCells, vector<int> otuCounts, vector<int>& centroids, vector<short>& uniqueFlowgrams, vector<string>& seqNameVector, vector<vector<int> >& aaI, vector<int>& nSeqsPerOTU, vector<int>& lengths, vector<short>& flowDataIntI) {
		ofstream otuCountsFile;
		File::openOutputFile(otuCountsFileName, otuCountsFile);

		string bases = flowOrder;

		for (int i = 0;i < numOTUs;i++) {

			if (ctrlc_pressed) {
				break;
			}
			//output the translated version of the centroid sequence for the otu
			if (otuCounts[i] > 0) {
				int index = centroids[i];

				otuCountsFile << "ideal\t";
				for (int j = 8;j < numFlowCells;j++) {
					char base = bases[j % bases.length()];
					for (int s = 0;s < uniqueFlowgrams[index * numFlowCells + j];s++) {
						otuCountsFile << base;
					}
				}
				otuCountsFile << endl;

				for (int j = 0;j < nSeqsPerOTU[i];j++) {
					int sequence = aaI[i][j];
					otuCountsFile << seqNameVector[sequence] << '\t';

					string newSeq = "";

					for (int k = 0;k < lengths[sequence];k++) {
						char base = bases[k % bases.length()];
						int freq = int(0.01 * (double)flowDataIntI[sequence * numFlowCells + k] + 0.5);

						for (int s = 0;s < freq;s++) {
							newSeq += base;
							//otuCountsFile << base;
						}
					}

					if (newSeq.length() >= 4) { otuCountsFile << newSeq.substr(4) << endl; }
					else { otuCountsFile << "NNNN" << endl; }
				}
				otuCountsFile << endl;
			}
		}
		otuCountsFile.close();
		outputNames.push_back(otuCountsFileName); outputTypes["counts"].push_back(otuCountsFileName);

	}

	/**************************************************************************************************/

	void ShhherCommand::getSingleLookUp() {
		try {
			//	these are the -log probabilities that a signal corresponds to a particular homopolymer length
			singleLookUp.assign(HOMOPS * NUMBINS, 0);

			int index = 0;
			ifstream lookUpFile;
			File::openInputFile(lookupFileName, lookUpFile);

			for (int i = 0;i < HOMOPS;i++) {

				if (ctrlc_pressed) { break; }

				float logFracFreq;
				lookUpFile >> logFracFreq;

				for (int j = 0;j < NUMBINS;j++) {
					lookUpFile >> singleLookUp[index];
					index++;
				}
			}
			lookUpFile.close();
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, getSingleLookUp";
			exit(1);
		}
	}

	/**************************************************************************************************/

	void ShhherCommand::getJointLookUp() {
		try {

			//	the most likely joint probability (-log) that two intenities have the same polymer length
			jointLookUp.resize(NUMBINS * NUMBINS, 0);

			for (int i = 0;i < NUMBINS;i++) {

				if (ctrlc_pressed) { break; }

				for (int j = 0;j < NUMBINS;j++) {

					double minSum = 100000000;

					for (int k = 0;k < HOMOPS;k++) {
						double sum = singleLookUp[k * NUMBINS + i] + singleLookUp[k * NUMBINS + j];

						if (sum < minSum) { minSum = sum; }
					}
					jointLookUp[i * NUMBINS + j] = minSum;
				}
			}
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, getJointLookUp";
			exit(1);
		}
	}

	/**************************************************************************************************/

	double ShhherCommand::getProbIntensity(int intIntensity) {
		try {

			double minNegLogProb = 100000000;


			for (int i = 0;i < HOMOPS;i++) {//loop signal strength

				if (ctrlc_pressed) { break; }

				float negLogProb = singleLookUp[i * NUMBINS + intIntensity];
				if (negLogProb < minNegLogProb) { minNegLogProb = negLogProb; }
			}

			return minNegLogProb;
		}
		catch (exception& e) {
			LOG(FATAL) << e.what() << " in ShhherCommand, getProbIntensity";
			exit(1);
		}
	}




