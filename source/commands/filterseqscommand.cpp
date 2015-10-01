/*
 *  filterseqscommand.cpp
 *  Mothur
 *
 *  Created by Thomas Ryabin on 5/4/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "filterseqscommand.h"
#include "sequence.hpp"
#include <thread>

 //**********************************************************************************************************************
vector<string> FilterSeqsCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "fasta-filter", false, true, true); parameters.push_back(pfasta);
		CommandParameter phard("hard", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(phard);
		CommandParameter ptrump("trump", "String", "", "*", "", "", "", "", false, false, true); parameters.push_back(ptrump);
		nkParameters.add(new NumberParameter("soft", -INFINITY, INFINITY, 0, false, false));
		CommandParameter pvertical("vertical", "Boolean", "", "T", "", "", "", "", false, false, true); parameters.push_back(pvertical);
		CommandParameter pprocessors("processors", "Number", "", "1", "", "", "", "", false, false, true); parameters.push_back(pprocessors);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in FilterSeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string FilterSeqsCommand::getHelpString() {
	try {
		string helpString = "The filter.seqs command reads a file containing sequences and creates a .filter and .filter.fasta file.\n"
			"The filter.seqs command parameters are fasta, trump, soft, hard, processors and vertical. \n"
			"The fasta parameter is required, unless you have a valid current fasta file. You may enter several fasta files to build the filter from and filter, by separating their names with -'s.\n"
			"For example: fasta=abrecovery.fasta-amazon.fasta \n"
			"The trump option will remove a column if the trump character is found at that position in any sequence of the alignment. Default=*, meaning no trump. \n"
			"A soft mask removes any column where the dominant base (i.e. A, T, G, C, or U) does not occur in at least a designated percentage of sequences. Default=0.\n"
			"The hard parameter allows you to enter a file containing the filter you want to use.\n"
			"The vertical parameter removes columns where all sequences contain a gap character. The default is T.\n"
			"The processors parameter allows you to specify the number of processors to use. The default is 1.\n"
			"The filter.seqs command should be in the following format: \n"
			"filter.seqs(fasta=yourFastaFile, trump=yourTrump) \n"
			"Example filter.seqs(fasta=abrecovery.fasta, trump=.).\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in FilterSeqsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string FilterSeqsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],filter.fasta"; }
	else if (type == "filter") { pattern = "[filename],filter"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
FilterSeqsCommand::FilterSeqsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["filter"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in FilterSeqsCommand, FilterSeqsCommand";
		exit(1);
	}
}
/**************************************************************************************/
FilterSeqsCommand::FilterSeqsCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;  recalced = false;
	filterFileName = "";

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter("filter.seqs");
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["filter"] = tempOutNames;

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

			it = parameters.find("hard");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["hard"] = inputDir + it->second; }
			}
		}

		//check for required parameters
		fasta = validParameter.validFile(parameters, "fasta", false);
		if (fasta == "not found") {
			fasta = settings.getCurrent("fasta");
			if (fasta != "") {
				fastafileNames.push_back(fasta);
				LOG(INFO) << "Using " + fasta + " as input file for the fasta parameter." << '\n';
				string simpleName = File::getSimpleName(fasta);
				filterFileName += simpleName.substr(0, simpleName.find_first_of('.'));
			}
			else { LOG(INFO) << "You have no current fastafile and the fasta parameter is required." << '\n'; abort = true; }
		}
		else {
			Utility::split(fasta, '-', fastafileNames);

			//go through files and make sure they are good, if not, then disregard them
			for (int i = 0; i < fastafileNames.size(); i++) {

				bool ignore = false;
				if (fastafileNames[i] == "current") {
					fastafileNames[i] = settings.getCurrent("fasta");
					if (fastafileNames[i] != "") { LOG(INFO) << "Using " + fastafileNames[i] + " as input file for the fasta parameter where you had given current." << '\n'; }
					else {
						LOG(INFO) << "You have no current fastafile, ignoring current." << '\n'; ignore = true;
						//erase from file list
						fastafileNames.erase(fastafileNames.begin() + i);
						i--;
					}
				}

				if (!ignore) {
					if (inputDir != "") {
						string path = File::getPath(fastafileNames[i]);
						//if the user has not given a path then, add inputdir. else leave path alone.
						if (path == "") { fastafileNames[i] = inputDir + fastafileNames[i]; }
					}

					ifstream in;
					int ableToOpen = File::openInputFile(fastafileNames[i], in, "noerror");

					//if you can't open it, try default location
					if (ableToOpen == 1) {
						if (settings.getDefaultPath() != "") { //default path is set
							string tryPath = settings.getDefaultPath() + File::getSimpleName(fastafileNames[i]);
							LOG(INFO) << "Unable to open " + fastafileNames[i] + ". Trying default " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							fastafileNames[i] = tryPath;
						}
					}

					//if you can't open it, try default location
					if (ableToOpen == 1) {
						if (settings.getOutputDir() != "") { //default path is set
							string tryPath = settings.getOutputDir() + File::getSimpleName(fastafileNames[i]);
							LOG(INFO) << "Unable to open " + fastafileNames[i] + ". Trying output directory " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							fastafileNames[i] = tryPath;
						}
					}

					in.close();

					if (ableToOpen == 1) {
						LOG(INFO) << "Unable to open " + fastafileNames[i] + ". It will be disregarded." << '\n';
						//erase from file list
						fastafileNames.erase(fastafileNames.begin() + i);
						i--;
					}
					else {
						string simpleName = File::getSimpleName(fastafileNames[i]);
						filterFileName += simpleName.substr(0, simpleName.find_first_of('.'));
						settings.setCurrent("fasta", fastafileNames[i]);
					}
					in.close();
				}
			}

			//make sure there is at least one valid file left
			if (fastafileNames.size() == 0) { LOG(INFO) << "no valid files." << '\n'; abort = true; }
		}

		if (!abort) {
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
				outputDir = "";
				outputDir += File::getPath(fastafileNames[0]); //if user entered a file with a path then preserve it	
			}
		}
		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...

		string temp;
		hard = validParameter.validFile(parameters, "hard", true);				if (hard == "not found") { hard = ""; }
		else if (hard == "not open") { hard = ""; abort = true; }

		temp = validParameter.validFile(parameters, "trump", false);			if (temp == "not found") { temp = "*"; }
		trump = temp[0];

		temp = validParameter.validFile(parameters, "soft", false);				if (temp == "not found") { soft = 0; }
		else { soft = (float)atoi(temp.c_str()) / 100.0; }

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		vertical = validParameter.validFile(parameters, "vertical", false);
		if (vertical == "not found") {
			if ((hard == "") && (trump == '*') && (soft == 0)) { vertical = "T"; } //you have not given a hard file or set the trump char.
			else { vertical = "F"; }
		}

		numSeqs = 0;
	}

}
/**************************************************************************************/

int FilterSeqsCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		ifstream inFASTA;
		File::openInputFile(fastafileNames[0], inFASTA);

		Sequence testSeq(inFASTA);
		alignmentLength = testSeq.getAlignLength();
		inFASTA.close();

		////////////create filter/////////////////
		LOG(INFO) << "Creating Filter... " << '\n';

		filter = createFilter();

		LOG(INFO) << "" << '\n';

		if (ctrlc_pressed) { outputTypes.clear(); return 0; }

		ofstream outFilter;

		//prevent giantic file name
		map<string, string> variables;
		variables["[filename]"] = outputDir + filterFileName + ".";
		if (fastafileNames.size() > 3) { variables["[filename]"] = outputDir + "merge."; }
		string filterFile = getOutputFileName("filter", variables);

		File::openOutputFile(filterFile, outFilter);
		outFilter << filter << endl;
		outFilter.close();
		outputNames.push_back(filterFile); outputTypes["filter"].push_back(filterFile);

		////////////run filter/////////////////

		LOG(INFO) << "Running Filter... " << '\n';

		filterSequences();

		LOG(INFO) << "";	m->mothurOutEndLine();

		int filteredLength = 0;
		for (int i = 0;i < alignmentLength;i++) {
			if (filter[i] == '1') { filteredLength++; }
		}

		if (ctrlc_pressed) { outputTypes.clear(); for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }


		LOG(INFO) << '\n' << "Length of filtered alignment: " + toString(filteredLength) << '\n';
		LOG(INFO) << "Number of columns removed: " + toString((alignmentLength - filteredLength)) << '\n';
		LOG(INFO) << "Length of the original alignment: " + toString(alignmentLength) << '\n';
		LOG(INFO) << "Number of sequences used to construct filter: " + toString(numSeqs) << '\n';

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
		LOG(FATAL) << e.what() << " in FilterSeqsCommand, execute";
		exit(1);
	}
}
/**************************************************************************************/
int FilterSeqsCommand::filterSequences() {
	try {

		numSeqs = 0;

		for (int s = 0; s < fastafileNames.size(); s++) {

			lines.clear();

			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastafileNames[s]));
			string filteredFasta = getOutputFileName("fasta", variables);

			vector<unsigned long long> positions;
			if (savedPositions.size() != 0) { positions = savedPositions[s]; }
			else {
				positions = File::divideFile(fastafileNames[s], processors);
			}
			//vector<unsigned long long> positions = File::divideFile(fastafileNames[s], processors);

			for (int i = 0; i < (positions.size() - 1); i++) {
				lines.push_back(linePair(positions[i], positions[(i + 1)]));
			}

			if (processors == 1) {
				int numFastaSeqs = driverRunFilter(filter, filteredFasta, fastafileNames[s], lines[0]);
				numSeqs += numFastaSeqs;
			}
			else {
				int numFastaSeqs = createProcessesRunFilter(filter, fastafileNames[s], filteredFasta);
				numSeqs += numFastaSeqs;
			}

			if (ctrlc_pressed) { return 1; }

			outputNames.push_back(filteredFasta); outputTypes["fasta"].push_back(filteredFasta);
		}

		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in FilterSeqsCommand, filterSequences";
		exit(1);
	}
}
/**************************************************************************************/
int FilterSeqsCommand::driverRunFilter(string F, string outputFilename, string inputFilename, linePair filePos) {
	try {
		ofstream out;
		File::openOutputFile(outputFilename, out);

		ifstream in;
		File::openInputFile(inputFilename, in);

		in.seekg(filePos.start);

		//adjust start if null strings
		if (filePos.start == 0) { m->zapGremlins(in); File::gobble(in); }

		bool done = false;
		int count = 0;

		while (!done) {

			if (ctrlc_pressed) { in.close(); out.close(); return 0; }

			Sequence seq(in); File::gobble(in);
			if (seq.getName() != "") {
				string align = seq.getAligned();
				string filterSeq = "";

				for (int j = 0;j < alignmentLength;j++) {
					if (filter[j] == '1') {
						filterSeq += align[j];
					}
				}

				out << '>' << seq.getName() << endl << filterSeq << endl;
				count++;
			}

			unsigned long long pos = in.tellg();
			if ((pos == -1) || (pos >= filePos.end)) { break; }

			//report progress
			if ((count) % 100 == 0) { LOG(SCREENONLY) << toString(count) + "\n"; }
		}
		//report progress
		if ((count) % 100 != 0) { LOG(SCREENONLY) << toString(count) + "\n"; }


		out.close();
		in.close();

		return count;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in FilterSeqsCommand, driverRunFilter";
		exit(1);
	}
}
/**************************************************************************************************/

void FilterSeqsCommand::driverRunFilterWithCount(string F, string outputFilename, string inputFilename, linePair filePos, int & count) {
	count = driverRunFilter(F, outputFilename, inputFilename, filePos);
}

int FilterSeqsCommand::createProcessesRunFilter(string F, string filename, string filteredFastaName) {
	vector<thread> thrds(lines.size() - 1);
	vector<int> nums(lines.size() - 1);

	//loop through and create all the processes you want
	for (int i = 0; i < thrds.size(); i++) {
		string filteredFasta = filename + toString(i + 1) + ".temp";
		thrds[i] = thread(&FilterSeqsCommand::driverRunFilterWithCount, this, F, filteredFasta, filename, lines[i + 1], ref(nums[i]));
	}

	// Task for main thread
	int num = driverRunFilter(F, filteredFastaName, filename, lines[0]);

	for (int i = 0; i < thrds.size(); i++) {
		thrds[i].join();
		num += nums[i];
	}

	for (int i = 1; i < lines.size(); i++) {
		File::appendFiles(filename + toString(i) + ".temp", filteredFastaName);
		File::remove(filename + toString(i) + ".temp");
	}

	return num;

}
/**************************************************************************************/
string FilterSeqsCommand::createFilter() {
	try {
		string filterString = "";
		Filters F;

		if (soft != 0) { F.setSoft(soft); }
		if (trump != '*') { F.setTrump(trump); }

		F.setLength(alignmentLength);

		if (trump != '*' || m->isTrue(vertical) || soft != 0) {
			F.initialize();
		}

		if (hard.compare("") != 0) { F.doHard(hard); }
		else { F.setFilter(string(alignmentLength, '1')); }

		numSeqs = 0;
		if (trump != '*' || m->isTrue(vertical) || soft != 0) {
			for (int s = 0; s < fastafileNames.size(); s++) {

				lines.clear();


				vector<unsigned long long> positions;
				positions = File::divideFile(fastafileNames[s], processors);
				for (int i = 0; i < (positions.size() - 1); i++) {
					lines.push_back(linePair(positions[i], positions[(i + 1)]));
				}

				if (processors == 1) {
					int numFastaSeqs = driverCreateFilter(F, fastafileNames[s], lines[0]);
					numSeqs += numFastaSeqs;
				}
				else {
					int numFastaSeqs = createProcessesCreateFilter(F, fastafileNames[s]);
					numSeqs += numFastaSeqs;
				}
				//save the file positions so we can reuse them in the runFilter function
				if (!recalced) { savedPositions[s] = positions; }

				if (ctrlc_pressed) { return filterString; }

			}
		}

		if (m->isTrue(vertical) == 1) { F.doVertical(); }
		if (soft != 0) { F.doSoft(); }
		filterString = F.getFilter();

		return filterString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in FilterSeqsCommand, createFilter";
		exit(1);
	}
}
/**************************************************************************************/
int FilterSeqsCommand::driverCreateFilter(Filters& F, string filename, linePair filePos) {
	try {

		ifstream in;
		File::openInputFile(filename, in);

		in.seekg(filePos.start);

		//adjust start if null strings
		if (filePos.start == 0) { m->zapGremlins(in); File::gobble(in); }

		bool done = false;
		int count = 0;
		bool error = false;

		while (!done) {

			if (ctrlc_pressed) { in.close(); return 1; }

			Sequence seq(in); File::gobble(in);
			if (seq.getName() != "") {
				if (app.isDebug) { LOG(SCREENONLY) << "[DEBUG]: " + seq.getName() + " length = " + toString(seq.getAligned().length()) << '\n'; }
				if (seq.getAligned().length() != alignmentLength) { LOG(LOGERROR) << "Sequences are not all the same length, please correct." << '\n'; error = true; if (!app.isDebug) { ctrlc_pressed = true; } else { LOG(FILEONLY) << "[DEBUG]: " + seq.getName() + " length = " + toString(seq.getAligned().length()) << '\n'; } }

				if (trump != '*') { F.doTrump(seq); }
				if (m->isTrue(vertical) || soft != 0) { F.getFreqs(seq); }
				cout.flush();
				count++;
			}

			unsigned long long pos = in.tellg();
			if ((pos == -1) || (pos >= filePos.end)) { break; }

			//report progress
			if ((count) % 100 == 0) { LOG(SCREENONLY) << toString(count) + "\n"; }
		}
		//report progress
		if ((count) % 100 != 0) { LOG(SCREENONLY) << toString(count) + "\n"; }
		in.close();

		if (error) { ctrlc_pressed = true; }

		F.setNumSeqs(count);
		return count;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in FilterSeqsCommand, driverCreateFilter";
		exit(1);
	}
}
/**************************************************************************************************/

int FilterSeqsCommand::createProcessesCreateFilter(Filters& F, string filename) {
	int num = 0;

	vector<Filters> pDataArray(lines.size() - 1);
	vector<thread> thrds(lines.size() - 1);

	//loop through and create all the processes you want
	for (int i = 0; i < thrds.size(); i++) {
		if (soft != 0) { pDataArray[i].setSoft(soft); }
		if (trump != '*') { pDataArray[i].setTrump(trump); }

		pDataArray[i].setLength(alignmentLength);

		if (trump != '*' || m->isTrue(vertical) || soft != 0) {
			pDataArray[i].initialize();
		}

		if (hard.compare("") != 0) { pDataArray[i].doHard(hard); }
		else { pDataArray[i].setFilter(string(alignmentLength, '1')); }

		thrds[i] = thread(&FilterSeqsCommand::driverCreateFilter, this, ref(pDataArray[i]), filename, lines[i + 1]);
	}

	// Process for main worker task
	num = driverCreateFilter(F, filename, lines[0]);

	for (int i = 0; i < pDataArray.size(); i++) {
		thrds[i].join();
		num += pDataArray[i].getNumSeqs();
		F.setNumSeqs(F.getNumSeqs() + pDataArray[i].getNumSeqs());
		F.mergeFilter(pDataArray[i].getFilter());

		for (int k = 0; k < alignmentLength; k++) { F.a[k] += pDataArray[i].a[k]; }
		for (int k = 0; k < alignmentLength; k++) { F.t[k] += pDataArray[i].t[k]; }
		for (int k = 0; k < alignmentLength; k++) { F.g[k] += pDataArray[i].g[k]; }
		for (int k = 0; k < alignmentLength; k++) { F.c[k] += pDataArray[i].c[k]; }
		for (int k = 0; k < alignmentLength; k++) { F.gap[k] += pDataArray[i].gap[k]; }
	}
	return num;

}
/**************************************************************************************/
