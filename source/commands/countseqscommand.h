#ifndef COuNTSEQSCOMMAND_H
#define COuNTSEQSCOMMAND_H

/*
 *  countseqscommand.h
 *  Mothur
 *
 *  Created by westcott on 6/1/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "command.hpp"
#include "groupmap.h"
#include "sharedrabundvector.h"


class CountSeqsCommand : public Command {

public:

	CountSeqsCommand(Settings& settings, string option);
	CountSeqsCommand(Settings& settings);
	~CountSeqsCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "count.seqs"; }
	string getCommandCategory() { return "Sequence Processing"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Count.seqs"; }
	string getDescription() { return "makes a count file from a names or shared file"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }


private:

	string namefile, groupfile, outputDir, groups, sharedfile;
	bool abort, large, allLines;
	vector<string> Groups, outputNames;
	int processors;
	set<string> labels;

	unsigned long long processSmall(string);
	unsigned long long processLarge(string);
	map<int, string> processNameFile(string);
	map<int, string> getGroupNames(string, set<string>&);

	unsigned long long createProcesses(GroupMap&, string);
	unsigned long long driver(unsigned long long, unsigned long long, string, GroupMap&);
	void driverWithCount(unsigned long long start, unsigned long long end, string outputFileName, GroupMap& groupMap, unsigned long long& numSeqs);
	unsigned long long processShared(vector<SharedRAbundVector*>& lookup, map<string, string> variables);


};

/***********************************************************************/
struct countData {
	unsigned long long start;
	unsigned long long end;
	MothurOut* m;
	string outputFileName, namefile, groupfile;
	GroupMap* groupMap;
	int total;
	vector<string> Groups;

	countData() {}
	countData(string fn, GroupMap* g, MothurOut* mout, unsigned long long st, unsigned long long en, string gfn, string nfn, vector<string> gr) {
		m = mout;
		start = st;
		end = en;
		groupMap = g;
		groupfile = gfn;
		namefile = nfn;
		outputFileName = fn;
		Groups = gr;
		total = 0;
	}
};

#endif


