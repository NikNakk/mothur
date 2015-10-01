/*
 *  groupmap.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 12/1/08.
 *  Copyright 2008 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "groupmap.h"

 /************************************************************/

GroupMap::GroupMap(string filename) {
	groupFileName = filename;
	File::openInputFile(filename, fileHandle);
	index = 0;
}

/************************************************************/
GroupMap::~GroupMap() {}
/************************************************************/
int GroupMap::readMap() {
	string seqName, seqGroup;
	int error = 0;
	string rest = "";
	char buffer[4096];
	bool pairDone = false;
	bool columnOne = true;

	while (!fileHandle.eof()) {
		if (ctrlc_pressed) { fileHandle.close();  return 1; }

		fileHandle.read(buffer, 4096);
		vector<string> pieces = m->splitWhiteSpace(rest, buffer, fileHandle.gcount());

		for (int i = 0; i < pieces.size(); i++) {
			if (columnOne) { seqName = pieces[i]; columnOne = false; }
			else { seqGroup = pieces[i]; pairDone = true; columnOne = true; }

			if (pairDone) {
				setNamesOfGroups(seqGroup);

				if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "', group = '" + seqGroup + "'\n"; }
				m->checkName(seqName);
				it = groupmap.find(seqName);

				if (it != groupmap.end()) { error = 1; LOG(INFO) << "Your groupfile contains more than 1 sequence named " + seqName + ", sequence names must be unique. Please correct." << '\n'; }
				else {
					groupmap[seqName] = seqGroup;	//store data in map
					seqsPerGroup[seqGroup]++;  //increment number of seqs in that group
				}
				pairDone = false;
			}
		}
	}
	fileHandle.close();

	if (rest != "") {
		vector<string> pieces = m->splitWhiteSpace(rest);

		for (int i = 0; i < pieces.size(); i++) {
			if (columnOne) { seqName = pieces[i]; columnOne = false; }
			else { seqGroup = pieces[i]; pairDone = true; columnOne = true; }

			if (pairDone) {
				setNamesOfGroups(seqGroup);

				if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "', group = '" + seqGroup + "'\n"; }
				m->checkName(seqName);
				it = groupmap.find(seqName);

				if (it != groupmap.end()) { error = 1; LOG(INFO) << "Your groupfile contains more than 1 sequence named " + seqName + ", sequence names must be unique. Please correct." << '\n'; }
				else {
					groupmap[seqName] = seqGroup;	//store data in map
					seqsPerGroup[seqGroup]++;  //increment number of seqs in that group
				}
				pairDone = false;
			}
		}
	}

	m->setAllGroups(namesOfGroups);
	return error;
}
/************************************************************/
int GroupMap::readDesignMap() {
	string seqName, seqGroup;
	int error = 0;
	string rest = "";
	char buffer[4096];
	bool pairDone = false;
	bool columnOne = true;

	while (!fileHandle.eof()) {
		if (ctrlc_pressed) { fileHandle.close();  return 1; }

		fileHandle.read(buffer, 4096);
		vector<string> pieces = m->splitWhiteSpace(rest, buffer, fileHandle.gcount());

		for (int i = 0; i < pieces.size(); i++) {
			if (columnOne) { seqName = pieces[i]; columnOne = false; }
			else { seqGroup = pieces[i]; pairDone = true; columnOne = true; }

			if (pairDone) {
				setNamesOfGroups(seqGroup);

				if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "', group = '" + seqGroup + "'\n"; }
				m->checkName(seqName);
				it = groupmap.find(seqName);

				if (it != groupmap.end()) { error = 1; LOG(INFO) << "Your designfile contains more than 1 sequence named " + seqName + ", sequence names must be unique. Please correct." << '\n'; }
				else {
					groupmap[seqName] = seqGroup;	//store data in map
					seqsPerGroup[seqGroup]++;  //increment number of seqs in that group
				}
				pairDone = false;
			}
		}
	}
	fileHandle.close();

	if (rest != "") {
		vector<string> pieces = m->splitWhiteSpace(rest);

		for (int i = 0; i < pieces.size(); i++) {
			if (columnOne) { seqName = pieces[i]; columnOne = false; }
			else { seqGroup = pieces[i]; pairDone = true; columnOne = true; }

			if (pairDone) {
				setNamesOfGroups(seqGroup);

				if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "', group = '" + seqGroup + "'\n"; }
				m->checkName(seqName);
				it = groupmap.find(seqName);

				if (it != groupmap.end()) { error = 1; LOG(INFO) << "Your designfile contains more than 1 sequence named " + seqName + ", sequence names must be unique. Please correct." << '\n'; }
				else {
					groupmap[seqName] = seqGroup;	//store data in map
					seqsPerGroup[seqGroup]++;  //increment number of seqs in that group
				}
				pairDone = false;
			}
		}

	}

	m->setAllGroups(namesOfGroups);
	return error;
}
/************************************************************/
int GroupMap::readMap(string filename) {
	groupFileName = filename;
	File::openInputFile(filename, fileHandle);
	index = 0;
	string seqName, seqGroup;
	int error = 0;
	string rest = "";
	char buffer[4096];
	bool pairDone = false;
	bool columnOne = true;

	while (!fileHandle.eof()) {
		if (ctrlc_pressed) { fileHandle.close();  return 1; }

		fileHandle.read(buffer, 4096);
		vector<string> pieces = m->splitWhiteSpace(rest, buffer, fileHandle.gcount());

		for (int i = 0; i < pieces.size(); i++) {
			if (columnOne) { seqName = pieces[i]; columnOne = false; }
			else { seqGroup = pieces[i]; pairDone = true; columnOne = true; }

			if (pairDone) {
				setNamesOfGroups(seqGroup);

				if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "', group = '" + seqGroup + "'\n"; }
				m->checkName(seqName);
				it = groupmap.find(seqName);

				if (it != groupmap.end()) { error = 1; LOG(INFO) << "Your group file contains more than 1 sequence named " + seqName + ", sequence names must be unique. Please correct." << '\n'; }
				else {
					groupmap[seqName] = seqGroup;	//store data in map
					seqsPerGroup[seqGroup]++;  //increment number of seqs in that group
				}
				pairDone = false;
			}
		}
	}
	fileHandle.close();

	if (rest != "") {
		vector<string> pieces = m->splitWhiteSpace(rest);

		for (int i = 0; i < pieces.size(); i++) {
			if (columnOne) { seqName = pieces[i]; columnOne = false; }
			else { seqGroup = pieces[i]; pairDone = true; columnOne = true; }

			if (pairDone) {
				setNamesOfGroups(seqGroup);

				if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "', group = '" + seqGroup + "'\n"; }
				m->checkName(seqName);
				it = groupmap.find(seqName);

				if (it != groupmap.end()) { error = 1; LOG(INFO) << "Your group file contains more than 1 sequence named " + seqName + ", sequence names must be unique. Please correct." << '\n'; }
				else {
					groupmap[seqName] = seqGroup;	//store data in map
					seqsPerGroup[seqGroup]++;  //increment number of seqs in that group
				}
				pairDone = false;
			}
		}
	}

	m->setAllGroups(namesOfGroups);
	return error;
}
/************************************************************/
int GroupMap::readDesignMap(string filename) {
	groupFileName = filename;
	File::openInputFile(filename, fileHandle);
	index = 0;
	string seqName, seqGroup;
	int error = 0;
	string rest = "";
	char buffer[4096];
	bool pairDone = false;
	bool columnOne = true;

	while (!fileHandle.eof()) {
		if (ctrlc_pressed) { fileHandle.close();  return 1; }

		fileHandle.read(buffer, 4096);
		vector<string> pieces = m->splitWhiteSpace(rest, buffer, fileHandle.gcount());

		for (int i = 0; i < pieces.size(); i++) {
			if (columnOne) { seqName = pieces[i]; columnOne = false; }
			else { seqGroup = pieces[i]; pairDone = true; columnOne = true; }

			if (pairDone) {
				setNamesOfGroups(seqGroup);

				if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "', group = '" + seqGroup + "'\n"; }
				m->checkName(seqName);
				it = groupmap.find(seqName);

				if (it != groupmap.end()) { error = 1; LOG(INFO) << "Your designfile contains more than 1 sequence named " + seqName + ", sequence names must be unique. Please correct." << '\n'; }
				else {
					groupmap[seqName] = seqGroup;	//store data in map
					seqsPerGroup[seqGroup]++;  //increment number of seqs in that group
				}
				pairDone = false;
			}
		}
	}
	fileHandle.close();

	if (rest != "") {
		vector<string> pieces = m->splitWhiteSpace(rest);

		for (int i = 0; i < pieces.size(); i++) {
			if (columnOne) { seqName = pieces[i]; columnOne = false; }
			else { seqGroup = pieces[i]; pairDone = true; columnOne = true; }

			if (pairDone) {
				setNamesOfGroups(seqGroup);

				if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "', group = '" + seqGroup + "'\n"; }
				m->checkName(seqName);
				it = groupmap.find(seqName);

				if (it != groupmap.end()) { error = 1; LOG(INFO) << "Your designfile contains more than 1 sequence named " + seqName + ", sequence names must be unique. Please correct." << '\n'; }
				else {
					groupmap[seqName] = seqGroup;	//store data in map
					seqsPerGroup[seqGroup]++;  //increment number of seqs in that group
				}
				pairDone = false;
			}
		}
	}

	m->setAllGroups(namesOfGroups);
	return error;
}
/************************************************************/
int GroupMap::getNumGroups() { return namesOfGroups.size(); }
/************************************************************/

string GroupMap::getGroup(string sequenceName) {

	it = groupmap.find(sequenceName);
	if (it != groupmap.end()) { //sequence name was in group file
		return it->second;
	}
	else {
		//look for it in names of groups to see if the user accidently used the wrong file
		if (m->inUsersGroups(sequenceName, namesOfGroups)) {
			LOG(WARNING) << "Your group or design file contains a group named " + sequenceName + ".  Perhaps you are used a group file instead of a design file? A common cause of this is using a tree file that relates your groups (created by the tree.shared command) with a group file that assigns sequences to a group." << '\n';
		}
		return "not found";
	}
}

/************************************************************/

void GroupMap::setGroup(string sequenceName, string groupN) {
	setNamesOfGroups(groupN);
	m->checkName(sequenceName);
	it = groupmap.find(sequenceName);

	if (it != groupmap.end()) { LOG(INFO) << "Your groupfile contains more than 1 sequence named " + sequenceName + ", sequence names must be unique. Please correct." << '\n'; }
	else {
		groupmap[sequenceName] = groupN;	//store data in map
		seqsPerGroup[groupN]++;  //increment number of seqs in that group
	}
}

/************************************************************/
void GroupMap::setNamesOfGroups(string seqGroup) {
	int i, count;
	count = 0;
	for (i = 0; i < namesOfGroups.size(); i++) {
		if (namesOfGroups[i] != seqGroup) {
			count++; //you have not found this group
		}
		else {
			break; //you already have it
		}
	}
	if (count == namesOfGroups.size()) {
		namesOfGroups.push_back(seqGroup); //new group
		seqsPerGroup[seqGroup] = 0;
		groupIndex[seqGroup] = index;
		index++;
	}
}
/************************************************************/
bool GroupMap::isValidGroup(string groupname) {
	for (int i = 0; i < namesOfGroups.size(); i++) {
		if (groupname == namesOfGroups[i]) { return true; }
	}

	return false;
}
/************************************************************/
int GroupMap::getCopy(GroupMap* g) {
	vector<string> names = g->getNamesSeqs();
	for (int i = 0; i < names.size(); i++) {
		if (ctrlc_pressed) { break; }
		string group = g->getGroup(names[i]);
		setGroup(names[i], group);
	}
	return names.size();
}
/************************************************************/
int GroupMap::getNumSeqs(string group) {

	map<string, int>::iterator itNum;

	itNum = seqsPerGroup.find(group);

	if (itNum == seqsPerGroup.end()) { return 0; }

	return seqsPerGroup[group];

}
/************************************************************/
int GroupMap::renameSeq(string oldName, string newName) {

	map<string, string>::iterator itName;

	itName = groupmap.find(oldName);

	if (itName == groupmap.end()) {
		LOG(LOGERROR) << "cannot find " + toString(oldName) + " in group file";
		ctrlc_pressed = true;
		return 0;
	}
	else {
		string group = itName->second;
		groupmap.erase(itName);
		groupmap[newName] = group;
	}

	return 0;

}
/************************************************************/
int GroupMap::print(ofstream& out) {

	for (map<string, string>::iterator itName = groupmap.begin(); itName != groupmap.end(); itName++) {
		out << itName->first << '\t' << itName->second << endl;
	}

	return 0;

}
/************************************************************/
int GroupMap::print(ofstream& out, vector<string> userGroups) {

	for (map<string, string>::iterator itName = groupmap.begin(); itName != groupmap.end(); itName++) {
		if (m->inUsersGroups(itName->second, userGroups)) {
			out << itName->first << '\t' << itName->second << endl;
		}
	}

	return 0;

}
/************************************************************/
vector<string> GroupMap::getNamesSeqs() {

	vector<string> names;

	for (it = groupmap.begin(); it != groupmap.end(); it++) {
		names.push_back(it->first);
	}

	return names;
}
/************************************************************/
vector<string> GroupMap::getNamesSeqs(vector<string> picked) {

	vector<string> names;

	for (it = groupmap.begin(); it != groupmap.end(); it++) {
		//if you are belong to one the the groups in the picked vector add you
		if (m->inUsersGroups(it->second, picked)) {
			names.push_back(it->first);
		}
	}

	return names;
}

/************************************************************/

