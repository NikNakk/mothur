#pragma once

/*
 *  listseqscommand.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 7/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "command.hpp"

class ListSeqsCommand : public Command {

public:

	ListSeqsCommand(Settings& settings, string option);
	ListSeqsCommand(Settings& settings);
	~ListSeqsCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "list.seqs"; }
	string getCommandCategory() { return "Sequence Processing"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/List.seqs"; }
	string getDescription() { return "lists sequences from a list, fasta, name, group, alignreport or taxonomy file"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }


private:
	vector<string> names, outputNames;
	string fastafile, namefile, groupfile, countfile, alignfile, inputFileName, outputDir, listfile, taxfile, fastqfile;
	bool abort;

	int readFasta();
	int readName();
	int readGroup();
	int readAlign();
	int readList();
	int readTax();
	int readCount();
	int readFastq();
};



