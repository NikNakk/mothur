/*
 *  chopseqscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/10/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "chopseqscommand.h"
#include "sequence.hpp"
#include "removeseqscommand.h"

 //**********************************************************************************************************************
vector<string> ChopSeqsCommand::setParameters() {
	try {
		nkParameters.add(new FastaFileParameter(settings, "fasta", true, true));
		nkParameters.add(new QFileParameter(settings, "qfile", false, true));
		nkParameters.add(new NameFileParameter(settings, "name", false, true, "NameCount"));
		nkParameters.add(new CountFileParameter(settings, "count", false, true, "NameCount-CountGroup"));
		nkParameters.add(new GroupFileParameter(settings, "group", false, true, "CountGroup"));
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new IntegerParameter("numbases", -INFINITY, INFINITY, 0, true, true);
		nkParameters.add(new BooleanParameter("countgaps", false, false, false));
		nkParameters.add(new BooleanParameter("short", false, false, false));
		nkParameters.add(new MultipleParameter("front-back", "front"));
		nkParameters.add(new BooleanParameter("keepn", false, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChopSeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChopSeqsCommand::getHelpString() {
	try {
		string helpString = "The chop.seqs command reads a fasta file and outputs a .chop.fasta containing the trimmed sequences. Note: If a sequence is completely 'chopped', an accnos file will be created with the names of the sequences removed. \n"
			"The chop.seqs command parameters are fasta, name, group, count, numbases, countgaps and keep. fasta is required unless you have a valid current fasta file. numbases is required.\n"
			"The chop.seqs command should be in the following format: chop.seqs(fasta=yourFasta, numbases=yourNum, keep=yourKeep).\n"
			"If you provide a name, group or count file any sequences removed from the fasta file will also be removed from those files.\n"
			"The qfile parameter allows you to provide a quality file associated with the fastafile.\n"
			"The numbases parameter allows you to specify the number of bases you want to keep.\n"
			"The keep parameter allows you to specify whether you want to keep the front or the back of your sequence, default=front.\n"
			"The countgaps parameter allows you to specify whether you want to count gaps as bases, default=false.\n"
			"The short parameter allows you to specify you want to keep sequences that are too short to chop, default=false.\n"
			"The keepn parameter allows you to specify you want to keep ambigous bases, default=false.\n"
			"The processors parameter allows you to specify how many processors you would like to use.  The default is 1. \n"
			"For example, if you ran chop.seqs with numbases=200 and short=t, if a sequence had 100 bases mothur would keep the sequence rather than eliminate it.\n"
			"Example chop.seqs(fasta=amazon.fasta, numbases=200, keep=front).\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChopSeqsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChopSeqsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],chop.fasta"; }
	else if (type == "qfile") { pattern = "[filename],chop.qual"; }
	else if (type == "name") { pattern = "[filename],chop.names"; }
	else if (type == "group") { pattern = "[filename],chop.groups"; }
	else if (type == "count") { pattern = "[filename],chop.count_table"; }
	else if (type == "accnos") { pattern = "[filename],chop.accnos"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ChopSeqsCommand::ChopSeqsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChopSeqsCommand, ChopSeqsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
ChopSeqsCommand::ChopSeqsCommand(Settings& settings, string option) : Command(settings, option) {
}
//**********************************************************************************************************************

int ChopSeqsCommand::execute() {

	map<string, string> variables;
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(fastafile); }
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(fastafile));
	string outputFileName = getOutputFileName("fasta", variables);
	outputNames.push_back(outputFileName); outputTypes["fasta"].push_back(outputFileName);
	string outputFileNameAccnos = getOutputFileName("accnos", variables);

	string fastafileTemp = "";
	if (qualfile != "") { fastafileTemp = outputFileName + ".qualFile.Positions.temp"; }

	vector<unsigned long long> positions;
	vector<linePair> lines;
#if defined (UNIX)
	positions = File::divideFile(fastafile, processors);
	for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }
#else
	int numSeqs = 0;
	positions = m->setFilePosFasta(fastafile, numSeqs);
	if (positions.size() < processors) { processors = positions.size(); }

	//figure out how many sequences you have to process
	int numSeqsPerProcessor = numSeqs / processors;
	for (int i = 0; i < processors; i++) {
		int startIndex = i * numSeqsPerProcessor;
		if (i == (processors - 1)) { numSeqsPerProcessor = numSeqs - i * numSeqsPerProcessor; }
		lines.push_back(linePair(positions[startIndex], numSeqsPerProcessor));
	}
#endif

	bool wroteAccnos = false;
	if (processors == 1) { wroteAccnos = driver(lines[0], fastafile, outputFileName, outputFileNameAccnos, fastafileTemp); }
	else { wroteAccnos = createProcesses(lines, fastafile, outputFileName, outputFileNameAccnos, fastafileTemp); }

	if (ctrlc_pressed) { return 0; }

	if (qualfile != "") {
		thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(qualfile); }
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(qualfile));
		string outputQualFileName = getOutputFileName("qfile", variables);
		outputNames.push_back(outputQualFileName); outputTypes["qfile"].push_back(outputQualFileName);

		processQual(outputQualFileName, fastafileTemp);
		File::remove(fastafileTemp);
	}

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (wroteAccnos) {
		outputNames.push_back(outputFileNameAccnos); outputTypes["accnos"].push_back(outputFileNameAccnos);

		//use remove.seqs to create new name, group and count file
		if ((countfile != "") || (namefile != "") || (groupfile != "")) {
			string inputString = "accnos=" + outputFileNameAccnos;

			if (countfile != "") { inputString += ", count=" + countfile; }
			else {
				if (namefile != "") { inputString += ", name=" + namefile; }
				if (groupfile != "") { inputString += ", group=" + groupfile; }
			}

			LOG(INFO) << "/******************************************/" << '\n';
			LOG(INFO) << "Running command: remove.seqs(" + inputString + ")" << '\n';
			m->mothurCalling = true;

			Command* removeCommand = new RemoveSeqsCommand(inputString);
			removeCommand->execute();

			map<string, vector<string> > filenames = removeCommand->getOutputFiles();

			delete removeCommand;
			m->mothurCalling = false;
			LOG(INFO) << "/******************************************/" << '\n';

			if (groupfile != "") {
				thisOutputDir = outputDir;
				if (outputDir == "") { thisOutputDir += File::getPath(groupfile); }
				variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(groupfile));
				string outGroup = getOutputFileName("group", variables);
				m->renameFile(filenames["group"][0], outGroup);
				outputNames.push_back(outGroup); outputTypes["group"].push_back(outGroup);
			}

			if (namefile != "") {
				thisOutputDir = outputDir;
				if (outputDir == "") { thisOutputDir += File::getPath(namefile); }
				variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(namefile));
				string outName = getOutputFileName("name", variables);
				m->renameFile(filenames["name"][0], outName);
				outputNames.push_back(outName); outputTypes["name"].push_back(outName);
			}

			if (countfile != "") {
				thisOutputDir = outputDir;
				if (outputDir == "") { thisOutputDir += File::getPath(countfile); }
				variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(countfile));
				string outCount = getOutputFileName("count", variables);
				m->renameFile(filenames["count"][0], outCount);
				outputNames.push_back(outCount); outputTypes["count"].push_back(outCount);
			}
		}
	}
	else { File::remove(outputFileNameAccnos); }

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	//set fasta file as new current fastafile
	string current = "";
	itTypes = outputTypes.find("fasta");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("fasta", current); }
	}

	if (wroteAccnos) { //set accnos file as new current accnosfile
		itTypes = outputTypes.find("accnos");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("accnos", current); }
		}

		itTypes = outputTypes.find("name");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("name", current); }
		}

		itTypes = outputTypes.find("group");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("group", current); }
		}

		itTypes = outputTypes.find("count");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
		}
	}



	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in ChopSeqsCommand, execute";
	exit(1);
}
}
/**************************************************************************************************/
bool ChopSeqsCommand::createProcesses(vector<linePair> lines, string filename, string outFasta, string outAccnos, string fastafileTemp) {
	try {
		int process = 1;
		bool wroteAccnos = false;
		vector<int> processIDS;
		vector<string> nonBlankAccnosFiles;
		bool recalc = false;

#if defined (UNIX)

		//loop through and create all the processes you want
		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				string fastafileTempThisProcess = fastafileTemp;
				if (fastafileTempThisProcess != "") { fastafileTempThisProcess = fastafileTempThisProcess + m->mothurGetpid(process) + ".temp"; }
				wroteAccnos = driver(lines[process], filename, outFasta + m->mothurGetpid(process) + ".temp", outAccnos + m->mothurGetpid(process) + ".temp", fastafileTempThisProcess);

				//pass numSeqs to parent
				ofstream out;
				string tempFile = fastafile + m->mothurGetpid(process) + ".bool.temp";
				File::openOutputFile(tempFile, out);
				out << wroteAccnos << endl;
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
			vector<unsigned long long> positions = File::divideFile(filename, processors);
			for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }
			processIDS.resize(0);
			process = 1;

			while (process != processors) {
				pid_t pid = fork();

				if (pid > 0) {
					processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
					process++;
				}
				else if (pid == 0) {
					string fastafileTempThisProcess = fastafileTemp;
					if (fastafileTempThisProcess != "") { fastafileTempThisProcess = fastafileTempThisProcess + m->mothurGetpid(process) + ".temp"; }
					wroteAccnos = driver(lines[process], filename, outFasta + m->mothurGetpid(process) + ".temp", outAccnos + m->mothurGetpid(process) + ".temp", fastafileTempThisProcess);

					//pass numSeqs to parent
					ofstream out;
					string tempFile = fastafile + m->mothurGetpid(process) + ".bool.temp";
					File::openOutputFile(tempFile, out);
					out << wroteAccnos << endl;
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

		//do your part
		wroteAccnos = driver(lines[0], filename, outFasta, outAccnos, fastafileTemp);

		//force parent to wait until all the processes are done
		for (int i = 0;i < processIDS.size();i++) {
			int temp = processIDS[i];
			wait(&temp);
		}


		if (wroteAccnos) { nonBlankAccnosFiles.push_back(outAccnos); }
		else { File::remove(outAccnos); } //remove so other files can be renamed to it

		//parent reads in and combine Filter info
		for (int i = 0; i < processIDS.size(); i++) {
			string tempFilename = fastafile + toString(processIDS[i]) + ".bool.temp";
			ifstream in;
			File::openInputFile(tempFilename, in);

			bool temp;
			in >> temp; File::gobble(in);
			if (temp) { wroteAccnos = temp; nonBlankAccnosFiles.push_back(outAccnos + toString(processIDS[i]) + ".temp"); }
			else { File::remove((outAccnos + toString(processIDS[i]) + ".temp")); }

			in.close();
			File::remove(tempFilename);
		}
#else
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//Windows version shared memory, so be careful when passing variables through the seqSumData struct. 
		//Above fork() will clone, so memory is separate, but that's not the case with windows, 
		//Taking advantage of shared memory to allow both threads to add info to vectors.
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		vector<chopData*> pDataArray;
		vector<DWORD> dwThreadIdArray(processors - 1);
		vector<HANDLE> hThreadArray(processors - 1);

		//Create processor worker threads.
		for (int i = 0; i < processors - 1; i++) {

			string extension = "";
			if (i != 0) { extension = toString(i) + ".temp"; processIDS.push_back(i); }
			// Allocate memory for thread data.
			string fastafileTempThisProcess = fastafileTemp;
			if (fastafileTempThisProcess != "") { fastafileTempThisProcess = fastafileTempThisProcess + extension; }
			chopData* tempChop = new chopData(filename, (outFasta + extension), (outAccnos + extension), m, lines[i].start, lines[i].end, keep, countGaps, numbases, Short, keepN, qualfile, fastafileTempThisProcess);
			pDataArray.push_back(tempChop);

			//MyChopThreadFunction is in header. It must be global or static to work with the threads.
			//default security attributes, thread function name, argument to thread function, use default creation flags, returns the thread identifier
			hThreadArray[i] = CreateThread(NULL, 0, MyChopThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]);
		}

		//do your part
		string fastafileTempThisProcess = fastafileTemp;
		if (fastafileTempThisProcess != "") { fastafileTempThisProcess = fastafileTempThisProcess + toString(processors - 1) + ".temp"; }
		wroteAccnos = driver(lines[processors - 1], filename, (outFasta + toString(processors - 1) + ".temp"), (outAccnos + toString(processors - 1) + ".temp"), fastafileTempThisProcess);
		processIDS.push_back(processors - 1);

		//Wait until all threads have terminated.
		WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

		if (wroteAccnos) { nonBlankAccnosFiles.push_back(outAccnos); }
		else { File::remove(outAccnos); } //remove so other files can be renamed to it

		//Close all thread handles and free memory allocations.
		for (int i = 0; i < pDataArray.size(); i++) {
			if (pDataArray[i]->wroteAccnos) { wroteAccnos = pDataArray[i]->wroteAccnos; nonBlankAccnosFiles.push_back(outAccnos + toString(processIDS[i]) + ".temp"); }
			else { File::remove((outAccnos + toString(processIDS[i]) + ".temp")); }
			//check to make sure the process finished
			if (pDataArray[i]->count != pDataArray[i]->end) {
				LOG(LOGERROR) << "process " + toString(i) + " only processed " + toString(pDataArray[i]->count) + " of " + toString(pDataArray[i]->end) + " sequences assigned to it, quitting. \n"; ctrlc_pressed = true;
			}
			CloseHandle(hThreadArray[i]);
			delete pDataArray[i];
		}
#endif		

		for (int i = 0; i < processIDS.size(); i++) {
			if (fastafileTemp != "") {
				File::appendFiles((fastafileTemp + toString(processIDS[i]) + ".temp"), fastafileTemp);
				File::remove((fastafileTemp + toString(processIDS[i]) + ".temp"));
			}
			File::appendFiles((outFasta + toString(processIDS[i]) + ".temp"), outFasta);
			File::remove((outFasta + toString(processIDS[i]) + ".temp"));
		}

		if (nonBlankAccnosFiles.size() != 0) {
			m->renameFile(nonBlankAccnosFiles[0], outAccnos);

			for (int h = 1; h < nonBlankAccnosFiles.size(); h++) {
				File::appendFiles(nonBlankAccnosFiles[h], outAccnos);
				File::remove(nonBlankAccnosFiles[h]);
			}
		}
		else { //recreate the accnosfile if needed
			ofstream out;
			File::openOutputFile(outAccnos, out);
			out.close();
		}

		return wroteAccnos;
	}
	/**************************************************************************************/
	bool ChopSeqsCommand::driver(linePair filePos, string filename, string outFasta, string outAccnos, string fastaFileTemp) {

		ofstream out;
		File::openOutputFile(outFasta, out);

		ofstream outAcc;
		File::openOutputFile(outAccnos, outAcc);

		ofstream outfTemp;
		if (fastaFileTemp != "") { File::openOutputFile(fastaFileTemp, outfTemp); }

		ifstream in;
		File::openInputFile(filename, in);

		in.seekg(filePos.start);

		//adjust
		if (filePos.start == 0) {
			m->zapGremlins(in); File::gobble(in);
		}

		bool done = false;
		bool wroteAccnos = false;
		int count = 0;

		while (!done) {

			if (ctrlc_pressed) { in.close(); out.close(); return 1; }

			Sequence seq(in); File::gobble(in);

			if (ctrlc_pressed) { in.close(); out.close(); outAcc.close(); File::remove(outFasta); File::remove(outAccnos); if (fastaFileTemp != "") { outfTemp.close(); File::remove(fastaFileTemp); } return 0; }

			if (seq.getName() != "") {
				string qualValues = "";
				string newSeqString = getChopped(seq, qualValues);

				//output trimmed sequence
				if (newSeqString != "") {
					out << ">" << seq.getName() << endl << newSeqString << endl;
				}
				else {
					outAcc << seq.getName() << endl;
					wroteAccnos = true;
				}
				if (fastaFileTemp != "") { outfTemp << qualValues << endl; }
				count++;
			}

#if defined (UNIX)
			unsigned long long pos = in.tellg();
			if ((pos == -1) || (pos >= filePos.end)) { break; }
#else
			if (in.eof()) { break; }
#endif
			//report progress
			if ((count) % 10000 == 0) { LOG(INFO) << toString(count) << '\n'; }

		}
		//report progress
		if ((count) % 10000 != 0) { LOG(INFO) << toString(count) << '\n'; }


		in.close();
		out.close();
		outAcc.close();
		if (fastaFileTemp != "") { outfTemp.close(); }

		return wroteAccnos;
	}
	//**********************************************************************************************************************
	string ChopSeqsCommand::getChopped(Sequence seq, string& qualValues) {
		string temp = seq.getAligned();
		string tempUnaligned = seq.getUnaligned();

		if (countGaps) {
			//if needed trim sequence
			if (keep == "front") {//you want to keep the beginning
				int tempLength = temp.length();

				if (tempLength > numbases) { //you have enough bases to remove some

					int stopSpot = 0;
					int numBasesCounted = 0;

					for (int i = 0; i < temp.length(); i++) {
						//eliminate N's
						if (!keepN) { if (toupper(temp[i]) == 'N') { temp[i] = '.'; } }

						numBasesCounted++;

						if (numBasesCounted >= numbases) { stopSpot = i; break; }
					}

					if (stopSpot == 0) { temp = ""; }
					else { temp = temp.substr(0, stopSpot + 1); }

				}
				else {
					if (!Short) { temp = ""; } //sequence too short
				}
			}
			else { //you are keeping the back
				int tempLength = temp.length();
				if (tempLength > numbases) { //you have enough bases to remove some

					int stopSpot = 0;
					int numBasesCounted = 0;

					for (int i = (temp.length() - 1); i >= 0; i--) {
						//eliminate N's
						if (!keepN) { if (toupper(temp[i]) == 'N') { temp[i] = '.'; } }

						numBasesCounted++;

						if (numBasesCounted >= numbases) { stopSpot = i; break; }
					}

					if (stopSpot == 0) { temp = ""; }
					else { temp = temp.substr(stopSpot + 1); }
				}
				else {
					if (!Short) { temp = ""; } //sequence too short
				}
			}

		}
		else {

			//if needed trim sequence
			if (keep == "front") {//you want to keep the beginning
				int tempLength = tempUnaligned.length();

				if (tempLength > numbases) { //you have enough bases to remove some

					int stopSpot = 0;
					int numBasesCounted = 0;

					for (int i = 0; i < temp.length(); i++) {
						//eliminate N's
						if (!keepN) {
							if (toupper(temp[i]) == 'N') {
								temp[i] = '.';
								tempLength--;
								if (tempLength < numbases) { stopSpot = 0; break; }
							}
						}
						if (isalpha(temp[i])) { numBasesCounted++; }

						if (numBasesCounted >= numbases) { stopSpot = i; break; }
					}

					if (stopSpot == 0) { temp = ""; }
					else { temp = temp.substr(0, stopSpot + 1); }

					qualValues = seq.getName() + '\t' + toString(0) + '\t' + toString(stopSpot + 1) + '\n';

				}
				else {
					if (!Short) { temp = ""; qualValues = seq.getName() + '\t' + toString(0) + '\t' + toString(0) + '\n'; } //sequence too short
					else { qualValues = seq.getName() + '\t' + toString(0) + '\t' + toString(tempLength) + '\n'; }
				}
			}
			else { //you are keeping the back
				int tempLength = tempUnaligned.length();
				if (tempLength > numbases) { //you have enough bases to remove some

					int stopSpot = 0;
					int numBasesCounted = 0;

					for (int i = (temp.length() - 1); i >= 0; i--) {
						if (!keepN) {
							//eliminate N's
							if (toupper(temp[i]) == 'N') {
								temp[i] = '.';
								tempLength--;
								if (tempLength < numbases) { stopSpot = 0; break; }
							}
						}
						if (isalpha(temp[i])) { numBasesCounted++; }

						if (numBasesCounted >= numbases) { stopSpot = i; break; }
					}

					if (stopSpot == 0) { temp = ""; }
					else { temp = temp.substr(stopSpot); }

					qualValues = seq.getName() + '\t' + toString(stopSpot) + '\t' + toString(temp.length() - 1) + '\n';

				}
				else {
					if (!Short) { temp = ""; qualValues = seq.getName() + '\t' + toString(0) + '\t' + toString(0) + '\n'; } //sequence too short
					else { qualValues = seq.getName() + '\t' + toString(0) + '\t' + toString(tempLength) + '\n'; }
				}
			}
		}

		return temp;
	}
	//**********************************************************************************************************************
	int ChopSeqsCommand::processQual(string outputFile, string inputFile) {
		ofstream out;
		File::openOutputFile(outputFile, out);

		ifstream in;
		File::openInputFile(inputFile, in);

		ifstream inQual;
		File::openInputFile(qualfile, inQual);

		LOG(INFO) << "Processing the quality file.\n";

		int count = 0;
		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); out.close(); return 0; }

			QualityScores qual(inQual);

			string name = "";
			int start = 0; int end = 0;
			in >> name >> start >> end; File::gobble(in);

			if (qual.getName() != "") {
				if (qual.getName() != name) { start = 0; end = 0; }
				else if (start != 0) {
					qual.trimQScores(start, -1);
					qual.printQScores(out);
				}
				else if ((start == 0) && (end == 0)) {}
				else if ((start == 0) && (end != 0)) {
					qual.trimQScores(-1, end);
					qual.printQScores(out);
				}
			}
			count++;
			//report progress
			if ((count) % 10000 == 0) { LOG(INFO) << toString(count) << '\n'; }

		}
		//report progress
		if ((count) % 10000 != 0) { LOG(INFO) << toString(count) << '\n'; }

		in.close();
		out.close();

		return 0;
	}
	//**********************************************************************************************************************


