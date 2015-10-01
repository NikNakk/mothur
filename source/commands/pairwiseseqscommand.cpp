/*
 *  pairwiseseqscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 10/20/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "pairwiseseqscommand.h"

 //**********************************************************************************************************************
vector<string> PairwiseSeqsCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "phylip-column", false, true, true); parameters.push_back(pfasta);
		CommandParameter palign("align", "Multiple", "needleman-gotoh-blast-noalign", "needleman", "", "", "", "", false, false); parameters.push_back(palign);
		nkParameters.add(new NumberParameter("match", -INFINITY, INFINITY, 1.0, false, false));
		nkParameters.add(new NumberParameter("mismatch", -INFINITY, INFINITY, -1.0, false, false));
		nkParameters.add(new NumberParameter("gapopen", -INFINITY, INFINITY, -2.0, false, false));
		nkParameters.add(new NumberParameter("gapextend", -INFINITY, INFINITY, -1.0, false, false));
		nkParameters.add(new ProcessorsParameter(settings));
		CommandParameter poutput("output", "Multiple", "column-lt-square-phylip", "column", "", "", "", "phylip-column", false, false, true); parameters.push_back(poutput);
		CommandParameter pcalc("calc", "Multiple", "nogaps-eachgap-onegap", "onegap", "", "", "", "", false, false); parameters.push_back(pcalc);
		nkParameters.add(new BooleanParameter("countends", true, false, false));
		nkParameters.add(new BooleanParameter("compress", false, false, false));
		CommandParameter pcutoff("cutoff", "Number", "", "1.0", "", "", "", "", false, false, true); parameters.push_back(pcutoff);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PairwiseSeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string PairwiseSeqsCommand::getHelpString() {
	try {
		string helpString = "The pairwise.seqs command reads a fasta file and creates distance matrix.\n"
			"The pairwise.seqs command parameters are fasta, align, match, mismatch, gapopen, gapextend, calc, output, cutoff and processors.\n"
			"The fasta parameter is required. You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amzon.fasta \n"
			"The align parameter allows you to specify the alignment method to use.  Your options are: gotoh, needleman, blast and noalign. The default is needleman.\n"
			"The match parameter allows you to specify the bonus for having the same base. The default is 1.0.\n"
			"The mistmatch parameter allows you to specify the penalty for having different bases.  The default is -1.0.\n"
			"The gapopen parameter allows you to specify the penalty for opening a gap in an alignment. The default is -2.0.\n"
			"The gapextend parameter allows you to specify the penalty for extending a gap in an alignment.  The default is -1.0.\n"
			"The calc parameter allows you to specify the method of calculating the distances.  Your options are: nogaps, onegap or eachgap. The default is onegap.\n"
			"The countends parameter allows you to specify whether to include terminal gaps in distance.  Your options are: T or F. The default is T.\n"
			"The cutoff parameter allows you to specify maximum distance to keep. The default is 1.0.\n"
			"The output parameter allows you to specify format of your distance matrix. Options are column, lt, and square. The default is column.\n"
			"The compress parameter allows you to indicate that you want the resulting distance file compressed.  The default is false.\n"
			"The pairwise.seqs command should be in the following format: \n"
			"pairwise.seqs(fasta=yourfastaFile, align=yourAlignmentMethod) \n"
			"Example pairwise.seqs(fasta=candidate.fasta, align=blast)\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PairwiseSeqsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string PairwiseSeqsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "phylip") { pattern = "[filename],[outputtag],dist"; }
	else if (type == "column") { pattern = "[filename],dist"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
PairwiseSeqsCommand::PairwiseSeqsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["phylip"] = tempOutNames;
		outputTypes["column"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PairwiseSeqsCommand, PairwiseSeqsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
PairwiseSeqsCommand::PairwiseSeqsCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter("pairwise.seqs");
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["phylip"] = tempOutNames;
		outputTypes["column"] = tempOutNames;

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }


		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);

		if (inputDir == "not found") { inputDir = ""; }

		fastaFileName = validParameter.validFile(parameters, "fasta", false);
		if (fastaFileName == "not found") {
			//if there is a current fasta file, use it
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

					//if you can't open it, try output location
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

		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		string temp;
		temp = validParameter.validFile(parameters, "match", false);		if (temp == "not found") { temp = "1.0"; }
		Utility::mothurConvert(temp, match);

		temp = validParameter.validFile(parameters, "mismatch", false);		if (temp == "not found") { temp = "-1.0"; }
		Utility::mothurConvert(temp, misMatch);
		if (misMatch > 0) { LOG(LOGERROR) << "mismatch must be negative.\n"; abort = true; }

		temp = validParameter.validFile(parameters, "gapopen", false);		if (temp == "not found") { temp = "-2.0"; }
		Utility::mothurConvert(temp, gapOpen);
		if (gapOpen > 0) { LOG(LOGERROR) << "gapopen must be negative.\n"; abort = true; }

		temp = validParameter.validFile(parameters, "gapextend", false);	if (temp == "not found") { temp = "-1.0"; }
		Utility::mothurConvert(temp, gapExtend);
		if (gapExtend > 0) { LOG(LOGERROR) << "gapextend must be negative.\n"; abort = true; }

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "cutoff", false);		if (temp == "not found") { temp = "1.0"; }
		Utility::mothurConvert(temp, cutoff);

		temp = validParameter.validFile(parameters, "countends", false);	if (temp == "not found") { temp = "T"; }
		countends = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "compress", false);		if (temp == "not found") { temp = "F"; }
		compress = m->isTrue(temp);

		align = validParameter.validFile(parameters, "align", false);		if (align == "not found") { align = "needleman"; }

		output = validParameter.validFile(parameters, "output", false);		if (output == "not found") { output = "column"; }
		if (output == "phylip") { output = "lt"; }
		if ((output != "column") && (output != "lt") && (output != "square")) { LOG(INFO) << output + " is not a valid output form. Options are column, lt and square. I will use column." << '\n'; output = "column"; }

		calc = validParameter.validFile(parameters, "calc", false);
		if (calc == "not found") { calc = "onegap"; }
		else {
			if (calc == "default") { calc = "onegap"; }
		}
		Utility::split(calc, '-', Estimators);
	}

}
//**********************************************************************************************************************

int PairwiseSeqsCommand::execute() {
	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	longestBase = 2000; //will need to update this in driver if we find sequences with more bases.  hardcoded so we don't have the pre-read user fasta file.

	cutoff += 0.005;

	for (int s = 0; s < fastaFileNames.size(); s++) {
		if (ctrlc_pressed) { outputTypes.clear(); return 0; }

		LOG(INFO) << "Processing sequences from " + fastaFileNames[s] + " ..." << '\n';

		if (outputDir == "") { outputDir += File::getPath(fastaFileNames[s]); }

		ifstream inFASTA;
		File::openInputFile(fastaFileNames[s], inFASTA);
		alignDB = SequenceDB(inFASTA);
		inFASTA.close();

		int numSeqs = alignDB.getNumSeqs();
		int startTime = time(NULL);
		string outputFile = "";

		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[s]));
		if (output == "lt") { //does the user want lower triangle phylip formatted file 
			variables["[outputtag]"] = "phylip";
			outputFile = getOutputFileName("phylip", variables);
			File::remove(outputFile); outputTypes["phylip"].push_back(outputFile);
		}
		else if (output == "column") { //user wants column format
			outputFile = getOutputFileName("column", variables);
			outputTypes["column"].push_back(outputFile);
			File::remove(outputFile);
		}
		else { //assume square
			variables["[outputtag]"] = "square";
			outputFile = getOutputFileName("phylip", variables);
			File::remove(outputFile);
			outputTypes["phylip"].push_back(outputFile);
		}


		//if you don't need to fork anything
		if (processors == 1) {
			if (output != "square") { driver(0, numSeqs, outputFile, cutoff); }
			else { driver(0, numSeqs, outputFile, "square"); }
		}
		else { //you have multiple processors

			for (int i = 0; i < processors; i++) {
				distlinePair tempLine;
				lines.push_back(tempLine);
				if (output != "square") {
					lines[i].start = int(sqrt(float(i) / float(processors)) * numSeqs);
					lines[i].end = int(sqrt(float(i + 1) / float(processors)) * numSeqs);
				}
				else {
					lines[i].start = int((float(i) / float(processors)) * numSeqs);
					lines[i].end = int((float(i + 1) / float(processors)) * numSeqs);
				}
			}

			createProcesses(outputFile);
		}

		if (ctrlc_pressed) { outputTypes.clear();   File::remove(outputFile); return 0; }

		ifstream fileHandle;
		fileHandle.open(outputFile.c_str());
		if (fileHandle) {
			File::gobble(fileHandle);
			if (fileHandle.eof()) { LOG(INFO) << outputFile + " is blank. This can result if there are no distances below your cutoff." << '\n'; }
		}

		if (compress) {
			LOG(INFO) << "Compressing..." << '\n';
			LOG(INFO) << "(Replacing " + outputFile + " with " + outputFile + ".gz)" << '\n';
			system(("gzip -v " + outputFile).c_str());
			outputNames.push_back(outputFile + ".gz");
		}
		else { outputNames.push_back(outputFile); }

		LOG(INFO) << "It took " + toString(time(NULL) - startTime) + " to calculate the distances for " + toString(numSeqs) + " sequences." << '\n';

		if (ctrlc_pressed) { outputTypes.clear(); File::remove(outputFile); return 0; }
	}

	//set phylip file as new current phylipfile
	string current = "";
	itTypes = outputTypes.find("phylip");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("phylip", current); }
	}

	//set column file as new current columnfile
	itTypes = outputTypes.find("column");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("column", current); }
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";


	return 0;
}

/**************************************************************************************************/
void PairwiseSeqsCommand::createProcesses(string filename) {
	int process = 1;
	processIDS.clear();
	bool recalc = false;

#if defined (UNIX)


	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);
			process++;
		}
		else if (pid == 0) {
			if (output != "square") { driver(lines[process].start, lines[process].end, filename + m->mothurGetpid(process) + ".temp", cutoff); }
			else { driver(lines[process].start, lines[process].end, filename + m->mothurGetpid(process) + ".temp", "square"); }
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

		//redo file divide
		int numSeqs = alignDB.getNumSeqs();
		lines.clear();
		for (int i = 0; i < processors; i++) {
			distlinePair tempLine;
			lines.push_back(tempLine);
			if (output != "square") {
				lines[i].start = int(sqrt(float(i) / float(processors)) * numSeqs);
				lines[i].end = int(sqrt(float(i + 1) / float(processors)) * numSeqs);
			}
			else {
				lines[i].start = int((float(i) / float(processors)) * numSeqs);
				lines[i].end = int((float(i + 1) / float(processors)) * numSeqs);
			}
		}

		processIDS.resize(0);
		process = 1;

		//loop through and create all the processes you want
		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);
				process++;
			}
			else if (pid == 0) {
				if (output != "square") { driver(lines[process].start, lines[process].end, filename + m->mothurGetpid(process) + ".temp", cutoff); }
				else { driver(lines[process].start, lines[process].end, filename + m->mothurGetpid(process) + ".temp", "square"); }
				exit(0);
			}
			else {
				LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
				for (int i = 0;i < processIDS.size();i++) { int temp = processIDS[i]; kill(temp, SIGINT); }
				exit(0);
			}
		}
	}


	//parent do my part
	if (output != "square") { driver(lines[0].start, lines[0].end, filename, cutoff); }
	else { driver(lines[0].start, lines[0].end, filename, "square"); }


	//force parent to wait until all the processes are done
	for (int i = 0;i < processIDS.size();i++) {
		int temp = processIDS[i];
		wait(&temp);
	}
#else     
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Windows version shared memory, so be careful when passing variables through the distanceData struct. 
	//Above fork() will clone, so memory is separate, but that's not the case with windows, 
	//that's why the distance calculator was moved inside of the driver to make separate copies.
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<pairwiseData*> pDataArray; //[processors-1];
	vector<DWORD> dwThreadIdArray(processors - 1);
	vector<HANDLE> hThreadArray(processors - 1);

	//Create processor-1 worker threads.
	for (int i = 0; i < processors - 1; i++) {
		string extension = toString(i) + ".temp";

		// Allocate memory for thread data.
		pairwiseData* tempDist = new pairwiseData((filename + extension), align, "square", Estimators[0], countends, output, alignDB, m, lines[i + 1].start, lines[i + 1].end, match, misMatch, gapOpen, gapExtend, longestBase, cutoff, i);
		pDataArray.push_back(tempDist);
		processIDS.push_back(i);

		if (output != "square") { hThreadArray[i] = CreateThread(NULL, 0, MyPairwiseThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]); }
		else { hThreadArray[i] = CreateThread(NULL, 0, MyPairwiseSquareThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]); }
	}

	//do your part
	if (output != "square") { driver(lines[0].start, lines[0].end, filename, cutoff); }
	else { driver(lines[0].start, lines[0].end, filename, "square"); }

	//Wait until all threads have terminated.
	WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

	//Close all thread handles and free memory allocations.
	for (int i = 0; i < pDataArray.size(); i++) {
		if (pDataArray[i]->count != (pDataArray[i]->end - pDataArray[i]->start)) {
			LOG(LOGERROR) << "process " + toString(i) + " only processed " + toString(pDataArray[i]->count) + " of " + toString(pDataArray[i]->end - pDataArray[i]->start) + " sequences assigned to it, quitting. \n"; ctrlc_pressed = true;
		}
		CloseHandle(hThreadArray[i]);
		delete pDataArray[i];
	}

#endif

	//append and remove temp files
	for (int i = 0;i < processIDS.size();i++) {
		File::appendFiles((filename + toString(processIDS[i]) + ".temp"), filename);
		File::remove((filename + toString(processIDS[i]) + ".temp"));
	}

}

/**************************************************************************************************/
/////// need to fix to work with calcs and sequencedb
int PairwiseSeqsCommand::driver(int startLine, int endLine, string dFileName, float cutoff) {

	int startTime = time(NULL);

	Alignment* alignment;
	if (align == "gotoh") { alignment = new GotohOverlap(gapOpen, gapExtend, match, misMatch, longestBase); }
	else if (align == "needleman") { alignment = new NeedlemanOverlap(gapOpen, match, misMatch, longestBase); }
	else if (align == "blast") { alignment = new BlastAlignment(gapOpen, gapExtend, match, misMatch); }
	else if (align == "noalign") { alignment = new NoAlign(); }
	else {
		LOG(INFO) << align + " is not a valid alignment option. I will run the command using needleman.";
		LOG(INFO) << "";
		alignment = new NeedlemanOverlap(gapOpen, match, misMatch, longestBase);
	}

	ValidCalculators validCalculator;
	Dist* distCalculator;
	if (countends) {
		if (validCalculator.isValidCalculator("distance", Estimators[0]) == true) {
			if (Estimators[0] == "nogaps") { distCalculator = new ignoreGaps(); }
			else if (Estimators[0] == "eachgap") { distCalculator = new eachGapDist(); }
			else if (Estimators[0] == "onegap") { distCalculator = new oneGapDist(); }
		}
	}
	else {
		if (validCalculator.isValidCalculator("distance", Estimators[0]) == true) {
			if (Estimators[0] == "nogaps") { distCalculator = new ignoreGaps(); }
			else if (Estimators[0] == "eachgap") { distCalculator = new eachGapIgnoreTermGapDist(); }
			else if (Estimators[0] == "onegap") { distCalculator = new oneGapIgnoreTermGapDist(); }
		}
	}

	//column file
	ofstream outFile(dFileName.c_str(), ios::trunc);
	outFile.setf(ios::fixed, ios::showpoint);
	outFile << setprecision(4);

	if ((output == "lt") && startLine == 0) { outFile << alignDB.getNumSeqs() << endl; }

	for (int i = startLine;i < endLine;i++) {
		if (output == "lt") {
			string name = alignDB.get(i).getName();
			if (name.length() < 10) { //pad with spaces to make compatible
				while (name.length() < 10) { name += " "; }
			}
			outFile << name;
		}

		for (int j = 0;j<i;j++) {

			if (ctrlc_pressed) { outFile.close(); delete alignment; delete distCalculator; return 0; }

			if (alignDB.get(i).getUnaligned().length() > alignment->getnRows()) {
				alignment->resize(alignDB.get(i).getUnaligned().length() + 1);
			}

			if (alignDB.get(j).getUnaligned().length() > alignment->getnRows()) {
				alignment->resize(alignDB.get(j).getUnaligned().length() + 1);
			}

			Sequence seqI(alignDB.get(i).getName(), alignDB.get(i).getAligned());
			Sequence seqJ(alignDB.get(j).getName(), alignDB.get(j).getAligned());

			alignment->align(seqI.getUnaligned(), seqJ.getUnaligned());
			seqI.setAligned(alignment->getSeqAAln());
			seqJ.setAligned(alignment->getSeqBAln());

			distCalculator->calcDist(seqI, seqJ);
			double dist = distCalculator->getDist();

			if (app.isDebug) { LOG(DEBUG) << "" + seqI.getName() + '\t' + alignment->getSeqAAln() + '\n' + seqJ.getName() + alignment->getSeqBAln() + '\n' + "distance = " + toString(dist) + "\n"; }

			if (dist <= cutoff) {
				if (output == "column") { outFile << alignDB.get(i).getName() << ' ' << alignDB.get(j).getName() << ' ' << dist << endl; }
			}
			if (output == "lt") { outFile << '\t' << dist; }
		}

		if (output == "lt") { outFile << endl; }

		if (i % 100 == 0) {
			LOG(SCREENONLY) << toString(i) + "\t" + toString(time(NULL) - startTime) + "\n";
		}

	}
	LOG(SCREENONLY) << toString(endLine - 1) + "\t" + toString(time(NULL) - startTime) + "\n";

	outFile.close();
	delete alignment;
	delete distCalculator;

	return 1;
}
/**************************************************************************************************/
/////// need to fix to work with calcs and sequencedb
int PairwiseSeqsCommand::driver(int startLine, int endLine, string dFileName, string square) {

	int startTime = time(NULL);

	Alignment* alignment;
	if (align == "gotoh") { alignment = new GotohOverlap(gapOpen, gapExtend, match, misMatch, longestBase); }
	else if (align == "needleman") { alignment = new NeedlemanOverlap(gapOpen, match, misMatch, longestBase); }
	else if (align == "blast") { alignment = new BlastAlignment(gapOpen, gapExtend, match, misMatch); }
	else if (align == "noalign") { alignment = new NoAlign(); }
	else {
		LOG(INFO) << align + " is not a valid alignment option. I will run the command using needleman.";
		LOG(INFO) << "";
		alignment = new NeedlemanOverlap(gapOpen, match, misMatch, longestBase);
	}

	ValidCalculators validCalculator;
	Dist* distCalculator;
	if (countends) {
		if (validCalculator.isValidCalculator("distance", Estimators[0]) == true) {
			if (Estimators[0] == "nogaps") { distCalculator = new ignoreGaps(); }
			else if (Estimators[0] == "eachgap") { distCalculator = new eachGapDist(); }
			else if (Estimators[0] == "onegap") { distCalculator = new oneGapDist(); }
		}
	}
	else {
		if (validCalculator.isValidCalculator("distance", Estimators[0]) == true) {
			if (Estimators[0] == "nogaps") { distCalculator = new ignoreGaps(); }
			else if (Estimators[0] == "eachgap") { distCalculator = new eachGapIgnoreTermGapDist(); }
			else if (Estimators[0] == "onegap") { distCalculator = new oneGapIgnoreTermGapDist(); }
		}
	}

	//column file
	ofstream outFile(dFileName.c_str(), ios::trunc);
	outFile.setf(ios::fixed, ios::showpoint);
	outFile << setprecision(4);

	if (startLine == 0) { outFile << alignDB.getNumSeqs() << endl; }

	for (int i = startLine;i < endLine;i++) {

		string name = alignDB.get(i).getName();
		//pad with spaces to make compatible
		if (name.length() < 10) { while (name.length() < 10) { name += " "; } }

		outFile << name;

		for (int j = 0;j<alignDB.getNumSeqs();j++) {

			if (ctrlc_pressed) { outFile.close(); delete alignment; delete distCalculator; return 0; }

			if (alignDB.get(i).getUnaligned().length() > alignment->getnRows()) {
				alignment->resize(alignDB.get(i).getUnaligned().length() + 1);
			}

			if (alignDB.get(j).getUnaligned().length() > alignment->getnRows()) {
				alignment->resize(alignDB.get(j).getUnaligned().length() + 1);
			}

			Sequence seqI(alignDB.get(i).getName(), alignDB.get(i).getAligned());
			Sequence seqJ(alignDB.get(j).getName(), alignDB.get(j).getAligned());

			alignment->align(seqI.getUnaligned(), seqJ.getUnaligned());
			seqI.setAligned(alignment->getSeqAAln());
			seqJ.setAligned(alignment->getSeqBAln());

			distCalculator->calcDist(seqI, seqJ);
			double dist = distCalculator->getDist();

			outFile << '\t' << dist;

			if (app.isDebug) { LOG(DEBUG) << "" + seqI.getName() + '\t' + alignment->getSeqAAln() + '\n' + seqJ.getName() + alignment->getSeqBAln() + '\n' + "distance = " + toString(dist) + "\n"; }
		}

		outFile << endl;

		if (i % 100 == 0) {
			LOG(SCREENONLY) << toString(i) + "\t" + toString(time(NULL) - startTime) + "\n";
		}

	}
	LOG(SCREENONLY) << toString(endLine - 1) + "\t" + toString(time(NULL) - startTime) + "\n";

	outFile.close();
	delete alignment;
	delete distCalculator;

	return 1;
}
/**************************************************************************************************/

