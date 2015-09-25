#ifndef Mothur_sequencecountparser_h
#define Mothur_sequencecountparser_h

//
//  sequencecountparser.h
//  Mothur
//
//  Created by Sarah Westcott on 8/7/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "mothur.h"
#include "mothurout.h"
#include "sequence.hpp"
#include "counttable.h"
#include "isequenceparser.h"

/* This class reads a fasta and count file and parses the data by group. The countfile must contain group information.
 
 Note: The sum of all the groups unique sequences will be larger than the original number of unique sequences. 
 This is because when we parse the count file we make a unique for each group instead of 1 unique for all
 groups. 
 
 */

class SequenceCountParser : public ISequenceParser {
	
public:
	SequenceCountParser(string, string);			//count, fasta - file mismatches will set m->control_pressed = true
	SequenceCountParser(string, CountTable&);		//fasta, counttable - file mismatches will set m->control_pressed = true
	virtual ~SequenceCountParser();

	//general operations
	virtual int getNumGroups();
	virtual vector<string> getNamesOfGroups();

	virtual vector<Sequence> getSeqs(string g) { return ISequenceParser::getSeqs(g); }; //returns unique sequences in a specific group
	virtual int getSeqs(string, string, bool); //prints unique sequences in a specific group to a file - group, filename, uchimeFormat=false

    map<string, int> getCountTable(string); //returns seqName -> numberOfRedundantSeqs for a specific group - the count file format, but each line is parsed by group.
    int getCountTable(string, string); //print seqName -> numberRedundantSeqs for a specific group - group, filename

private:
    CountTable countTable;	
    map<string, map<string, int> > countTablePerGroup; //countTable for each group
    vector<string> namesOfGroups;
};



#endif
