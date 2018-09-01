#pragma once

/*
 *  sequenceParser.h
 *  Mothur
 *
 *  Created by westcott on 9/9/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */


#include "mothur.h"
#include "sequence.hpp"
#include "groupmap.h"
#include "isequenceparser.h"

 /* This class reads a fasta and group file with a namesfile as optional and parses the data by group.

	 Note: The sum of all the groups unique sequences will be larger than the original number of unique sequences.
	 This is because when we parse the name file we make a unique for each group instead of 1 unique for all
	 groups.

  */

class SequenceParser : public ISequenceParser {

public:

	SequenceParser(string, string);			//group, fasta - file mismatches will set ctrlc_pressed = true
	SequenceParser(string, string, string);	//group, fasta, name  - file mismatches will set ctrlc_pressed = true

	//general operations
	vector<string> getNamesOfGroups();
	bool isValidGroup(string);  //return true if string is a valid group

	map<string, string> getNameMap(string); //returns seqName -> namesOfRedundantSeqs separated by commas for a specific group - the name file format, but each line is parsed by group.

	virtual vector<Sequence> getSeqs(string g) { return ISequenceParser::getSeqs(g); }; //returns unique sequences in a specific group
	virtual int getSeqs(string, string, bool); //prints unique sequences in a specific group to a file - group, filename, uchimeFormat=false
	int getNameMap(string, string); //print seqName -> namesOfRedundantSeqs separated by commas for a specific group - group, filename
	virtual int getNumGroups();

private:

	GroupMap groupMap;

	map<string, string> allSeqsMap;
	map<string, map<string, string> > nameMapPerGroup; //nameMap for each group
};



