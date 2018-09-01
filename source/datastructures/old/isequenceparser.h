#pragma once

#include <mothur.h>
#include <mothurout.h>
#include "sequence.hpp"

class ISequenceParser {
public:
	virtual ~ISequenceParser() {};
	virtual int getNumGroups() = 0;
	virtual vector<string> getNamesOfGroups() = 0;

	virtual int getNumSeqs(string);		//returns the number of unique sequences in a specific group
	virtual vector<Sequence> getSeqs(string); //returns unique sequences in a specific group
	virtual int getSeqs(string, string, bool) = 0; //prints unique sequences in a specific group to a file - group, filename, uchimeFormat=false

	virtual map<string, string> getAllSeqsMap();   //returns map where the key=sequenceName and the value=representativeSequence - helps us remove duplicates after group by group processing


protected:


	int numSeqs;
	map<string, string> allSeqsMap;
	map<string, vector<Sequence> > seqs; //a vector for each group
};

