#ifndef FILTERSEQSCOMMAND_H
#define FILTERSEQSCOMMAND_H

/*
 *  filterseqscommand.h
 *  Mothur
 *
 *  Created by Thomas Ryabin on 5/4/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "command.hpp"
#include "filters.h"


class Sequence;
class FilterSeqsCommand : public Command {

public:
	FilterSeqsCommand(string);
	FilterSeqsCommand();
	~FilterSeqsCommand() {};
	
	vector<string> setParameters();
	string getCommandName()			{ return "filter.seqs";			}
	string getCommandCategory()		{ return "Sequence Processing";	}
	
	string getHelpString();	
    string getOutputPattern(string);	
	string getCitation() { return "http://www.mothur.org/wiki/Filter.seqs"; }
	string getDescription()		{ return "removes columns from alignments based on a criteria defined by the user"; }
	
	int execute(); 
	void help() { m->mothurOut(getHelpString()); }	
	
private:

	vector<linePair*> lines;
	vector<int> processIDS;
    map<int, vector<unsigned long long> > savedPositions;

	string vertical, filter, fasta, hard, outputDir, filterFileName;
	vector<string> fastafileNames;	
	int alignmentLength, processors;
	vector<int> bufferSizes;
	vector<string> outputNames;

	char trump;
	bool abort, recalced;
	float soft;
	int numSeqs;
	
	string createFilter();
	int filterSequences();
	int createProcessesCreateFilter(Filters&, string);
	int createProcessesRunFilter(string, string, string);
	int driverRunFilter(string, string, string, linePair*);
	int driverCreateFilter(Filters& F, string filename, linePair* line);
	
};
#endif