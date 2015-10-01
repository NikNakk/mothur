/*
 *  chimeraperseuscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 10/26/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "chimeraperseuscommand.h"
#include "deconvolutecommand.h"
#include "sequence.hpp"
#include "counttable.h"
#include "sequencecountparser.h"
 //**********************************************************************************************************************
vector<string> ChimeraPerseusCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "chimera-accnos", false, true, true); parameters.push_back(pfasta);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "NameCount", "none", "", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "NameCount", "none", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none", "", false, false, true); parameters.push_back(pgroup);
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new BooleanParameter("dereplicate", false, false, false));

		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));
		nkParameters.add(new NumberParameter("cutoff", -INFINITY, INFINITY, 0.5, false, false));
		nkParameters.add(new NumberParameter("alpha", -INFINITY, INFINITY, -5.54, false, false));
		nkParameters.add(new NumberParameter("beta", -INFINITY, INFINITY, 0.33, false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraPerseusCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraPerseusCommand::getHelpString() {
	try {
		string helpString = "The chimera.perseus command reads a fastafile and namefile or countfile and outputs potentially chimeric sequences.\n"
			"The chimera.perseus command parameters are fasta, name, group, cutoff, processors, dereplicate, alpha and beta.\n"
			"The fasta parameter allows you to enter the fasta file containing your potentially chimeric sequences, and is required, unless you have a valid current fasta file. \n"
			"The name parameter allows you to provide a name file associated with your fasta file.\n"
			"The count parameter allows you to provide a count file associated with your fasta file. A count or name file is required. When you use a count file with group info and dereplicate=T, mothur will create a *.pick.count_table file containing seqeunces after chimeras are removed.\n"
			"You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amazon.fasta \n"
			"The group parameter allows you to provide a group file.  When checking sequences, only sequences from the same group as the query sequence will be used as the reference. \n"
			"The processors parameter allows you to specify how many processors you would like to use.  The default is 1. \n"
			"If the dereplicate parameter is false, then if one group finds the seqeunce to be chimeric, then all groups find it to be chimeric, default=f.\n"
			"The alpha parameter ....  The default is -5.54. \n"
			"The beta parameter ....  The default is 0.33. \n"
			"The cutoff parameter ....  The default is 0.50. \n"
			"The chimera.perseus command should be in the following format: \n"
			"chimera.perseus(fasta=yourFastaFile, name=yourNameFile) \n"
			"Example: chimera.perseus(fasta=AD.align, name=AD.names) \n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraPerseusCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraPerseusCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "chimera") { pattern = "[filename],perseus.chimeras"; }
	else if (type == "accnos") { pattern = "[filename],perseus.accnos"; }
	else if (type == "count") { pattern = "[filename],perseus.pick.count_table"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ChimeraPerseusCommand::ChimeraPerseusCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraPerseusCommand, ChimeraPerseusCommand";
		exit(1);
	}
}
//***************************************************************************************************************
ChimeraPerseusCommand::ChimeraPerseusCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	hasCount = false;
	hasName = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter("chimera.perseus");
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["count"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }

		//check for required parameters
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
			if (fastaFileNames.size() == 0) { LOG(LOGERROR) << "no valid files." << '\n'; abort = true; }
		}


		//check for required parameters
		namefile = validParameter.validFile(parameters, "name", false);
		if (namefile == "not found") { namefile = ""; }
		else {
			Utility::split(namefile, '-', nameFileNames);

			//go through files and make sure they are good, if not, then disregard them
			for (int i = 0; i < nameFileNames.size(); i++) {

				bool ignore = false;
				if (nameFileNames[i] == "current") {
					nameFileNames[i] = settings.getCurrent("name");
					if (nameFileNames[i] != "") { LOG(INFO) << "Using " + nameFileNames[i] + " as input file for the name parameter where you had given current." << '\n'; }
					else {
						LOG(INFO) << "You have no current namefile, ignoring current." << '\n'; ignore = true;
						//erase from file list
						nameFileNames.erase(nameFileNames.begin() + i);
						i--;
					}
				}

				if (!ignore) {

					if (inputDir != "") {
						string path = File::getPath(nameFileNames[i]);
						//if the user has not given a path then, add inputdir. else leave path alone.
						if (path == "") { nameFileNames[i] = inputDir + nameFileNames[i]; }
					}

					int ableToOpen;
					ifstream in;

					ableToOpen = File::openInputFile(nameFileNames[i], in, "noerror");

					//if you can't open it, try default location
					if (ableToOpen == 1) {
						if (settings.getDefaultPath() != "") { //default path is set
							string tryPath = settings.getDefaultPath() + File::getSimpleName(nameFileNames[i]);
							LOG(INFO) << "Unable to open " + nameFileNames[i] + ". Trying default " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							nameFileNames[i] = tryPath;
						}
					}

					if (ableToOpen == 1) {
						if (settings.getOutputDir() != "") { //default path is set
							string tryPath = settings.getOutputDir() + File::getSimpleName(nameFileNames[i]);
							LOG(INFO) << "Unable to open " + nameFileNames[i] + ". Trying output directory " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							nameFileNames[i] = tryPath;
						}
					}

					in.close();

					if (ableToOpen == 1) {
						LOG(INFO) << "Unable to open " + nameFileNames[i] + ". It will be disregarded." << '\n';
						//erase from file list
						nameFileNames.erase(nameFileNames.begin() + i);
						i--;
					}
					else {
						settings.setCurrent("name", nameFileNames[i]);
					}
				}
			}
		}

		if (nameFileNames.size() != 0) { hasName = true; }

		//check for required parameters
		vector<string> countfileNames;
		countfile = validParameter.validFile(parameters, "count", false);
		if (countfile == "not found") {
			countfile = "";
		}
		else {
			Utility::split(countfile, '-', countfileNames);

			//go through files and make sure they are good, if not, then disregard them
			for (int i = 0; i < countfileNames.size(); i++) {

				bool ignore = false;
				if (countfileNames[i] == "current") {
					countfileNames[i] = settings.getCurrent("counttable");
					if (countfileNames[i] != "") { LOG(INFO) << "Using " + countfileNames[i] + " as input file for the count parameter where you had given current." << '\n'; }
					else {
						LOG(INFO) << "You have no current count file, ignoring current." << '\n'; ignore = true;
						//erase from file list
						countfileNames.erase(countfileNames.begin() + i);
						i--;
					}
				}

				if (!ignore) {

					if (inputDir != "") {
						string path = File::getPath(countfileNames[i]);
						//if the user has not given a path then, add inputdir. else leave path alone.
						if (path == "") { countfileNames[i] = inputDir + countfileNames[i]; }
					}

					int ableToOpen;
					ifstream in;

					ableToOpen = File::openInputFile(countfileNames[i], in, "noerror");

					//if you can't open it, try default location
					if (ableToOpen == 1) {
						if (settings.getDefaultPath() != "") { //default path is set
							string tryPath = settings.getDefaultPath() + File::getSimpleName(countfileNames[i]);
							LOG(INFO) << "Unable to open " + countfileNames[i] + ". Trying default " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							countfileNames[i] = tryPath;
						}
					}

					if (ableToOpen == 1) {
						if (settings.getOutputDir() != "") { //default path is set
							string tryPath = settings.getOutputDir() + File::getSimpleName(countfileNames[i]);
							LOG(INFO) << "Unable to open " + countfileNames[i] + ". Trying output directory " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							countfileNames[i] = tryPath;
						}
					}

					in.close();

					if (ableToOpen == 1) {
						LOG(INFO) << "Unable to open " + countfileNames[i] + ". It will be disregarded." << '\n';
						//erase from file list
						countfileNames.erase(countfileNames.begin() + i);
						i--;
					}
					else {
						settings.setCurrent("counttable", countfileNames[i]);
					}
				}
			}
		}

		if (countfileNames.size() != 0) { hasCount = true; }

		//make sure there is at least one valid file left
		if (hasName && hasCount) { LOG(LOGERROR) << "You must enter ONLY ONE of the following: count or name." << '\n'; abort = true; }

		if (!hasName && !hasCount) {
			//if there is a current name file, use it, else look for current count file
			string filename = settings.getCurrent("name");
			if (filename != "") { hasName = true; nameFileNames.push_back(filename); LOG(INFO) << "Using " + filename + " as input file for the name parameter." << '\n'; }
			else {
				filename = settings.getCurrent("counttable");
				if (filename != "") { hasCount = true; countfileNames.push_back(filename); LOG(INFO) << "Using " + filename + " as input file for the count parameter." << '\n'; }
				else { LOG(LOGERROR) << "You must provide a count or name file." << '\n'; abort = true; }
			}
		}
		if (!hasName && hasCount) { nameFileNames = countfileNames; }

		if (nameFileNames.size() != fastaFileNames.size()) { LOG(LOGERROR) << "The number of name or count files does not match the number of fastafiles, please correct." << '\n'; abort = true; }

		bool hasGroup = true;
		groupfile = validParameter.validFile(parameters, "group", false);
		if (groupfile == "not found") { groupfile = "";  hasGroup = false; }
		else {
			Utility::split(groupfile, '-', groupFileNames);

			//go through files and make sure they are good, if not, then disregard them
			for (int i = 0; i < groupFileNames.size(); i++) {

				bool ignore = false;
				if (groupFileNames[i] == "current") {
					groupFileNames[i] = settings.getCurrent("group");
					if (groupFileNames[i] != "") { LOG(INFO) << "Using " + groupFileNames[i] + " as input file for the group parameter where you had given current." << '\n'; }
					else {
						LOG(INFO) << "You have no current namefile, ignoring current." << '\n'; ignore = true;
						//erase from file list
						groupFileNames.erase(groupFileNames.begin() + i);
						i--;
					}
				}

				if (!ignore) {

					if (inputDir != "") {
						string path = File::getPath(groupFileNames[i]);
						//if the user has not given a path then, add inputdir. else leave path alone.
						if (path == "") { groupFileNames[i] = inputDir + groupFileNames[i]; }
					}

					int ableToOpen;
					ifstream in;

					ableToOpen = File::openInputFile(groupFileNames[i], in, "noerror");

					//if you can't open it, try default location
					if (ableToOpen == 1) {
						if (settings.getDefaultPath() != "") { //default path is set
							string tryPath = settings.getDefaultPath() + File::getSimpleName(groupFileNames[i]);
							LOG(INFO) << "Unable to open " + groupFileNames[i] + ". Trying default " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							groupFileNames[i] = tryPath;
						}
					}

					if (ableToOpen == 1) {
						if (settings.getOutputDir() != "") { //default path is set
							string tryPath = settings.getOutputDir() + File::getSimpleName(groupFileNames[i]);
							LOG(INFO) << "Unable to open " + groupFileNames[i] + ". Trying output directory " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							groupFileNames[i] = tryPath;
						}
					}

					in.close();

					if (ableToOpen == 1) {
						LOG(INFO) << "Unable to open " + groupFileNames[i] + ". It will be disregarded." << '\n';
						//erase from file list
						groupFileNames.erase(groupFileNames.begin() + i);
						i--;
					}
					else {
						settings.setCurrent("group", groupFileNames[i]);
					}
				}
			}

			//make sure there is at least one valid file left
			if (groupFileNames.size() == 0) { LOG(LOGERROR) << "no valid group files." << '\n'; abort = true; }
		}

		if (hasGroup && (groupFileNames.size() != fastaFileNames.size())) { LOG(LOGERROR) << "The number of groupfiles does not match the number of fastafiles, please correct." << '\n'; abort = true; }

		if (hasGroup && hasCount) { LOG(LOGERROR) << "You must enter ONLY ONE of the following: count or group." << '\n'; abort = true; }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		string temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "cutoff", false);	if (temp == "not found") { temp = "0.50"; }
		Utility::mothurConvert(temp, cutoff);

		temp = validParameter.validFile(parameters, "alpha", false);	if (temp == "not found") { temp = "-5.54"; }
		Utility::mothurConvert(temp, alpha);

		temp = validParameter.validFile(parameters, "cutoff", false);	if (temp == "not found") { temp = "0.33"; }
		Utility::mothurConvert(temp, beta);

		temp = validParameter.validFile(parameters, "dereplicate", false);
		if (temp == "not found") { temp = "false"; }
		dups = m->isTrue(temp);
	}
}
//***************************************************************************************************************

int ChimeraPerseusCommand::execute() {
	try {
		if (abort == true) { if (calledHelp) { return 0; }  return 2; }


		//process each file
		for (int s = 0; s < fastaFileNames.size(); s++) {

			LOG(INFO) << "Checking sequences from " + fastaFileNames[s] + " ..." << '\n';

			int start = time(NULL);
			if (outputDir == "") { outputDir = File::getPath(fastaFileNames[s]); }//if user entered a file with a path then preserve it	
			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[s]));
			string outputFileName = getOutputFileName("chimera", variables);
			string accnosFileName = getOutputFileName("accnos", variables);
			string newCountFile = "";

			//string newFasta = File::getRootName(fastaFileNames[s]) + "temp";

			//you provided a groupfile
			string groupFile = "";
			if (groupFileNames.size() != 0) { groupFile = groupFileNames[s]; }

			string nameFile = "";
			if (nameFileNames.size() != 0) { //you provided a namefile and we don't need to create one
				nameFile = nameFileNames[s];
			}
			else { nameFile = getNamesFile(fastaFileNames[s]); }

			if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } return 0; }

			int numSeqs = 0;
			int numChimeras = 0;

			if (hasCount) {
				CountTable* ct = new CountTable();
				ct->readTable(nameFile, true, false);

				if (ct->hasGroupInfo()) {
					cparser = new SequenceCountParser(fastaFileNames[s], *ct);
					variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(nameFile));
					newCountFile = getOutputFileName("count", variables);

					vector<string> groups = cparser->getNamesOfGroups();

					if (ctrlc_pressed) { delete ct; delete cparser; for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

					//clears files
					ofstream out, out1, out2;
					File::openOutputFile(outputFileName, out); out.close();
					File::openOutputFile(accnosFileName, out1); out1.close();

					if (processors == 1) {
						numSeqs = driverGroups(outputFileName, accnosFileName, newCountFile, 0, groups.size(), groups);
						if (dups) {
							CountTable c; c.readTable(nameFile, true, false);
							if (!File::isBlank(newCountFile)) {
								ifstream in2;
								File::openInputFile(newCountFile, in2);

								string name, group;
								while (!in2.eof()) {
									in2 >> name >> group; File::gobble(in2);
									c.setAbund(name, group, 0);
								}
								in2.close();
							}
							File::remove(newCountFile);
							c.printTable(newCountFile);
						}

					}
					else { numSeqs = createProcessesGroups(outputFileName, accnosFileName, newCountFile, groups, groupFile, fastaFileNames[s], nameFile); }

					if (ctrlc_pressed) { delete ct; delete cparser; for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }
					map<string, string> uniqueNames = cparser->getAllSeqsMap();
					if (!dups) {
						numChimeras = deconvoluteResults(uniqueNames, outputFileName, accnosFileName);
					}
					else {
						set<string> doNotRemove;
						CountTable c; c.readTable(newCountFile, true, true);
						vector<string> namesInTable = c.getNamesOfSeqs();
						for (int i = 0; i < namesInTable.size(); i++) {
							int temp = c.getNumSeqs(namesInTable[i]);
							if (temp == 0) { c.remove(namesInTable[i]); }
							else { doNotRemove.insert((namesInTable[i])); }
						}
						//remove names we want to keep from accnos file.
						set<string> accnosNames = m->readAccnos(accnosFileName);
						ofstream out2;
						File::openOutputFile(accnosFileName, out2);
						for (set<string>::iterator it = accnosNames.begin(); it != accnosNames.end(); it++) {
							if (doNotRemove.count(*it) == 0) { out2 << (*it) << endl; }
						}
						out2.close();
						c.printTable(newCountFile);
						outputNames.push_back(newCountFile); outputTypes["count"].push_back(newCountFile);

					}
					delete cparser;

					LOG(INFO) << "The number of sequences checked may be larger than the number of unique sequences because some sequences are found in several samples." << '\n';

					if (ctrlc_pressed) { delete ct; for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

				}
				else {
					if (processors != 1) { LOG(INFO) << "Your count file does not contain group information, mothur can only use 1 processor, continuing." << '\n'; processors = 1; }

					//read sequences and store sorted by frequency
					vector<seqData> sequences = readFiles(fastaFileNames[s], ct);

					if (ctrlc_pressed) { delete ct; for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } return 0; }

					numSeqs = driver(outputFileName, sequences, accnosFileName, numChimeras);
				}
				delete ct;
			}
			else {
				if (groupFile != "") {
					//Parse sequences by group
					parser = new SequenceParser(groupFile, fastaFileNames[s], nameFile);
					vector<string> groups = parser->getNamesOfGroups();

					if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

					//clears files
					ofstream out, out1, out2;
					File::openOutputFile(outputFileName, out); out.close();
					File::openOutputFile(accnosFileName, out1); out1.close();

					if (processors == 1) { numSeqs = driverGroups(outputFileName, accnosFileName, "", 0, groups.size(), groups); }
					else { numSeqs = createProcessesGroups(outputFileName, accnosFileName, "", groups, groupFile, fastaFileNames[s], nameFile); }

					if (ctrlc_pressed) { delete parser; for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }
					map<string, string> uniqueNames = parser->getAllSeqsMap();
					if (!dups) {
						numChimeras = deconvoluteResults(uniqueNames, outputFileName, accnosFileName);
					}
					delete parser;

					LOG(INFO) << "The number of sequences checked may be larger than the number of unique sequences because some sequences are found in several samples." << '\n';

					if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }
				}
				else {
					if (processors != 1) { LOG(INFO) << "Without a groupfile, mothur can only use 1 processor, continuing." << '\n'; processors = 1; }

					//read sequences and store sorted by frequency
					vector<seqData> sequences = readFiles(fastaFileNames[s], nameFile);

					if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } return 0; }

					numSeqs = driver(outputFileName, sequences, accnosFileName, numChimeras);
				}
			}

			if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } return 0; }

			LOG(INFO) << '\n' << "It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences. " + toString(numChimeras) + " chimeras were found." << '\n';
			outputNames.push_back(outputFileName); outputTypes["chimera"].push_back(outputFileName);
			outputNames.push_back(accnosFileName); outputTypes["accnos"].push_back(accnosFileName);
		}

		//set accnos file as new current accnosfile
		string current = "";
		itTypes = outputTypes.find("accnos");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("accnos", current); }
		}

		itTypes = outputTypes.find("count");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
		}

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraPerseusCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraPerseusCommand::getNamesFile(string& inputFile) {
	string nameFile = "";

	LOG(INFO) << std::endl << "No namesfile given, running unique.seqs command to generate one." << '\n' << '\n';

	//use unique.seqs to create new name and fastafile
	string inputString = "fasta=" + inputFile;
	LOG(INFO) << "/******************************************/" << '\n';
	LOG(INFO) << "Running command: unique.seqs(" + inputString + ")" << '\n';
	m->mothurCalling = true;

	Command* uniqueCommand = new DeconvoluteCommand(inputString);
	uniqueCommand->execute();

	map<string, vector<string> > filenames = uniqueCommand->getOutputFiles();

	delete uniqueCommand;
	m->mothurCalling = false;
	LOG(INFO) << "/******************************************/" << '\n';

	nameFile = filenames["name"][0];
	inputFile = filenames["fasta"][0];

	return nameFile;
}
//**********************************************************************************************************************
int ChimeraPerseusCommand::driverGroups(string outputFName, string accnos, string countlist, int start, int end, vector<string> groups) {

	int totalSeqs = 0;
	int numChimeras = 0;

	ofstream outCountList;
	if (hasCount && dups) { File::openOutputFile(countlist, outCountList); }

	for (int i = start; i < end; i++) {

		LOG(INFO) << '\n' << "Checking sequences from group " + groups[i] + "..." << '\n';

		int start = time(NULL);	 if (ctrlc_pressed) { return 0; }

		vector<seqData> sequences = loadSequences(groups[i]);

		if (ctrlc_pressed) { return 0; }

		int numSeqs = driver((outputFName + groups[i]), sequences, (accnos + groups[i]), numChimeras);
		totalSeqs += numSeqs;

		if (ctrlc_pressed) { return 0; }

		if (dups) {
			if (!File::isBlank(accnos + groups[i])) {
				ifstream in;
				File::openInputFile(accnos + groups[i], in);
				string name;
				if (hasCount) {
					while (!in.eof()) {
						in >> name; File::gobble(in);
						outCountList << name << '\t' << groups[i] << endl;
					}
					in.close();
				}
				else {
					map<string, string> thisnamemap = parser->getNameMap(groups[i]);
					map<string, string>::iterator itN;
					ofstream out;
					File::openOutputFile(accnos + groups[i] + ".temp", out);
					while (!in.eof()) {
						in >> name; File::gobble(in);
						itN = thisnamemap.find(name);
						if (itN != thisnamemap.end()) {
							vector<string> tempNames; m->splitAtComma(itN->second, tempNames);
							for (int j = 0; j < tempNames.size(); j++) { out << tempNames[j] << endl; }

						}
						else { LOG(LOGERROR) << "parsing cannot find " + name + ".\n"; ctrlc_pressed = true; }
					}
					out.close();
					in.close();
					m->renameFile(accnos + groups[i] + ".temp", accnos + groups[i]);
				}

			}
		}

		//append files
		File::appendFiles((outputFName + groups[i]), outputFName); File::remove((outputFName + groups[i]));
		File::appendFiles((accnos + groups[i]), accnos); File::remove((accnos + groups[i]));

		LOG(INFO) << '\n' << "It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences from group " + groups[i] + "." << '\n';
	}

	if (hasCount && dups) { outCountList.close(); }

	return totalSeqs;

}
//**********************************************************************************************************************
vector<seqData> ChimeraPerseusCommand::loadSequences(string group) {
	bool error = false;
	alignLength = 0;
	vector<seqData> sequences;
	if (hasCount) {
		vector<Sequence> thisGroupsSeqs = cparser->getSeqs(group);
		map<string, int> counts = cparser->getCountTable(group);
		map<string, int>::iterator it;

		for (int i = 0; i < thisGroupsSeqs.size(); i++) {

			if (ctrlc_pressed) { return sequences; }

			it = counts.find(thisGroupsSeqs[i].getName());
			if (it == counts.end()) { error = true; LOG(LOGERROR) << "" + thisGroupsSeqs[i].getName() + " is in your fasta file and not in your count file, please correct." << '\n'; }
			else {
				thisGroupsSeqs[i].setAligned(removeNs(thisGroupsSeqs[i].getUnaligned()));
				sequences.push_back(seqData(thisGroupsSeqs[i].getName(), thisGroupsSeqs[i].getUnaligned(), it->second));
				if (thisGroupsSeqs[i].getUnaligned().length() > alignLength) { alignLength = thisGroupsSeqs[i].getUnaligned().length(); }
			}
		}
	}
	else {
		vector<Sequence> thisGroupsSeqs = parser->getSeqs(group);
		map<string, string> nameMap = parser->getNameMap(group);
		map<string, string>::iterator it;

		for (int i = 0; i < thisGroupsSeqs.size(); i++) {

			if (ctrlc_pressed) { return sequences; }

			it = nameMap.find(thisGroupsSeqs[i].getName());
			if (it == nameMap.end()) { error = true; LOG(LOGERROR) << "" + thisGroupsSeqs[i].getName() + " is in your fasta file and not in your namefile, please correct." << '\n'; }
			else {
				int num = m->getNumNames(it->second);
				thisGroupsSeqs[i].setAligned(removeNs(thisGroupsSeqs[i].getUnaligned()));
				sequences.push_back(seqData(thisGroupsSeqs[i].getName(), thisGroupsSeqs[i].getUnaligned(), num));
				if (thisGroupsSeqs[i].getUnaligned().length() > alignLength) { alignLength = thisGroupsSeqs[i].getUnaligned().length(); }
			}
		}

	}

	if (error) { ctrlc_pressed = true; }
	//sort by frequency
	sort(sequences.rbegin(), sequences.rend());

	return sequences;
}

//**********************************************************************************************************************
vector<seqData> ChimeraPerseusCommand::readFiles(string inputFile, string name) {
	map<string, int>::iterator it;
	map<string, int> nameMap = m->readNames(name);

	//read fasta file and create sequenceData structure - checking for file mismatches
	vector<seqData> sequences;
	bool error = false;
	ifstream in;
	File::openInputFile(inputFile, in);
	alignLength = 0;

	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); return sequences; }

		Sequence temp(in); File::gobble(in);

		it = nameMap.find(temp.getName());
		if (it == nameMap.end()) { error = true; LOG(LOGERROR) << "" + temp.getName() + " is in your fasta file and not in your namefile, please correct." << '\n'; }
		else {
			temp.setAligned(removeNs(temp.getUnaligned()));
			sequences.push_back(seqData(temp.getName(), temp.getUnaligned(), it->second));
			if (temp.getUnaligned().length() > alignLength) { alignLength = temp.getUnaligned().length(); }
		}
	}
	in.close();

	if (error) { ctrlc_pressed = true; }

	//sort by frequency
	sort(sequences.rbegin(), sequences.rend());

	return sequences;
}
//**********************************************************************************************************************
string ChimeraPerseusCommand::removeNs(string seq) {
	string newSeq = "";
	for (int i = 0; i < seq.length(); i++) {
		if (seq[i] != 'N') { newSeq += seq[i]; }
	}
	return newSeq;
}
//**********************************************************************************************************************
vector<seqData> ChimeraPerseusCommand::readFiles(string inputFile, CountTable* ct) {
	try {
		//read fasta file and create sequenceData structure - checking for file mismatches
		vector<seqData> sequences;
		ifstream in;
		File::openInputFile(inputFile, in);
		alignLength = 0;

		while (!in.eof()) {
			Sequence temp(in); File::gobble(in);

			int count = ct->getNumSeqs(temp.getName());
			if (ctrlc_pressed) { break; }
			else {
				temp.setAligned(removeNs(temp.getUnaligned()));
				sequences.push_back(seqData(temp.getName(), temp.getUnaligned(), count));
				if (temp.getUnaligned().length() > alignLength) { alignLength = temp.getUnaligned().length(); }
			}
		}
		in.close();

		//sort by frequency
		sort(sequences.rbegin(), sequences.rend());

		return sequences;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraPerseusCommand, getNamesFile";
		exit(1);
	}
}
//**********************************************************************************************************************
int ChimeraPerseusCommand::driver(string chimeraFileName, vector<seqData>& sequences, string accnosFileName, int& numChimeras) {

	vector<vector<double> > correctModel(4);	//could be an option in the future to input own model matrix
	for (int i = 0;i < 4;i++) { correctModel[i].resize(4); }

	correctModel[0][0] = 0.000000;	//AA
	correctModel[1][0] = 11.619259;	//CA
	correctModel[2][0] = 11.694004;	//TA
	correctModel[3][0] = 7.748623;	//GA

	correctModel[1][1] = 0.000000;	//CC
	correctModel[2][1] = 7.619657;	//TC
	correctModel[3][1] = 12.852562;	//GC

	correctModel[2][2] = 0.000000;	//TT
	correctModel[3][2] = 10.964048;	//TG

	correctModel[3][3] = 0.000000;	//GG

	for (int i = 0;i < 4;i++) {
		for (int j = 0;j < i;j++) {
			correctModel[j][i] = correctModel[i][j];
		}
	}

	int numSeqs = sequences.size();
	//int alignLength = sequences[0].sequence.size();

	ofstream chimeraFile;
	ofstream accnosFile;
	File::openOutputFile(chimeraFileName, chimeraFile);
	File::openOutputFile(accnosFileName, accnosFile);

	Perseus myPerseus;
	vector<vector<double> > binMatrix = myPerseus.binomial(alignLength);

	chimeraFile << "SequenceIndex\tName\tDiffsToBestMatch\tBestMatchIndex\tBestMatchName\tDiffstToChimera\tIndexofLeftParent\tIndexOfRightParent\tNameOfLeftParent\tNameOfRightParent\tDistanceToBestMatch\tcIndex\t(cIndex - singleDist)\tloonIndex\tMismatchesToChimera\tMismatchToTrimera\tChimeraBreakPoint\tLogisticProbability\tTypeOfSequence\n";

	vector<bool> chimeras(numSeqs, 0);

	for (int i = 0;i < numSeqs;i++) {
		if (ctrlc_pressed) { chimeraFile.close(); File::remove(chimeraFileName); accnosFile.close(); File::remove(accnosFileName); return 0; }

		vector<bool> restricted = chimeras;

		vector<vector<int> > leftDiffs(numSeqs);
		vector<vector<int> > leftMaps(numSeqs);
		vector<vector<int> > rightDiffs(numSeqs);
		vector<vector<int> > rightMaps(numSeqs);

		vector<int> singleLeft, bestLeft;
		vector<int> singleRight, bestRight;

		int bestSingleIndex, bestSingleDiff;
		vector<pwAlign> alignments(numSeqs);

		int comparisons = myPerseus.getAlignments(i, sequences, alignments, leftDiffs, leftMaps, rightDiffs, rightMaps, bestSingleIndex, bestSingleDiff, restricted);
		if (ctrlc_pressed) { chimeraFile.close(); File::remove(chimeraFileName); accnosFile.close(); File::remove(accnosFileName); return 0; }

		int minMismatchToChimera, leftParentBi, rightParentBi, breakPointBi;

		string dummyA, dummyB;

		if (sequences[i].sequence.size() < 3) {
			chimeraFile << i << '\t' << sequences[i].seqName << "\t0\t0\tNull\t0\t0\t0\tNull\tNull\t0.0\t0.0\t0.0\t0\t0\t0\t0.0\t0.0\tgood" << endl;
		}
		else if (comparisons >= 2) {
			minMismatchToChimera = myPerseus.getChimera(sequences, leftDiffs, rightDiffs, leftParentBi, rightParentBi, breakPointBi, singleLeft, bestLeft, singleRight, bestRight, restricted);
			if (ctrlc_pressed) { chimeraFile.close(); File::remove(chimeraFileName); accnosFile.close(); File::remove(accnosFileName); return 0; }

			int minMismatchToTrimera = numeric_limits<int>::max();
			int leftParentTri, middleParentTri, rightParentTri, breakPointTriA, breakPointTriB;

			if (minMismatchToChimera >= 3 && comparisons >= 3) {
				minMismatchToTrimera = myPerseus.getTrimera(sequences, leftDiffs, leftParentTri, middleParentTri, rightParentTri, breakPointTriA, breakPointTriB, singleLeft, bestLeft, singleRight, bestRight, restricted);
				if (ctrlc_pressed) { chimeraFile.close(); File::remove(chimeraFileName); accnosFile.close(); File::remove(accnosFileName); return 0; }
			}

			double singleDist = myPerseus.modeledPairwiseAlignSeqs(sequences[i].sequence, sequences[bestSingleIndex].sequence, dummyA, dummyB, correctModel);

			if (ctrlc_pressed) { chimeraFile.close(); File::remove(chimeraFileName); accnosFile.close(); File::remove(accnosFileName); return 0; }

			string type;
			string chimeraRefSeq;

			if (minMismatchToChimera - minMismatchToTrimera >= 3) {
				type = "trimera";
				chimeraRefSeq = myPerseus.stitchTrimera(alignments, leftParentTri, middleParentTri, rightParentTri, breakPointTriA, breakPointTriB, leftMaps, rightMaps);
			}
			else {
				type = "chimera";
				chimeraRefSeq = myPerseus.stitchBimera(alignments, leftParentBi, rightParentBi, breakPointBi, leftMaps, rightMaps);
			}

			if (ctrlc_pressed) { chimeraFile.close(); File::remove(chimeraFileName); accnosFile.close(); File::remove(accnosFileName); return 0; }

			double chimeraDist = myPerseus.modeledPairwiseAlignSeqs(sequences[i].sequence, chimeraRefSeq, dummyA, dummyB, correctModel);

			if (ctrlc_pressed) { chimeraFile.close(); File::remove(chimeraFileName); accnosFile.close(); File::remove(accnosFileName); return 0; }

			double cIndex = chimeraDist;//modeledPairwiseAlignSeqs(sequences[i].sequence, chimeraRefSeq);
			double loonIndex = myPerseus.calcLoonIndex(sequences[i].sequence, sequences[leftParentBi].sequence, sequences[rightParentBi].sequence, breakPointBi, binMatrix);

			if (ctrlc_pressed) { chimeraFile.close(); File::remove(chimeraFileName); accnosFile.close(); File::remove(accnosFileName); return 0; }

			chimeraFile << i << '\t' << sequences[i].seqName << '\t' << bestSingleDiff << '\t' << bestSingleIndex << '\t' << sequences[bestSingleIndex].seqName << '\t';
			chimeraFile << minMismatchToChimera << '\t' << leftParentBi << '\t' << rightParentBi << '\t' << sequences[leftParentBi].seqName << '\t' << sequences[rightParentBi].seqName << '\t';
			chimeraFile << singleDist << '\t' << cIndex << '\t' << (cIndex - singleDist) << '\t' << loonIndex << '\t';
			chimeraFile << minMismatchToChimera << '\t' << minMismatchToTrimera << '\t' << breakPointBi << '\t';

			double probability = myPerseus.classifyChimera(singleDist, cIndex, loonIndex, alpha, beta);

			chimeraFile << probability << '\t';

			if (probability > cutoff) {
				chimeraFile << type << endl;
				accnosFile << sequences[i].seqName << endl;
				chimeras[i] = 1;
				numChimeras++;
			}
			else {
				chimeraFile << "good" << endl;
			}

		}
		else {
			chimeraFile << i << '\t' << sequences[i].seqName << "\t0\t0\tNull\t0\t0\t0\tNull\tNull\t0.0\t0.0\t0.0\t0\t0\t0\t0.0\t0.0\tgood" << endl;
		}

		//report progress
		if ((i + 1) % 100 == 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(i + 1) + "\n"; }
	}

	if ((numSeqs) % 100 != 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(numSeqs) + "\n"; }

	chimeraFile.close();
	accnosFile.close();

	return numSeqs;
}
/**************************************************************************************************/
int ChimeraPerseusCommand::createProcessesGroups(string outputFName, string accnos, string newCountFile, vector<string> groups, string group, string fasta, string name) {

	vector<int> processIDS;
	int process = 1;
	int num = 0;
	bool recalc = false;

	CountTable newCount;
	if (hasCount && dups) { newCount.readTable(name, true, false); }

	//sanity check
	if (groups.size() < processors) { processors = groups.size(); }

	//divide the groups between the processors
	vector<linePair> lines;
	int remainingPairs = groups.size();
	int startIndex = 0;
	for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
		int numPairs = remainingPairs; //case for last processor
		if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
		lines.push_back(linePair(startIndex, (startIndex + numPairs))); //startIndex, endIndex
		startIndex = startIndex + numPairs;
		remainingPairs = remainingPairs - numPairs;
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
			num = driverGroups(outputFName + toString(m->mothurGetpid(process)) + ".temp", accnos + toString(m->mothurGetpid(process)) + ".temp", accnos + ".byCount." + toString(m->mothurGetpid(process)) + ".temp", lines[process].start, lines[process].end, groups);

			//pass numSeqs to parent
			ofstream out;
			string tempFile = outputFName + toString(m->mothurGetpid(process)) + ".num.temp";
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
			recalc = true;
			break;
		}
	}

	if (recalc) {
		//test line, also set recalc to true.
		//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";
		lines.clear();
		remainingPairs = groups.size();
		int startIndex = 0;
		for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
			int numPairs = remainingPairs; //case for last processor
			if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
			lines.push_back(linePair(startIndex, (startIndex + numPairs))); //startIndex, endIndex
			startIndex = startIndex + numPairs;
			remainingPairs = remainingPairs - numPairs;
		}
		num = 0;
		processIDS.resize(0);
		process = 1;

		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				num = driverGroups(outputFName + toString(m->mothurGetpid(process)) + ".temp", accnos + toString(m->mothurGetpid(process)) + ".temp", accnos + ".byCount." + toString(m->mothurGetpid(process)) + ".temp", lines[process].start, lines[process].end, groups);

				//pass numSeqs to parent
				ofstream out;
				string tempFile = outputFName + toString(m->mothurGetpid(process)) + ".num.temp";
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
	num = driverGroups(outputFName, accnos, accnos + ".byCount", lines[0].start, lines[0].end, groups);

	//force parent to wait until all the processes are done
	for (int i = 0;i < processIDS.size();i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	for (int i = 0; i < processIDS.size(); i++) {
		ifstream in;
		string tempFile = outputFName + toString(processIDS[i]) + ".num.temp";
		File::openInputFile(tempFile, in);
		if (!in.eof()) { int tempNum = 0; in >> tempNum; num += tempNum; }
		in.close(); File::remove(tempFile);
	}

#else
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Windows version shared memory, so be careful when passing variables through the preClusterData struct. 
	//Above fork() will clone, so memory is separate, but that's not the case with windows, 
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<perseusData*> pDataArray;
	vector<DWORD> dwThreadIdArray(processors - 1);
	vector<HANDLE> hThreadArray(processors - 1);

	//Create processor worker threads.
	for (int i = 1; i < processors; i++) {
		// Allocate memory for thread data.
		string extension = toString(i) + ".temp";

		perseusData* tempPerseus = new perseusData(dups, hasName, hasCount, alpha, beta, cutoff, outputFName + extension, fasta, name, group, accnos + extension, accnos + ".byCount." + extension, groups, m, lines[i].start, lines[i].end, i);

		pDataArray.push_back(tempPerseus);
		processIDS.push_back(i);

		//MyPerseusThreadFunction is in header. It must be global or static to work with the threads.
		//default security attributes, thread function name, argument to thread function, use default creation flags, returns the thread identifier
		hThreadArray[i - 1] = CreateThread(NULL, 0, MyPerseusThreadFunction, pDataArray[i - 1], 0, &dwThreadIdArray[i - 1]);
	}


	//using the main process as a worker saves time and memory
	num = driverGroups(outputFName, accnos, accnos + ".byCount", lines[0].start, lines[0].end, groups);

	//Wait until all threads have terminated.
	WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

	//Close all thread handles and free memory allocations.
	for (int i = 0; i < pDataArray.size(); i++) {
		num += pDataArray[i]->count;
		CloseHandle(hThreadArray[i]);
		delete pDataArray[i];
	}
#endif		
	//read my own
	if (hasCount && dups) {
		if (!File::isBlank(accnos + ".byCount")) {
			ifstream in2;
			File::openInputFile(accnos + ".byCount", in2);

			string name, group;
			while (!in2.eof()) {
				in2 >> name >> group; File::gobble(in2);
				newCount.setAbund(name, group, 0);
			}
			in2.close();
		}
		File::remove(accnos + ".byCount");
	}


	//append output files
	for (int i = 0;i < processIDS.size();i++) {
		File::appendFiles((outputFName + toString(processIDS[i]) + ".temp"), outputFName);
		File::remove((outputFName + toString(processIDS[i]) + ".temp"));

		File::appendFiles((accnos + toString(processIDS[i]) + ".temp"), accnos);
		File::remove((accnos + toString(processIDS[i]) + ".temp"));

		if (hasCount && dups) {
			if (!File::isBlank(accnos + ".byCount." + toString(processIDS[i]) + ".temp")) {
				ifstream in2;
				File::openInputFile(accnos + ".byCount." + toString(processIDS[i]) + ".temp", in2);

				string name, group;
				while (!in2.eof()) {
					in2 >> name >> group; File::gobble(in2);
					newCount.setAbund(name, group, 0);
				}
				in2.close();
			}
			File::remove(accnos + ".byCount." + toString(processIDS[i]) + ".temp");
		}

	}

	//print new *.pick.count_table
	if (hasCount && dups) { newCount.printTable(newCountFile); }

	return num;

}
//**********************************************************************************************************************
int ChimeraPerseusCommand::deconvoluteResults(map<string, string>& uniqueNames, string outputFileName, string accnosFileName) {
	map<string, string>::iterator itUnique;
	int total = 0;

	//edit accnos file
	ifstream in2;
	File::openInputFile(accnosFileName, in2);

	ofstream out2;
	File::openOutputFile(accnosFileName + ".temp", out2);

	string name;
	set<string> namesInFile; //this is so if a sequence is found to be chimera in several samples we dont write it to the results file more than once
	set<string>::iterator itNames;
	set<string> chimerasInFile;
	set<string>::iterator itChimeras;


	while (!in2.eof()) {
		if (ctrlc_pressed) { in2.close(); out2.close(); File::remove(outputFileName); File::remove((accnosFileName + ".temp")); return 0; }

		in2 >> name; File::gobble(in2);

		//find unique name
		itUnique = uniqueNames.find(name);

		if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing accnos results. Cannot find " + name + "." << '\n'; ctrlc_pressed = true; }
		else {
			itChimeras = chimerasInFile.find((itUnique->second));

			if (itChimeras == chimerasInFile.end()) {
				out2 << itUnique->second << endl;
				chimerasInFile.insert((itUnique->second));
				total++;
			}
		}
	}
	in2.close();
	out2.close();

	File::remove(accnosFileName);
	rename((accnosFileName + ".temp").c_str(), accnosFileName.c_str());

	//edit chimera file
	ifstream in;
	File::openInputFile(outputFileName, in);

	ofstream out;
	File::openOutputFile(outputFileName + ".temp", out); out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);

	int DiffsToBestMatch, BestMatchIndex, DiffstToChimera, IndexofLeftParent, IndexOfRightParent;
	float temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	string index, BestMatchName, parent1, parent2, flag;
	name = "";
	namesInFile.clear();
	//assumptions - in file each read will always look like 
	/*
	 SequenceIndex	Name	DiffsToBestMatch	BestMatchIndex	BestMatchName	DiffstToChimera	IndexofLeftParent	IndexOfRightParent	NameOfLeftParent	NameOfRightParent	DistanceToBestMatch	cIndex	(cIndex - singleDist)	loonIndex	MismatchesToChimera	MismatchToTrimera	ChimeraBreakPoint	LogisticProbability	TypeOfSequence
	 0	F01QG4L02JVBQY	0	0	Null	0	0	0	Null	Null	0.0	0.0	0.0	0.0	0	0	0	0.0	0.0	good
	 1	F01QG4L02ICTC6	0	0	Null	0	0	0	Null	Null	0.0	0.0	0.0	0.0	0	0	0	0.0	0.0	good
	 2	F01QG4L02JZOEC	48	0	F01QG4L02JVBQY	47	0	0	F01QG4L02JVBQY	F01QG4L02JVBQY	2.0449	2.03545	-0.00944493	0	47	2147483647	138	0	good
	 3	F01QG4L02G7JEC	42	0	F01QG4L02JVBQY	40	1	0	F01QG4L02ICTC6	F01QG4L02JVBQY	1.87477	1.81113	-0.0636404	5.80145	40	2147483647	25	0	good
	 */

	 //get and print headers
	BestMatchName = File::getline(in); File::gobble(in);
	out << BestMatchName << endl;

	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); out.close(); File::remove((outputFileName + ".temp")); return 0; }

		bool print = false;
		in >> index;	File::gobble(in);

		if (index != "SequenceIndex") { //if you are not a header line, there will be a header line for each group if group file is given
			in >> name;		File::gobble(in);
			in >> DiffsToBestMatch; File::gobble(in);
			in >> BestMatchIndex; File::gobble(in);
			in >> BestMatchName; File::gobble(in);
			in >> DiffstToChimera; File::gobble(in);
			in >> IndexofLeftParent; File::gobble(in);
			in >> IndexOfRightParent; File::gobble(in);
			in >> parent1;	File::gobble(in);
			in >> parent2;	File::gobble(in);
			in >> temp1 >> temp2 >> temp3 >> temp4 >> temp5 >> temp6 >> temp7 >> temp8 >> flag; File::gobble(in);

			//find unique name
			itUnique = uniqueNames.find(name);

			if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find " + name + "." << '\n'; ctrlc_pressed = true; }
			else {
				name = itUnique->second;
				//is this name already in the file
				itNames = namesInFile.find((name));

				if (itNames == namesInFile.end()) { //no not in file
					if (flag == "good") { //are you really a no??
						//is this sequence really not chimeric??
						itChimeras = chimerasInFile.find(name);

						//then you really are a no so print, otherwise skip
						if (itChimeras == chimerasInFile.end()) { print = true; }
					}
					else { print = true; }
				}
			}

			if (print) {
				out << index << '\t' << name << '\t' << DiffsToBestMatch << '\t' << BestMatchIndex << '\t';
				namesInFile.insert(name);

				if (BestMatchName != "Null") {
					itUnique = uniqueNames.find(BestMatchName);
					if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find BestMatchName " + BestMatchName + "." << '\n'; ctrlc_pressed = true; }
					else { out << itUnique->second << '\t'; }
				}
				else { out << "Null" << '\t'; }

				out << DiffstToChimera << '\t' << IndexofLeftParent << '\t' << IndexOfRightParent << '\t';

				if (parent1 != "Null") {
					itUnique = uniqueNames.find(parent1);
					if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find parent1 " + parent1 + "." << '\n'; ctrlc_pressed = true; }
					else { out << itUnique->second << '\t'; }
				}
				else { out << "Null" << '\t'; }

				if (parent1 != "Null") {
					itUnique = uniqueNames.find(parent2);
					if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find parent2 " + parent2 + "." << '\n'; ctrlc_pressed = true; }
					else { out << itUnique->second << '\t'; }
				}
				else { out << "Null" << '\t'; }

				out << temp1 << '\t' << temp2 << '\t' << temp3 << '\t' << temp4 << '\t' << temp5 << '\t' << temp6 << '\t' << temp7 << '\t' << temp8 << '\t' << flag << endl;
			}
		}
		else { index = File::getline(in); File::gobble(in); }
	}
	in.close();
	out.close();

	File::remove(outputFileName);
	rename((outputFileName + ".temp").c_str(), outputFileName.c_str());

	return total;
}
//**********************************************************************************************************************


