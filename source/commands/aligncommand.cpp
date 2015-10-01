/*
 *  aligncommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 5/15/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 *	This version of nast does everything I think that the greengenes nast server does and then some.  I have added the
 *	feature of allowing users to define their database, kmer size for searching, alignment penalty values and alignment
 *	method.  This latter feature is perhaps most significant.  nastPlus enables a user to use either a Needleman-Wunsch
 *	(non-affine gap penalty) or Gotoh (affine gap penalty) pairwise alignment algorithm.  This is significant because it
 *	allows for a global alignment and not the local alignment provided by bLAst.  Furthermore, it has the potential to
 *	provide a better alignment because of the banding method employed by blast (I'm not sure about this).
 *
 */

#include "aligncommand.h"
#include "referencedb.h"
#include <thread>

 //**********************************************************************************************************************
vector<string> AlignCommand::setParameters() {
	try {
		nkParameters.add(new ReferenceFileParameter(settings, "reference", true, true));
		nkParameters.add(new FastaFileParameter(settings, "fasta", true, true));
		nkParameters.add(new MultipleParameter("search", vector<string> {"kmer", "blast", "suffix"}, "kmer", false, false, true));
		nkParameters.add(new NumberParameter("ksize", -INFINITY, INFINITY, 8, false, false));
		nkParameters.add(new NumberParameter("match", -INFINITY, INFINITY, 1.0, false, false));
		nkParameters.add(new MultipleParameter("align", vector<string>{"needleman", "gotoh", "blast", "noalign"}, "needleman", false, false, true));
		nkParameters.add(new NumberParameter("mismatch", -INFINITY, INFINITY, -1.0, false, false));
		nkParameters.add(new NumberParameter("gapopen", -INFINITY, INFINITY, -5.0, false, false));
		nkParameters.add(new NumberParameter("gapextend", -INFINITY, INFINITY, -2.0, false, false));
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new BooleanParameter("flip", false, false, false));
		nkParameters.add(new BooleanParameter("save", false, false, false));
		nkParameters.add(new NumberParameter("threshold", -INFINITY, INFINITY, 0.50, false, false));
		nkParameters.addStandardParameters();
		return nkParameters.getNames();
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in AlignCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string AlignCommand::getHelpString() {
	try {
		string helpString = "The align.seqs command reads a file containing sequences and creates an alignment file and a report file."
			"The align.seqs command parameters are reference, fasta, search, ksize, align, match, mismatch, gapopen, gapextend and processors."
			"The reference and fasta parameters are required. You may leave fasta blank if you have a valid fasta file. You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amzon.fasta."
			"The search parameter allows you to specify the method to find most similar template.  Your options are: suffix, kmer and blast. The default is kmer."
			"The align parameter allows you to specify the alignment method to use.  Your options are: gotoh, needleman, blast and noalign. The default is needleman."
			"The ksize parameter allows you to specify the kmer size for finding most similar template to candidate.  The default is 8."
			"The match parameter allows you to specify the bonus for having the same base. The default is 1.0."
			"The mistmatch parameter allows you to specify the penalty for having different bases.  The default is -1.0."
			"The gapopen parameter allows you to specify the penalty for opening a gap in an alignment. The default is -5.0."
			"The gapextend parameter allows you to specify the penalty for extending a gap in an alignment.  The default is -2.0."
			"The flip parameter is used to specify whether or not you want mothur to try the reverse complement if a sequence falls below the threshold.  The default is false."
			"The threshold is used to specify a cutoff at which an alignment is deemed 'bad' and the reverse complement may be tried. The default threshold is 0.50, meaning 50% of the bases are removed in the alignment."
			"If the flip parameter is set to true the reverse complement of the sequence is aligned and the better alignment is reported."
			"If the save parameter is set to true the reference sequences will be saved in memory, to clear them later you can use the clear.memory command. Default=f."
			"The default for the threshold parameter is 0.50, meaning at least 50% of the bases must remain or the sequence is reported as potentially reversed."
			"The align.seqs command should be in the following format:"
			"align.seqs(reference=yourTemplateFile, fasta=yourCandidateFile, align=yourAlignmentMethod, search=yourSearchmethod, ksize=yourKmerSize, match=yourMatchBonus, mismatch=yourMismatchpenalty, gapopen=yourGapopenPenalty, gapextend=yourGapExtendPenalty)"
			"Example align.seqs(candidate=candidate.fasta, template=core.filtered, align=kmer, search=gotoh, ksize=8, match=2.0, mismatch=3.0, gapopen=-2.0, gapextend=-1.0)"
			"Note: No spaces between parameter labels (i.e. candidate), '=' and parameters (i.e.yourFastaFile).";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in AlignCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string AlignCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],align"; } //makes file like: amazon.align
	else if (type == "alignreport") { pattern = "[filename],align.report"; }
	else if (type == "accnos") { pattern = "[filename],flip.accnos"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; abort = true; }

	return pattern;
}
//**********************************************************************************************************************
AlignCommand::AlignCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["alignreport"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in AlignCommand, AlignCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
//AlignCommand::AlignCommand(Settings& settings, string option) : Command(settings, option) {
//	try {
//			//check for optional parameter and set defaults
//			// ...at some point should added some additional type checking...
//			//rdb.save = save; 
//			//if (save) { //clear out old references
//			//	rdb.clearMemory();
//			//}
//			//
//			////this has to go after save so that if the user sets save=t and provides no reference we abort
//			//templateFileName = validParameter.validFile(parameters, "reference", true);
//			//if (templateFileName == "not found") { 
//			//	//check for saved reference sequences
//			//	if (rdb.referenceSeqs.size() != 0) {
//			//		templateFileName = "saved";
//			//	}else {
//			//		LOG(LOGERROR) << "You don't have any saved reference sequences and the reference parameter is a required for the align.seqs command."; 
//			//		LOG(INFO) << "";
//			//		abort = true; 
//			//	}
//			//}else if (templateFileName == "not open") { abort = true; }	
//			//else {	if (save) {	rdb.setSavedReference(templateFileName);	}	}
//			
//		}
//		
//	}
//	catch(exception& e) {
//		LOG(FATAL) << e.what() << " in AlignCommand, AlignCommand";
//		exit(1);
//	}
//}
//**********************************************************************************************************************

int AlignCommand::execute() {
	templateDB = unique_ptr<AlignmentDB>(new AlignmentDB(templateFileName, search, kmerSize, gapOpen, gapExtend, match, misMatch, rand()));

	for (int s = 0; s < candidateFileNames.size(); s++) {
		if (ctrlc_pressed) { outputTypes.clear(); return 0; }

		LOG(INFO) << "Aligning sequences from " + candidateFileNames[s] + " ...";

		outputDir = nkParameters["outputdir"]->getValue();

		if (outputDir == "") { outputDir += File::getPath(candidateFileNames[s]); }
		map<string, string> variables; variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(candidateFileNames[s]));
		string alignFileName = getOutputFileName("fasta", variables);
		string reportFileName = getOutputFileName("alignreport", variables);
		string accnosFileName = getOutputFileName("accnos", variables);

		bool hasAccnos = true;

		int numFastaSeqs = 0;
		lines.clear();
		int start = time(NULL);


		vector<unsigned long long> positions;
		positions = File::divideFile(candidateFileNames[s], processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }

		if (processors == 1) {
			numFastaSeqs = driver(lines[0], alignFileName, reportFileName, accnosFileName, candidateFileNames[s]);
		}
		else {
			numFastaSeqs = createProcesses(alignFileName, reportFileName, accnosFileName, candidateFileNames[s]);
		}

		if (ctrlc_pressed) { File::remove(accnosFileName); File::remove(alignFileName); File::remove(reportFileName); outputTypes.clear();  return 0; }

		//delete accnos file if its blank else report to user
		if (File::isBlank(accnosFileName)) { File::remove(accnosFileName);  hasAccnos = false; }
		else {
			LOG(INFO) << "Some of your sequences generated alignments that eliminated too many bases, a list is provided in " + accnosFileName + ".";
			if (!flip) {
				LOG(INFO) << " If you set the flip parameter to true mothur will try aligning the reverse compliment as well.";
			}
			else { LOG(INFO) << " If the reverse compliment proved to be better it was reported."; }
			LOG(INFO) << "";
		}

		outputNames.push_back(alignFileName); outputTypes["fasta"].push_back(alignFileName);
		outputNames.push_back(reportFileName); outputTypes["alignreport"].push_back(reportFileName);
		if (hasAccnos) { outputNames.push_back(accnosFileName);	outputTypes["accnos"].push_back(accnosFileName); }

		LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to align " + toString(numFastaSeqs) + " sequences.";
	}

	//set align file as new current fastafile
	string currentFasta = "";
	itTypes = outputTypes.find("fasta");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { currentFasta = (itTypes->second)[0]; settings.setCurrent("fasta", currentFasta); }
	}

	LOG(INFO) << std::endl << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}

//**********************************************************************************************************************
int AlignCommand::driver(linePair filePos, string alignFName, string reportFName, string accnosFName, string filename) {

	ofstream alignmentFile;
	File::openOutputFile(alignFName, alignmentFile);

	ofstream accnosFile;
	File::openOutputFile(accnosFName, accnosFile);

	NastReport report(reportFName);

	ifstream inFASTA;
	File::openInputFile(filename, inFASTA);

	inFASTA.seekg(filePos.start);

	bool done = false;
	int count = 0;

	//moved this into driver to avoid deep copies in windows paralellized version
	shared_ptr<Alignment> alignment;
	int longestBase = templateDB->getLongestBase();
	LOG(DEBUG) << "template longest base = " << templateDB->getLongestBase();
	if (align == "gotoh") { alignment = make_shared<GotohOverlap>(gapOpen, gapExtend, match, misMatch, longestBase); }
	else if (align == "needleman") { alignment = make_shared<NeedlemanOverlap>(gapOpen, match, misMatch, longestBase); }
	else if (align == "blast") { alignment = make_shared<BlastAlignment>(gapOpen, gapExtend, match, misMatch); }
	else if (align == "noalign") { alignment = make_shared<NoAlign>(); }
	else {
		LOG(INFO) << align + " is not a valid alignment option. I will run the command using needleman.";
		LOG(INFO) << "";
		alignment = make_shared<NeedlemanOverlap>(gapOpen, match, misMatch, longestBase);
	}

	while (!done) {

		if (ctrlc_pressed) { break; }

		shared_ptr<Sequence> candidateSeq = make_shared<Sequence>(inFASTA);  File::gobble(inFASTA);
		report.setCandidate(candidateSeq.get());

		int origNumBases = candidateSeq->getNumBases();
		string originalUnaligned = candidateSeq->getUnaligned();
		int numBasesNeeded = origNumBases * threshold;

		if (candidateSeq->getName() != "") { //incase there is a commented sequence at the end of a file
			if (candidateSeq->getUnaligned().length() + 1 > alignment->getnRows()) {
				LOG(DEBUG) << "" + candidateSeq->getName() + " " + toString(candidateSeq->getUnaligned().length()) + " " + toString(alignment->getnRows());
				alignment->resize(candidateSeq->getUnaligned().length() + 2);
			}
			Sequence temp = templateDB->findClosestSequence(candidateSeq.get());
			shared_ptr<Sequence> templateSeq = make_shared<Sequence>(temp.getName(), temp.getAligned());

			float searchScore = templateDB->getSearchScore();

			Nast nast = Nast(alignment, candidateSeq, templateSeq);

			//if there is a possibility that this sequence should be reversed
			if (candidateSeq->getNumBases() < numBasesNeeded) {

				string wasBetter = "";
				//if the user wants you to try the reverse
				if (flip) {

					//get reverse compliment
					shared_ptr<Sequence> copy = make_shared<Sequence>(candidateSeq->getName(), originalUnaligned);
					copy->reverseComplement();

					LOG(DEBUG) << "flipping " + candidateSeq->getName() + " \n";

					//rerun alignment
					Sequence temp2 = templateDB->findClosestSequence(copy.get());
					shared_ptr<Sequence> templateSeq2 = make_shared<Sequence>(temp2.getName(), temp2.getAligned());

					LOG(DEBUG) << "closest template " + temp2.getName() + " \n";

					searchScore = templateDB->getSearchScore();

					Nast nast2 = Nast(alignment, copy, templateSeq2);

					LOG(DEBUG) << "completed Nast2 " + candidateSeq->getName() + " flipped numBases = " + toString(copy->getNumBases()) + " old numbases = " + toString(candidateSeq->getNumBases()) + " \n";

					//check if any better
					if (copy->getNumBases() > candidateSeq->getNumBases()) {
						candidateSeq->setAligned(copy->getAligned());  //use reverse compliments alignment since its better
						templateSeq = templateSeq2;
						nast = nast2;
						wasBetter = "\treverse complement produced a better alignment, so mothur used the reverse complement.";
					}
					else {
						wasBetter = "\treverse complement did NOT produce a better alignment so it was not used, please check sequence.";
					}
					LOG(DEBUG) << "done.\n";
				}

				//create accnos file with names
				accnosFile << candidateSeq->getName() << wasBetter << endl;
			}

			report.setTemplate(templateSeq.get());
			report.setSearchParameters(search, searchScore);
			report.setAlignmentParameters(align, alignment.get());
			report.setNastParameters(nast);

			alignmentFile << '>' << candidateSeq->getName() << '\n' << candidateSeq->getAligned() << endl;

			report.print();

			count++;
		}

		unsigned long long pos = inFASTA.tellg();
		if ((pos == -1) || (pos >= filePos.end)) { break; }

		//report progress
		if ((count) % 100 == 0) { LOG(SCREENONLY) << toString(count) + "\n"; }

	}
	//report progress
	if ((count) % 100 != 0) { LOG(SCREENONLY) << toString(count) + "\n"; }

	alignmentFile.close();
	inFASTA.close();
	accnosFile.close();

	return count;
}
/**************************************************************************************************/

void AlignCommand::driverWithCount(linePair filePos, string alignFName, string reportFName, string accnosFName, string filename, int & count) {
	count = driver(filePos, alignFName, reportFName, accnosFName, filename);
}

int AlignCommand::createProcesses(string alignFileName, string reportFileName, string accnosFName, string filename) {
	int num = 0;
	vector<thread> thrds(lines.size() - 1);
	vector<int> nums(lines.size() - 1);

	//loop through and create all the processes you want
	for (int i = 1; i < lines.size(); i++) {
		thrds[i - 1] = thread(&AlignCommand::driverWithCount, this, lines[i], alignFileName + toString(i) + ".temp", reportFileName + toString(i) + ".temp", accnosFName + toString(i) + ".temp", filename, ref(nums[i - 1]));
	}

	//do my part
	num = driver(lines[0], alignFileName, reportFileName, accnosFName, filename);

	//force parent to wait until all the processes are done
	for (int i = 0; i < thrds.size(); i++) {
		thrds[i].join();
		num += nums[i];
	}

	vector<string> nonBlankAccnosFiles;
	if (!(File::isBlank(accnosFName))) { nonBlankAccnosFiles.push_back(accnosFName); }
	else { File::remove(accnosFName); } //remove so other files can be renamed to it

	for (int i = 1; i < lines.size(); i++) {

		File::appendFiles(alignFileName + toString(i) + ".temp", alignFileName);
		File::remove(alignFileName + toString(i) + ".temp");

		appendReportFiles(reportFileName + toString(i) + ".temp", reportFileName);
		File::remove(reportFileName + toString(i) + ".temp");

		if (!(File::isBlank(accnosFName + toString(i) + ".temp"))) {
			nonBlankAccnosFiles.push_back(accnosFName + toString(i) + ".temp");
		}
		else { File::remove(accnosFName + toString(i) + ".temp"); }

	}

	//append accnos files
	if (nonBlankAccnosFiles.size() != 0) {
		rename(nonBlankAccnosFiles[0].c_str(), accnosFName.c_str());

		for (int h = 1; h < nonBlankAccnosFiles.size(); h++) {
			File::appendFiles(nonBlankAccnosFiles[h], accnosFName);
			File::remove(nonBlankAccnosFiles[h]);
		}
	}
	else { //recreate the accnosfile if needed
		ofstream out;
		File::openOutputFile(accnosFName, out);
		out.close();
	}

	return num;
}
//**********************************************************************************************************************

void AlignCommand::appendReportFiles(string temp, string filename) {
	try {

		ofstream output;
		ifstream input;
		File::openOutputFileAppend(filename, output);
		File::openInputFile(temp, input);

		while (!input.eof()) { char c = input.get(); if (c == 10 || c == 13) { break; } } // get header line

		char buffer[4096];
		while (!input.eof()) {
			input.read(buffer, 4096);
			output.write(buffer, input.gcount());
		}

		input.close();
		output.close();
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in AlignCommand, appendReportFiles";
		exit(1);
	}
}
//**********************************************************************************************************************
