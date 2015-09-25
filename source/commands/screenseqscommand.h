#ifndef SCREENSEQSCOMMAND_H
#define SCREENSEQSCOMMAND_H

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
#include "sequence.hpp"

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

struct sumData {
	vector<int> startPosition;
	vector<int> endPosition;
	vector<int> seqLength;
	vector<int> ambigBases;
	vector<int> longHomoPolymer;
	vector<int> numNs;
	int numSeqs;

	sumData() :
		numSeqs(0)
	{}
};

class ScreenSeqsCommand : public Command {

public:
	ScreenSeqsCommand(string);
	ScreenSeqsCommand();
	~ScreenSeqsCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "screen.seqs"; }
	string getCommandCategory() { return "Sequence Processing"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Screen.seqs"; }
	string getDescription() { return "enables you to keep sequences that fulfill certain user defined criteria"; }

	int execute();
	void help() { m->mothurOut(getHelpString()); }


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
	int driver(linePair, string, string, string, map<string, string>&);
	void driverWithCount(linePair filePos, string goodFName, string badAccnosFName, string filename, sumScreenData& ssData);
	int createProcesses(string, string, string, map<string, string>&);
	int screenSummary(map<string, string>&);
	int screenContigs(map<string, string>&);
	int runFastaScreening(map<string, string>&);
	int screenFasta(map<string, string>&);
	int screenReports(map<string, string>&);
	int getSummary(vector<unsigned long long>&);
	int createProcessesCreateSummary(vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<int>&, string);
	int driverCreateSummary(vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<int>&, string, linePair);
	int getSummaryReport();
	int driverContigsSummary(vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<int>&, linePair);
	void driverContigsSummaryWithData(contigsSumData & csData, linePair filePos);
	int createProcessesContigsSummary(vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<int>&, vector<linePair>);
	void driverCreateSummaryWithData(sumData& sData, string filename, linePair filePos);
	int driverAlignSummary(vector<float>&, vector<float>&, vector<int>&, linePair);
	void driverAlignSummaryWithData(alignsData& aData, linePair filePos);
	int createProcessesAlignSummary(vector<float>&, vector<float>&, vector<int>&, vector<linePair>);

	bool abort;
	string fastafile, namefile, groupfile, alignreport, outputDir, qualfile, taxonomy, countfile, contigsreport, summaryfile;
	int startPos, endPos, maxAmbig, maxHomoP, minLength, maxLength, processors, minOverlap, oStart, oEnd, mismatches, maxN, maxInsert;
	float minSim, minScore, criteria;
	vector<string> outputNames;
	vector<string> optimize;
	map<string, int> nameMap;
};

#endif