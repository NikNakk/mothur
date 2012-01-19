#ifndef PARSEFASTAQCOMMAND_H
#define PARSEFASTAQCOMMAND_H

/*
 *  parsefastaqcommand.h
 *  Mothur
 *
 *  Created by westcott on 9/30/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */


#include "command.hpp"

class ParseFastaQCommand : public Command {

public:
	ParseFastaQCommand(string);
	ParseFastaQCommand();
	~ParseFastaQCommand() {}
	
	vector<string> setParameters();
	string getCommandName()			{ return "parse.fastq";		}
	string getCommandCategory()		{ return "Sequence Processing"; }
	string getHelpString();	
	string getCitation() { return "http://www.mothur.org/wiki/Parse.fastq"; }
	string getDescription()		{ return "reads a fastq file and creates a fasta and quality file"; }

	
	int execute(); 
	void help() { m->mothurOut(getHelpString()); }		
private:

	vector<string> outputNames;	
	string outputDir, fastaQFile;
	bool abort, fasta, qual;
	
	vector<int> convertQual(string);
};

#endif


