//
//  mergesfffilecommand.h
//  Mothur
//
//  Created by Sarah Westcott on 1/31/14.
//  Copyright (c) 2014 Schloss Lab. All rights reserved.
//

#pragma once

#include "command.hpp"


/**********************************************************/
class MergeSfffilesCommand : public Command {

public:
	MergeSfffilesCommand(Settings& settings, string option);
	MergeSfffilesCommand(Settings& settings);
	~MergeSfffilesCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "merge.sfffiles"; }
	string getCommandCategory() { return "Sequence Processing"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/merge.sfffiles"; }
	string getDescription() { return "merge individual sfffiles into a single .sff file"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:
	string sffFilename, outputDir, file, currentFileName;
	vector<string> filenames, outputNames;
	bool abort, keyTrim;
	int numTotalReads, allFilesnumFlowReads, allFileskeyLength;
	string outputFile, outputFileHeader;
	vector<CommonHeader> commonHeaders;

	//extract sff file functions
	int mergeSffInfo(string, ofstream&);
	int readCommonHeader(ifstream&, CommonHeader&);
	int readHeader(ifstream&, Header&);
	bool readSeqData(ifstream&, seqRead&, int, Header&, ofstream&);
	int decodeName(string&, string&, string&, string);

	bool sanityCheck(Header&, seqRead&);
	int adjustCommonHeader();
	int readFile();
	int printCommonHeaderForDebug(CommonHeader& header, ofstream& out, int numReads);
};

/**********************************************************/


