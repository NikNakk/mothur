//
//  prcseqscommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 3/14/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "pcrseqscommand.h"

//**********************************************************************************************************************
vector<string> PcrSeqsCommand::setParameters() {
	try {
		nkParameters.add(new FastaFileParameter(settings, "fasta", true, true));
		CommandParameter poligos("oligos", "InputTypes", "", "", "ecolioligos", "none", "none", "", false, false, true); parameters.push_back(poligos);
		nkParameters.add(new NameFileParameter(settings, "name", false, true, "NameCount"));
		nkParameters.add(new CountFileParameter(settings, "count", false, true, "NameCount-CountGroup"));
		nkParameters.add(new GroupFileParameter(settings, "group", false, true, "CountGroup"));
		CommandParameter ptax("taxonomy", "InputTypes", "", "", "none", "none", "none", "taxonomy", false, false, true); parameters.push_back(ptax);
		CommandParameter pecoli("ecoli", "InputTypes", "", "", "ecolioligos", "none", "none", "", false, false); parameters.push_back(pecoli);
		nkParameters.add(new NumberParameter("start", -INFINITY, INFINITY, -1, false, false));
		nkParameters.add(new NumberParameter("end", -INFINITY, INFINITY, -1, false, false));
		CommandParameter pnomatch("nomatch", "Multiple", "reject-keep", "reject", "", "", "", "", false, false); parameters.push_back(pnomatch);
		CommandParameter ppdiffs("pdiffs", "Number", "", "0", "", "", "", "", false, false, true); parameters.push_back(ppdiffs);

		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new BooleanParameter("keepprimer", false, false, false));
		nkParameters.add(new BooleanParameter("keepdots", true, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PcrSeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string PcrSeqsCommand::getHelpString() {
	try {
		string helpString = "The pcr.seqs command reads a fasta file.\n"
			"The pcr.seqs command parameters are fasta, oligos, name, group, count, taxonomy, ecoli, start, end, nomatch, pdiffs, processors, keepprimer and keepdots.\n"
			"The ecoli parameter is used to provide a fasta file containing a single reference sequence (e.g. for e. coli) this must be aligned. Mothur will trim to the start and end positions of the reference sequence.\n"
			"The start parameter allows you to provide a starting position to trim to.\n"
			"The end parameter allows you to provide a ending position to trim from.\n"
			"The nomatch parameter allows you to decide what to do with sequences where the primer is not found. Default=reject, meaning remove from fasta file.  if nomatch=true, then do nothing to sequence.\n"
			"The processors parameter allows you to use multiple processors.\n"
			"The keepprimer parameter allows you to keep the primer, default=false.\n"
			"The keepdots parameter allows you to keep the leading and trailing .'s, default=true.\n"
			"The pdiffs parameter is used to specify the number of differences allowed in the primer. The default is 0.\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n"
			"For more details please check out the wiki http://www.mothur.org/wiki/Pcr.seqs .\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PcrSeqsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string PcrSeqsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],pcr,[extension]-[filename],[tag],pcr,[extension]"; }
	else if (type == "taxonomy") { pattern = "[filename],pcr,[extension]"; }
	else if (type == "name") { pattern = "[filename],pcr,[extension]"; }
	else if (type == "group") { pattern = "[filename],pcr,[extension]"; }
	else if (type == "count") { pattern = "[filename],pcr,[extension]"; }
	else if (type == "accnos") { pattern = "[filename],bad.accnos"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************

PcrSeqsCommand::PcrSeqsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["taxonomy"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PcrSeqsCommand, PcrSeqsCommand";
		exit(1);
	}
}
//***************************************************************************************************************

PcrSeqsCommand::PcrSeqsCommand(Settings& settings, string option) : Command(settings, option) {

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
		outputTypes["taxonomy"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["count"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("fasta");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fasta"] = inputDir + it->second; }
			}

			it = parameters.find("oligos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["oligos"] = inputDir + it->second; }
			}

			it = parameters.find("ecoli");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["ecoli"] = inputDir + it->second; }
			}

			it = parameters.find("taxonomy");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["taxonomy"] = inputDir + it->second; }
			}

			it = parameters.find("name");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["name"] = inputDir + it->second; }
			}

			it = parameters.find("group");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["group"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}

		}


		//check for required parameters
		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not found") {
			fastafile = settings.getCurrent("fasta");
			if (fastafile != "") { LOG(INFO) << "Using " + fastafile + " as input file for the fasta parameter." << '\n'; }
			else { LOG(INFO) << "You have no current fastafile and the fasta parameter is required." << '\n'; abort = true; }
		}
		else if (fastafile == "not open") { fastafile = ""; abort = true; }
		else { settings.setCurrent("fasta", fastafile); }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(fastafile); }

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		string temp;
		temp = validParameter.validFile(parameters, "keepprimer", false);  if (temp == "not found") { temp = "f"; }
		keepprimer = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "keepdots", false);  if (temp == "not found") { temp = "t"; }
		keepdots = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "oligos", true);
		if (temp == "not found") { oligosfile = ""; }
		else if (temp == "not open") { oligosfile = ""; abort = true; }
		else { oligosfile = temp; settings.setCurrent("oligos", oligosfile); }

		ecolifile = validParameter.validFile(parameters, "ecoli", true);
		if (ecolifile == "not found") { ecolifile = ""; }
		else if (ecolifile == "not open") { ecolifile = ""; abort = true; }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not found") { namefile = ""; }
		else if (namefile == "not open") { namefile = ""; abort = true; }
		else { settings.setCurrent("name", namefile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not found") { groupfile = ""; }
		else if (groupfile == "not open") { groupfile = ""; abort = true; }
		else { settings.setCurrent("group", groupfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		if ((namefile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: name or count." << '\n'; abort = true;
		}

		if ((groupfile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: group or count." << '\n'; abort = true;
		}

		taxfile = validParameter.validFile(parameters, "taxonomy", true);
		if (taxfile == "not found") { taxfile = ""; }
		else if (taxfile == "not open") { taxfile = ""; abort = true; }
		else { settings.setCurrent("taxonomy", taxfile); }

		temp = validParameter.validFile(parameters, "start", false);	if (temp == "not found") { temp = "-1"; }
		Utility::mothurConvert(temp, start);

		temp = validParameter.validFile(parameters, "end", false);	if (temp == "not found") { temp = "-1"; }
		Utility::mothurConvert(temp, end);

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "pdiffs", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, pdiffs);

		nomatch = validParameter.validFile(parameters, "nomatch", false);	if (nomatch == "not found") { nomatch = "reject"; }

		if ((nomatch != "reject") && (nomatch != "keep")) { LOG(LOGERROR) << "" + nomatch + " is not a valid entry for nomatch. Choices are reject and keep.\n";  abort = true; }

		//didnt set anything
		if ((oligosfile == "") && (ecolifile == "") && (start == -1) && (end == -1)) {
			LOG(LOGERROR) << "You did not set any options. Please provide an oligos or ecoli file, or set start or end.\n"; abort = true;
		}

		if ((oligosfile == "") && (ecolifile == "") && (start < 0) && (end == -1)) { LOG(LOGERROR) << "Invalid start value.\n"; abort = true; }

		if ((ecolifile != "") && (start != -1) && (end != -1)) {
			LOG(LOGERROR) << "You provided an ecoli file , but set the start or end parameters. Unsure what you intend.  When you provide the ecoli file, mothur thinks you want to use the start and end of the sequence in the ecoli file.\n"; abort = true;
		}


		if ((oligosfile != "") && (ecolifile != "")) {
			LOG(LOGERROR) << "You can not use an ecoli file at the same time as an oligos file.\n"; abort = true;
		}

		//check to make sure you didn't forget the name file by mistake			
		if (countfile == "") {
			if (namefile == "") {
				vector<string> files; files.push_back(fastafile);
				OptionParser::getNameFile(files);
			}
		}
	}

}
//***************************************************************************************************************

int PcrSeqsCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		int start = time(NULL);
		fileAligned = true; pairedOligos = false;

		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(fastafile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(fastafile));
		variables["[extension]"] = m->getExtension(fastafile);
		string trimSeqFile = getOutputFileName("fasta", variables);
		outputNames.push_back(trimSeqFile); outputTypes["fasta"].push_back(trimSeqFile);
		variables["[tag]"] = "scrap";
		string badSeqFile = getOutputFileName("fasta", variables);


		length = 0;
		if (oligosfile != "") { readOligos();     if (app.isDebug) { LOG(DEBUG) << "read oligos file. numprimers = " + toString(numFPrimers) + ", revprimers = " + toString(numRPrimers) + ".\n"; } }  if (ctrlc_pressed) { return 0; }
		if (ecolifile != "") { readEcoli(); }  if (ctrlc_pressed) { return 0; }

		vector<unsigned long long> positions;
		int numFastaSeqs = 0;
#if defined (UNIX)
		positions = File::divideFile(fastafile, processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }
#else
		if (processors == 1) {
			lines.push_back(linePair(0, 1000));
		}
		else {
			positions = m->setFilePosFasta(fastafile, numFastaSeqs);
			if (positions.size() < processors) { processors = positions.size(); }

			//figure out how many sequences you have to process
			int numSeqsPerProcessor = numFastaSeqs / processors;
			for (int i = 0; i < processors; i++) {
				int startIndex = i * numSeqsPerProcessor;
				if (i == (processors - 1)) { numSeqsPerProcessor = numFastaSeqs - i * numSeqsPerProcessor; }
				lines.push_back(linePair(positions[startIndex], numSeqsPerProcessor));
			}
		}
#endif
		if (ctrlc_pressed) { return 0; }

		set<string> badNames;
		numFastaSeqs = createProcesses(fastafile, trimSeqFile, badSeqFile, badNames);

		if (ctrlc_pressed) { return 0; }

		//don't write or keep if blank
		if (badNames.size() != 0) { writeAccnos(badNames); }
		if (File::isBlank(badSeqFile)) { File::remove(badSeqFile); }
		else { outputNames.push_back(badSeqFile); outputTypes["fasta"].push_back(badSeqFile); }

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }
		if (namefile != "") { readName(badNames); }
		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }
		if (groupfile != "") { readGroup(badNames); }
		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }
		if (taxfile != "") { readTax(badNames); }
		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }
		if (countfile != "") { readCount(badNames); }
		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << '\n' << "";

		//set fasta file as new current fastafile
		string current = "";
		itTypes = outputTypes.find("fasta");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("fasta", current); }
		}

		itTypes = outputTypes.find("name");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("name", current); }
		}

		itTypes = outputTypes.find("group");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("group", current); }
		}

		itTypes = outputTypes.find("accnos");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("accnos", current); }
		}

		itTypes = outputTypes.find("taxonomy");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("taxonomy", current); }
		}

		itTypes = outputTypes.find("count");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
		}

		LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to screen " + toString(numFastaSeqs) + " sequences.";
		LOG(INFO) << "";


		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PcrSeqsCommand, execute";
		exit(1);
	}
}
/**************************************************************************************************/
int PcrSeqsCommand::createProcesses(string filename, string goodFileName, string badFileName, set<string>& badSeqNames) {

	vector<int> processIDS;
	int process = 1;
	int num = 0;
	int pstart = -1; int pend = -1;
	bool adjustNeeded = false;
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
			string locationsFile = m->mothurGetpid(process) + ".temp";
			num = driverPcr(filename, goodFileName + m->mothurGetpid(process) + ".temp", badFileName + m->mothurGetpid(process) + ".temp", locationsFile, badSeqNames, lines[process], pstart, adjustNeeded);

			//pass numSeqs to parent
			ofstream out;
			string tempFile = filename + m->mothurGetpid(process) + ".num.temp";
			File::openOutputFile(tempFile, out);
			out << pstart << '\t' << adjustNeeded << endl;
			out << num << '\t' << badSeqNames.size() << endl;
			for (set<string>::iterator it = badSeqNames.begin(); it != badSeqNames.end(); it++) {
				out << (*it) << endl;
			}
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
				File::remove(filename + (toString(processIDS[i]) + ".num.temp"));
			}
			recalc = true;
			break;

		}
	}

	if (recalc) {
		//test line, also set recalc to true.
		//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove(filename + (toString(processIDS[i]) + ".num.temp"));}processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

		//redo file divide
		lines.clear();
		vector<unsigned long long> positions = File::divideFile(filename, processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }

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
				string locationsFile = m->mothurGetpid(process) + ".temp";
				num = driverPcr(filename, goodFileName + m->mothurGetpid(process) + ".temp", badFileName + m->mothurGetpid(process) + ".temp", locationsFile, badSeqNames, lines[process], pstart, adjustNeeded);

				//pass numSeqs to parent
				ofstream out;
				string tempFile = filename + m->mothurGetpid(process) + ".num.temp";
				File::openOutputFile(tempFile, out);
				out << pstart << '\t' << adjustNeeded << endl;
				out << num << '\t' << badSeqNames.size() << endl;
				for (set<string>::iterator it = badSeqNames.begin(); it != badSeqNames.end(); it++) {
					out << (*it) << endl;
				}
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

	string locationsFile = m->mothurGetpid(process) + ".temp";
	num = driverPcr(filename, goodFileName, badFileName, locationsFile, badSeqNames, lines[0], pstart, adjustNeeded);

	//force parent to wait until all the processes are done
	for (int i = 0;i < processIDS.size();i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	for (int i = 0; i < processIDS.size(); i++) {
		ifstream in;
		string tempFile = filename + toString(processIDS[i]) + ".num.temp";
		File::openInputFile(tempFile, in);
		int numBadNames = 0; string name = "";
		int tpstart = -1; bool tempAdjust = false;

		if (!in.eof()) {
			in >> tpstart >> tempAdjust; File::gobble(in);

			if (tempAdjust) { adjustNeeded = true; }
			if (tpstart != -1) {
				if (tpstart != pstart) { adjustNeeded = true; }
				if (tpstart < pstart) { pstart = tpstart; } //smallest start
			}
			int tempNum = 0; in >> tempNum >> numBadNames; num += tempNum; File::gobble(in);
		}
		for (int j = 0; j < numBadNames; j++) {
			in >> name; File::gobble(in);
			badSeqNames.insert(name);
		}
		in.close(); File::remove(tempFile);

		File::appendFiles((goodFileName + toString(processIDS[i]) + ".temp"), goodFileName);
		File::remove((goodFileName + toString(processIDS[i]) + ".temp"));

		File::appendFiles((badFileName + toString(processIDS[i]) + ".temp"), badFileName);
		File::remove((badFileName + toString(processIDS[i]) + ".temp"));

		File::appendFiles((toString(processIDS[i]) + ".temp"), locationsFile);
		File::remove((toString(processIDS[i]) + ".temp"));
	}
#else

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Windows version shared memory, so be careful when passing variables through the sumScreenData struct. 
	//Above fork() will clone, so memory is separate, but that's not the case with windows, 
	//Taking advantage of shared memory to allow both threads to add info to badSeqNames.
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<pcrData*> pDataArray;
	vector<DWORD> dwThreadIdArray(processors - 1);
	vector<HANDLE> hThreadArray(processors - 1);

	string locationsFile = "locationsFile.txt";
	File::remove(locationsFile);
	File::remove(goodFileName);
	File::remove(badFileName);

	//Create processor worker threads.
	for (int i = 0; i < processors - 1; i++) {

		string extension = "";
		if (i != 0) { extension += toString(i) + ".temp"; processIDS.push_back(i); }

		// Allocate memory for thread data.
		pcrData* tempPcr = new pcrData(filename, goodFileName + extension, badFileName + extension, locationsFile + extension, m, oligosfile, ecolifile, nomatch, keepprimer, keepdots, start, end, length, pdiffs, lines[i].start, lines[i].end);
		pDataArray.push_back(tempPcr);

		//default security attributes, thread function name, argument to thread function, use default creation flags, returns the thread identifier
		hThreadArray[i] = CreateThread(NULL, 0, MyPcrThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]);
	}

	//do your part
	num = driverPcr(filename, (goodFileName + toString(processors - 1) + ".temp"), (badFileName + toString(processors - 1) + ".temp"), (locationsFile + toString(processors - 1) + ".temp"), badSeqNames, lines[processors - 1], pstart, adjustNeeded);
	processIDS.push_back(processors - 1);

	//Wait until all threads have terminated.
	WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

	//Close all thread handles and free memory allocations.
	for (int i = 0; i < pDataArray.size(); i++) {
		num += pDataArray[i]->count;
		if (pDataArray[i]->count != pDataArray[i]->fend) {
			LOG(LOGERROR) << "process " + toString(i) + " only processed " + toString(pDataArray[i]->count) + " of " + toString(pDataArray[i]->fend) + " sequences assigned to it, quitting. \n"; ctrlc_pressed = true;
		}
		if (pDataArray[i]->adjustNeeded) { adjustNeeded = true; }
		if (pDataArray[i]->pstart != -1) {
			if (pDataArray[i]->pstart != pstart) { adjustNeeded = true; }
			if (pDataArray[i]->pstart < pstart) { pstart = pDataArray[i]->pstart; }
		} //smallest start

		for (set<string>::iterator it = pDataArray[i]->badSeqNames.begin(); it != pDataArray[i]->badSeqNames.end(); it++) { badSeqNames.insert(*it); }
		CloseHandle(hThreadArray[i]);
		delete pDataArray[i];
	}

	for (int i = 0; i < processIDS.size(); i++) {
		File::appendFiles((goodFileName + toString(processIDS[i]) + ".temp"), goodFileName);
		File::remove((goodFileName + toString(processIDS[i]) + ".temp"));

		File::appendFiles((badFileName + toString(processIDS[i]) + ".temp"), badFileName);
		File::remove((badFileName + toString(processIDS[i]) + ".temp"));

		File::appendFiles((locationsFile + toString(processIDS[i]) + ".temp"), locationsFile);
		File::remove((locationsFile + toString(processIDS[i]) + ".temp"));
	}

#endif	



	if (fileAligned && adjustNeeded) {
		//find pend - pend is the biggest ending value, but we must account for when we adjust the start.  That adjustment may make the "new" end larger then the largest end. So lets find out what that "new" end will be.
		ifstream inLocations;
		File::openInputFile(locationsFile, inLocations);

		while (!inLocations.eof()) {

			if (ctrlc_pressed) { break; }

			string name = "";
			int thisStart = -1; int thisEnd = -1;
			if (numFPrimers != 0) { inLocations >> name >> thisStart; File::gobble(inLocations); }
			if (numRPrimers != 0) { inLocations >> name >> thisEnd;   File::gobble(inLocations); }
			else { pend = -1; break; }

			int myDiff = 0;
			if (pstart != -1) {
				if (thisStart != -1) {
					if (thisStart != pstart) { myDiff += (thisStart - pstart); }
				}
			}

			int myEnd = thisEnd + myDiff;
			//cout << name << '\t' << thisStart << '\t' << thisEnd << " diff = " << myDiff << '\t' << myEnd << endl;

			if (thisEnd != -1) {
				if (myEnd > pend) { pend = myEnd; }
			}

		}
		inLocations.close();

		adjustDots(goodFileName, locationsFile, pstart, pend);
	}
	else { File::remove(locationsFile); }

	return num;

}

//**********************************************************************************************************************
int PcrSeqsCommand::driverPcr(string filename, string goodFasta, string badFasta, string locationsName, set<string>& badSeqNames, linePair filePos, int& pstart, bool& adjustNeeded) {
	ofstream goodFile;
	File::openOutputFile(goodFasta, goodFile);

	ofstream badFile;
	File::openOutputFile(badFasta, badFile);

	ofstream locationsFile;
	File::openOutputFile(locationsName, locationsFile);

	ifstream inFASTA;
	File::openInputFile(filename, inFASTA);

	inFASTA.seekg(filePos.start);

	bool done = false;
	int count = 0;
	set<int> lengths;
	set<int> locations; //locations[0] = beginning locations, 

	//pdiffs, bdiffs, primers, barcodes, revPrimers
	map<string, int> primers;
	map<string, int> barcodes; //not used
	vector<string> revPrimer;
	if (pairedOligos) {
		map<int, oligosPair> primerPairs = oligos.getPairedPrimers();
		for (map<int, oligosPair>::iterator it = primerPairs.begin(); it != primerPairs.end(); it++) {
			primers[(it->second).forward] = it->first;
			revPrimer.push_back((it->second).reverse);
		}
	}
	else {
		primers = oligos.getPrimers();
		revPrimer = oligos.getReversePrimers();
	}

	TrimOligos trim(pdiffs, 0, primers, barcodes, revPrimer);

	while (!done) {

		if (ctrlc_pressed) { break; }

		Sequence currSeq(inFASTA); File::gobble(inFASTA);

		if (fileAligned) { //assume aligned until proven otherwise
			lengths.insert(currSeq.getAligned().length());
			if (lengths.size() > 1) { fileAligned = false; }
		}

		string trashCode = "";
		string locationsString = "";
		int thisPStart = -1;
		int thisPEnd = -1;
		int totalDiffs = 0;
		string commentString = "";

		if (ctrlc_pressed) { break; }

		if (currSeq.getName() != "") {

			if (app.isDebug) { LOG(DEBUG) << "seq name = " + currSeq.getName() + ".\n"; }

			bool goodSeq = true;
			if (oligosfile != "") {
				map<int, int> mapAligned;
				bool aligned = isAligned(currSeq.getAligned(), mapAligned);


				//process primers
				if (primers.size() != 0) {
					int primerStart = 0; int primerEnd = 0;
					vector<int> results = trim.findForward(currSeq, primerStart, primerEnd);
					bool good = true;
					if (results[0] > pdiffs) { good = false; }
					totalDiffs += results[0];
					commentString += "fpdiffs=" + toString(results[0]) + "(" + trim.getCodeValue(results[1], pdiffs) + ") ";

					if (!good) { if (nomatch == "reject") { goodSeq = false; } trashCode += "f"; }
					else {
						//are you aligned
						if (aligned) {
							if (!keepprimer) {
								if (keepdots) { currSeq.filterToPos(mapAligned[primerEnd - 1] + 1); } //mapAligned[primerEnd-1] is the location of the last base in the primer. we want to trim to the space just after that.  The -1 & +1 ensures if the primer is followed by gaps they are not trimmed causing an aligned sequence dataset to become unaligned.
								else {
									currSeq.setAligned(currSeq.getAligned().substr(mapAligned[primerEnd - 1] + 1));
									if (fileAligned) {
										thisPStart = mapAligned[primerEnd - 1] + 1; //locations[0].insert(mapAligned[primerEnd-1]+1);
										locationsString += currSeq.getName() + "\t" + toString(mapAligned[primerEnd - 1] + 1) + "\n";
									}
								}
							}
							else {
								if (keepdots) { currSeq.filterToPos(mapAligned[primerStart]); }
								else {
									currSeq.setAligned(currSeq.getAligned().substr(mapAligned[primerStart]));
									if (fileAligned) {
										thisPStart = mapAligned[primerStart]; //locations[0].insert(mapAligned[primerStart]);
										locationsString += currSeq.getName() + "\t" + toString(mapAligned[primerStart]) + "\n";
									}
								}
							}
							isAligned(currSeq.getAligned(), mapAligned);
						}
						else {
							if (!keepprimer) { currSeq.setAligned(currSeq.getUnaligned().substr(primerEnd)); }
							else { currSeq.setAligned(currSeq.getUnaligned().substr(primerStart)); }
						}
					}
				}

				//process reverse primers
				if (revPrimer.size() != 0) {
					int primerStart = 0; int primerEnd = 0;
					vector<int> results = trim.findReverse(currSeq, primerStart, primerEnd);
					bool good = true;
					if (results[0] > pdiffs) { good = false; }
					totalDiffs += results[0];
					commentString += "rpdiffs=" + toString(results[0]) + "(" + trim.getCodeValue(results[1], pdiffs) + ") ";

					if (!good) { if (nomatch == "reject") { goodSeq = false; } trashCode += "r"; }
					else {
						//are you aligned
						if (aligned) {
							if (!keepprimer) {
								if (keepdots) { currSeq.filterFromPos(mapAligned[primerStart]); }
								else {
									currSeq.setAligned(currSeq.getAligned().substr(0, mapAligned[primerStart]));
									if (fileAligned) {
										thisPEnd = mapAligned[primerStart]; //locations[1].insert(mapAligned[primerStart]);
										locationsString += currSeq.getName() + "\t" + toString(mapAligned[primerStart]) + "\n";
									}
								}
							}
							else {
								if (keepdots) { currSeq.filterFromPos(mapAligned[primerEnd - 1] + 1); }
								else {
									currSeq.setAligned(currSeq.getAligned().substr(0, mapAligned[primerEnd - 1] + 1));
									if (fileAligned) {
										thisPEnd = mapAligned[primerEnd - 1] + 1; //locations[1].insert(mapAligned[primerEnd-1]+1);
										locationsString += currSeq.getName() + "\t" + toString(mapAligned[primerEnd - 1] + 1) + "\n";
									}
								}
							}
						}
						else {
							if (!keepprimer) { currSeq.setAligned(currSeq.getUnaligned().substr(0, primerStart)); }
							else { currSeq.setAligned(currSeq.getUnaligned().substr(0, primerEnd)); }
						}
					}
				}
			}
			else if (ecolifile != "") {
				//make sure the seqs are aligned
				if (!fileAligned) { LOG(LOGERROR) << "seqs are not aligned. When using start and end your sequences must be aligned.\n"; ctrlc_pressed = true; break; }
				else if (currSeq.getAligned().length() != length) {
					LOG(LOGERROR) << "seqs are not the same length as ecoli seq. When using ecoli option your sequences must be aligned and the same length as the ecoli sequence.\n"; ctrlc_pressed = true; break;
				}
				else {
					if (keepdots) {
						currSeq.filterToPos(start);
						currSeq.filterFromPos(end);
					}
					else {
						string seqString = currSeq.getAligned().substr(0, end);
						seqString = seqString.substr(start);
						currSeq.setAligned(seqString);
					}
				}
			}
			else { //using start and end to trim
			   //make sure the seqs are aligned
				if (!fileAligned) { LOG(LOGERROR) << "seqs are not aligned. When using start and end your sequences must be aligned.\n"; ctrlc_pressed = true; break; }
				else {

					if (end != -1) {
						if (end > currSeq.getAligned().length()) { LOG(LOGERROR) << "end is longer than your sequence length, aborting.\n"; ctrlc_pressed = true; break; }
						else {
							if (keepdots) { currSeq.filterFromPos(end); }
							else {

								string seqString = currSeq.getAligned().substr(0, end);
								currSeq.setAligned(seqString);

							}
						}
					}
					if (start != -1) {
						if (keepdots) { currSeq.filterToPos(start); }
						else {
							string seqString = currSeq.getAligned().substr(start);
							currSeq.setAligned(seqString);

						}
					}
				}
			}

			if (commentString != "") {
				string seqComment = currSeq.getComment();
				currSeq.setComment("\t" + commentString + "\t" + seqComment);
			}

			if (totalDiffs > pdiffs) { trashCode += "t"; goodSeq = false; }

			//trimming removed all bases
		   // if (currSeq.getUnaligned() == "") { goodSeq = false; }

			if (goodSeq == 1) {
				currSeq.printSequence(goodFile);
				if (app.isDebug) { LOG(DEBUG) << "" + locationsString + "\n"; }
				if (thisPStart != -1) { locations.insert(thisPStart); }
				if (locationsString != "") { locationsFile << locationsString; }
			}
			else {
				badSeqNames.insert(currSeq.getName());
				currSeq.setName(currSeq.getName() + '|' + trashCode);
				currSeq.printSequence(badFile);
			}
			count++;
		}

#if defined (UNIX)
		unsigned long long pos = inFASTA.tellg();
		if ((pos == -1) || (pos >= filePos.end)) { break; }
#else
		if (inFASTA.eof()) { break; }
#endif

		//report progress
		if ((count) % 100 == 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(count) + "\n"; }
	}
	//report progress
	if ((count) % 100 != 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(count) + "\n"; }

	badFile.close();
	goodFile.close();
	inFASTA.close();
	locationsFile.close();

	if (app.isDebug) { LOG(DEBUG) << "fileAligned = " + toString(fileAligned) + '\n'; }

	if (fileAligned && !keepdots) { //print out smallest start value and largest end value
		if (locations.size() > 1) { adjustNeeded = true; }
		if (primers.size() != 0) { set<int>::iterator it = locations.begin();  pstart = *it; }
	}

	return count;
}
//********************************************************************/
bool PcrSeqsCommand::isAligned(string seq, map<int, int>& aligned) {
	aligned.clear();
	bool isAligned = false;

	int countBases = 0;
	for (int i = 0; i < seq.length(); i++) {
		if (!isalpha(seq[i])) { isAligned = true; }
		else { aligned[countBases] = i; countBases++; } //maps location in unaligned -> location in aligned.
	}                                                   //ie. the 3rd base may be at spot 10 in the alignment
														//later when we trim we want to trim from spot 10.
	return isAligned;
}
//**********************************************************************************************************************
int PcrSeqsCommand::adjustDots(string goodFasta, string locations, int pstart, int pend) {
	ifstream inFasta;
	File::openInputFile(goodFasta, inFasta);

	ifstream inLocations;
	File::openInputFile(locations, inLocations);

	ofstream out;
	File::openOutputFile(goodFasta + ".temp", out);

	set<int> lengths;
	//cout << pstart << '\t' << pend << endl;
	//if (pstart > pend) { //swap them

	while (!inFasta.eof()) {
		if (ctrlc_pressed) { break; }

		Sequence seq(inFasta); File::gobble(inFasta);

		string name = "";
		int thisStart = -1; int thisEnd = -1;
		if (numFPrimers != 0) { inLocations >> name >> thisStart; File::gobble(inLocations); }
		if (numRPrimers != 0) { inLocations >> name >> thisEnd;   File::gobble(inLocations); }


		//cout << seq.getName() << '\t' << thisStart << '\t' << thisEnd << '\t' << seq.getAligned().length() << endl;
		//cout << seq.getName() << '\t' << pstart << '\t' << pend << endl;

		if (name != seq.getName()) { LOG(LOGERROR) << "name mismatch in pcr.seqs.\n"; }
		else {
			if (pstart != -1) {
				if (thisStart != -1) {
					if (thisStart != pstart) {
						string dots = "";
						for (int i = pstart; i < thisStart; i++) { dots += "."; }
						thisEnd += dots.length();
						dots += seq.getAligned();
						seq.setAligned(dots);
					}
				}
			}

			if (pend != -1) {
				if (thisEnd != -1) {
					if (thisEnd != pend) {
						string dots = seq.getAligned();
						for (int i = thisEnd; i < pend; i++) { dots += "."; }
						seq.setAligned(dots);
					}
				}
			}
			lengths.insert(seq.getAligned().length());
		}

		seq.printSequence(out);
	}
	inFasta.close();
	inLocations.close();
	out.close();
	File::remove(locations);
	File::remove(goodFasta);
	m->renameFile(goodFasta + ".temp", goodFasta);

	//cout << "final lengths = \n";
	//for (set<int>::iterator it = lengths.begin(); it != lengths.end(); it++) {
	   //cout << *it << endl;
	   // cout << lengths.count(*it) << endl;
   // }

	return 0;
}
//***************************************************************************************************************
bool PcrSeqsCommand::readEcoli() {
	ifstream in;
	File::openInputFile(ecolifile, in);

	//read seq
	if (!in.eof()) {
		Sequence ecoli(in);
		length = ecoli.getAligned().length();
		start = ecoli.getStartPos();
		end = ecoli.getEndPos();
	}
	else { in.close(); ctrlc_pressed = true; return false; }
	in.close();

	return true;

}
//***************************************************************************************************************
int PcrSeqsCommand::writeAccnos(set<string> badNames) {
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(fastafile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(fastafile));
	string outputFileName = getOutputFileName("accnos", variables);
	outputNames.push_back(outputFileName); outputTypes["accnos"].push_back(outputFileName);

	ofstream out;
	File::openOutputFile(outputFileName, out);

	for (set<string>::iterator it = badNames.begin(); it != badNames.end(); it++) {
		if (ctrlc_pressed) { break; }
		out << (*it) << endl;
	}

	out.close();
	return 0;

}
//***************************************************************************************************************
int PcrSeqsCommand::readName(set<string>& names) {
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(namefile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(namefile));
	variables["[extension]"] = m->getExtension(namefile);
	string outputFileName = getOutputFileName("name", variables);

	ofstream out;
	File::openOutputFile(outputFileName, out);

	ifstream in;
	File::openInputFile(namefile, in);
	string name, firstCol, secondCol;

	bool wroteSomething = false;
	int removedCount = 0;

	while (!in.eof()) {
		if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

		in >> firstCol;		File::gobble(in);
		in >> secondCol;

		string savedSecond = secondCol;
		vector<string> parsedNames;
		m->splitAtComma(secondCol, parsedNames);

		vector<string> validSecond;  validSecond.clear();
		for (int i = 0; i < parsedNames.size(); i++) {
			if (names.count(parsedNames[i]) == 0) {
				validSecond.push_back(parsedNames[i]);
			}
		}

		if (validSecond.size() != parsedNames.size()) {  //we want to get rid of someone, so get rid of everyone
			for (int i = 0; i < parsedNames.size(); i++) { names.insert(parsedNames[i]); }
			removedCount += parsedNames.size();
		}
		else {
			out << firstCol << '\t' << savedSecond << endl;
			wroteSomething = true;
		}
		File::gobble(in);
	}
	in.close();
	out.close();

	if (wroteSomething == false) { LOG(INFO) << "Your file contains only sequences from the .accnos file." << '\n'; }
	outputTypes["name"].push_back(outputFileName); outputNames.push_back(outputFileName);

	LOG(INFO) << "Removed " + toString(removedCount) + " sequences from your name file." << '\n';

	return 0;
}
//**********************************************************************************************************************
int PcrSeqsCommand::readGroup(set<string> names) {
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(groupfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(groupfile));
	variables["[extension]"] = m->getExtension(groupfile);
	string outputFileName = getOutputFileName("group", variables);

	ofstream out;
	File::openOutputFile(outputFileName, out);

	ifstream in;
	File::openInputFile(groupfile, in);
	string name, group;

	bool wroteSomething = false;
	int removedCount = 0;

	while (!in.eof()) {
		if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

		in >> name;				//read from first column
		in >> group;			//read from second column

		//if this name is in the accnos file
		if (names.count(name) == 0) {
			wroteSomething = true;
			out << name << '\t' << group << endl;
		}
		else { removedCount++; }

		File::gobble(in);
	}
	in.close();
	out.close();

	if (wroteSomething == false) { LOG(INFO) << "Your file contains only sequences from the .accnos file." << '\n'; }
	outputTypes["group"].push_back(outputFileName); outputNames.push_back(outputFileName);

	LOG(INFO) << "Removed " + toString(removedCount) + " sequences from your group file." << '\n';


	return 0;
}
//**********************************************************************************************************************
int PcrSeqsCommand::readTax(set<string> names) {
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(taxfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(taxfile));
	variables["[extension]"] = m->getExtension(taxfile);
	string outputFileName = getOutputFileName("taxonomy", variables);

	ofstream out;
	File::openOutputFile(outputFileName, out);

	ifstream in;
	File::openInputFile(taxfile, in);
	string name, tax;

	bool wroteSomething = false;
	int removedCount = 0;

	while (!in.eof()) {
		if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

		in >> name;				//read from first column
		in >> tax;			//read from second column

		//if this name is in the accnos file
		if (names.count(name) == 0) {
			wroteSomething = true;
			out << name << '\t' << tax << endl;
		}
		else { removedCount++; }

		File::gobble(in);
	}
	in.close();
	out.close();

	if (wroteSomething == false) { LOG(INFO) << "Your file contains only sequences from the .accnos file." << '\n'; }
	outputTypes["taxonomy"].push_back(outputFileName); outputNames.push_back(outputFileName);

	LOG(INFO) << "Removed " + toString(removedCount) + " sequences from your taxonomy file." << '\n';

	return 0;
}
//***************************************************************************************************************
int PcrSeqsCommand::readCount(set<string> badSeqNames) {
	ifstream in;
	File::openInputFile(countfile, in);
	set<string>::iterator it;

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(countfile));
	variables["[extension]"] = m->getExtension(countfile);
	string goodCountFile = getOutputFileName("count", variables);

	outputNames.push_back(goodCountFile);  outputTypes["count"].push_back(goodCountFile);
	ofstream goodCountOut;	File::openOutputFile(goodCountFile, goodCountOut);

	string headers = File::getline(in); File::gobble(in);
	goodCountOut << headers << endl;
	string test = headers; vector<string> pieces = m->splitWhiteSpace(test);

	string name, rest; int thisTotal, removedCount; removedCount = 0; rest = "";
	bool wroteSomething = false;
	while (!in.eof()) {

		if (ctrlc_pressed) { goodCountOut.close(); in.close(); File::remove(goodCountFile); return 0; }

		in >> name; File::gobble(in);
		in >> thisTotal; File::gobble(in);
		if (pieces.size() > 2) { rest = File::getline(in); File::gobble(in); }

		if (badSeqNames.count(name) != 0) { removedCount += thisTotal; }
		else {
			wroteSomething = true;
			goodCountOut << name << '\t' << thisTotal << '\t' << rest << endl;
		}
	}
	in.close();
	goodCountOut.close();

	if (ctrlc_pressed) { File::remove(goodCountFile); }

	if (wroteSomething == false) { LOG(INFO) << "Your count file contains only sequences from the .accnos file." << '\n'; }

	//check for groups that have been eliminated
	CountTable ct;
	if (ct.testGroups(goodCountFile)) {
		ct.readTable(goodCountFile, true, false);
		ct.printTable(goodCountFile);
	}

	if (ctrlc_pressed) { File::remove(goodCountFile); }

	LOG(INFO) << "Removed " + toString(removedCount) + " sequences from your count file." << '\n';


	return 0;

}
//***************************************************************************************************************

int PcrSeqsCommand::readOligos() {
	oligos.read(oligosfile);

	if (ctrlc_pressed) { return false; } //error in reading oligos

	if (oligos.hasPairedPrimers()) {
		pairedOligos = true;
		numFPrimers = oligos.getPairedPrimers().size();
	}
	else {
		pairedOligos = false;
		numFPrimers = oligos.getPrimers().size();
	}
	numRPrimers = oligos.getReversePrimers().size();

	if (oligos.getLinkers().size() != 0) { LOG(WARNING) << "pcr.seqs is not setup to remove linkers, ignoring.\n"; }
	if (oligos.getSpacers().size() != 0) { LOG(WARNING) << "pcr.seqs is not setup to remove spacers, ignoring.\n"; }

	return true;

}

/**************************************************************************************/


