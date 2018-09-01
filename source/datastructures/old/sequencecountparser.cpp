//
//  sequencecountparser.cpp
//  Mothur
//
//  Created by Sarah Westcott on 8/7/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "sequencecountparser.h"

/************************************************************/
SequenceCountParser::SequenceCountParser(string countfile, string fastafile) {

	//read count file
	CountTable countTable;
	countTable.readTable(countfile, true, false);

	//initialize maps
	namesOfGroups = countTable.getNamesOfGroups();
	for (int i = 0; i < namesOfGroups.size(); i++) {
		vector<Sequence> temp;
		map<string, int> tempMap;
		seqs[namesOfGroups[i]] = temp;
		countTablePerGroup[namesOfGroups[i]] = tempMap;
	}

	//read fasta file making sure each sequence is in the group file
	ifstream in;
	File::openInputFile(fastafile, in);

	int fastaCount = 0;
	while (!in.eof()) {

		if (ctrlc_pressed) { break; }

		Sequence seq(in); File::gobble(in);
		fastaCount++;
		if (app.isDebug) { if ((fastaCount) % 1000 == 0) { LOG(DEBUG) << "reading seq " + toString(fastaCount) + "\n."; } }

		if (seq.getName() != "") {

			allSeqsMap[seq.getName()] = seq.getName();
			vector<int> groupCounts = countTable.getGroupCounts(seq.getName());

			for (int i = 0; i < namesOfGroups.size(); i++) {
				if (groupCounts[i] != 0) {
					seqs[namesOfGroups[i]].push_back(seq);
					countTablePerGroup[namesOfGroups[i]][seq.getName()] = groupCounts[i];
				}
			}
		}
	}
	in.close();
}
/************************************************************/
SequenceCountParser::SequenceCountParser(string fastafile, CountTable& countTable) {

	//initialize maps
	if (countTable.hasGroupInfo()) {
		namesOfGroups = countTable.getNamesOfGroups();
		for (int i = 0; i < namesOfGroups.size(); i++) {
			vector<Sequence> temp;
			map<string, int> tempMap;
			seqs[namesOfGroups[i]] = temp;
			countTablePerGroup[namesOfGroups[i]] = tempMap;
		}

		//read fasta file making sure each sequence is in the group file
		ifstream in;
		File::openInputFile(fastafile, in);

		int fastaCount = 0;
		while (!in.eof()) {

			if (ctrlc_pressed) { break; }

			Sequence seq(in); File::gobble(in);
			fastaCount++;
			if (app.isDebug) { if ((fastaCount) % 1000 == 0) { LOG(DEBUG) << "reading seq " + toString(fastaCount) + "\n."; } }

			if (seq.getName() != "") {

				allSeqsMap[seq.getName()] = seq.getName();
				vector<int> groupCounts = countTable.getGroupCounts(seq.getName());

				for (int i = 0; i < namesOfGroups.size(); i++) {
					if (groupCounts[i] != 0) {
						seqs[namesOfGroups[i]].push_back(seq);
						countTablePerGroup[namesOfGroups[i]][seq.getName()] = groupCounts[i];
					}
				}
			}
		}
		in.close();
	}
	else { ctrlc_pressed = true;  LOG(LOGERROR) << "cannot parse fasta file by group with a count table that does not include group data, please correct.\n"; }

}
/************************************************************/
SequenceCountParser::~SequenceCountParser() {  }
/************************************************************/
int SequenceCountParser::getNumGroups() { return namesOfGroups.size(); }
/************************************************************/
vector<string> SequenceCountParser::getNamesOfGroups() { return namesOfGroups; }
/************************************************************/
int SequenceCountParser::getSeqs(string g, string filename, bool uchimeFormat = false) {
	try {
		map<string, vector<Sequence> >::iterator it;
		vector<Sequence> seqForThisGroup;
		vector<seqPriorityNode> nameVector;

		it = seqs.find(g);
		if (it == seqs.end()) {
			LOG(LOGERROR) << "No sequences available for group " + g + ", please correct." << '\n';
		}
		else {

			ofstream out;
			File::openOutputFile(filename, out);

			seqForThisGroup = it->second;

			if (uchimeFormat) {
				// format should look like 
				//>seqName /ab=numRedundantSeqs/
				//sequence

				map<string, int> countForThisGroup = getCountTable(g);
				map<string, int>::iterator itCount;
				int error = 0;

				for (int i = 0; i < seqForThisGroup.size(); i++) {
					itCount = countForThisGroup.find(seqForThisGroup[i].getName());

					if (itCount == countForThisGroup.end()) {
						error = 1;
						LOG(LOGERROR) << "" + seqForThisGroup[i].getName() + " is in your fastafile, but is not in your count file, please correct." << '\n';
					}
					else {
						seqPriorityNode temp(itCount->second, seqForThisGroup[i].getAligned(), seqForThisGroup[i].getName());
						nameVector.push_back(temp);
					}
				}

				if (error == 1) { out.close(); File::remove(filename); return 1; }

				//sort by num represented
				sort(nameVector.begin(), nameVector.end(), compareSeqPriorityNodes);

				//print new file in order of
				for (int i = 0; i < nameVector.size(); i++) {

					if (ctrlc_pressed) { out.close(); File::remove(filename); return 1; }

					out << ">" << nameVector[i].name << "/ab=" << nameVector[i].numIdentical << "/" << endl << nameVector[i].seq << endl; //
				}

			}
			else {
				//LOG(INFO) << "Group " + g +  " contains " + toString(seqForThisGroup.size()) + " unique seqs.\n";
				for (int i = 0; i < seqForThisGroup.size(); i++) {

					if (ctrlc_pressed) { out.close(); File::remove(filename); return 1; }

					seqForThisGroup[i].printSequence(out);
				}
			}
			out.close();
		}

		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SequenceCountParser, getSeqs";
		exit(1);
	}
}

/************************************************************/
map<string, int> SequenceCountParser::getCountTable(string g) {
	try {
		map<string, map<string, int> >::iterator it;
		map<string, int> countForThisGroup;

		it = countTablePerGroup.find(g);
		if (it == countTablePerGroup.end()) {
			LOG(LOGERROR) << "No countTable available for group " + g + ", please correct." << '\n';
		}
		else {
			countForThisGroup = it->second;
			if (app.isDebug) { LOG(DEBUG) << "group " + g + " count file has " + toString(countForThisGroup.size()) + " unique sequences."; }
		}

		return countForThisGroup;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SequenceCountParser, getCountTable";
		exit(1);
	}
}
/************************************************************/
int SequenceCountParser::getCountTable(string g, string filename) {
	try {
		map<string, map<string, int> >::iterator it;
		map<string, int> countForThisGroup;

		it = countTablePerGroup.find(g);
		if (it == countTablePerGroup.end()) {
			LOG(LOGERROR) << "No countTable available for group " + g + ", please correct." << '\n';
		}
		else {
			countForThisGroup = it->second;

			ofstream out;
			File::openOutputFile(filename, out);
			out << "Representative_Sequence\ttotal\t" << g << endl;

			for (map<string, int>::iterator itFile = countForThisGroup.begin(); itFile != countForThisGroup.end(); itFile++) {

				if (ctrlc_pressed) { out.close(); File::remove(filename); return 1; }

				out << itFile->first << '\t' << itFile->second << '\t' << itFile->second << endl;
			}

			out.close();
		}

		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SequenceParser, getCountTable";
		exit(1);
	}
}
/************************************************************/



