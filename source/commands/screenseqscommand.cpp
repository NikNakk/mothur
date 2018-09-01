/*
 *  screenseqscommand.cpp
 *  Mothur
 *
 *  Created by Pat Schloss on 6/3/09.
 *  Copyright 2009 Patrick D. Schloss. All rights reserved.
 *
 */

#include "screenseqscommand.h"
#include "counttable.h"
#include "commandparameters/alignreportfileparameter.h"
#include "commandparameters/contigsreportfileparameter.h"
#include "commandparameters/countfileparameter.h"
#include "commandparameters/fastafileparameter.h"
#include "commandparameters/groupfileparameter.h"
#include "commandparameters/multipleparameter.h"
#include "commandparameters/namefileparameter.h"
#include "commandparameters/integerparameter.h"
#include "commandparameters/numberparameter.h"
#include "commandparameters/processorsparameter.h"
#include "commandparameters/qfileparameter.h"
#include "commandparameters/summaryfileparameter.h"
#include "commandparameters/taxonomyfileparameter.h"
#include "filehandling/fastafileread.h"
#include "filehandling/fastafilewrite.h"
#include "filehandling/summaryfileread.h"
#include "filehandling/textfilewrite.h"
#include <thread>
#include <future>

 //**********************************************************************************************************************
void ScreenSeqsCommand::setParameters() {
	parameters.add(new FastaFileParameter(fastafile, settings, "fasta", true, true));
	parameters.add(new ContigsReportFileParameter(contigsreport, settings, "contigsreport", false, true));
	parameters.add(new AlignReportFileParameter(alignreport, settings, "alignreport", false, false));
	parameters.add(new SummaryFileParameter(summaryfile, settings, "summary", false, true));
	parameters.add(new NameFileParameter(namefile, settings, "name", false, true, "NameCount"));
	parameters.add(new CountFileParameter(countfile, settings, "count", false, true, "NameCount-CountGroup"));
	parameters.add(new GroupFileParameter(groupfile, settings, "group", false, true, "CountGroup"));
	parameters.add(new QFileFileParameter(qualfile, settings, "qfile", false, false));
	parameters.add(new TaxonomyFileParameter(taxonomy, settings, "taxonomy", false, false));
	parameters.add(new IntegerParameter(startPos, "start", -1, INT_MAX, -1, false, true));
	parameters.add(new IntegerParameter(endPos, "end", -1, INT_MAX, -1, false, true));
	parameters.add(new IntegerParameter(maxAmbig, "maxambig", -1, INT_MAX, -1));
	parameters.add(new IntegerParameter(maxHomoP, "maxhomop", -1, INT_MAX, -1));
	parameters.add(new IntegerParameter(minLength, "minlength", -1, INT_MAX, -1));
	parameters.add(new IntegerParameter(maxLength, "maxlength", -1, INT_MAX, -1));
	parameters.add(new ProcessorsParameter(processors, settings));
	parameters.add(new NumberParameter(criteria, "criteria", -INFINITY, INFINITY, 90, false, false));
	parameters.add(new MultipleParameter(optimize, "optimize", { "none", "start", "end", "maxambig", "maxhomop", "minlength", "maxlength", "maxn" }, "none", true));

	//report parameters
	parameters.add(new IntegerParameter(minOverlap, "minoverlap", -1, INT_MAX, -1, false, false));
	parameters.add(new IntegerParameter(oStart, "ostart", -1, INT_MAX, -1, false, false));
	parameters.add(new IntegerParameter(oEnd, "oend", -1, INT_MAX, -1, false, false));
	parameters.add(new IntegerParameter(mismatches, "mismatches", -1, INT_MAX, -1, false, false));
	parameters.add(new IntegerParameter(maxN, "maxn", -1, INT_MAX, -1, false, false));
	parameters.add(new NumberParameter(minScore, "minscore", -1, INT_MAX, -1, false, false));
	parameters.add(new IntegerParameter(maxInsert, "maxinsert", -1, INT_MAX, -1, false, false));
	parameters.add(new NumberParameter(minSim, "minsim", -1, INT_MAX, -1, false, false));
}
//**********************************************************************************************************************
std::string ScreenSeqsCommand::getHelpString() const {
	return
		R"(The screen.seqs command reads a fastafile and screens sequences.
The screen.seqs command parameters are fasta, start, end, maxambig, maxhomop, minlength, maxlength, name, group, count, qfile, alignreport, contigsreport, summary, taxonomy, optimize, criteria and processors.
The fasta parameter is required.
The contigsreport parameter allows you to use the contigsreport file to determine if a sequence is good. Screening parameters include: minoverlap, ostart, oend and mismatches. 
The alignreport parameter allows you to use the alignreport file to determine if a sequence is good. Screening parameters include: minsim, minscore and maxinsert. 
The summary parameter allows you to use the summary file from summary.seqs to save time processing.
The taxonomy parameter allows you to remove bad seqs from taxonomy files.
The start parameter is used to set a position the "good" sequences must start by. The default is -1.
The end parameter is used to set a position the "good" sequences must end after. The default is -1.
The maxambig parameter allows you to set the maximum number of ambiguous bases allowed. The default is -1.
The maxhomop parameter allows you to set a maximum homopolymer length. 
The minlength parameter allows you to set and minimum sequence length. 
The maxn parameter allows you to set and maximum number of N's allowed in a sequence. 
The minoverlap parameter allows you to set and minimum overlap. The default is -1. 
The ostart parameter is used to set an overlap position the "good" sequences must start by. The default is -1. 
The oend parameter is used to set an overlap position the "good" sequences must end after. The default is -1.
The mismatches parameter allows you to set and maximum mismatches in the contigs.report. 
The minsim parameter allows you to set the minimum similarity to template sequences during alignment. Found in column 'SimBtwnQuery&Template' in align.report file.
The minscore parameter allows you to set the minimum search score during alignment. Found in column 'SearchScore' in align.report file.
The maxinsert parameter allows you to set the maximum number of insertions during alignment. Found in column 'LongestInsert' in align.report file.
The processors parameter allows you to specify the number of processors to use while running the command. The default is 1.
The optimize and criteria parameters allow you set the start, end, maxambig, maxhomop, minlength and maxlength parameters relative to your set of sequences .
For example optimize=start-end, criteria=90, would set the start and end values to the position 90% of your sequences started and ended.
The name parameter allows you to provide a namesfile, and the group parameter allows you to provide a groupfile.
The screen.seqs command should be in the following format: 
screen.seqs(fasta=yourFastaFile, name=youNameFile, group=yourGroupFile, start=yourStart, end=yourEnd, maxambig=yourMaxambig,  
maxhomop=yourMaxhomop, minlength=youMinlength, maxlength=yourMaxlength)  
Example screen.seqs(fasta=abrecovery.fasta, name=abrecovery.names, group=abrecovery.groups, start=..., end=..., maxambig=..., maxhomop=..., minlength=..., maxlength=...).
Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).)";
}
//**********************************************************************************************************************
void ScreenSeqsCommand::setOutputTypes() {
	outputTypes.add("fasta", "[filename],good,[extension]");
	outputTypes.add("taxonomy", "[filename],good,[extension]");
	outputTypes.add("name", "[filename],good,[extension]");
	outputTypes.add("group", "[filename],good,[extension]");
	outputTypes.add("count", "[filename],good,[extension]");
	outputTypes.add("accnos", "[filename],bad.accnos");
	outputTypes.add("qfile", "[filename],good,[extension]");
	outputTypes.add("alignreport", "[filename],good.align.report");
	outputTypes.add("contigsreport", "[filename],good.contigs.report");
	outputTypes.add("summary", "[filename],good.summary");
}
//***************************************************************************************************************

ScreenSeqsCommand::ScreenSeqsCommand(Settings& settings, ParameterListToProcess ptp) : Command(settings, ptp) {

	//	//check to make sure you have the files you need for certain screening
	//	if ((contigsreport == "") && ((minOverlap != -1) || (oStart != -1) || (oEnd != -1) || (mismatches != -1))) {
	//		LOG(LOGERROR) << "minoverlap, ostart, oend and mismatches can only be used with a contigs.report file, aborting.\n"; abort = true;
	//	}

	//	if ((alignreport == "") && ((minScore != -1) || (maxInsert != -1) || (minSim != -1))) {
	//		LOG(LOGERROR) << "minscore, maxinsert and minsim can only be used with a align.report file, aborting.\n"; abort = true;
	//	}


	//	if (countfile == "") {
	//		if (namefile == "") {
	//			vector<string> files; files.push_back(fastafile);
	//			OptionParser::getNameFile(files);
	//		}
	//	}
	//}

}
//***************************************************************************************************************

int ScreenSeqsCommand::execute() {
	map<string, string> badSeqNames;
	time_t start = time(NULL);
	int numFastaSeqs = 0;

	std::vector<FastaFileRead> fastaSplit = FastaFileRead(fastafile).split(processors);
	std::map<std::string, std::string> badSeqNames;

	bool needsOptimizationFromFasta = false;
	bool needsOptimizationFromSummary = false;
	bool needsOptimizationFromContigs = false;
	bool needsOptimizationFromAlign = false;
	for (auto opt : optimize) {
		if (opt == "maxn") {
			needsOptimizationFromFasta = true;
		}
		else if (opt == "maxambig" || opt == "maxhomop" || opt == "maxlength" || opt == "minlength" || opt == "start" || opt == "end") {
			if (summaryfile == "") {
				needsOptimizationFromFasta = true;
			}
			else {
				needsOptimizationFromSummary = true;
			}
		}
		else if (opt == "minoverlap" || opt == "ostart" || opt == "oend" || opt == "mismatches") {
			needsOptimizationFromContigs = true;
		}
		else if (opt == "minsim" || opt == "minscore" || opt == "maxinsert") {
			needsOptimizationFromAlign = true;
		}
	}
	if (needsOptimizationFromFasta || needsOptimizationFromSummary) {
		if (namefile != "") {
			nwt = NamesWithTotals(namefile);
		}
		else if (countfile != "") {
			nwt = CountTable(namefile).getNamesWithTotals();
		}
		if (needsOptimizationFromFasta) {
			optimizeFromFasta(fastaSplit);
		}
		else {
			optimizeFromSummaryFile(summaryfile);
		}
	}
	if (needsOptimizationFromContigs) {
		optimizeContigs();
	}
	if (ctrlc_pressed) { return 0; }
	if (needsOptimizationFromAlign) {
		optimizeAlign();
	}
	if (ctrlc_pressed) { return 0; }

	if (summaryfile != "") { numFastaSeqs = screenSummary(badSeqNames); }
	if (ctrlc_pressed) { return 0; }

	//add in any seqs that fail due to contigs report results
	if (contigsreport != "") { numFastaSeqs = screenContigs(badSeqNames); }
	if (ctrlc_pressed) { return 0; }

	//add in any seqs that fail due to align report
	if (alignreport != "") { numFastaSeqs = screenAlignReport(badSeqNames); }

	if (ctrlc_pressed) { return 0; }

	int numFastaSeqs = runFastaScreening(fastaSplit, badSeqNames);

	if (ctrlc_pressed) { return 0; }

	if (namefile != "") {
		screenNameGroupFile(badSeqNames);
	}
	else if (groupfile != "") {
		screenGroupFile(badSeqNames);
	}	// this screens just the group
	else if (countfile != "") {
		screenCountFile(badSeqNames);
	}

	if (ctrlc_pressed) { removeAllOutput(); return 0; }

	if (qualfile != "") { screenQual(badSeqNames); }
	if (taxonomy != "") { screenTaxonomy(badSeqNames); }

	if (ctrlc_pressed) { removeAllOutput(); return 0; }

	LOG(INFO) << "\nOutput File Names: ";
	for (auto ot : outputTypes) {
		if (ot.second.files.size() > 0) {
			LOG(INFO) << Utility::join(ot.second.files, "\n");
			settings.setCurrent(ot.first, ot.second.files[0]);
		}
	}

	LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to screen " + toString(numFastaSeqs) + " sequences.\n";

	return 0;
}
//***************************************************************************************************************/
int ScreenSeqsCommand::runFastaScreening(std::vector<FastaFileRead> fastaSplit, std::map<std::string, std::string> & badSeqNames) {
	int numFastaSeqs = 0;
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getSimpleRootName(fastafile);
	string badAccnosFile = getOutputFileName("accnos", variables);
	variables["[extension]"] = File::getExtension(fastafile);
	string goodSeqFile = getOutputFileName("fasta", variables);
	outputTypes["fasta"].files.push_back(goodSeqFile);
	outputTypes["accnos"].files.push_back(badAccnosFile);

	std::vector<std::future<sumScreenData>> futSsd(fastaSplit.size());
	std::vector<std::string> tmpGood(fastaSplit.size());
	std::vector<std::string> tmpBad(fastaSplit.size());

	for (int i = 1; i < fastaSplit.size(); ++i) {
		tmpGood[i] = goodSeqFile + std::to_string(i) + ".temp";
		tmpBad[i] = badAccnosFile + std::to_string(i) + ".temp";
		futSsd[i] = std::async(std::launch::async, &ScreenSeqsCommand::driver, this, fastaSplit[i], tmpGood[i], tmpBad[i], badSeqNames);
	}
	sumScreenData ssd = driver(fastaSplit[0], goodSeqFile, badAccnosFile, badSeqNames);

	for (int i = 1; i < fastaSplit.size(); ++i) {
		sumScreenData ssdTemp = futSsd[i].get();
		Utility::addToMap(ssd.badSeqs, ssdTemp.badSeqs);
		File::appendFilesAndRemove(tmpGood[i], goodSeqFile);
		File::appendFilesAndRemove(tmpBad[i], badAccnosFile);
		ssd.numSeqs += ssdTemp.numSeqs;
	}

	// Update master list of bad sequence names
	badSeqNames = ssd.badSeqs;

	return numFastaSeqs;
}
//**********************************************************************************************************************
sumScreenData ScreenSeqsCommand::driver(FastaFileRead& fasta, string goodFName, string badAccnosFName, const std::map<std::string, std::string> & badSeqNames) {
	FastaFileWrite goodFile(goodFName);
	TextFileWrite badAccnosFile(badAccnosFName);
	sumScreenData ssd;

	bool done = false;
	int count = 0;
	while (!fasta.reachedEnd() && !ctrlc_pressed) {
		Sequence currSeq = fasta.readSequence();
		if (currSeq.getName() != "") {
			bool goodSeq = true; //	innocent until proven guilty
			string trashCode = "";
			//have the report files found you bad
			auto it = badSeqNames.find(currSeq.getName());
			if (it != badSeqNames.end()) {
				goodSeq = false;
				trashCode += it->second;
			}
			if (summaryfile == "") {
				SequenceSummary seqSum = currSeq.getSequenceSummary();
				if (startPos != -1 && startPos < seqSum.startPosition) { goodSeq = false; trashCode += "start|"; }
				if (endPos != -1 && endPos > seqSum.endPosition) { goodSeq = false; trashCode += "end|"; }
				if (maxAmbig != -1 && maxAmbig < seqSum.ambigBases) { goodSeq = false; trashCode += "ambig|"; }
				if (maxHomoP != -1 && maxHomoP < seqSum.longHomoPolymer) { goodSeq = false; trashCode += "homop|"; }
				if (minLength != -1 && minLength > seqSum.seqLength) { goodSeq = false; trashCode += "<length|"; }
				if (maxLength != -1 && maxLength < seqSum.seqLength) { goodSeq = false; trashCode += ">length|"; }
				if (maxN != -1 && maxN < seqSum.numNs) { goodSeq = false; trashCode += "n|"; }
			}
			else if (contigsreport == "") { //contigs report includes this so no need to check again
				if (maxN != -1 && maxN < currSeq.getNumNs()) { goodSeq = false;	trashCode += "n|"; }
			}

			if (goodSeq) {
				goodFile.writeSequence(currSeq);
			}
			else {
				badAccnosFile.writeTwoColumns(currSeq.getName(), trashCode.substr(0, trashCode.length() - 1));
				ssd.badSeqs[currSeq.getName()] = trashCode;
			}
			count++;
		}

		//report progress
		if (count % 100 == 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(count); }
	}
	//report progress
	if (count % 100 != 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(count); }

	if (!ctrlc_pressed) {
		goodFile.commit();
		badAccnosFile.commit();
	}
	return ssd;
}
//***************************************************************************************************************
int ScreenSeqsCommand::screenAlignReport(map<string, string>& badSeqNames) {

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(alignreport));
	string outSummary = getOutputFileName("alignreport", variables);
	outputNames.push_back(outSummary); outputTypes["alignreport"].push_back(outSummary);

	string name, TemplateName, SearchMethod, AlignmentMethod;
	//QueryName	QueryLength	TemplateName	TemplateLength	SearchMethod	SearchScore	AlignmentMethod	QueryStart	QueryEnd	TemplateStart	TemplateEnd	PairwiseAlignmentLength	GapsInQuery	GapsInTemplate	LongestInsert	SimBtwnQuery&Template
	//checking for minScore, maxInsert, minSim
	int length, TemplateLength, QueryStart, QueryEnd, TemplateStart, TemplateEnd, PairwiseAlignmentLength, GapsInQuery, GapsInTemplate, LongestInsert;
	float SearchScore, SimBtwnQueryTemplate;

	ofstream out;
	File::openOutputFile(outSummary, out);

	//read summary file
	ifstream in;
	File::openInputFile(alignreport, in);
	out << (File::getline(in)) << endl;   //skip headers

	int count = 0;

	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); out.close(); return 0; }

		//seqname	start	end	nbases	ambigs	polymer	numSeqs
		in >> name >> length >> TemplateName >> TemplateLength >> SearchMethod >> SearchScore >> AlignmentMethod >> QueryStart >> QueryEnd >> TemplateStart >> TemplateEnd >> PairwiseAlignmentLength >> GapsInQuery >> GapsInTemplate >> LongestInsert >> SimBtwnQueryTemplate; File::gobble(in);

		bool goodSeq = 1;		//	innocent until proven guilty
		string trashCode = "";
		if (maxInsert != -1 && maxInsert < LongestInsert) { goodSeq = 0; trashCode += "insert|"; }
		if (minScore != -1 && minScore > SearchScore) { goodSeq = 0; trashCode += "score|"; }
		if (minSim != -1 && minSim > SimBtwnQueryTemplate) { goodSeq = 0; trashCode += "sim|"; }

		if (goodSeq == 1) {
			out << name << '\t' << length << '\t' << TemplateName << '\t' << TemplateLength << '\t' << SearchMethod << '\t' << SearchScore << '\t' << AlignmentMethod << '\t' << QueryStart << '\t' << QueryEnd << '\t' << TemplateStart << '\t' << TemplateEnd << '\t' << PairwiseAlignmentLength << '\t' << GapsInQuery << '\t' << GapsInTemplate << '\t' << LongestInsert << '\t' << SimBtwnQueryTemplate << endl;
		}
		else { badSeqNames[name] = trashCode; }
		count++;
	}
	in.close();
	out.close();

	int oldBadSeqsCount = badSeqNames.size();

	int numFastaSeqs = runFastaScreening(badSeqNames);

	if (oldBadSeqsCount != badSeqNames.size()) { //more seqs were removed by maxns
		m->renameFile(outSummary, outSummary + ".temp");

		ofstream out2;
		File::openOutputFile(outSummary, out2);

		//read summary file
		ifstream in2;
		File::openInputFile(outSummary + ".temp", in2);
		out2 << (File::getline(in2)) << endl;   //skip headers

		while (!in2.eof()) {

			if (ctrlc_pressed) { in2.close(); out2.close(); return 0; }

			//seqname	start	end	nbases	ambigs	polymer	numSeqs
			in2 >> name >> length >> TemplateName >> TemplateLength >> SearchMethod >> SearchScore >> AlignmentMethod >> QueryStart >> QueryEnd >> TemplateStart >> TemplateEnd >> PairwiseAlignmentLength >> GapsInQuery >> GapsInTemplate >> LongestInsert >> SimBtwnQueryTemplate; File::gobble(in2);

			if (badSeqNames.count(name) == 0) { //are you good?
				out2 << name << '\t' << length << '\t' << TemplateName << '\t' << TemplateLength << '\t' << SearchMethod << '\t' << SearchScore << '\t' << AlignmentMethod << '\t' << QueryStart << '\t' << QueryEnd << '\t' << TemplateStart << '\t' << TemplateEnd << '\t' << PairwiseAlignmentLength << '\t' << GapsInQuery << '\t' << GapsInTemplate << '\t' << LongestInsert << '\t' << SimBtwnQueryTemplate << endl;
			}
		}
		in2.close();
		out2.close();
		File::remove(outSummary + ".temp");
	}

	if (numFastaSeqs != count) { LOG(LOGERROR) << "found " + toString(numFastaSeqs) + " sequences in your fasta file, and " + toString(count) + " sequences in your align report file, quitting.\n"; ctrlc_pressed = true; }


	return count;

	return 0;


}
//***************************************************************************************************************/
int ScreenSeqsCommand::screenContigs(map<string, string>& badSeqNames) {
	try {
		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(contigsreport));
		string outSummary = getOutputFileName("contigsreport", variables);
		outputNames.push_back(outSummary); outputTypes["contigsreport"].push_back(outSummary);

		string name;
		//Name	Length	Overlap_Length	Overlap_Start	Overlap_End	MisMatches	Num_Ns
		int length, OLength, thisOStart, thisOEnd, numMisMatches, numNs;

		ofstream out;
		File::openOutputFile(outSummary, out);

		//read summary file
		ifstream in;
		File::openInputFile(contigsreport, in);
		out << (File::getline(in)) << endl;   //skip headers

		int count = 0;

		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); out.close(); return 0; }

			//seqname	start	end	nbases	ambigs	polymer	numSeqs
			in >> name >> length >> OLength >> thisOStart >> thisOEnd >> numMisMatches >> numNs; File::gobble(in);

			bool goodSeq = 1;		//	innocent until proven guilty
			string trashCode = "";
			if (oStart != -1 && oStart < thisOStart) { goodSeq = 0;	trashCode += "ostart|"; }
			if (oEnd != -1 && oEnd > thisOEnd) { goodSeq = 0;	trashCode += "oend|"; }
			if (maxN != -1 && maxN < numNs) { goodSeq = 0;	trashCode += "n|"; }
			if (minOverlap != -1 && minOverlap > OLength) { goodSeq = 0;	trashCode += "olength|"; }
			if (mismatches != -1 && mismatches < numMisMatches) { goodSeq = 0;	trashCode += "mismatches|"; }

			if (goodSeq == 1) {
				out << name << '\t' << length << '\t' << OLength << '\t' << thisOStart << '\t' << thisOEnd << '\t' << numMisMatches << '\t' << numNs << endl;
			}
			else { badSeqNames[name] = trashCode; }
			count++;
		}
		in.close();
		out.close();

		int oldBadSeqsCount = badSeqNames.size();

		int numFastaSeqs = runFastaScreening(badSeqNames);

		if (oldBadSeqsCount != badSeqNames.size()) { //more seqs were removed by maxns
			m->renameFile(outSummary, outSummary + ".temp");

			ofstream out2;
			File::openOutputFile(outSummary, out2);

			//read summary file
			ifstream in2;
			File::openInputFile(outSummary + ".temp", in2);
			out2 << (File::getline(in2)) << endl;   //skip headers

			while (!in2.eof()) {

				if (ctrlc_pressed) { in2.close(); out2.close(); return 0; }

				//seqname	start	end	nbases	ambigs	polymer	numSeqs
				in2 >> name >> length >> OLength >> thisOStart >> thisOEnd >> numMisMatches >> numNs; File::gobble(in2);

				if (badSeqNames.count(name) == 0) { //are you good?
					out2 << name << '\t' << length << '\t' << OLength << '\t' << thisOStart << '\t' << thisOEnd << '\t' << numMisMatches << '\t' << numNs << endl;
				}
			}
			in2.close();
			out2.close();
			File::remove(outSummary + ".temp");
		}

		if (numFastaSeqs != count) { LOG(LOGERROR) << "found " + toString(numFastaSeqs) + " sequences in your fasta file, and " + toString(count) + " sequences in your contigs report file, quitting.\n"; ctrlc_pressed = true; }


		return count;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ScreenSeqsCommand, screenContigs";
		exit(1);
	}
}
//***************************************************************************************************************/
int ScreenSeqsCommand::screenSummary(map<string, string>& badSeqNames) {
	try {
		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(summaryfile));
		string outSummary = getOutputFileName("summary", variables);
		outputNames.push_back(outSummary); outputTypes["summary"].push_back(outSummary);

		string name;
		int start, end, length, ambigs, polymer, numReps;

		ofstream out;
		File::openOutputFile(outSummary, out);

		//read summary file
		ifstream in;
		File::openInputFile(summaryfile, in);
		out << (File::getline(in)) << endl;   //skip headers

		int count = 0;

		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); out.close(); return 0; }

			//seqname	start	end	nbases	ambigs	polymer	numSeqs
			in >> name >> start >> end >> length >> ambigs >> polymer >> numReps; File::gobble(in);

			bool goodSeq = 1;		//	innocent until proven guilty
			string trashCode = "";
			if (startPos != -1 && startPos < start) { goodSeq = 0;	trashCode += "start|"; }
			if (endPos != -1 && endPos > end) { goodSeq = 0;	trashCode += "end|"; }
			if (maxAmbig != -1 && maxAmbig < ambigs) { goodSeq = 0;	trashCode += "ambig|"; }
			if (maxHomoP != -1 && maxHomoP < polymer) { goodSeq = 0;	trashCode += "homop|"; }
			if (minLength != -1 && minLength > length) { goodSeq = 0;	trashCode += "<length|"; }
			if (maxLength != -1 && maxLength < length) { goodSeq = 0;	trashCode += ">length|"; }

			if (goodSeq == 1) {
				out << name << '\t' << start << '\t' << end << '\t' << length << '\t' << ambigs << '\t' << polymer << '\t' << numReps << endl;
			}
			else { badSeqNames[name] = trashCode; }
			count++;
		}
		in.close();
		out.close();

		int oldBadSeqsCount = badSeqNames.size();

		int numFastaSeqs = runFastaScreening(badSeqNames);

		if (oldBadSeqsCount != badSeqNames.size()) { //more seqs were removed by maxns
			m->renameFile(outSummary, outSummary + ".temp");

			ofstream out2;
			File::openOutputFile(outSummary, out2);

			//read summary file
			ifstream in2;
			File::openInputFile(outSummary + ".temp", in2);
			out2 << (File::getline(in2)) << endl;   //skip headers

			while (!in2.eof()) {

				if (ctrlc_pressed) { in2.close(); out2.close(); return 0; }

				//seqname	start	end	nbases	ambigs	polymer	numSeqs
				in2 >> name >> start >> end >> length >> ambigs >> polymer >> numReps; File::gobble(in2);

				if (badSeqNames.count(name) == 0) { //are you good?
					out2 << name << '\t' << start << '\t' << end << '\t' << length << '\t' << ambigs << '\t' << polymer << '\t' << numReps << endl;
				}
			}
			in2.close();
			out2.close();
			File::remove(outSummary + ".temp");
		}

		if (numFastaSeqs != count) { LOG(LOGERROR) << "found " + toString(numFastaSeqs) + " sequences in your fasta file, and " + toString(count) + " sequences in your summary file, quitting.\n"; ctrlc_pressed = true; }



		return count;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ScreenSeqsCommand, screenSummary";
		exit(1);
	}
}
//***************************************************************************************************************/
//***************************************************************************************************************

int ScreenSeqsCommand::screenNameGroupFile(map<string, string> badSeqNames) {
	std::unordered_set<std::string> badSeqGroups;
	string seqName, seqList, group;
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(namefile));
	variables["[extension]"] = File::getExtension(namefile);
	string goodNameFile = getOutputFileName("name", variables);
	outputTypes["name"].files.push_back(goodNameFile);

	NameFileRead inputNames(namefile);
	NameFileWrite outputNames(goodNameFile);
	while (!inputNames.eof()) {
		std::vector<std::string> cols = inputNames.readRawNameFileLine();
		auto it = badSeqNames.find(cols[0]);

		if (it != badSeqNames.end()) {
			std::vector<std::string> seqNames = Utility::split(cols[1], ',');
			badSeqGroups.insert(seqNames.begin(), seqNames.end());
			badSeqNames.erase(it);
		}
		else {
			outputNames.writeTwoColumns(cols[0], cols[1]);
		}
	}

	//we were unable to remove some of the bad sequences
	if (badSeqNames.size() != 0) {
		for (auto badSeq : badSeqNames) {
			LOG(INFO) << "Your namefile does not include the sequence " + badSeq.first + " please correct.";
		}
	}

	if (groupfile != "") {

		ifstream inputGroups;
		File::openInputFile(groupfile, inputGroups);
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(groupfile));
		variables["[extension]"] = m->getExtension(groupfile);
		string goodGroupFile = getOutputFileName("group", variables);

		outputNames.push_back(goodGroupFile);   outputTypes["group"].push_back(goodGroupFile);

		ofstream goodGroupOut;	File::openOutputFile(goodGroupFile, goodGroupOut);

		while (!inputGroups.eof()) {
			if (ctrlc_pressed) { goodGroupOut.close(); inputGroups.close(); File::remove(goodNameFile);  File::remove(goodGroupFile); return 0; }

			inputGroups >> seqName; File::gobble(inputGroups); inputGroups >> group;

			it = badSeqGroups.find(seqName);

			if (it != badSeqGroups.end()) {
				badSeqGroups.erase(it);
			}
			else {
				goodGroupOut << seqName << '\t' << group << endl;
			}
			File::gobble(inputGroups);
		}
		inputGroups.close();
		goodGroupOut.close();

		//we were unable to remove some of the bad sequences
		if (badSeqGroups.size() != 0) {
			for (it = badSeqGroups.begin(); it != badSeqGroups.end(); it++) {
				LOG(INFO) << "Your groupfile does not include the sequence " + it->first + " please correct.";
				LOG(INFO) << "";
			}
		}
	}

	return 0;

}
//***************************************************************************************************************
//***************************************************************************************************************
int ScreenSeqsCommand::optimizeContigs() {
	vector<int> olengths;
	vector<int> oStarts;
	vector<int> oEnds;
	vector<int> numMismatches;
	vector<int> numNs;

	vector<unsigned long long> positions;
	vector<linePair> contigsLines;
	positions = File::divideFilePerLine(contigsreport, processors);
	for (int i = 0; i < (positions.size() - 1); i++) { contigsLines.push_back(linePair(positions[i], positions[(i + 1)])); }

	createProcessesContigsSummary(olengths, oStarts, oEnds, numMismatches, numNs, contigsLines);

	if (ctrlc_pressed) { return 0; }

	sort(olengths.begin(), olengths.end());
	sort(oStarts.begin(), oStarts.end());
	sort(oEnds.begin(), oEnds.end());
	sort(numMismatches.begin(), numMismatches.end());
	sort(numNs.begin(), numNs.end());

	//numSeqs is the number of unique seqs, startPosition.size() is the total number of seqs, we want to optimize using all seqs
	int criteriaPercentile = int(oStarts.size() * (criteria / (float)100));

	for (int i = 0; i < optimize.size(); i++) {
		if (optimize[i] == "ostart") { oStart = oStarts[criteriaPercentile]; LOG(INFO) << "Optimizing ostart to " + toString(oStart) + "." << '\n'; }
		else if (optimize[i] == "oend") { int endcriteriaPercentile = int(oEnds.size() * ((100 - criteria) / (float)100));  oEnd = oEnds[endcriteriaPercentile]; LOG(INFO) << "Optimizing oend to " + toString(oEnd) + "." << '\n'; }
		else if (optimize[i] == "mismatches") { mismatches = numMismatches[criteriaPercentile]; LOG(INFO) << "Optimizing mismatches to " + toString(mismatches) + "." << '\n'; }
		else if (optimize[i] == "maxn") { maxN = numNs[criteriaPercentile]; LOG(INFO) << "Optimizing maxn to " + toString(maxN) + "." << '\n'; }
		else if (optimize[i] == "minoverlap") { int mincriteriaPercentile = int(olengths.size() * ((100 - criteria) / (float)100)); minOverlap = olengths[mincriteriaPercentile]; LOG(INFO) << "Optimizing minoverlap to " + toString(minOverlap) + "." << '\n'; }

	}

	return 0;
}
/**************************************************************************************/
int ScreenSeqsCommand::driverContigsSummary(vector<int>& oLength, vector<int>& ostartPosition, vector<int>& oendPosition, vector<int>& omismatches, vector<int>& numNs, linePair filePos) {
	try {

		string name;
		//Name	Length	Overlap_Length	Overlap_Start	Overlap_End	MisMatches	Num_Ns
		int length, OLength, thisOStart, thisOEnd, numMisMatches, numns;

		ifstream in;
		File::openInputFile(contigsreport, in);

		in.seekg(filePos.start);
		if (filePos.start == 0) { //read headers
			m->zapGremlins(in); File::gobble(in); File::getline(in); File::gobble(in);
		}

		bool done = false;
		int count = 0;

		while (!done) {

			if (ctrlc_pressed) { in.close(); return 1; }

			//seqname	start	end	nbases	ambigs	polymer	numSeqs
			in >> name >> length >> OLength >> thisOStart >> thisOEnd >> numMisMatches >> numns; File::gobble(in);

			int num = 1;
			if ((namefile != "") || (countfile != "")) {
				//make sure this sequence is in the namefile, else error 
				map<string, int>::iterator it = nameMap.find(name);

				if (it == nameMap.end()) { LOG(LOGERROR) << "" + name + " is not in your namefile, please correct." << '\n'; ctrlc_pressed = true; }
				else { num = it->second; }
			}

			//for each sequence this sequence represents
			for (int i = 0; i < num; i++) {
				ostartPosition.push_back(thisOStart);
				oendPosition.push_back(thisOEnd);
				oLength.push_back(OLength);
				omismatches.push_back(numMisMatches);
				numNs.push_back(numns);
			}

			count++;

			//if((count) % 100 == 0){	LOG(INFO) << "Optimizing sequence: " + toString(count) << '\n';		}
			unsigned long long pos = in.tellg();
			if ((pos == -1) || (pos >= filePos.end)) { break; }
		}

		in.close();

		return count;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ScreenSeqsCommand, driverContigsSummary";
		exit(1);
	}
}

void ScreenSeqsCommand::driverContigsSummaryWithData(contigsSumData& csData, linePair filePos) {
	csData.numSeqs = driverContigsSummary(csData.oLength, csData.ostartPosition, csData.oendPosition, csData.omismatches, csData.numNs, filePos);
}

/**************************************************************************************************/
int ScreenSeqsCommand::createProcessesContigsSummary(vector<int>& oLength, vector<int>& ostartPosition, vector<int>& oendPosition, vector<int>& omismatches, vector<int>& numNs, vector<linePair> contigsLines) {
	int num = 0;

	//loop through and create all the processes you want
	vector<thread> thrds(contigsLines.size() - 1);
	vector<contigsSumData> csDataItems(contigsLines.size() - 1);

	//loop through and create all the processes you want
	for (int i = 1; i < contigsLines.size(); i++) {
		thrds[i - 1] = thread(&ScreenSeqsCommand::driverContigsSummaryWithData, this, ref(csDataItems[i - 1]), contigsLines[i]);
	}

	num = driverContigsSummary(oLength, ostartPosition, oendPosition, omismatches, numNs, contigsLines[0]);

	//force parent to wait until all the processes are done
	for (int i = 0;i < thrds.size();i++) {
		thrds[i].join();
		num += csDataItems[i].numSeqs;
	}

	ostartPosition.reserve(num);
	oendPosition.reserve(num);
	oLength.reserve(num);
	omismatches.reserve(num);
	numNs.reserve(num);
	for (int i = 0; i < csDataItems.size(); i++) {
		ostartPosition.insert(ostartPosition.end(), csDataItems[i].ostartPosition.begin(), csDataItems[i].ostartPosition.end());
		oendPosition.insert(oendPosition.end(), csDataItems[i].oendPosition.begin(), csDataItems[i].oendPosition.end());
		oLength.insert(oLength.end(), csDataItems[i].oLength.begin(), csDataItems[i].oLength.end());
		omismatches.insert(omismatches.end(), csDataItems[i].omismatches.begin(), csDataItems[i].omismatches.end());
		numNs.insert(numNs.end(), csDataItems[i].numNs.begin(), csDataItems[i].numNs.end());
	}
	return num;
}
//***************************************************************************************************************
int ScreenSeqsCommand::optimizeAlign() {

	vector<float> sims;
	vector<float> scores;
	vector<int> inserts;

	vector<unsigned long long> positions;
	vector<linePair> alignLines;
	positions = File::divideFilePerLine(alignreport, processors);
	for (int i = 0; i < (positions.size() - 1); i++) { alignLines.push_back(linePair(positions[i], positions[(i + 1)])); }

	createProcessesAlignSummary(sims, scores, inserts, alignLines);

	if (ctrlc_pressed) { return 0; }

	sort(sims.begin(), sims.end());
	sort(scores.begin(), scores.end());
	sort(inserts.begin(), inserts.end());

	//numSeqs is the number of unique seqs, startPosition.size() is the total number of seqs, we want to optimize using all seqs
	int criteriaPercentile = int(sims.size() * (criteria / (float)100));

	for (int i = 0; i < optimize.size(); i++) {
		if (optimize[i] == "minsim") { int mincriteriaPercentile = int(sims.size() * ((100 - criteria) / (float)100)); minSim = sims[mincriteriaPercentile];  LOG(INFO) << "Optimizing minsim to " + toString(minSim) + "." << '\n'; }
		else if (optimize[i] == "minscore") { int mincriteriaPercentile = int(scores.size() * ((100 - criteria) / (float)100)); minScore = scores[mincriteriaPercentile];  LOG(INFO) << "Optimizing minscore to " + toString(minScore) + "." << '\n'; }
		else if (optimize[i] == "maxinsert") { maxInsert = inserts[criteriaPercentile]; LOG(INFO) << "Optimizing maxinsert to " + toString(maxInsert) + "." << '\n'; }
	}

	return 0;
}
/**************************************************************************************/
int ScreenSeqsCommand::driverAlignSummary(vector<float>& sims, vector<float>& scores, vector<int>& inserts, linePair filePos) {
	try {

		string name, TemplateName, SearchMethod, AlignmentMethod;
		//QueryName	QueryLength	TemplateName	TemplateLength	SearchMethod	SearchScore	AlignmentMethod	QueryStart	QueryEnd	TemplateStart	TemplateEnd	PairwiseAlignmentLength	GapsInQuery	GapsInTemplate	LongestInsert	SimBtwnQuery&Template
		//checking for minScore, maxInsert, minSim
		int length, TemplateLength, QueryStart, QueryEnd, TemplateStart, TemplateEnd, PairwiseAlignmentLength, GapsInQuery, GapsInTemplate, LongestInsert;
		float SearchScore, SimBtwnQueryTemplate;

		ifstream in;
		File::openInputFile(alignreport, in);

		in.seekg(filePos.start);
		if (filePos.start == 0) { //read headers
			m->zapGremlins(in); File::gobble(in); File::getline(in); File::gobble(in);
		}

		bool done = false;
		int count = 0;

		while (!done) {

			if (ctrlc_pressed) { in.close(); return 1; }

			in >> name >> length >> TemplateName >> TemplateLength >> SearchMethod >> SearchScore >> AlignmentMethod >> QueryStart >> QueryEnd >> TemplateStart >> TemplateEnd >> PairwiseAlignmentLength >> GapsInQuery >> GapsInTemplate >> LongestInsert >> SimBtwnQueryTemplate; File::gobble(in);

			int num = 1;
			if ((namefile != "") || (countfile != "")) {
				//make sure this sequence is in the namefile, else error 
				map<string, int>::iterator it = nameMap.find(name);

				if (it == nameMap.end()) { LOG(LOGERROR) << "" + name + " is not in your namefile, please correct." << '\n'; ctrlc_pressed = true; }
				else { num = it->second; }
			}

			//for each sequence this sequence represents
			for (int i = 0; i < num; i++) {
				sims.push_back(SimBtwnQueryTemplate);
				scores.push_back(SearchScore);
				inserts.push_back(LongestInsert);
			}

			count++;

			//if((count) % 100 == 0){	LOG(INFO) << "Optimizing sequence: " + toString(count) << '\n';		}
			unsigned long long pos = in.tellg();
			if ((pos == -1) || (pos >= filePos.end)) { break; }
		}

		in.close();

		return count;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ScreenSeqsCommand, driverAlignSummary";
		exit(1);
	}
}

void ScreenSeqsCommand::driverAlignSummaryWithData(alignsData& aData, linePair filePos) {
	driverAlignSummary(aData.sims, aData.scores, aData.inserts, filePos);
}

/**************************************************************************************************/
int ScreenSeqsCommand::createProcessesAlignSummary(vector<float>& sims, vector<float>& scores, vector<int>& inserts, vector<linePair> alignLines) {

	int num = 0;

	vector<thread> thrds(alignLines.size() - 1);
	vector<alignsData> aDataItems(alignLines.size() - 1);

	//loop through and create all the processes you want
	for (int i = 1; i < alignLines.size(); i++) {
		thrds[i - 1] = thread(&ScreenSeqsCommand::driverAlignSummaryWithData, this, ref(aDataItems[i - 1]), alignLines[i]);
	}

	num = driverAlignSummary(sims, scores, inserts, alignLines[0]);

	//force parent to wait until all the processes are done
	for (int i = 0;i < thrds.size();i++) {
		thrds[i].join();
		num += aDataItems[i].numSeqs;
	}

	sims.reserve(num);
	scores.reserve(num);
	inserts.reserve(num);
	for (int i = 0; i < aDataItems.size(); i++) {
		sims.insert(sims.end(), aDataItems[i].sims.begin(), aDataItems[i].sims.end());
		scores.insert(scores.end(), aDataItems[i].scores.begin(), aDataItems[i].scores.end());
		inserts.insert(inserts.end(), aDataItems[i].inserts.begin(), aDataItems[i].inserts.end());
	}
	return num;
}
//***************************************************************************************************************
inline int ScreenSeqsCommand::optimizeParameter(const std::string & parameterName, const std::map<int, long long>& mapData, double criteria, long long numSeqs)
{
	int result = Utility::findCentile(mapData, criteria / 100.0, numSeqs);
	LOG(INFO) << "Optimizing " << parameterName << " to " << toString(startPos) << ".";
	return result;
}
//***************************************************************************************************************
void ScreenSeqsCommand::optimizeFromFasta(vector<FastaFileRead> fastaSplit) {
	SummaryData sumData;
	
	std::vector<std::future<SummaryData>> futSumData(fastaSplit.size());
	for (int i = 1; i < fastaSplit.size(); ++i) {
		futSumData[i] = std::async(std::launch::async, &ScreenSeqsCommand::driverCreateSummary, this, fastaSplit[i]);
	}
	sumData = driverCreateSummary(fastaSplit[0]);

	for (int i = 1; i < fastaSplit.size(); ++i) {
		SummaryData sdTemp = futSumData[i].get();
		sumData += sdTemp;
	}

	if (ctrlc_pressed) { return; }

	optimizeFromSummaryData(sumData);

	return;
}
/**************************************************************************************/
void ScreenSeqsCommand::optimizeFromSummaryData(const SummaryData & sumData) {
	for (auto opt : optimize) {
		if (opt == "start") { startPos = optimizeParameter("start", sumData.startPositionMap, criteria, sumData.numSeqs); }
		else if (opt == "end") { endPos = optimizeParameter("end", sumData.endPositionMap, 100.0 - criteria, sumData.numSeqs); }
		else if (opt == "maxambig") { maxAmbig = optimizeParameter("maxambig", sumData.ambigBasesMap, criteria, sumData.numSeqs); }
		else if (opt == "maxhomop") { maxHomoP = optimizeParameter("maxhomop", sumData.longHomoPolymerMap, criteria, sumData.numSeqs); }
		else if (opt == "minlength") { minLength = optimizeParameter("end", sumData.seqLengthMap, 100.0 - criteria, sumData.numSeqs); }
		else if (opt == "maxlength") { maxLength = optimizeParameter("end", sumData.endPositionMap, criteria, sumData.numSeqs); }
		else if (opt == "maxn") { maxLength = optimizeParameter("maxn", sumData.endPositionMap, criteria, sumData.numSeqs); }
	}
}
/**************************************************************************************/
void ScreenSeqsCommand::optimizeFromSummaryFile(std::string summaryfile) {
	SummaryData sumData;
	std::vector<SummaryFileRead> summarySplit = SummaryFileRead(summaryfile).split(processors);

	std::vector<std::future<SummaryData>> futSumData(summarySplit.size());
	for (int i = 1; i < summarySplit.size(); ++i) {
		futSumData[i] = std::async(std::launch::async, &ScreenSeqsCommand::driverReadSummary, this, summarySplit[i]);
	}
	sumData = driverReadSummary(summarySplit[0]);

	for (int i = 1; i < summarySplit.size(); ++i) {
		SummaryData sdTemp = futSumData[i].get();
		sumData += sdTemp;
	}

	if (ctrlc_pressed) { return; }

	optimizeFromSummaryData(sumData);
}

SummaryData ScreenSeqsCommand::driverReadSummary(SummaryFileRead & summary) {
	long long lineNum = 1;
	SummaryData sumData;
	while (!summary.reachedEnd() & !ctrlc_pressed) {
		if (++lineNum % 1000 == 0) {
			LOG(SCREENONLY) << lineNum - 1;
		}
		//seqname	start	end	nbases	ambigs	polymer	numSeqs
		NamedSequenceSummary seqSum = summary.readSequenceSummary();
		seqSum.numSeqs = nwt.getTotal(seqSum.name);
		if (seqSum.numSeqs == 0) {
			ctrlc_pressed = true;
		}
		else {
			sumData += seqSum;
		}
	}

	return sumData;
}

SummaryData ScreenSeqsCommand::driverCreateSummary(FastaFileRead & fasta) {
	long long lineNum = 1;
	SummaryData sumData;
	while (!fasta.reachedEnd() && !ctrlc_pressed) {
		if (++lineNum % 1000 == 0) {
			LOG(SCREENONLY) << lineNum - 1;
		}
		Sequence seq = fasta.readSequence();
		SequenceSummary seqSum = seq.getSequenceSummary();
		seqSum.numSeqs = nwt.getTotal(seq.getName());
		if (seqSum.numSeqs == 0) {
			ctrlc_pressed = true;
		}
		else {
			sumData += seqSum;
		}
	}
	return sumData;
}

//***************************************************************************************************************

int ScreenSeqsCommand::screenGroupFile(map<string, string> badSeqNames) {
	ifstream inputGroups;
	File::openInputFile(groupfile, inputGroups);
	string seqName, group;
	map<string, string>::iterator it;
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(groupfile));
	variables["[extension]"] = m->getExtension(groupfile);
	string goodGroupFile = getOutputFileName("group", variables);
	outputNames.push_back(goodGroupFile);  outputTypes["group"].push_back(goodGroupFile);
	ofstream goodGroupOut;	File::openOutputFile(goodGroupFile, goodGroupOut);

	while (!inputGroups.eof()) {
		if (ctrlc_pressed) { goodGroupOut.close(); inputGroups.close(); File::remove(goodGroupFile); return 0; }

		inputGroups >> seqName; File::gobble(inputGroups); inputGroups >> group; File::gobble(inputGroups);
		it = badSeqNames.find(seqName);

		if (it != badSeqNames.end()) {
			badSeqNames.erase(it);
		}
		else {
			goodGroupOut << seqName << '\t' << group << endl;
		}
	}

	if (ctrlc_pressed) { goodGroupOut.close();  inputGroups.close(); File::remove(goodGroupFile);  return 0; }

	//we were unable to remove some of the bad sequences
	if (badSeqNames.size() != 0) {
		for (it = badSeqNames.begin(); it != badSeqNames.end(); it++) {
			LOG(INFO) << "Your groupfile does not include the sequence " + it->first + " please correct.";
			LOG(INFO) << "";
		}
	}

	inputGroups.close();
	goodGroupOut.close();

	if (ctrlc_pressed) { File::remove(goodGroupFile); }

	return 0;

}
//***************************************************************************************************************
int ScreenSeqsCommand::screenCountFile(map<string, string> badSeqNames) {
	ifstream in;
	File::openInputFile(countfile, in);
	map<string, string>::iterator it;
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(countfile));
	variables["[extension]"] = m->getExtension(countfile);
	string goodCountFile = getOutputFileName("count", variables);

	outputNames.push_back(goodCountFile);  outputTypes["count"].push_back(goodCountFile);
	ofstream goodCountOut;	File::openOutputFile(goodCountFile, goodCountOut);

	string headers = File::getline(in); File::gobble(in);
	goodCountOut << headers << endl;
	string test = headers; vector<string> pieces = m->splitWhiteSpace(test);

	string name, rest; int thisTotal; rest = "";
	while (!in.eof()) {

		if (ctrlc_pressed) { goodCountOut.close(); in.close(); File::remove(goodCountFile); return 0; }

		in >> name; File::gobble(in);
		in >> thisTotal;
		if (pieces.size() > 2) { rest = File::getline(in); File::gobble(in); }

		it = badSeqNames.find(name);

		if (it != badSeqNames.end()) {
			badSeqNames.erase(it);
		}
		else {
			goodCountOut << name << '\t' << thisTotal << '\t' << rest << endl;
		}
	}
	if (ctrlc_pressed) { goodCountOut.close();  in.close(); File::remove(goodCountFile);  return 0; }

	//we were unable to remove some of the bad sequences
	if (badSeqNames.size() != 0) {
		for (it = badSeqNames.begin(); it != badSeqNames.end(); it++) {
			LOG(INFO) << "Your count file does not include the sequence " + it->first + " please correct.";
			LOG(INFO) << "";
		}
	}

	in.close();
	goodCountOut.close();

	//check for groups that have been eliminated
	CountTable ct;
	if (ct.testGroups(goodCountFile)) {
		ct.readTable(goodCountFile, true, false);
		ct.printTable(goodCountFile);
	}

	if (ctrlc_pressed) { File::remove(goodCountFile); }

	return 0;

}
//***************************************************************************************************************

int ScreenSeqsCommand::screenTaxonomy(map<string, string> badSeqNames) {
	ifstream input;
	File::openInputFile(taxonomy, input);
	string seqName, tax;
	map<string, string>::iterator it;
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(taxonomy));
	variables["[extension]"] = m->getExtension(taxonomy);
	string goodTaxFile = getOutputFileName("taxonomy", variables);

	outputNames.push_back(goodTaxFile);  outputTypes["taxonomy"].push_back(goodTaxFile);
	ofstream goodTaxOut;	File::openOutputFile(goodTaxFile, goodTaxOut);

	while (!input.eof()) {
		if (ctrlc_pressed) { goodTaxOut.close(); input.close(); File::remove(goodTaxFile); return 0; }

		input >> seqName; File::gobble(input); input >> tax;
		it = badSeqNames.find(seqName);

		if (it != badSeqNames.end()) { badSeqNames.erase(it); }
		else {
			goodTaxOut << seqName << '\t' << tax << endl;
		}
		File::gobble(input);
	}

	if (ctrlc_pressed) { goodTaxOut.close(); input.close(); File::remove(goodTaxFile); return 0; }

	//we were unable to remove some of the bad sequences
	if (badSeqNames.size() != 0) {
		for (it = badSeqNames.begin(); it != badSeqNames.end(); it++) {
			LOG(INFO) << "Your taxonomy file does not include the sequence " + it->first + " please correct.";
			LOG(INFO) << "";
		}
	}

	input.close();
	goodTaxOut.close();

	if (ctrlc_pressed) { File::remove(goodTaxFile);  return 0; }

	return 0;


}
//***************************************************************************************************************

int ScreenSeqsCommand::screenQual(map<string, string> badSeqNames) {
	ifstream in;
	File::openInputFile(qualfile, in);
	map<string, string>::iterator it;
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(qualfile));
	variables["[extension]"] = m->getExtension(qualfile);
	string goodQualFile = getOutputFileName("qfile", variables);

	outputNames.push_back(goodQualFile);  outputTypes["qfile"].push_back(goodQualFile);
	ofstream goodQual;	File::openOutputFile(goodQualFile, goodQual);

	while (!in.eof()) {

		if (ctrlc_pressed) { goodQual.close(); in.close(); File::remove(goodQualFile); return 0; }

		string saveName = "";
		string name = "";
		string scores = "";

		in >> name;

		if (name.length() != 0) {
			saveName = name.substr(1);
			while (!in.eof()) {
				char c = in.get();
				if (c == 10 || c == 13 || c == -1) { break; }
				else { name += c; }
			}
			File::gobble(in);
		}

		while (in) {
			char letter = in.get();
			if (letter == '>') { in.putback(letter);	break; }
			else { scores += letter; }
		}

		File::gobble(in);

		it = badSeqNames.find(saveName);

		if (it != badSeqNames.end()) {
			badSeqNames.erase(it);
		}
		else {
			goodQual << name << endl << scores;
		}

		File::gobble(in);
	}

	in.close();
	goodQual.close();

	//we were unable to remove some of the bad sequences
	if (badSeqNames.size() != 0) {
		for (it = badSeqNames.begin(); it != badSeqNames.end(); it++) {
			LOG(INFO) << "Your qual file does not include the sequence " + it->first + " please correct.";
			LOG(INFO) << "";
		}
	}

	if (ctrlc_pressed) { File::remove(goodQualFile);  return 0; }

	return 0;


}
/**************************************************************************************************/

int ScreenSeqsCommand::createProcesses(string goodFileName, string badAccnos, string filename, map<string, string>& badSeqNames) {

	int num = 0;

	vector<thread> thrds(lines.size() - 1);
	vector<sumScreenData> ssDataItems(lines.size() - 1);

	//loop through and create all the processes you want
	for (int i = 1; i < lines.size(); i++) {
		thrds[i - 1] = thread(&ScreenSeqsCommand::driverWithCount, this, lines[i], goodFileName + toString(i) + ".temp", badAccnos + toString(i) + ".temp", filename, ref(ssDataItems[i - 1]));
	}

	num = driver(lines[0], goodFileName, badAccnos, filename, badSeqNames);

	//force parent to wait until all the processes are done
	for (int i = 0;i < thrds.size();i++) {
		thrds[i].join();
		num += ssDataItems[i].numSeqs;
		badSeqNames.insert(ssDataItems[i].badSeqs.begin(), ssDataItems[i].badSeqs.end());
		File::appendFiles((goodFileName + toString(i + 1) + ".temp"), goodFileName);
		File::remove(goodFileName + toString(i + 1) + ".temp");
		File::appendFiles((badAccnos + toString(i + 1) + ".temp"), badAccnos);
		File::remove(badAccnos + toString(i + 1) + ".temp");
	}
	return num;

}

//***************************************************************************************************************
