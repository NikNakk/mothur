/*
 *  chimeracheckcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 3/31/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "chimeracheckcommand.h"
#include "referencedb.h"

 //**********************************************************************************************************************
vector<string> ChimeraCheckCommand::setParameters() {
	try {
		nkParameters.add(new ReferenceFileParameter(settings, "reference", true, true));
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "chimera", false, true, true); parameters.push_back(pfasta);
		CommandParameter pname("name", "InputTypes", "", "", "none", "none", "none", "", false, false, true); parameters.push_back(pname);
		nkParameters.add(new BooleanParameter("svg", false, false, false));
		nkParameters.add(new NumberParameter("increment", -INFINITY, INFINITY, 10, false, false));
		nkParameters.add(new NumberParameter("ksize", -INFINITY, INFINITY, 7, false, false));
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));
		nkParameters.add(new BooleanParameter("save", false, false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraCheckCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraCheckCommand::getHelpString() {
	try {
		string helpString = "The chimera.check command reads a fastafile and referencefile and outputs potentially chimeric sequences.\n"
			"This command was created using the algorithms described in CHIMERA_CHECK version 2.7 written by Niels Larsen. \n"
			"The chimera.check command parameters are fasta, reference, processors, ksize, increment, svg and name.\n"
			"The fasta parameter allows you to enter the fasta file containing your potentially chimeric sequences, and is required unless you have a valid current fasta file. \n"
			"You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amzon.fasta \n"
			"The reference parameter allows you to enter a reference file containing known non-chimeric sequences, and is required. \n"
			"The processors parameter allows you to specify how many processors you would like to use.  The default is 1. \n"
			"The increment parameter allows you to specify how far you move each window while finding chimeric sequences, default is 10.\n"
			"The ksize parameter allows you to input kmersize, default is 7. \n"
			"The svg parameter allows you to specify whether or not you would like a svg file outputted for each query sequence, default is False.\n"
			"The name parameter allows you to enter a file containing names of sequences you would like .svg files for.\n"
			"You may enter multiple name files by separating their names with dashes. ie. fasta=abrecovery.svg.names-amzon.svg.names \n"
			"If the save parameter is set to true the reference sequences will be saved in memory, to clear them later you can use the clear.memory command. Default=f."
			"The chimera.check command should be in the following format: \n"
			"chimera.check(fasta=yourFastaFile, reference=yourTemplateFile, processors=yourProcessors, ksize=yourKmerSize) \n"
			"Example: chimera.check(fasta=AD.fasta, reference=core_set_aligned,imputed.fasta, processors=4, ksize=8) \n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraCheckCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraCheckCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "chimera") { pattern = "[filename],chimeracheck.chimeras"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ChimeraCheckCommand::ChimeraCheckCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraCheckCommand, ChimeraCheckCommand";
		exit(1);
	}
}
//***************************************************************************************************************
ChimeraCheckCommand::ChimeraCheckCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	ReferenceDB& rdb = ReferenceDB::getInstance();

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter("chimera.check");
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			it = parameters.find("reference");
			//user has given a template file
			if (it != parameters.end()) {
				string path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["reference"] = inputDir + it->second; }
			}
		}

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

					//if you can't open it, try default location
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

			//make sure there is at least one valid file left
			if (nameFileNames.size() != 0) {
				if (nameFileNames.size() != fastaFileNames.size()) {
					LOG(INFO) << "Different number of valid name files and fasta files, aborting command." << '\n';
					abort = true;
				}
			}
		}

		string temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "save", false);			if (temp == "not found") { temp = "f"; }
		save = m->isTrue(temp);
		rdb.save = save;
		if (save) { //clear out old references
			rdb.clearMemory();
		}

		//this has to go after save so that if the user sets save=t and provides no reference we abort
		templatefile = validParameter.validFile(parameters, "reference", true);
		if (templatefile == "not found") {
			//check for saved reference sequences
			if (rdb.referenceSeqs.size() != 0) {
				templatefile = "saved";
			}
			else {
				LOG(LOGERROR) << "You don't have any saved reference sequences and the reference parameter is a required.";
				LOG(INFO) << "";
				abort = true;
			}
		}
		else if (templatefile == "not open") { abort = true; }
		else { if (save) { rdb.setSavedReference(templatefile); } }


		temp = validParameter.validFile(parameters, "ksize", false);			if (temp == "not found") { temp = "7"; }
		Utility::mothurConvert(temp, ksize);

		temp = validParameter.validFile(parameters, "svg", false);				if (temp == "not found") { temp = "F"; }
		svg = m->isTrue(temp);
		if (nameFileNames.size() != 0) { svg = true; }

		temp = validParameter.validFile(parameters, "increment", false);		if (temp == "not found") { temp = "10"; }
		Utility::mothurConvert(temp, increment);
	}
}
//***************************************************************************************************************

int ChimeraCheckCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		for (int i = 0; i < fastaFileNames.size(); i++) {

			LOG(INFO) << "Checking sequences from " + fastaFileNames[i] + " ..." << '\n';

			int start = time(NULL);

			string thisNameFile = "";
			if (nameFileNames.size() != 0) { thisNameFile = nameFileNames[i]; }

			chimera = new ChimeraCheckRDP(fastaFileNames[i], templatefile, thisNameFile, svg, increment, ksize, outputDir);

			if (ctrlc_pressed) { delete chimera;	return 0; }

			if (outputDir == "") { outputDir = File::getPath(fastaFileNames[i]); }//if user entered a file with a path then preserve it
			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[i]));
			string outputFileName = getOutputFileName("chimera", variables);
			outputNames.push_back(outputFileName); outputTypes["chimera"].push_back(outputFileName);


			//break up file
#if defined (UNIX)
			vector<unsigned long long> positions = File::divideFile(fastaFileNames[i], processors);

			for (int s = 0; s < (positions.size() - 1); s++) {
				lines.push_back(new linePair(positions[s], positions[(s + 1)]));
			}

			if (processors == 1) {
				numSeqs = driver(lines[0], outputFileName, fastaFileNames[i]);

				if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } for (int j = 0; j < lines.size(); j++) { delete lines[j]; } outputTypes.clear();  lines.clear(); delete chimera; return 0; }

			}
			else {
				processIDS.resize(0);

				numSeqs = createProcesses(outputFileName, fastaFileNames[i]);

				rename((outputFileName + toString(processIDS[0]) + ".temp").c_str(), outputFileName.c_str());

				//append output files
				for (int j = 1;j < processors;j++) {
					File::appendFiles((outputFileName + toString(processIDS[j]) + ".temp"), outputFileName);
					File::remove((outputFileName + toString(processIDS[j]) + ".temp"));
				}

				if (ctrlc_pressed) {
					for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } outputTypes.clear();
					for (int j = 0; j < lines.size(); j++) { delete lines[j]; }  lines.clear();
					delete chimera;
					return 0;
				}
			}

#else
			lines.push_back(new linePair(0, 1000));
			numSeqs = driver(lines[0], outputFileName, fastaFileNames[i]);

			if (ctrlc_pressed) { for (int j = 0; j < lines.size(); j++) { delete lines[j]; }  lines.clear(); for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } outputTypes.clear(); delete chimera; return 0; }
#endif

			delete chimera;
			for (int j = 0; j < lines.size(); j++) { delete lines[j]; }  lines.clear();

			LOG(INFO) << '\n' << "This method does not determine if a sequence is chimeric, but allows you to make that determination based on the IS values." << '\n';
			LOG(INFO) << std::endl << "It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences.\n\n";

		}

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraCheckCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************

int ChimeraCheckCommand::driver(linePair* filePos, string outputFName, string filename) {
	ofstream out;
	File::openOutputFile(outputFName, out);

	ofstream out2;

	ifstream inFASTA;
	File::openInputFile(filename, inFASTA);

	inFASTA.seekg(filePos->start);

	bool done = false;
	int count = 0;

	while (!done) {

		if (ctrlc_pressed) { return 1; }

		Sequence* candidateSeq = new Sequence(inFASTA);  File::gobble(inFASTA);

		if (candidateSeq->getName() != "") { //incase there is a commented sequence at the end of a file
			//find chimeras
			chimera->getChimeras(candidateSeq);

			if (ctrlc_pressed) { delete candidateSeq; return 1; }

			//print results
			chimera->print(out, out2);
			count++;
		}
		delete candidateSeq;

#if defined (UNIX)
		unsigned long long pos = inFASTA.tellg();
		if ((pos == -1) || (pos >= filePos->end)) { break; }
#else
		if (inFASTA.eof()) { break; }
#endif

		//report progress
		if ((count) % 100 == 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(count) + "\n"; }
	}
	//report progress
	if ((count) % 100 != 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(count) + "\n"; }

	out.close();
	inFASTA.close();

	return count;
}
/**************************************************************************************************/

int ChimeraCheckCommand::createProcesses(string outputFileName, string filename) {
#if defined (UNIX)
	int process = 0;
	int num = 0;
	bool recalc = false;

	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
			process++;
		}
		else if (pid == 0) {
			num = driver(lines[process], outputFileName + toString(m->mothurGetpid(process)) + ".temp", filename);

			//pass numSeqs to parent
			ofstream out;
			string tempFile = outputFileName + toString(m->mothurGetpid(process)) + ".num.temp";
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
		for (int i = 0; i < lines.size(); i++) { delete lines[i]; }  lines.clear();
		vector<unsigned long long> positions = File::divideFile(filename, processors);
		for (int s = 0; s < (positions.size() - 1); s++) {
			lines.push_back(new linePair(positions[s], positions[(s + 1)]));
		}
		num = 0;
		processIDS.resize(0);
		process = 0;

		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				num = driver(lines[process], outputFileName + toString(m->mothurGetpid(process)) + ".temp", filename);

				//pass numSeqs to parent
				ofstream out;
				string tempFile = outputFileName + toString(m->mothurGetpid(process)) + ".num.temp";
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

	//force parent to wait until all the processes are done
	for (int i = 0;i < processors;i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	for (int i = 0; i < processIDS.size(); i++) {
		ifstream in;
		string tempFile = outputFileName + toString(processIDS[i]) + ".num.temp";
		File::openInputFile(tempFile, in);
		if (!in.eof()) { int tempNum = 0; in >> tempNum; num += tempNum; }
		in.close(); File::remove(tempFile);
	}

	return num;
#endif		
}
/**************************************************************************************************/


