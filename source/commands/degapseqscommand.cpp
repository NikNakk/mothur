/*
 *  degapseqscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 6/21/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "degapseqscommand.h"


 //**********************************************************************************************************************
vector<string> DegapSeqsCommand::setParameters() {
	try {
		nkParameters.add(new FastaFileParameter(settings, "fasta", true, true));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DegapSeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string DegapSeqsCommand::getHelpString() {
	try {
		string helpString = "The degap.seqs command reads a fastafile and removes all gap characters.\n"
			"The degap.seqs command parameter are fasta and processors.\n"
			"The fasta parameter allows you to enter the fasta file containing your sequences, and is required unless you have a valid current fasta file. \n"
			"You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amzon.fasta \n"
			"The processors parameter allows you to enter the number of processors you would like to use. \n"
			"The degap.seqs command should be in the following format: \n"
			"degap.seqs(fasta=yourFastaFile) \n";
		helpString += "Example: degap.seqs(fasta=abrecovery.align) \n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DegapSeqsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string DegapSeqsCommand::getOutputPattern(string type) {
	string pattern = "";
	if (type == "fasta") { pattern = "[filename],ng.fasta"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
DegapSeqsCommand::DegapSeqsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DegapSeqsCommand, DegapSeqsCommand";
		exit(1);
	}
}
//***************************************************************************************************************
DegapSeqsCommand::DegapSeqsCommand(Settings& settings, string option) : Command(settings, option) {
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

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }

		//check for required parameters
		fastafile = validParameter.validFile(parameters, "fasta", false);
		if (fastafile == "not found") {
			fastafile = settings.getCurrent("fasta");
			if (fastafile != "") { fastaFileNames.push_back(fastafile); LOG(INFO) << "Using " + fastafile + " as input file for the fasta parameter." << '\n'; }
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
					else { settings.setCurrent("fasta", fastaFileNames[i]); }
				}
			}

			//make sure there is at least one valid file left
			if (fastaFileNames.size() == 0) { LOG(INFO) << "no valid files." << '\n'; abort = true; }
		}

		string temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = "";
			outputDir += File::getPath(fastafile); //if user entered a file with a path then preserve it	
		}

	}
}
//***************************************************************************************************************
int DegapSeqsCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		for (int s = 0; s < fastaFileNames.size(); s++) {

			LOG(INFO) << "Degapping sequences from " + fastaFileNames[s] + " ..." << '\n';

			string tempOutputDir = outputDir;
			if (outputDir == "") { tempOutputDir = File::getPath(fastaFileNames[s]); }
			map<string, string> variables;
			variables["[filename]"] = tempOutputDir + File::getRootName(File::getSimpleName(fastaFileNames[s]));
			string degapFile = getOutputFileName("fasta", variables);
			outputNames.push_back(degapFile); outputTypes["fasta"].push_back(degapFile);

			int start = time(NULL);

			int numSeqs = createProcesses(fastaFileNames[s], degapFile);

			LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to degap " + toString(numSeqs) + " sequences.\n\n";

			if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } return 0; }
		}

		//set fasta file as new current fastafile
		string current = "";
		itTypes = outputTypes.find("fasta");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("fasta", current); }
		}

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";


		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DegapSeqsCommand, execute";
		exit(1);
	}
}
//***************************************************************************************************************
int DegapSeqsCommand::createProcesses(string filename, string outputFileName) {
	try {
		int numSeqs = 0;
		vector<int> processIDS; processIDS.resize(0);
		bool recalc = false;
		vector<linePair> lines;
		vector<unsigned long long> positions;

#if defined (UNIX)
		positions = File::divideFile(filename, processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }
		int process = 1;

		//loop through and create all the processes you want
		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				numSeqs = driver(lines[process], filename, outputFileName + toString(m->mothurGetpid(process)) + ".temp");

				//pass numSeqs to parent
				ofstream out;
				string tempFile = outputFileName + toString(m->mothurGetpid(process)) + ".num.temp";
				File::openOutputFile(tempFile, out);
				out << numSeqs << endl;
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
			positions.clear();
			positions = File::divideFile(filename, processors);
			for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }

			numSeqs = 0;
			processIDS.resize(0);
			process = 1;

			while (process != processors) {
				pid_t pid = fork();

				if (pid > 0) {
					processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
					process++;
				}
				else if (pid == 0) {
					numSeqs = driver(lines[process], filename, outputFileName + toString(m->mothurGetpid(process)) + ".temp");

					//pass numSeqs to parent
					ofstream out;
					string tempFile = outputFileName + toString(m->mothurGetpid(process)) + ".num.temp";
					File::openOutputFile(tempFile, out);
					out << numSeqs << endl;
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
		numSeqs = driver(lines[0], filename, outputFileName);

		//force parent to wait until all the processes are done
		for (int i = 0;i < processIDS.size();i++) {
			int temp = processIDS[i];
			wait(&temp);
		}

		for (int i = 0; i < processIDS.size(); i++) {
			ifstream in;
			string tempFile = outputFileName + toString(processIDS[i]) + ".num.temp";
			File::openInputFile(tempFile, in);
			if (!in.eof()) { int tempNum = 0; in >> tempNum; numSeqs += tempNum; }
			in.close(); File::remove(tempFile);

			File::appendFiles((outputFileName + toString(processIDS[i]) + ".temp"), outputFileName);
			File::remove((outputFileName + toString(processIDS[i]) + ".temp"));
		}
#else
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//Windows version shared memory, so be careful when passing variables through the degapData struct.
		//Above fork() will clone, so memory is separate, but that's not the case with windows,
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		if (processors == 1) {
			lines.push_back(linePair(0, 1000));
		}
		else {
			positions = m->setFilePosFasta(filename, numSeqs);
			if (positions.size() < processors) { processors = positions.size(); }

			//figure out how many sequences you have to process
			int numSeqsPerProcessor = numSeqs / processors;
			for (int i = 0; i < processors; i++) {
				int startIndex = i * numSeqsPerProcessor;
				if (i == (processors - 1)) { numSeqsPerProcessor = numSeqs - i * numSeqsPerProcessor; }
				lines.push_back(linePair(positions[startIndex], numSeqsPerProcessor));
			}
		}

		vector<degapData*> pDataArray;
		vector<DWORD> dwThreadIdArray(processors - 1);
		vector<HANDLE> hThreadArray(processors - 1);

		//Create processor worker threads.
		for (int i = 0; i < processors - 1; i++) {
			// Allocate memory for thread data.
			string extension = "";
			if (i != 0) { extension = toString(i) + ".temp"; }

			degapData* tempDegap = new degapData(filename, (outputFileName + extension), m, lines[i].start, lines[i].end);
			pDataArray.push_back(tempDegap);
			processIDS.push_back(i);

			//MySeqSumThreadFunction is in header. It must be global or static to work with the threads.
			//default security attributes, thread function name, argument to thread function, use default creation flags, returns the thread identifier
			hThreadArray[i] = CreateThread(NULL, 0, MyDegapThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]);
		}


		//using the main process as a worker saves time and memory
		//do my part - do last piece because windows is looking for eof
		numSeqs = driver(lines[processors - 1], filename, (outputFileName + toString(processors - 1) + ".temp"));

		//Wait until all threads have terminated.
		WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

		//Close all thread handles and free memory allocations.
		for (int i = 0; i < pDataArray.size(); i++) {
			if (pDataArray[i]->count != pDataArray[i]->end) {
				LOG(LOGERROR) << "process " + toString(i) + " only processed " + toString(pDataArray[i]->count) + " of " + toString(pDataArray[i]->end) + " sequences assigned to it, quitting. \n"; ctrlc_pressed = true;
			}
			numSeqs += pDataArray[i]->count;
			CloseHandle(hThreadArray[i]);
			delete pDataArray[i];
		}


		for (int i = 1; i < processors; i++) {
			File::appendFiles((outputFileName + toString(i) + ".temp"), outputFileName);
			File::remove((outputFileName + toString(i) + ".temp"));
		}
#endif

		return numSeqs;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DegapSeqsCommand, createProcesses";
		exit(1);
	}
}
//***************************************************************************************************************
int DegapSeqsCommand::driver(linePair filePos, string filename, string outputFileName) {
	try {
		int numSeqs = 0;

		ifstream inFASTA;
		File::openInputFile(filename, inFASTA);

		inFASTA.seekg(filePos.start);

		if (filePos.start == 0) { m->zapGremlins(inFASTA); File::gobble(inFASTA); }

		ofstream outFASTA;
		File::openOutputFile(outputFileName, outFASTA);

		while (!inFASTA.eof()) {
			if (ctrlc_pressed) { break; }

			Sequence currSeq(inFASTA);  File::gobble(inFASTA);
			if (currSeq.getName() != "") {
				outFASTA << ">" << currSeq.getName() << endl;
				outFASTA << currSeq.getUnaligned() << endl;
				numSeqs++;
			}

#if defined (UNIX)
			unsigned long long pos = inFASTA.tellg();
			if ((pos == -1) || (pos >= filePos.end)) { break; }
#else
			if (inFASTA.eof()) { break; }
#endif

			//report progress
			if ((numSeqs) % 1000 == 0) { LOG(SCREENONLY) << toString(numSeqs) + "\n"; }

		}
		//report progress
		if ((numSeqs) % 1000 != 0) { LOG(SCREENONLY) << toString(numSeqs) + "\n"; }

		inFASTA.close();
		outFASTA.close();

		return numSeqs;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DegapSeqsCommand, driver";
		exit(1);
	}
}
//***************************************************************************************************************

