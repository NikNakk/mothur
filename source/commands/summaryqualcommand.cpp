/*
 *  summaryqualcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 11/28/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "summaryqualcommand.h"
#include "counttable.h"

 //**********************************************************************************************************************
vector<string> SummaryQualCommand::setParameters() {
	try {
		CommandParameter pqual("qfile", "InputTypes", "", "", "none", "none", "none", "summary", false, true, true); parameters.push_back(pqual);
		CommandParameter pname("name", "InputTypes", "", "", "namecount", "none", "none", "", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "namecount", "none", "none", "", false, false, true); parameters.push_back(pcount);
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SummaryQualCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string SummaryQualCommand::getHelpString() {
	try {
		string helpString = "The summary.qual command reads a quality file and an optional name or count file, and summarizes the quality information.\n"
			"The summary.tax command parameters are qfile, name, count and processors. qfile is required, unless you have a valid current quality file.\n"
			"The name parameter allows you to enter a name file associated with your quality file. \n"
			"The count parameter allows you to enter a count file associated with your quality file. \n"
			"The summary.qual command should be in the following format: \n"
			"summary.qual(qfile=yourQualityFile) \n"
			"Note: No spaces between parameter labels (i.e. qfile), '=' and parameters (i.e.yourQualityFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SummaryQualCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string SummaryQualCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "summary") { pattern = "[filename],qual.summary"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
SummaryQualCommand::SummaryQualCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["summary"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SummaryQualCommand, SummaryQualCommand";
		exit(1);
	}
}
//***************************************************************************************************************

SummaryQualCommand::SummaryQualCommand(Settings& settings, string option) : Command(settings, option) {
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

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("qfile");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["qfile"] = inputDir + it->second; }
			}

			it = parameters.find("name");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["name"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["summary"] = tempOutNames;

		//check for required parameters
		qualfile = validParameter.validFile(parameters, "qfile", true);
		if (qualfile == "not open") { qualfile = ""; abort = true; }
		else if (qualfile == "not found") {
			qualfile = settings.getCurrent("qual");
			if (qualfile != "") { LOG(INFO) << "Using " + qualfile + " as input file for the qfile parameter." << '\n'; }
			else { LOG(INFO) << "You have no current quality file and the qfile parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("qual", qualfile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { namefile = ""; abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { settings.setCurrent("name", namefile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { abort = true; countfile = ""; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		if ((countfile != "") && (namefile != "")) { LOG(INFO) << "You must enter ONLY ONE of the following: count or name." << '\n'; abort = true; }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = "";
			outputDir += File::getPath(qualfile); //if user entered a file with a path then preserve it	
		}

		string temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);


		if (countfile == "") {
			if (namefile == "") {
				vector<string> files; files.push_back(qualfile);
				OptionParser::getNameFile(files);
			}
		}
	}
}
//***************************************************************************************************************
int SummaryQualCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		int start = time(NULL);
		int numSeqs = 0;

		vector<int> position;
		vector<int> averageQ;
		vector< vector<int> > scores;

		if (ctrlc_pressed) { return 0; }

		if (namefile != "") { nameMap = m->readNames(namefile); }
		else if (countfile != "") {
			CountTable ct;
			ct.readTable(countfile, false, false);
			nameMap = ct.getNameMap();
		}

		vector<unsigned long long> positions;
#if defined (UNIX)
		positions = File::divideFile(qualfile, processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }
#else	
		if (processors == 1) {
			lines.push_back(linePair(0, 1000));
		}
		else {
			positions = m->setFilePosFasta(qualfile, numSeqs);
			if (positions.size() < processors) { processors = positions.size(); }

			//figure out how many sequences you have to process
			int numSeqsPerProcessor = numSeqs / processors;
			for (int i = 0; i < processors; i++) {
				int startIndex = i * numSeqsPerProcessor;
				if (i == (processors - 1)) { numSeqsPerProcessor = numSeqs - i * numSeqsPerProcessor; }
				lines.push_back(linePair(positions[startIndex], numSeqsPerProcessor));
			}
		}
#endif


		if (processors == 1) { numSeqs = driverCreateSummary(position, averageQ, scores, qualfile, lines[0]); }
		else { numSeqs = createProcessesCreateSummary(position, averageQ, scores, qualfile); }

		if (ctrlc_pressed) { return 0; }

		//print summary file
		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(qualfile));
		string summaryFile = getOutputFileName("summary", variables);
		printQual(summaryFile, position, averageQ, scores);

		if (ctrlc_pressed) { File::remove(summaryFile); return 0; }

		//output results to screen
		cout.setf(ios::fixed, ios::floatfield); cout.setf(ios::showpoint);
		LOG(INFO) << '\n' << "Position\tNumSeqs\tAverageQ" << '\n';
		for (int i = 0; i < position.size(); i += 100) {
			float average = averageQ[i] / (float)position[i];
			cout << i << '\t' << position[i] << '\t' << average;
			LOG(FILEONLY) << toString(i) + "\t" + toString(position[i]) + "\t" + toString(average) << '\n';
		}

		LOG(INFO) << std::endl << "It took " + toString(time(NULL) - start) + " secs to create the summary file for " + toString(numSeqs) + " sequences." << '\n' << '\n';
		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		LOG(INFO) << summaryFile << '\n';	outputNames.push_back(summaryFile); outputTypes["summary"].push_back(summaryFile);
		LOG(INFO) << "";

		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SummaryQualCommand, execute";
		exit(1);
	}
}
/**************************************************************************************/
int SummaryQualCommand::driverCreateSummary(vector<int>& position, vector<int>& averageQ, vector< vector<int> >& scores, string filename, linePair filePos) {
	try {
		ifstream in;
		File::openInputFile(filename, in);

		in.seekg(filePos.start);

		//adjust start if null strings
		if (filePos.start == 0) { m->zapGremlins(in); File::gobble(in); }

		bool done = false;
		int count = 0;

		while (!done) {

			if (ctrlc_pressed) { in.close(); return 1; }

			QualityScores current(in); File::gobble(in);

			if (current.getName() != "") {

				int num = 1;
				if ((namefile != "") || (countfile != "")) {
					//make sure this sequence is in the namefile, else error 
					map<string, int>::iterator it = nameMap.find(current.getName());

					if (it == nameMap.end()) { LOG(LOGERROR) << "" + current.getName() + " is not in your namefile, please correct." << '\n'; ctrlc_pressed = true; }
					else { num = it->second; }
				}

				vector<int> thisScores = current.getQualityScores();

				//resize to num of positions setting number of seqs with that size to 1
				if (position.size() < thisScores.size()) { position.resize(thisScores.size(), 0); }
				if (averageQ.size() < thisScores.size()) { averageQ.resize(thisScores.size(), 0); }
				if (scores.size() < thisScores.size()) {
					scores.resize(thisScores.size());
					for (int i = 0; i < scores.size(); i++) { scores[i].resize(41, 0); }
				}

				//increase counts of number of seqs with this position
				//average is really the total, we will average in execute
				for (int i = 0; i < thisScores.size(); i++) {
					position[i] += num;
					averageQ[i] += (thisScores[i] * num); //weighting for namesfile
					if (thisScores[i] > 40) { LOG(LOGERROR) << "" + current.getName() + " has a quality scores of " + toString(thisScores[i]) + ", expecting values to be less than 40." << '\n'; ctrlc_pressed = true; }
					else { scores[i][thisScores[i]] += num; }
				}

				count += num;
			}

#if defined (UNIX)
			unsigned long long pos = in.tellg();
			if ((pos == -1) || (pos >= filePos.end)) { break; }
#else
			if (in.eof()) { break; }
#endif
		}

		in.close();

		return count;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SummaryQualCommand, driverCreateSummary";
		exit(1);
	}
}
/**************************************************************************************************/
int SummaryQualCommand::createProcessesCreateSummary(vector<int>& position, vector<int>& averageQ, vector< vector<int> >& scores, string filename) {
	int process = 1;
	int numSeqs = 0;
	processIDS.clear();
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
			numSeqs = driverCreateSummary(position, averageQ, scores, qualfile, lines[process]);

			//pass numSeqs to parent
			ofstream out;
			string tempFile = qualfile + m->mothurGetpid(process) + ".num.temp";
			File::openOutputFile(tempFile, out);

			out << numSeqs << endl;
			out << position.size() << endl;
			for (int k = 0; k < position.size(); k++) { out << position[k] << '\t'; }  out << endl;
			for (int k = 0; k < averageQ.size(); k++) { out << averageQ[k] << '\t'; }  out << endl;
			for (int k = 0; k < scores.size(); k++) {
				for (int j = 0; j < 41; j++) {
					out << scores[k][j] << '\t';
				}
				out << endl;
			}
			out << endl;

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
				File::remove(qualfile + (toString(processIDS[i]) + ".num.temp"));
			}
			recalc = true;
			break;
		}
	}

	if (recalc) {
		//test line, also set recalc to true.
		//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove(qualfile + (toString(processIDS[i]) + ".num.temp"));}processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

		//redo file divide
		lines.clear();
		vector<unsigned long long> positions = File::divideFile(qualfile, processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }

		numSeqs = 0;
		processIDS.resize(0);
		process = 1;
		position.clear();
		averageQ.clear();
		scores.clear();

		//loop through and create all the processes you want
		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				numSeqs = driverCreateSummary(position, averageQ, scores, qualfile, lines[process]);

				//pass numSeqs to parent
				ofstream out;
				string tempFile = qualfile + m->mothurGetpid(process) + ".num.temp";
				File::openOutputFile(tempFile, out);

				out << numSeqs << endl;
				out << position.size() << endl;
				for (int k = 0; k < position.size(); k++) { out << position[k] << '\t'; }  out << endl;
				for (int k = 0; k < averageQ.size(); k++) { out << averageQ[k] << '\t'; }  out << endl;
				for (int k = 0; k < scores.size(); k++) {
					for (int j = 0; j < 41; j++) {
						out << scores[k][j] << '\t';
					}
					out << endl;
				}
				out << endl;

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
	numSeqs = driverCreateSummary(position, averageQ, scores, qualfile, lines[0]);

	//force parent to wait until all the processes are done
	for (int i = 0;i < processIDS.size();i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	//parent reads in and combine Filter info
	for (int i = 0; i < processIDS.size(); i++) {
		string tempFilename = qualfile + toString(processIDS[i]) + ".num.temp";
		ifstream in;
		File::openInputFile(tempFilename, in);

		int temp, tempNum;
		in >> tempNum; File::gobble(in); numSeqs += tempNum;
		in >> tempNum; File::gobble(in);

		if (position.size() < tempNum) { position.resize(tempNum, 0); }
		if (averageQ.size() < tempNum) { averageQ.resize(tempNum, 0); }
		if (scores.size() < tempNum) {
			scores.resize(tempNum);
			for (int i = 0; i < scores.size(); i++) { scores[i].resize(41, 0); }
		}

		for (int k = 0; k < tempNum; k++) { in >> temp; position[k] += temp; }		File::gobble(in);
		for (int k = 0; k < tempNum; k++) { in >> temp; averageQ[k] += temp; }		File::gobble(in);
		for (int k = 0; k < tempNum; k++) {
			for (int j = 0; j < 41; j++) {
				in >> temp; scores[k][j] += temp;
				File::gobble(in);
			}
		}

		in.close();
		File::remove(tempFilename);
	}

#else
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Windows version shared memory, so be careful when passing variables through the seqSumQualData struct. 
	//Above fork() will clone, so memory is separate, but that's not the case with windows, 
	//Taking advantage of shared memory to pass results vectors.
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<seqSumQualData*> pDataArray;
	vector<DWORD> dwThreadIdArray(processors);
	vector<HANDLE> hThreadArray(processors);

	bool hasNameMap = false;
	if ((namefile != "") || (countfile != "")) { hasNameMap = true; }

	//Create processor worker threads.
	for (int i = 0; i < processors; i++) {

		// Allocate memory for thread data.
		seqSumQualData* tempSum = new seqSumQualData(filename, m, lines[i].start, lines[i].end, hasNameMap, nameMap);
		pDataArray.push_back(tempSum);
		processIDS.push_back(i);

		hThreadArray[i] = CreateThread(NULL, 0, MySeqSumQualThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]);
	}

	//Wait until all threads have terminated.
	WaitForMultipleObjects(processors, &(hThreadArray[0]), TRUE, INFINITE);

	//Close all thread handles and free memory allocations.
	for (int i = 0; i < pDataArray.size(); i++) {
		numSeqs += pDataArray[i]->numSeqs;
		if (pDataArray[i]->count != pDataArray[i]->end) {
			LOG(LOGERROR) << "process " + toString(i) + " only processed " + toString(pDataArray[i]->count) + " of " + toString(pDataArray[i]->end) + " sequences assigned to it, quitting. \n"; ctrlc_pressed = true;
		}
		int tempNum = pDataArray[i]->position.size();
		if (position.size() < tempNum) { position.resize(tempNum, 0); }
		if (averageQ.size() < tempNum) { averageQ.resize(tempNum, 0); }
		if (scores.size() < tempNum) {
			scores.resize(tempNum);
			for (int i = 0; i < scores.size(); i++) { scores[i].resize(41, 0); }
		}

		for (int k = 0; k < tempNum; k++) { position[k] += pDataArray[i]->position[k]; }
		for (int k = 0; k < tempNum; k++) { averageQ[k] += pDataArray[i]->averageQ[k]; }
		for (int k = 0; k < tempNum; k++) { for (int j = 0; j < 41; j++) { scores[k][j] += pDataArray[i]->scores[k][j]; } }

		CloseHandle(hThreadArray[i]);
		delete pDataArray[i];
	}
#endif		
	return numSeqs;
}
/**************************************************************************************************/
int SummaryQualCommand::printQual(string sumFile, vector<int>& position, vector<int>& averageQ, vector< vector<int> >& scores) {
	ofstream out;
	File::openOutputFile(sumFile, out);
	out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);
	outputNames.push_back(sumFile); outputTypes["summary"].push_back(sumFile);

	//print headings
	out << "Position\tnumSeqs\tAverageQ";
	for (int i = 0; i < 41; i++) { out << '\t' << "q" << i; }
	out << endl;

	for (int i = 0; i < position.size(); i++) {

		if (ctrlc_pressed) { out.close(); return 0; }

		double average = averageQ[i] / (float)position[i];
		out << i << '\t' << position[i] << '\t' << average;

		for (int j = 0; j < 41; j++) {
			out << '\t' << scores[i][j];
		}
		out << endl;
	}

	out.close();

	return 0;
}

/**************************************************************************************/


