#pragma once

/*
 *  seqcoordcommand.h
 *  Mothur
 *
 *  Created by Pat Schloss on 5/30/09.
 *  Copyright 2009 Patrick D. Schloss. All rights reserved.
 *
 */

#include "mothurdefs.h"
#include "command.hpp"
#include "sequence.h"
#include "filehandling/fastafileread.h"
#include "filehandling/summaryfilewrite.h"
#include <map>
#include "datastructures/namemap.h"

 /**************************************************************************************************/

struct SummaryData {
public:
	std::map<int, long long> startPositionMap;
	std::map<int, long long> endPositionMap;
	std::map<int, long long> seqLengthMap;
	std::map<int, long long> ambigBasesMap;
	std::map<int, long long> longHomoPolymerMap;
	std::map<int, long long> nMap;
	long long uniqueSeqs = 0;
	long long numSeqs = 0;
	SummaryData& operator+= (const SummaryData & sd);
	SummaryData& operator+= (const SequenceSummary & sd);
};

class SeqSummaryCommand : public Command {
public:
	SeqSummaryCommand(Settings& settings, ParameterListToProcess ptp);
	SeqSummaryCommand(Settings& settings) : Command(settings) {}
	~SeqSummaryCommand() = default;

	virtual void setParameters() override;
	virtual void setOutputTypes() override;
	virtual string getCommandName() const override { return "summary.seqs"; }
	virtual string getCommandCategory() const override { return "Sequence Processing"; }

	virtual string getHelpString() const override;
	virtual string getDescription() const override { return "summarize the quality of sequences in an unaligned or aligned fasta file"; }

	virtual int execute() override;
	SummaryData createSummary(FastaFileRead& fasta, const std::string & sumFileName);
private:
	int processors = 1;
	std::string	fastafile;
	std::string namefile;
	std::string countfile;
	NamesWithTotals nwt;
};