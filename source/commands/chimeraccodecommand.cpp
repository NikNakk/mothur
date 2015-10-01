/*
 *  chimeraccodecommand.cpp
 *  Mothur
 *
 *  Created by westcott on 3/30/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "chimeraccodecommand.h"
#include "ccode.h"
#include "referencedb.h"
 //**********************************************************************************************************************
vector<string> ChimeraCcodeCommand::setParameters() {
	try {
		nkParameters.add(new ReferenceFileParameter(settings, "reference", true, true));
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "chimera-mapinfo-accnos", false, true, true); parameters.push_back(pfasta);
		nkParameters.add(new BooleanParameter("filter", false, false, false));
		nkParameters.add(new NumberParameter("window", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("numwanted", -INFINITY, INFINITY, 20, false, false));
		nkParameters.add(new StringParameter("mask", "", false, false));
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
		LOG(FATAL) << e.what() << " in ChimeraCcodeCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraCcodeCommand::getHelpString() {
	try {
		string helpString = "The chimera.ccode command reads a fastafile and referencefile and outputs potentially chimeric sequences.\n"
			"This command was created using the algorithms described in the 'Evaluating putative chimeric sequences from PCR-amplified products' paper by Juan M. Gonzalez, Johannes Zimmerman and Cesareo Saiz-Jimenez.\n"
			"The chimera.ccode command parameters are fasta, reference, filter, mask, processors, window and numwanted.\n"
			"The fasta parameter allows you to enter the fasta file containing your potentially chimeric sequences, and is required unless you have a valid current fasta file. \n"
			"You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amzon.fasta \n"
			"The reference parameter allows you to enter a reference file containing known non-chimeric sequences, and is required. \n"
			"The filter parameter allows you to specify if you would like to apply a vertical and 50% soft filter. \n"
			"The processors parameter allows you to specify how many processors you would like to use.  The default is 1. \n"
			"The mask parameter allows you to specify a file containing one sequence you wish to use as a mask for the your sequences. \n"
			"The window parameter allows you to specify the window size for searching for chimeras. \n"
			"The numwanted parameter allows you to specify how many sequences you would each query sequence compared with.\n"
			"If the save parameter is set to true the reference sequences will be saved in memory, to clear them later you can use the clear.memory command. Default=f."
			"The chimera.ccode command should be in the following format: \n"
			"chimera.ccode(fasta=yourFastaFile, reference=yourTemplate) \n"
			"Example: chimera.ccode(fasta=AD.align, reference=core_set_aligned.imputed.fasta) \n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraCcodeCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraCcodeCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "chimera") { pattern = "[filename],[tag],ccode.chimeras-[filename],ccode.chimeras"; }
	else if (type == "accnos") { pattern = "[filename],[tag],ccode.accnos-[filename],ccode.accnos"; }
	else if (type == "mapinfo") { pattern = "[filename],mapinfo"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ChimeraCcodeCommand::ChimeraCcodeCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
		outputTypes["mapinfo"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraCcodeCommand, ChimeraCcodeCommand";
		exit(1);
	}
}
//***************************************************************************************************************
ChimeraCcodeCommand::ChimeraCcodeCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	ReferenceDB& rdb = ReferenceDB::getInstance();

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter("chimera.ccode");
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
		outputTypes["mapinfo"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;


		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("reference");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["reference"] = inputDir + it->second; }
			}
		}

		//check for required parameters
		fastafile = validParameter.validFile(parameters, "fasta", false);
		if (fastafile == "not found") { 				//if there is a current fasta file, use it
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

		maskfile = validParameter.validFile(parameters, "mask", false);
		if (maskfile == "not found") { maskfile = ""; }
		else if (maskfile != "default") {
			if (inputDir != "") {
				string path = File::getPath(maskfile);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { maskfile = inputDir + maskfile; }
			}

			ifstream in;
			int	ableToOpen = File::openInputFile(maskfile, in);
			if (ableToOpen == 1) { abort = true; }
			in.close();
		}

		string temp;
		temp = validParameter.validFile(parameters, "filter", false);			if (temp == "not found") { temp = "F"; }
		filter = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "window", false);			if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, window);

		temp = validParameter.validFile(parameters, "numwanted", false);		if (temp == "not found") { temp = "20"; }
		Utility::mothurConvert(temp, numwanted);

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


	}
}
//***************************************************************************************************************
int ChimeraCcodeCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		for (int s = 0; s < fastaFileNames.size(); s++) {

			LOG(INFO) << "Checking sequences from " + fastaFileNames[s] + " ..." << '\n';

			int start = time(NULL);

			//set user options
			if (maskfile == "default") { LOG(INFO) << "I am using the default 236627 EU009184.1 Shigella dysenteriae str. FBD013." << '\n'; }

			chimera = new Ccode(fastaFileNames[s], templatefile, filter, maskfile, window, numwanted, outputDir);

			//is your template aligned?
			if (chimera->getUnaligned()) { LOG(INFO) << "Your template sequences are different lengths, please correct." << '\n'; delete chimera; return 0; }
			templateSeqsLength = chimera->getLength();

			if (outputDir == "") { outputDir = File::getPath(fastaFileNames[s]); }//if user entered a file with a path then preserve it
			string outputFileName, accnosFileName;
			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[s]));
			string mapInfo = getOutputFileName("mapinfo", variables);
			if (maskfile != "") { variables["[tag]"] = maskfile; }
			outputFileName = getOutputFileName("chimera", variables);
			accnosFileName = getOutputFileName("accnos", variables);

			if (ctrlc_pressed) { delete chimera;  for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } outputTypes.clear(); return 0; }

			ofstream outHeader;
			string tempHeader = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[s])) + maskfile + "ccode.chimeras.tempHeader";
			File::openOutputFile(tempHeader, outHeader);

			outHeader << "For full window mapping info refer to " << mapInfo << endl << endl;

			outHeader.close();



			//break up file
#if defined (UNIX)
			vector<unsigned long long> positions = File::divideFile(fastaFileNames[s], processors);

			for (int i = 0; i < (positions.size() - 1); i++) {
				lines.push_back(new linePair(positions[i], positions[(i + 1)]));
			}

			if (processors == 1) {

				numSeqs = driver(lines[0], outputFileName, fastaFileNames[s], accnosFileName);

				if (ctrlc_pressed) { File::remove(outputFileName); File::remove(tempHeader); File::remove(accnosFileName); for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } for (int i = 0; i < lines.size(); i++) { delete lines[i]; } outputTypes.clear();  lines.clear(); delete chimera; return 0; }

			}
			else {
				processIDS.resize(0);

				numSeqs = createProcesses(outputFileName, fastaFileNames[s], accnosFileName);

				rename((outputFileName + toString(processIDS[0]) + ".temp").c_str(), outputFileName.c_str());
				rename((accnosFileName + toString(processIDS[0]) + ".temp").c_str(), accnosFileName.c_str());

				//append output files
				for (int i = 1;i < processors;i++) {
					File::appendFiles((outputFileName + toString(processIDS[i]) + ".temp"), outputFileName);
					File::remove((outputFileName + toString(processIDS[i]) + ".temp"));
				}

				//append output files
				for (int i = 1;i < processors;i++) {
					File::appendFiles((accnosFileName + toString(processIDS[i]) + ".temp"), accnosFileName);
					File::remove((accnosFileName + toString(processIDS[i]) + ".temp"));
				}

				if (ctrlc_pressed) {
					File::remove(outputFileName);
					File::remove(accnosFileName);
					for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } outputTypes.clear();
					for (int i = 0; i < lines.size(); i++) { delete lines[i]; }  lines.clear();
					delete chimera;
					return 0;
				}

			}

#else
			lines.push_back(new linePair(0, 1000));
			numSeqs = driver(lines[0], outputFileName, fastaFileNames[s], accnosFileName);

			if (ctrlc_pressed) { File::remove(outputFileName); File::remove(tempHeader); File::remove(accnosFileName); for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } for (int i = 0; i < lines.size(); i++) { delete lines[i]; } outputTypes.clear();  lines.clear(); delete chimera; return 0; }

#endif

			File::appendFiles(outputFileName, tempHeader);

			File::remove(outputFileName);
			rename(tempHeader.c_str(), outputFileName.c_str());

			delete chimera;

			outputNames.push_back(outputFileName); outputTypes["chimera"].push_back(outputFileName);
			outputNames.push_back(mapInfo);	outputTypes["mapinfo"].push_back(mapInfo);
			outputNames.push_back(accnosFileName); outputTypes["accnos"].push_back(accnosFileName);

			for (int i = 0; i < lines.size(); i++) { delete lines[i]; }  lines.clear();

			LOG(INFO) << '\n' << "It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences." << '\n';
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
		LOG(FATAL) << e.what() << " in ChimeraCcodeCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************

int ChimeraCcodeCommand::driver(linePair* filePos, string outputFName, string filename, string accnos) {
	ofstream out;
	File::openOutputFile(outputFName, out);

	ofstream out2;
	File::openOutputFile(accnos, out2);

	ifstream inFASTA;
	File::openInputFile(filename, inFASTA);

	inFASTA.seekg(filePos->start);

	bool done = false;
	int count = 0;

	while (!done) {

		if (ctrlc_pressed) { return 1; }

		Sequence* candidateSeq = new Sequence(inFASTA);  File::gobble(inFASTA);

		if (candidateSeq->getName() != "") { //incase there is a commented sequence at the end of a file

			if (candidateSeq->getAligned().length() != templateSeqsLength) {
				LOG(INFO) << candidateSeq->getName() + " is not the same length as the template sequences. Skipping." << '\n';
			}
			else {
				//find chimeras
				chimera->getChimeras(candidateSeq);

				if (ctrlc_pressed) { delete candidateSeq; return 1; }

				//print results
				chimera->print(out, out2);
			}
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
	out2.close();
	inFASTA.close();

	return count;
}
/**************************************************************************************************/

int ChimeraCcodeCommand::createProcesses(string outputFileName, string filename, string accnos) {
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
			num = driver(lines[process], outputFileName + toString(m->mothurGetpid(process)) + ".temp", filename, accnos + toString(m->mothurGetpid(process)) + ".temp");

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
		vector<unsigned long long> positions;
		positions = File::divideFile(filename, processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(new linePair(positions[i], positions[(i + 1)])); }

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
				num = driver(lines[process], outputFileName + toString(m->mothurGetpid(process)) + ".temp", filename, accnos + toString(m->mothurGetpid(process)) + ".temp");

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
//**********************************************************************************************************************

