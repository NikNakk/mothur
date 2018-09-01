/*
 *  sharedSharedListVector.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 1/22/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "sabundvector.hpp"
#include "rabundvector.hpp"
#include "ordervector.hpp"
#include "sharedlistvector.h"
#include "sharedordervector.h"
#include "sharedutilities.h"

 /***********************************************************************/

SharedListVector::SharedListVector() : DataVector(), maxRank(0), numBins(0), numSeqs(0) { groupmap = NULL; countTable = NULL; }

/***********************************************************************/

SharedListVector::SharedListVector(int n) : DataVector(), data(n, ""), maxRank(0), numBins(0), numSeqs(0) { groupmap = NULL; countTable = NULL; }

/***********************************************************************/
SharedListVector::SharedListVector(ifstream& f) : DataVector(), maxRank(0), numBins(0), numSeqs(0) {
	groupmap = NULL; countTable = NULL;
	//set up groupmap for later.
	if (m->groupMode == "group") {
		groupmap = new GroupMap(settings.getCurrent("group"));
		groupmap->readMap();
	}
	else {
		countTable = new CountTable();
		countTable->readTable(settings.getCurrent("counttable"), true, false);
	}

	int hold;

	//are we at the beginning of the file??
	if (m->saveNextLabel == "") {
		f >> label;

		//is this a shared file that has headers
		if (label == "label") {

			//gets "numOtus"
			f >> label; File::gobble(f);

			//eat rest of line
			label = File::getline(f); File::gobble(f);

			//parse labels to save
			istringstream iStringStream(label);
			m->listBinLabelsInFile.clear();
			while (!iStringStream.eof()) {
				if (ctrlc_pressed) { break; }
				string temp;
				iStringStream >> temp;  File::gobble(iStringStream);

				m->listBinLabelsInFile.push_back(temp);
			}

			f >> label >> hold;
		}
		else {
			//read in first row
			f >> hold;

			//make binlabels because we don't have any
			string snumBins = toString(hold);
			m->listBinLabelsInFile.clear();
			for (int i = 0; i < hold; i++) {
				//if there is a bin label use it otherwise make one
				string binLabel = "Otu";
				string sbinNumber = toString(i + 1);
				if (sbinNumber.length() < snumBins.length()) {
					int diff = snumBins.length() - sbinNumber.length();
					for (int h = 0; h < diff; h++) { binLabel += "0"; }
				}
				binLabel += sbinNumber;
				m->listBinLabelsInFile.push_back(binLabel);
			}
		}
		m->saveNextLabel = label;
	}
	else {
		f >> label >> hold;
		m->saveNextLabel = label;
	}

	binLabels.assign(m->listBinLabelsInFile.begin(), m->listBinLabelsInFile.begin() + hold);

	data.assign(hold, "");
	string inputData = "";

	for (int i = 0;i < hold;i++) {
		f >> inputData;
		set(i, inputData);
	}
	File::gobble(f);

	if (f.eof()) { m->saveNextLabel = ""; }

}

/***********************************************************************/
void SharedListVector::set(int binNumber, string seqNames) {
	int nNames_old = m->getNumNames(data[binNumber]);
	data[binNumber] = seqNames;
	int nNames_new = m->getNumNames(seqNames);

	if (nNames_old == 0) { numBins++; }
	if (nNames_new == 0) { numBins--; }
	if (nNames_new > maxRank) { maxRank = nNames_new; }

	numSeqs += (nNames_new - nNames_old);


}

/***********************************************************************/

string SharedListVector::get(int index) {
	return data[index];
}
/***********************************************************************/

void SharedListVector::setLabels(vector<string> labels) {
	binLabels = labels;
}

/***********************************************************************/
//could potentially end up with duplicate binlabel names with code below.
//we don't currently use them in a way that would do that.
//if you had a listfile that had been subsampled and then added to it, dup names would be possible.
vector<string> SharedListVector::getLabels() {
	string tagHeader = "Otu";
	if (m->sharedHeaderMode == "tax") { tagHeader = "PhyloType"; }

	if (binLabels.size() < data.size()) {
		string snumBins = toString(numBins);

		for (int i = 0; i < numBins; i++) {
			string binLabel = tagHeader;

			if (i < binLabels.size()) { //label exists, check leading zeros length
				string sbinNumber = m->getSimpleLabel(binLabels[i]);
				if (sbinNumber.length() < snumBins.length()) {
					int diff = snumBins.length() - sbinNumber.length();
					for (int h = 0; h < diff; h++) { binLabel += "0"; }
				}
				binLabel += sbinNumber;
				binLabels[i] = binLabel;
			}
			else {
				string sbinNumber = toString(i + 1);
				if (sbinNumber.length() < snumBins.length()) {
					int diff = snumBins.length() - sbinNumber.length();
					for (int h = 0; h < diff; h++) { binLabel += "0"; }
				}
				binLabel += sbinNumber;
				binLabels.push_back(binLabel);
			}
		}
	}
	return binLabels;
}
/***********************************************************************/

void SharedListVector::push_back(string seqNames) {
	data.push_back(seqNames);
	int nNames = m->getNumNames(seqNames);

	numBins++;

	if (nNames > maxRank) { maxRank = nNames; }

	numSeqs += nNames;
}

/***********************************************************************/

void SharedListVector::resize(int size) {
	data.resize(size);
}

/***********************************************************************/

int SharedListVector::size() {
	return data.size();
}
/***********************************************************************/

void SharedListVector::clear() {
	numBins = 0;
	maxRank = 0;
	numSeqs = 0;
	return data.clear();

}

/***********************************************************************/

void SharedListVector::print(ostream& output) {
	output << label << '\t' << numBins;

	for (int i = 0;i < data.size();i++) {
		if (data[i] != "") {
			output << '\t' << data[i];
		}
	}
	output << endl;
}


/***********************************************************************/

RAbundVector SharedListVector::getRAbundVector() {
	RAbundVector rav;

	for (int i = 0;i < data.size();i++) {
		int binSize = m->getNumNames(data[i]);
		rav.push_back(binSize);
	}

	//  This was here before to output data in a nice format, but it screws up the name mapping steps
	//	sort(rav.rbegin(), rav.rend());
	//	
	//	for(int i=data.size()-1;i>=0;i--){
	//		if(rav.get(i) == 0){	rav.pop_back();	}
	//		else{
	//			break;
	//		}
	//	}
	rav.setLabel(label);

	return rav;
}

/***********************************************************************/

SAbundVector SharedListVector::getSAbundVector() {
	SAbundVector sav(maxRank + 1);

	for (int i = 0;i < data.size();i++) {
		int binSize = m->getNumNames(data[i]);
		sav.set(binSize, sav.get(binSize) + 1);
	}
	sav.set(0, 0);
	sav.setLabel(label);

	return sav;
}

/***********************************************************************/
SharedOrderVector* SharedListVector::getSharedOrderVector() {
	SharedOrderVector* order = new SharedOrderVector();
	order->setLabel(label);

	for (int i = 0;i < numBins;i++) {
		int binSize = m->getNumNames(get(i));	//find number of individual in given bin	
		string names = get(i);
		vector<string> binNames;
		m->splitAtComma(names, binNames);
		if (m->groupMode != "group") {
			binSize = 0;
			for (int j = 0; j < binNames.size(); j++) { binSize += countTable->getNumSeqs(binNames[i]); }
		}
		for (int j = 0; j < binNames.size(); j++) {
			if (ctrlc_pressed) { return order; }
			if (m->groupMode == "group") {
				string groupName = groupmap->getGroup(binNames[i]);
				if (groupName == "not found") { LOG(INFO) << "Error: Sequence '" + binNames[i] + "' was not found in the group file, please correct." << '\n';  exit(1); }

				order->push_back(i, binSize, groupName);  //i represents what bin you are in
			}
			else {
				vector<int> groupAbundances = countTable->getGroupCounts(binNames[i]);
				vector<string> groupNames = countTable->getNamesOfGroups();
				for (int k = 0; k < groupAbundances.size(); k++) { //groupAbundances.size() == 0 if there is a file mismatch and ctrlc_pressed is true.
					if (ctrlc_pressed) { return order; }
					for (int l = 0; l < groupAbundances[k]; l++) { order->push_back(i, binSize, groupNames[k]); }
				}
			}
		}
	}

	random_shuffle(order->begin(), order->end());
	order->updateStats();

	return order;
}
/***********************************************************************/
SharedRAbundVector SharedListVector::getSharedRAbundVector(string groupName) {
	settings.currentSharedBinLabels = binLabels;

	SharedRAbundVector rav(data.size());

	for (int i = 0;i < numBins;i++) {
		string names = get(i);
		vector<string> binNames;
		m->splitAtComma(names, binNames);
		for (int j = 0; j < binNames.size(); j++) {
			if (ctrlc_pressed) { return rav; }
			if (m->groupMode == "group") {
				string group = groupmap->getGroup(binNames[j]);
				if (group == "not found") { LOG(INFO) << "Error: Sequence '" + binNames[j] + "' was not found in the group file, please correct." << '\n';  exit(1); }
				if (group == groupName) { //this name is in the group you want the vector for.
					rav.set(i, rav.getAbundance(i) + 1, group);  //i represents what bin you are in
				}
			}
			else {
				int count = countTable->getGroupCount(binNames[j], groupName);
				rav.set(i, rav.getAbundance(i) + count, groupName);
			}
		}
	}

	rav.setLabel(label);
	rav.setGroup(groupName);

	return rav;

}
/***********************************************************************/
vector<SharedRAbundVector*> SharedListVector::getSharedRAbundVector() {
	settings.currentSharedBinLabels = binLabels;

	SharedUtil* util;
	util = new SharedUtil();
	vector<SharedRAbundVector*> lookup;  //contains just the groups the user selected
	vector<SharedRAbundVector*> lookupDelete;
	map<string, SharedRAbundVector*> finder;  //contains all groups in groupmap

	vector<string> Groups = settings.getGroups();
	vector<string> allGroups;
	if (m->groupMode == "group") { allGroups = groupmap->getNamesOfGroups(); }
	else { allGroups = countTable->getNamesOfGroups(); }
	util->setGroups(Groups, allGroups);
	m->setGroups(Groups);
	delete util;

	for (int i = 0; i < allGroups.size(); i++) {
		SharedRAbundVector* temp = new SharedRAbundVector(data.size());
		finder[allGroups[i]] = temp;
		finder[allGroups[i]]->setLabel(label);
		finder[allGroups[i]]->setGroup(allGroups[i]);
		if (m->inUsersGroups(allGroups[i], settings.getGroups())) {  //if this group is in user groups
			lookup.push_back(finder[allGroups[i]]);
		}
		else {
			lookupDelete.push_back(finder[allGroups[i]]);
		}
	}

	//fill vectors
	for (int i = 0;i < numBins;i++) {
		string names = get(i);
		vector<string> binNames;
		m->splitAtComma(names, binNames);
		for (int j = 0; j < binNames.size(); j++) {
			if (m->groupMode == "group") {
				string group = groupmap->getGroup(binNames[j]);
				if (group == "not found") { LOG(INFO) << "Error: Sequence '" + binNames[j] + "' was not found in the group file, please correct." << '\n';  exit(1); }
				finder[group]->set(i, finder[group]->getAbundance(i) + 1, group);  //i represents what bin you are in	
			}
			else {
				vector<int> counts = countTable->getGroupCounts(binNames[j]);
				for (int k = 0; k < allGroups.size(); k++) {
					finder[allGroups[k]]->set(i, finder[allGroups[k]]->getAbundance(i) + counts[k], allGroups[k]);
				}
			}
		}
	}

	for (int j = 0; j < lookupDelete.size(); j++) { delete lookupDelete[j]; }

	return lookup;
}

/***********************************************************************/
SharedSAbundVector SharedListVector::getSharedSAbundVector(string groupName) {
	try {
		SharedSAbundVector sav;
		SharedRAbundVector rav;

		rav = this->getSharedRAbundVector(groupName);
		sav = rav.getSharedSAbundVector();

		return sav;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SharedListVector, getSharedSAbundVector";
		exit(1);
	}
}
/***********************************************************************/

OrderVector SharedListVector::getOrderVector(map<string, int>* orderMap = NULL) {

	try {
		if (orderMap == NULL) {
			OrderVector ov;

			for (int i = 0;i < data.size();i++) {
				string names = data[i];
				vector<string> binNames;
				m->splitAtComma(names, binNames);
				int binSize = binNames.size();
				if (m->groupMode != "group") {
					binSize = 0;
					for (int j = 0; j < binNames.size(); j++) { binSize += countTable->getNumSeqs(binNames[i]); }
				}
				for (int j = 0;j < binSize;j++) {
					ov.push_back(i);
				}
			}
			random_shuffle(ov.begin(), ov.end());
			ov.setLabel(label);
			ov.getNumBins();

			return ov;

		}
		else {
			OrderVector ov(numSeqs);

			for (int i = 0;i < data.size();i++) {
				string listOTU = data[i];
				vector<string> binNames;
				m->splitAtComma(listOTU, binNames);
				for (int j = 0; j < binNames.size(); j++) {
					if (orderMap->count(binNames[j]) == 0) {
						LOG(INFO) << binNames[j] + " not found, check *.names file\n";
						exit(1);
					}
					ov.set((*orderMap)[binNames[j]], i);
				}
			}

			ov.setLabel(label);
			ov.getNumBins();

			return ov;
		}
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SharedListVector, getOrderVector";
		exit(1);
	}
}

/***********************************************************************/
