/*
 *  sequenceParser.cpp
 *  Mothur
 *
 *  Created by westcott on 9/9/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "sequenceparser.h"


 /************************************************************/
SequenceParser::SequenceParser(string groupFile, string fastaFile, string nameFile) :
	groupMap(groupFile)
{
	try {
		int error;

		//read group file
		error = groupMap.readMap();

		if (error == 1) { ctrlc_pressed = true; }

		//initialize maps
		vector<string> namesOfGroups = groupMap.getNamesOfGroups();
		for (int i = 0; i < namesOfGroups.size(); i++) {
			vector<Sequence> temp;
			map<string, string> tempMap;
			seqs[namesOfGroups[i]] = temp;
			nameMapPerGroup[namesOfGroups[i]] = tempMap;
		}

		//read fasta file making sure each sequence is in the group file
		ifstream in;
		File::openInputFile(fastaFile, in);

		map<string, string> seqName; //stores name -> sequence string so we can make new "unique" sequences when we parse the name file
		int fastaCount = 0;
		while (!in.eof()) {

			if (ctrlc_pressed) { break; }

			Sequence seq(in); File::gobble(in);
			fastaCount++;
			if (app.isDebug) { if ((fastaCount) % 1000 == 0) { LOG(DEBUG) << "reading seq " + toString(fastaCount) + "\n."; } }

			if (seq.getName() != "") {

				string group = groupMap.getGroup(seq.getName());
				if (group == "not found") { error = 1; LOG(LOGERROR) << "" + seq.getName() + " is in your fasta file and not in your groupfile, please correct." << '\n'; }
				else {
					seqs[group].push_back(seq);
					seqName[seq.getName()] = seq.getAligned();
				}
			}
		}
		in.close();

		if (error == 1) { ctrlc_pressed = true; }

		//read name file
		ifstream inName;
		File::openInputFile(nameFile, inName);

		//string first, second;
		int countName = 0;
		set<string> thisnames1;

		string rest = "";
		char buffer[4096];
		bool pairDone = false;
		bool columnOne = true;
		string firstCol, secondCol;

		while (!inName.eof()) {
			if (ctrlc_pressed) { break; }

			inName.read(buffer, 4096);
			vector<string> pieces = m->splitWhiteSpace(rest, buffer, inName.gcount());

			for (int i = 0; i < pieces.size(); i++) {
				if (columnOne) { firstCol = pieces[i]; columnOne = false; }
				else { secondCol = pieces[i]; pairDone = true; columnOne = true; }

				if (pairDone) { //save one line
					if (app.isDebug) { LOG(DEBUG) << "reading names: " + firstCol + '\t' + secondCol + ".\n"; }
					vector<string> names;
					m->splitAtChar(secondCol, names, ',');

					//get aligned string for these seqs from the fasta file
					string alignedString = "";
					map<string, string>::iterator itAligned = seqName.find(names[0]);
					if (itAligned == seqName.end()) {
						error = 1; LOG(LOGERROR) << "" + names[0] + " is in your name file and not in your fasta file, please correct." << '\n';
					}
					else {
						alignedString = itAligned->second;
					}

					//separate by group - parse one line in name file
					map<string, string> splitMap; //group -> name1,name2,...
					map<string, string>::iterator it;
					for (int i = 0; i < names.size(); i++) {

						string group = groupMap.getGroup(names[i]);
						if (group == "not found") { error = 1; LOG(LOGERROR) << "" + names[i] + " is in your name file and not in your groupfile, please correct." << '\n'; }
						else {

							it = splitMap.find(group);
							if (it != splitMap.end()) { //adding seqs to this group
								(it->second) += "," + names[i];
								thisnames1.insert(names[i]);
								countName++;
							}
							else { //first sighting of this group
								splitMap[group] = names[i];
								countName++;
								thisnames1.insert(names[i]);

								//is this seq in the fasta file?
								if (i != 0) { //if not then we need to add a duplicate sequence to the seqs for this group so the new "fasta" and "name" files will match
									Sequence tempSeq(names[i], alignedString); //get the first guys sequence string since he's in the fasta file.
									seqs[group].push_back(tempSeq);
								}
							}
						}

						allSeqsMap[names[i]] = names[0];
					}


					//fill nameMapPerGroup - holds all lines in namefile separated by group
					for (it = splitMap.begin(); it != splitMap.end(); it++) {
						//grab first name
						string firstName = "";
						for (int i = 0; i < (it->second).length(); i++) {
							if (((it->second)[i]) != ',') {
								firstName += ((it->second)[i]);
							}
							else { break; }
						}

						//group1 -> seq1 -> seq1,seq2,seq3
						nameMapPerGroup[it->first][firstName] = it->second;
					}

					pairDone = false;
				}
			}
		}
		inName.close();

		//in case file does not end in white space
		if (rest != "") {
			vector<string> pieces = m->splitWhiteSpace(rest);

			for (int i = 0; i < pieces.size(); i++) {
				if (columnOne) { firstCol = pieces[i]; columnOne = false; }
				else { secondCol = pieces[i]; pairDone = true; columnOne = true; }

				if (pairDone) { //save one line
					if (app.isDebug) { LOG(DEBUG) << "reading names: " + firstCol + '\t' + secondCol + ".\n"; }
					vector<string> names;
					m->splitAtChar(secondCol, names, ',');

					//get aligned string for these seqs from the fasta file
					string alignedString = "";
					map<string, string>::iterator itAligned = seqName.find(names[0]);
					if (itAligned == seqName.end()) {
						error = 1; LOG(LOGERROR) << "" + names[0] + " is in your name file and not in your fasta file, please correct." << '\n';
					}
					else {
						alignedString = itAligned->second;
					}

					//separate by group - parse one line in name file
					map<string, string> splitMap; //group -> name1,name2,...
					map<string, string>::iterator it;
					for (int i = 0; i < names.size(); i++) {

						string group = groupMap.getGroup(names[i]);
						if (group == "not found") { error = 1; LOG(LOGERROR) << "" + names[i] + " is in your name file and not in your groupfile, please correct." << '\n'; }
						else {

							it = splitMap.find(group);
							if (it != splitMap.end()) { //adding seqs to this group
								(it->second) += "," + names[i];
								thisnames1.insert(names[i]);
								countName++;
							}
							else { //first sighting of this group
								splitMap[group] = names[i];
								countName++;
								thisnames1.insert(names[i]);

								//is this seq in the fasta file?
								if (i != 0) { //if not then we need to add a duplicate sequence to the seqs for this group so the new "fasta" and "name" files will match
									Sequence tempSeq(names[i], alignedString); //get the first guys sequence string since he's in the fasta file.
									seqs[group].push_back(tempSeq);
								}
							}
						}

						allSeqsMap[names[i]] = names[0];
					}


					//fill nameMapPerGroup - holds all lines in namefile separated by group
					for (it = splitMap.begin(); it != splitMap.end(); it++) {
						//grab first name
						string firstName = "";
						for (int i = 0; i < (it->second).length(); i++) {
							if (((it->second)[i]) != ',') {
								firstName += ((it->second)[i]);
							}
							else { break; }
						}

						//group1 -> seq1 -> seq1,seq2,seq3
						nameMapPerGroup[it->first][firstName] = it->second;
					}

					pairDone = false;
				}
			}
		}

		if (error == 1) { ctrlc_pressed = true; }

		if (countName != (groupMap.getNumSeqs())) {
			vector<string> groupseqsnames = groupMap.getNamesSeqs();

			for (int i = 0; i < groupseqsnames.size(); i++) {
				set<string>::iterator itnamesfile = thisnames1.find(groupseqsnames[i]);
				if (itnamesfile == thisnames1.end()) {
					cout << "missing name " + groupseqsnames[i] << '\t' << allSeqsMap[groupseqsnames[i]] << endl;
				}
			}

			LOG(INFO) << "";
			LOG(LOGERROR) << "Your name file contains " + toString(countName) + " valid sequences, and your groupfile contains " + toString(groupMap.getNumSeqs()) + ", please correct.";
			LOG(INFO) << "";
			ctrlc_pressed = true;
		}

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SequenceParser, SequenceParser";
		exit(1);
	}
}
/************************************************************/
SequenceParser::SequenceParser(string groupFile, string fastaFile) :
	groupMap(groupFile)
{
	try {
		int error;

		//read group file
		error = groupMap.readMap();

		if (error == 1) { ctrlc_pressed = true; }

		//initialize maps
		vector<string> namesOfGroups = groupMap.getNamesOfGroups();
		for (int i = 0; i < namesOfGroups.size(); i++) {
			vector<Sequence> temp;
			seqs[namesOfGroups[i]] = temp;
		}

		//read fasta file making sure each sequence is in the group file
		ifstream in;
		File::openInputFile(fastaFile, in);
		int count = 0;

		while (!in.eof()) {

			if (ctrlc_pressed) { break; }

			Sequence seq(in); File::gobble(in);

			if (seq.getName() != "") {

				string group = groupMap.getGroup(seq.getName());
				if (group == "not found") { error = 1; LOG(LOGERROR) << "" + seq.getName() + " is in your fasta file and not in your groupfile, please correct." << '\n'; }
				else { seqs[group].push_back(seq);	count++; }
			}
		}
		in.close();

		if (error == 1) { ctrlc_pressed = true; }

		if (count != (groupMap.getNumSeqs())) {
			LOG(INFO) << "";
			LOG(LOGERROR) << "Your fasta file contains " + toString(count) + " valid sequences, and your groupfile contains " + toString(groupMap.getNumSeqs()) + ", please correct.";
			if (count < (groupMap.getNumSeqs())) { LOG(INFO) << " Did you forget to include the name file?"; }
			LOG(INFO) << "";
			ctrlc_pressed = true;
		}

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SequenceParser, SequenceParser";
		exit(1);
	}
}
/************************************************************/
int SequenceParser::getNumGroups() { return groupMap.getNumGroups(); }
/************************************************************/
vector<string> SequenceParser::getNamesOfGroups() { return groupMap.getNamesOfGroups(); }
/************************************************************/
bool SequenceParser::isValidGroup(string g) { return groupMap.isValidGroup(g); }
/************************************************************/
int SequenceParser::getSeqs(string g, string filename, bool uchimeFormat = false) {
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

				map<string, string> nameMapForThisGroup = getNameMap(g);
				map<string, string>::iterator itNameMap;
				int error = 0;

				for (int i = 0; i < seqForThisGroup.size(); i++) {
					itNameMap = nameMapForThisGroup.find(seqForThisGroup[i].getName());

					if (itNameMap == nameMapForThisGroup.end()) {
						error = 1;
						LOG(LOGERROR) << "" + seqForThisGroup[i].getName() + " is in your fastafile, but is not in your namesfile, please correct." << '\n';
					}
					else {
						int num = m->getNumNames(itNameMap->second);

						seqPriorityNode temp(num, seqForThisGroup[i].getAligned(), seqForThisGroup[i].getName());
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
		LOG(FATAL) << e.what() << " in SequenceParser, getSeqs";
		exit(1);
	}
}

/************************************************************/
map<string, string> SequenceParser::getNameMap(string g) {
	try {
		map<string, map<string, string> >::iterator it;
		map<string, string> nameMapForThisGroup;

		it = nameMapPerGroup.find(g);
		if (it == nameMapPerGroup.end()) {
			LOG(LOGERROR) << "No nameMap available for group " + g + ", please correct." << '\n';
		}
		else {
			nameMapForThisGroup = it->second;
			if (app.isDebug) { LOG(DEBUG) << "group " + g + " name file has " + toString(nameMapForThisGroup.size()) + " unique sequences."; }
		}

		return nameMapForThisGroup;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SequenceParser, getNameMap";
		exit(1);
	}
}
/************************************************************/
int SequenceParser::getNameMap(string g, string filename) {
	try {
		map<string, map<string, string> >::iterator it;
		map<string, string> nameMapForThisGroup;

		it = nameMapPerGroup.find(g);
		if (it == nameMapPerGroup.end()) {
			LOG(LOGERROR) << "No nameMap available for group " + g + ", please correct." << '\n';
		}
		else {
			nameMapForThisGroup = it->second;

			ofstream out;
			File::openOutputFile(filename, out);

			for (map<string, string>::iterator itFile = nameMapForThisGroup.begin(); itFile != nameMapForThisGroup.end(); itFile++) {

				if (ctrlc_pressed) { out.close(); File::remove(filename); return 1; }

				out << itFile->first << '\t' << itFile->second << endl;
			}

			out.close();
		}

		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SequenceParser, getNameMap";
		exit(1);
	}
}
/************************************************************/



