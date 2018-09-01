#pragma once

/*
 *  degapseqscommand.h
 *  Mothur
 *
 *  Created by westcott on 6/21/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */


#include "command.hpp"
#include "sequence.hpp"


class DegapSeqsCommand : public Command {
public:
	DegapSeqsCommand(Settings& settings, string option);
	DegapSeqsCommand(Settings& settings);
	~DegapSeqsCommand() {}

	vector<string> setParameters();
	string getCommandName() { return "degap.seqs"; }
	string getCommandCategory() { return "Sequence Processing"; }

	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Degap.seqs"; }
	string getDescription() { return "removes gap characters from sequences"; }

	int execute();
	void help() { LOG(INFO) << getHelpString(); }

private:

	int processors;
	bool abort;
	string fastafile, outputDir;
	vector<string> outputNames;
	vector<string> fastaFileNames;

	int driver(linePair, string, string);
	int createProcesses(string, string);

};
/**************************************************************************************************/
//custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
struct degapData {
	string filename;
	string outputFileName;
	unsigned long long start;
	unsigned long long end;
	int count;

	degapData() {}
	degapData(string f, string of, MothurOut* mout, unsigned long long st, unsigned long long en) {
		filename = f;
		outputFileName = of;
		m = mout;
		start = st;
		end = en;
	}
};

/**************************************************************************************************/
#if defined (UNIX)
#else
static DWORD WINAPI MyDegapThreadFunction(LPVOID lpParam) {
	degapData* pDataArray;
	pDataArray = (degapData*)lpParam;

	try {

		pDataArray->count = 0;

		ifstream inFASTA;
		File::openInputFile(pDataArray->filename, inFASTA);

		if ((pDataArray->start == 0) || (pDataArray->start == 1)) {
			inFASTA.seekg(0);
			pDataArray->m->zapGremlins(inFASTA);
		}
		else { //this accounts for the difference in line endings.
			inFASTA.seekg(pDataArray->start - 1); pDataArray->File::gobble(inFASTA);
		}

		ofstream outFASTA;
		File::openOutputFile(pDataArray->outputFileName, outFASTA);

		for (int i = 0; i < pDataArray->end; i++) { //end is the number of sequences to process

			if (ctrlc_pressed) { break; }

			Sequence currSeq(inFASTA);  pDataArray->File::gobble(inFASTA);
			if (currSeq.getName() != "") {
				outFASTA << ">" << currSeq.getName() << endl;
				outFASTA << currSeq.getUnaligned() << endl;
				pDataArray->count++;
			}

			//report progress
			if ((pDataArray->count) % 1000 == 0) { LOG(SCREENONLY) << toString(pDataArray->count) + "\n"; }

		}
		//report progress
		if ((pDataArray->count) % 1000 != 0) { LOG(SCREENONLY) << toString(pDataArray->count) + "\n"; }

		inFASTA.close();
		outFASTA.close();

		return pDataArray->count;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DegapSeqsCommand, MyDegapThreadFunction";
		exit(1);
	}
}
#endif






