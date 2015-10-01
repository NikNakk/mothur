/*
 *  chimerabellerophoncommand.cpp
 *  Mothur
 *
 *  Created by westcott on 4/1/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "chimerabellerophoncommand.h"
#include "bellerophon.h"

 //**********************************************************************************************************************
vector<string> ChimeraBellerophonCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "chimera-accnos", false, true, true); parameters.push_back(pfasta);
		nkParameters.add(new BooleanParameter("filter", false, false, false));
		nkParameters.add(new BooleanParameter("correction", true, false, false));
		nkParameters.add(new NumberParameter("window", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("increment", -INFINITY, INFINITY, 25, false, false));
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraBellerophonCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraBellerophonCommand::getHelpString() {
	try {
		string helpString = "The chimera.bellerophon command reads a fastafile and creates list of potentially chimeric sequences.\n"
			"The chimera.bellerophon command parameters are fasta, filter, correction, processors, window, increment. The fasta parameter is required, unless you have a valid current file.\n"
			"The fasta parameter is required.  You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amzon.fasta \n"
			"The filter parameter allows you to specify if you would like to apply a vertical and 50% soft filter, default=false. \n"
			"The correction parameter allows you to put more emphasis on the distance between highly similar sequences and less emphasis on the differences between remote homologs, default=true.\n"
			"The processors parameter allows you to specify how many processors you would like to use.  The default is 1. \n"
			"The window parameter allows you to specify the window size for searching for chimeras, default is 1/4 sequence length. \n"
			"The increment parameter allows you to specify how far you move each window while finding chimeric sequences, default is 25.\n"
			"chimera.bellerophon(fasta=yourFastaFile, filter=yourFilter, correction=yourCorrection, processors=yourProcessors) \n"
			"Example: chimera.bellerophon(fasta=AD.align, filter=True, correction=true, window=200) \n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraBellerophonCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraBellerophonCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "chimera") { pattern = "[filename],bellerophon.chimeras"; }
	else if (type == "accnos") { pattern = "[filename],bellerophon.accnos"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ChimeraBellerophonCommand::ChimeraBellerophonCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraBellerophonCommand, ChimeraBellerophonCommand";
		exit(1);
	}
}
//***************************************************************************************************************
ChimeraBellerophonCommand::ChimeraBellerophonCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {

		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter("chimera.bellerophon");
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }

		fastafile = validParameter.validFile(parameters, "fasta", false);
		if (fastafile == "not found") {
			//if there is a current fasta file, use it
			string filename = settings.getCurrent("fasta");
			if (filename != "") { fastaFileNames.push_back(filename); LOG(INFO) << "Using " + filename + " as input file for the fasta parameter." << '\n'; }
			else { LOG(INFO) << "You have no current fastafile and the fasta parameter is required." << '\n'; abort = true; }
		}
		else {
			Utility::split(fastafile, '-', fastaFileNames);

			//go through files and make sure they are good, if not, then disregard them
			for (int i = 0; i < fastaFileNames.size(); i++) {

				bool ignore = false;
				if (fastaFileNames[i] == "current") {
					fastaFileNames[i] = settings.getCurrent("fasta");
					if (fastaFileNames[i] != "") { LOG(INFO) << "Using " + fastaFileNames[i] + " as input file for the fasta parameter where you had given current." << '\n'; }
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

					int ableToOpen;
					ifstream in;

					ableToOpen = File::openInputFile(fastaFileNames[i], in, "noerror");

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
					else {
						settings.setCurrent("fasta", fastaFileNames[i]);
					}
				}
			}

			//make sure there is at least one valid file left
			if (fastaFileNames.size() == 0) { LOG(INFO) << "no valid files." << '\n'; abort = true; }
		}

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		string temp;
		temp = validParameter.validFile(parameters, "filter", false);			if (temp == "not found") { temp = "F"; }
		filter = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "correction", false);		if (temp == "not found") { temp = "T"; }
		correction = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "window", false);			if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, window);

		temp = validParameter.validFile(parameters, "increment", false);		if (temp == "not found") { temp = "25"; }
		Utility::mothurConvert(temp, increment);
	}
}
//***************************************************************************************************************
int ChimeraBellerophonCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		for (int i = 0; i < fastaFileNames.size(); i++) {

			LOG(INFO) << "Checking sequences from " + fastaFileNames[i] + " ..." << '\n';

			int start = time(NULL);

			chimera = new Bellerophon(fastaFileNames[i], filter, correction, window, increment, processors, outputDir);

			if (outputDir == "") { outputDir = File::getPath(fastaFileNames[i]); }//if user entered a file with a path then preserve it	
			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[i]));
			string outputFileName = getOutputFileName("chimera", variables);
			string accnosFileName = getOutputFileName("accnos", variables);

			chimera->getChimeras();

			if (ctrlc_pressed) { delete chimera; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear(); return 0; }

			ofstream out;
			File::openOutputFile(outputFileName, out);

			ofstream out2;
			File::openOutputFile(accnosFileName, out2);

			numSeqs = chimera->print(out, out2, "");
			out.close();
			out2.close();

			if (ctrlc_pressed) { File::remove(accnosFileName); File::remove(outputFileName); for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear(); delete chimera;	return 0; }

			LOG(INFO) << std::endl << "It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences." << '\n' << '\n';

			outputNames.push_back(outputFileName);  outputTypes["chimera"].push_back(outputFileName);
			outputNames.push_back(accnosFileName);  outputTypes["accnos"].push_back(accnosFileName);

			delete chimera;
		}

		//set accnos file as new current accnosfile
		string current = "";
		itTypes = outputTypes.find("accnos");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("accnos", current); }
		}

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraBellerophonCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************

