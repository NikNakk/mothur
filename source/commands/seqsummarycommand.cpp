/*
 *  seqcoordcommand.cpp
 *  Mothur
 *
 *  Created by Pat Schloss on 5/30/09.
 *  Copyright 2009 Patrick D. Schloss. All rights reserved.
 *
 */

#include <future>
#include <array>
#include "filehandling/summaryfilewrite.h"
#include "seqsummarycommand.h"
#include "counttable.h"
#include "commandparameters/fastafileparameter.h"
#include "commandparameters/namefileparameter.h"
#include "commandparameters/countfileparameter.h"
#include "commandparameters/processorsparameter.h"
#include "datastructures/namemap.h"
#include "datastructures/counttable.h"

 //**********************************************************************************************************************
void SeqSummaryCommand::setParameters() {
	parameters.add(new FastaFileParameter(fastafile, settings, "fasta", true, true));
	parameters.add(new NameFileParameter(namefile, settings, "name", false, true, "namecount"));
	parameters.add(new CountFileParameter(countfile, settings, "count", false, true, "namecount"));
	parameters.add(new ProcessorsParameter(processors, settings));
	parameters.addStandardParameters(inputDir, outputDir);
}
void SeqSummaryCommand::setOutputTypes()
{
	outputTypes.add("summary", "[filename], summary");
}
//**********************************************************************************************************************
string SeqSummaryCommand::getHelpString() const {
	return R"(The summary.seqs command reads a fastafile and summarizes the sequences.
The summary.seqs command parameters are fasta, name, count and processors, fasta is required, unless you have a valid current fasta file.
The name parameter allows you to enter a name file associated with your fasta file. 
The count parameter allows you to enter a count file associated with your fasta file. 
The summary.seqs command should be in the following format: 
summary.seqs(fasta=yourFastaFile, processors=2) 
Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).)";
}
//**********************************************************************************************************************

SeqSummaryCommand::SeqSummaryCommand(Settings& settings, ParameterListToProcess ptp) : Command(settings, ptp) {
	//if (countfile == "") {
	//	if (namefile == "") {
	//		vector<string> files; files.push_back(fastafile);
	//		NameFileParameter::getNameFile(settings, files);
	//	}
	//}
}
//***************************************************************************************************************

int SeqSummaryCommand::execute() {
	time_t start_time = time(nullptr);

	std::map<string, string> variables;
	variables["[filename]"] = outputDir + File::getSimpleRootName(fastafile);
	std::string summaryFileName(getOutputFileName("summary", variables));

	if (namefile != "") { 
		nwt = NamesWithTotals(namefile);
	}
	else if (countfile != "") {
		nwt = CountTable(countfile).getNamesWithTotals();
	}

	if (ctrlc_pressed) { return 0; }

	std::vector<FastaFileRead> fastaSplit = FastaFileRead(fastafile).split(processors);

	if (ctrlc_pressed) { return 0; }

	std::vector<std::future<SummaryData>> futSumData(fastaSplit.size());
	std::vector<std::string> sumFileNames(fastaSplit.size());
	for (int i = 1; i < fastaSplit.size(); i++) {
		sumFileNames[i] = summaryFileName + std::to_string(i) + ".temp";
		futSumData[i] = std::async(std::launch::async, &SeqSummaryCommand::createSummary, this, std::ref(fastaSplit[i]), std::ref(sumFileNames[i]));
	}
	SummaryData sumData = createSummary(fastaSplit[0], summaryFileName);
	for (int i = 1; i < fastaSplit.size(); i++) {
		SummaryData sdTemp = futSumData[i].get();
		sumData += sdTemp;
		File::appendFilesAndRemove(sumFileNames[i], summaryFileName);
	}

	const vector<double> centiles = { 0, 0.025, 0.25, 0.5, 0.75, 0.975, 1.0 };

	MeanWithCentiles<int, long long> starts = Utility::findCentiles(sumData.startPositionMap, centiles, sumData.numSeqs);
	MeanWithCentiles<int, long long> ends = Utility::findCentiles(sumData.endPositionMap, centiles, sumData.numSeqs);
	MeanWithCentiles<int, long long> lengths = Utility::findCentiles(sumData.seqLengthMap, centiles, sumData.numSeqs);
	MeanWithCentiles<int, long long> ambigs = Utility::findCentiles(sumData.ambigBasesMap, centiles, sumData.numSeqs);
	MeanWithCentiles<int, long long> longHPs = Utility::findCentiles(sumData.longHomoPolymerMap, centiles, sumData.numSeqs);

	if (ctrlc_pressed) { return 0; }

	LOG(INFO)<<"\n          \tStart\tEnd\tNBases\tAmbigs\tPolymer\tNumSeqs";
	LOGF(INFO, "Minimum:    \t%d\t%d\t%d\t%d\t%d\t%d", starts.centileValues[0], ends.centileValues[0], lengths.centileValues[0], ambigs.centileValues[0], longHPs.centileValues[0], 1);
	LOGF(INFO, "2.5%%-tile: \t%d\t%d\t%d\t%d\t%d\t%d", starts.centileValues[1], ends.centileValues[1], lengths.centileValues[1], ambigs.centileValues[1], longHPs.centileValues[1], starts.centilePositions[1]);
	LOGF(INFO, "25%%-tile:  \t%d\t%d\t%d\t%d\t%d\t%d", starts.centileValues[2], ends.centileValues[2], lengths.centileValues[2], ambigs.centileValues[2], longHPs.centileValues[2], starts.centilePositions[2]);
	LOGF(INFO, "Median:     \t%d\t%d\t%d\t%d\t%d\t%d", starts.centileValues[3], ends.centileValues[3], lengths.centileValues[3], ambigs.centileValues[3], longHPs.centileValues[3], starts.centilePositions[3]);
	LOGF(INFO, "75%%-tile:  \t%d\t%d\t%d\t%d\t%d\t%d", starts.centileValues[4], ends.centileValues[4], lengths.centileValues[4], ambigs.centileValues[4], longHPs.centileValues[4], starts.centilePositions[4]);
	LOGF(INFO, "97.5%%-tile:\t%d\t%d\t%d\t%d\t%d\t%d", starts.centileValues[5], ends.centileValues[5], lengths.centileValues[5], ambigs.centileValues[5], longHPs.centileValues[5], starts.centilePositions[5]);
	LOGF(INFO, "Maximum:    \t%d\t%d\t%d\t%d\t%d\t%d", starts.centileValues[6], ends.centileValues[6], lengths.centileValues[6], ambigs.centileValues[6], longHPs.centileValues[6], sumData.numSeqs);
	LOGF(INFO, "Mean:       \t%0.2f\t%0.2f\t%0.2f\t%0.2f\t%0.2f", starts.mean, ends.mean, lengths.mean, ambigs.mean, longHPs.mean);
	if ((namefile == "") && (countfile == "")) {
		LOGF(INFO, "# of Seqs:\t%d", sumData.numSeqs);
	}
	else {
		LOGF(INFO, "# of unique seqs:\t%d\ntotal # of seqs:\t", sumData.uniqueSeqs, sumData.numSeqs);
	}

	if (ctrlc_pressed) { return 0; }

	LOG(INFO) << '\n' << "Output File Names: ";
	LOG(INFO) << summaryFileName << '\n';

	LOG(INFO) << "It took " << time(nullptr) - start_time << " secs to summarize " << sumData.numSeqs << " sequences.";

	//set summary file as new current summary file
	settings.setCurrent("summary", summaryFileName);
	return 0;
}
/**************************************************************************************/

SummaryData SeqSummaryCommand::createSummary(FastaFileRead& fasta, const std::string & sumFileName) {
	SummaryData sumData;
	SummaryFileWrite sumFile(sumFileName);
	if (!sumFile.isValid()) {
		throw(InvalidFile("Error writing " + sumFileName));
	}
	if (fasta.getStart() == 0) {
		sumFile.writeHeader();
	}
	long long lineNum = 1;
	while (!fasta.reachedEnd() && !ctrlc_pressed) {
		if (lineNum++ % 1000 == 0) {
			LOG(SCREENONLY) << lineNum - 1;
		}
		Sequence seq = fasta.readSequence();
		SequenceSummary seqSum = seq.getSequenceSummary();
		seqSum.numSeqs = nwt.getTotal(seq.getName());

		sumData += seqSum;

		sumFile.writeSequenceSummary(seq, seqSum);
	}
	if (!ctrlc_pressed) {
		sumFile.commit();
	}
	return sumData;
}

SummaryData & SummaryData::operator+=(const SummaryData & sd)
{
	Utility::addToMap(startPositionMap, sd.startPositionMap);
	Utility::addToMap(endPositionMap, sd.endPositionMap);
	Utility::addToMap(seqLengthMap, sd.seqLengthMap);
	Utility::addToMap(ambigBasesMap, sd.ambigBasesMap);
	Utility::addToMap(longHomoPolymerMap, sd.longHomoPolymerMap);
	Utility::addToMap(nMap, sd.nMap);
	uniqueSeqs += sd.uniqueSeqs;
	numSeqs += sd.numSeqs;
	return *this;
}

SummaryData & SummaryData::operator+=(const SequenceSummary & seqSum) {
	long long count = seqSum.numSeqs;
	startPositionMap[seqSum.startPosition] += count;
	endPositionMap[seqSum.endPosition] += count;
	seqLengthMap[seqSum.seqLength] += count;
	ambigBasesMap[seqSum.ambigBases] += count;
	longHomoPolymerMap[seqSum.longHomoPolymer] += count;
	numSeqs += count;
	++uniqueSeqs;
	return *this;
}