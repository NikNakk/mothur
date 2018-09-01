#pragma once

/*
 *  screenseqscommand.h
 *  Mothur
 *
 *  Created by Pat Schloss on 6/3/09.
 *  Copyright 2009 Patrick D. Schloss. All rights reserved.
 *
 */
#include "mothur.h"
#include "command.hpp"
#include "datastructures/sequence.h"
#include "datastructures/namemap.h"
#include "commands/seqsummarycommand.h"
#include "filehandling/fastafileread.h"
#include <unordered_set>

 /**************************************************************************************************/
 //custom data structure for threads to use.
struct contigsSumData {
	vector<int> oLength;
	vector<int> ostartPosition;
	vector<int> oendPosition;
	vector<int> omismatches;
	vector<int> numNs;
	int numSeqs;

	contigsSumData() :
		numSeqs(0)
	{}
};

struct alignsData {
	vector<float> sims;
	vector<float> scores;
	vector<int> inserts;
	int numSeqs;

	alignsData() :
		numSeqs(0)
	{}
};

struct sumScreenData {
	map<string, string> badSeqs;
	int numSeqs;
};

class ScreenSeqsCommand : public Command {

public:
	ScreenSeqsCommand(Settings& settings, ParameterListToProcess ptp);
	ScreenSeqsCommand(Settings& settings) : Command(settings) {}
	~ScreenSeqsCommand() = default;

	virtual string getCommandName() const override { return "screen.seqs"; }
	virtual string getCommandCategory() const override { return "Sequence Processing"; }

	virtual void setParameters() override;
	virtual void setOutputTypes() override;

	virtual string getHelpString() const override;
	virtual string getDescription() const override { return "enables you to keep sequences that fulfill certain user defined criteria"; }

	virtual int execute() override;


private:
	vector<linePair> lines;

	int screenNameGroupFile(map<string, string>);
	int screenGroupFile(map<string, string>);
	int screenCountFile(map<string, string>);
	int screenAlignReport(map<string, string>&);
	int screenQual(map<string, string>);
	int screenTaxonomy(map<string, string>);

	int optimizeContigs();
	int optimizeAlign();
	int createProcesses(string, string, string, map<string, string>&);
	int screenSummary(map<string, string>&);
	int screenContigs(map<string, string>&);
	int screenFasta(map<string, string>&);
	int runFastaScreening(std::vector<FastaFileRead> fastaSplit, std::map<std::string, std::string> & badSeqNames);
	sumScreenData driver(FastaFileRead & fasta, string goodFName, string badAccnosFName, const std::map<std::string, std::string>& badSeqNames);
	int optimizeParameter(const std::string & parameterName, const std::map<int, long long>& mapData, double centile, long long numSeqs);
	void optimizeFromFasta(std::vector<FastaFileRead> fastaSplit);
	void optimizeFromSummaryData(const SummaryData & sumData);
	void optimizeFromSummaryFile(std::string summaryfile);
	SummaryData driverReadSummary(SummaryFileRead & summary);
	int createProcessesCreateSummary(vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<int>&, string);
	SummaryData driverCreateSummary(FastaFileRead & fasta);
	int driverContigsSummary(vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<int>&, linePair);
	void driverContigsSummaryWithData(contigsSumData & csData, linePair filePos);
	int createProcessesContigsSummary(vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<linePair>);
	int driverAlignSummary(vector<float>&, vector<float>&, vector<int>&, linePair);
	void driverAlignSummaryWithData(alignsData& aData, linePair filePos);
	int createProcessesAlignSummary(vector<float>&, vector<float>&, vector<int>&, vector<linePair>);

	bool abort;
	string fastafile, namefile, groupfile, alignreport, outputDir, qualfile, taxonomy, countfile, contigsreport, summaryfile;
	int startPos, endPos, maxAmbig, maxHomoP, minLength, maxLength, processors, minOverlap, oStart, oEnd, mismatches, maxN, maxInsert;
	double minSim, minScore, criteria;
	vector<string> outputNames;
	vector<string> optimize;
	NamesWithTotals nwt;
};

