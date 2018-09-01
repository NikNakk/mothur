/*
 *  sharedvector.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 12/5/08.
 *  Copyright 2008 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "sharedrabundvector.h" 
#include "sabundvector.hpp"
#include "ordervector.hpp"
#include "sharedutilities.h"


 /***********************************************************************/
SharedRAbundVector::SharedRAbundVector() : DataVector(), maxRank(0), numBins(0), numSeqs(0) {}
/***********************************************************************/

SharedRAbundVector::~SharedRAbundVector() {
	//for (int i = 0; i < lookup.size(); i++) {  delete lookup[i];  }

}

/***********************************************************************/

SharedRAbundVector::SharedRAbundVector(int n) : DataVector(), maxRank(0), numBins(n), numSeqs(0) {
	individual newGuy;
	//initialize data
	for (int i = 0; i < n; i++) {
		newGuy.bin = i;
		newGuy.abundance = 0;
		data.push_back(newGuy);
	}
}

/***********************************************************************

SharedRAbundVector::SharedRAbundVector(string id, vector<individual> rav) : DataVector(id), data(rav) {
		numBins = 0;
		maxRank = 0;
		numSeqs = 0;

		for(int i=0;i<data.size();i++){
			if(data[i].abundance != 0)		{	numBins = i+1;		}
			if(data[i].abundance > maxRank)	{	maxRank = data[i].abundance;	}
			numSeqs += data[i].abundance;
		}
}


***********************************************************************/
//reads a shared file
SharedRAbundVector::SharedRAbundVector(ifstream& f) : DataVector(), maxRank(0), numBins(0), numSeqs(0) {
	m->clearAllGroups();
	vector<string> allGroups;

	int num, inputData, count;
	count = 0;
	string holdLabel, nextLabel, groupN;
	individual newguy;

	for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }  lookup.clear();

	//are we at the beginning of the file??
	if (m->saveNextLabel == "") {
		f >> label;

		//is this a shared file that has headers
		if (label == "label") {
			//gets "group"
			f >> label; File::gobble(f);

			//gets "numOtus"
			f >> label; File::gobble(f);

			//eat rest of line
			label = File::getline(f); File::gobble(f);

			//parse labels to save
			istringstream iStringStream(label);
			m->sharedBinLabelsInFile.clear();
			while (!iStringStream.eof()) {
				if (ctrlc_pressed) { break; }
				string temp;
				iStringStream >> temp;  File::gobble(iStringStream);

				m->sharedBinLabelsInFile.push_back(temp);
			}

			f >> label >> groupN >> num;
		}
		else {
			//read in first row since you know there is at least 1 group.
			f >> groupN >> num;

			//make binlabels because we don't have any
			string snumBins = toString(num);
			m->sharedBinLabelsInFile.clear();
			for (int i = 0; i < num; i++) {
				//if there is a bin label use it otherwise make one
				string binLabel = "Otu";
				string sbinNumber = toString(i + 1);
				if (sbinNumber.length() < snumBins.length()) {
					int diff = snumBins.length() - sbinNumber.length();
					for (int h = 0; h < diff; h++) { binLabel += "0"; }
				}
				binLabel += sbinNumber;
				m->sharedBinLabelsInFile.push_back(binLabel);
			}
		}
	}
	else {
		label = m->saveNextLabel;

		//read in first row since you know there is at least 1 group.
		f >> groupN >> num;

		if (app.isDebug) { LOG(DEBUG) << "" + groupN + '\t' + toString(num); }
	}

	//reset labels, currentLabels may have gotten changed as otus were eliminated because of group choices or sampling
	settings.currentSharedBinLabels = m->sharedBinLabelsInFile;

	holdLabel = label;

	//add new vector to lookup
	SharedRAbundVector* temp = new SharedRAbundVector();
	lookup.push_back(temp);
	lookup[0]->setLabel(label);
	lookup[0]->setGroup(groupN);

	allGroups.push_back(groupN);

	//fill vector.  data = first sharedrabund in file
	for (int i = 0;i < num;i++) {
		f >> inputData;
		if (app.isDebug) { LOG(DEBUG) << "OTU" + toString(i + 1) + '\t' + toString(inputData); }

		lookup[0]->push_back(inputData, groupN); //abundance, bin, group
		push_back(inputData, groupN);

		if (inputData > maxRank) { maxRank = inputData; }
	}

	File::gobble(f);

	if (!(f.eof())) { f >> nextLabel; }

	//read the rest of the groups info in
	while ((nextLabel == holdLabel) && (f.eof() != true)) {
		f >> groupN >> num;
		if (app.isDebug) { LOG(DEBUG) << "" + groupN + '\t' + toString(num); }
		count++;

		allGroups.push_back(groupN);

		//add new vector to lookup
		temp = new SharedRAbundVector();
		lookup.push_back(temp);
		lookup[count]->setLabel(label);
		lookup[count]->setGroup(groupN);

		//fill vector.  
		for (int i = 0;i < num;i++) {
			f >> inputData;
			if (app.isDebug) { LOG(DEBUG) << "OTU" + toString(i + 1) + '\t' + toString(inputData); }

			lookup[count]->push_back(inputData, groupN); //abundance, bin, group
		}

		File::gobble(f);

		if (f.eof() != true) { f >> nextLabel; }
	}
	m->saveNextLabel = nextLabel;
	m->setAllGroups(allGroups);
}

/***********************************************************************/

void SharedRAbundVector::set(int binNumber, int newBinSize, string groupname) {
	int oldBinSize = data[binNumber].abundance;
	data[binNumber].abundance = newBinSize;
	data[binNumber].group = groupname;

	if (newBinSize > maxRank) { maxRank = newBinSize; }

	numSeqs += (newBinSize - oldBinSize);
}
/***********************************************************************/

void SharedRAbundVector::setData(vector <individual> newData) {
	data = newData;
}

/***********************************************************************/

int SharedRAbundVector::getAbundance(int index) {
	return data[index].abundance;

}
/***********************************************************************/
//returns vector of abundances 
vector<int> SharedRAbundVector::getAbundances() {
	vector<int> abunds;
	for (int i = 0; i < data.size(); i++) {
		abunds.push_back(data[i].abundance);
	}

	return abunds;
}


/***********************************************************************/

int SharedRAbundVector::numNZ() {
	int sum = 0;
	for (int i = 1; i < numBins; i++)
		if (data[i].abundance > 0)
			sum++;
	return sum;
}
/***********************************************************************/

void SharedRAbundVector::sortD() {
	struct individual indObj;
	sort(data.begin() + 1, data.end(), indObj);
}
/***********************************************************************/

individual SharedRAbundVector::get(int index) {
	return data[index];

}
/***********************************************************************/

vector <individual> SharedRAbundVector::getData() {
	return data;
}
/***********************************************************************/

void SharedRAbundVector::clear() {
	numBins = 0;
	maxRank = 0;
	numSeqs = 0;
	data.clear();
	for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }
	lookup.clear();
}
/***********************************************************************/

void SharedRAbundVector::push_back(int binSize, string groupName) {
	individual newGuy;
	newGuy.abundance = binSize;
	newGuy.group = groupName;
	newGuy.bin = data.size();

	data.push_back(newGuy);
	numBins++;

	if (binSize > maxRank) {
		maxRank = binSize;
	}

	numSeqs += binSize;
}

/***********************************************************************/

void SharedRAbundVector::insert(int binSize, int otu, string groupName) {
	individual newGuy;
	newGuy.abundance = binSize;
	newGuy.group = groupName;
	newGuy.bin = otu;

	data.insert(data.begin() + otu, newGuy);
	numBins++;

	if (binSize > maxRank) {
		maxRank = binSize;
	}

	numSeqs += binSize;
}

/***********************************************************************/

void SharedRAbundVector::push_front(int binSize, int otu, string groupName) {
	individual newGuy;
	newGuy.abundance = binSize;
	newGuy.group = groupName;
	newGuy.bin = otu;

	data.insert(data.begin(), newGuy);
	numBins++;

	if (binSize > maxRank) {
		maxRank = binSize;
	}

	numSeqs += binSize;
}

/***********************************************************************/
void SharedRAbundVector::pop_back() {
	numSeqs -= data[data.size() - 1].abundance;
	numBins--;
	return data.pop_back();
}

/***********************************************************************/


vector<individual>::reverse_iterator SharedRAbundVector::rbegin() {
	return data.rbegin();
}

/***********************************************************************/

vector<individual>::reverse_iterator SharedRAbundVector::rend() {
	return data.rend();
}

/***********************************************************************/
void SharedRAbundVector::resize(int size) {

	data.resize(size);
}

/***********************************************************************/

int SharedRAbundVector::size() {
	return data.size();
}


/***********************************************************************/
void SharedRAbundVector::printHeaders(ostream& output) {
	string snumBins = toString(numBins);
	output << "label\tGroup\tnumOtus";
	if (m->sharedHeaderMode == "tax") {
		for (int i = 0; i < numBins; i++) {

			//if there is a bin label use it otherwise make one
			string binLabel = "PhyloType";
			string sbinNumber = toString(i + 1);
			if (sbinNumber.length() < snumBins.length()) {
				int diff = snumBins.length() - sbinNumber.length();
				for (int h = 0; h < diff; h++) { binLabel += "0"; }
			}
			binLabel += sbinNumber;
			if (i < settings.currentSharedBinLabels.size()) { binLabel = settings.currentSharedBinLabels[i]; }

			output << '\t' << binLabel;
		}
		output << endl;
	}
	else {
		for (int i = 0; i < numBins; i++) {
			//if there is a bin label use it otherwise make one
			string binLabel = "Otu";
			string sbinNumber = toString(i + 1);
			if (sbinNumber.length() < snumBins.length()) {
				int diff = snumBins.length() - sbinNumber.length();
				for (int h = 0; h < diff; h++) { binLabel += "0"; }
			}
			binLabel += sbinNumber;
			if (i < settings.currentSharedBinLabels.size()) { binLabel = settings.currentSharedBinLabels[i]; }

			output << '\t' << binLabel;
		}

		output << endl;
	}
	m->printedSharedHeaders = true;
}
/***********************************************************************/
void SharedRAbundVector::print(ostream& output) {
	output << numBins;

	for (int i = 0;i < data.size();i++) { output << '\t' << data[i].abundance; }
	output << endl;
}
/***********************************************************************/
string SharedRAbundVector::getGroup() {
	return group;
}

/***********************************************************************/

void SharedRAbundVector::setGroup(string groupName) {
	group = groupName;
}
/***********************************************************************/
int SharedRAbundVector::getGroupIndex() { return index; }
/***********************************************************************/
void SharedRAbundVector::setGroupIndex(int vIndex) { index = vIndex; }
/***********************************************************************/
int SharedRAbundVector::getNumBins() {
	return numBins;
}

/***********************************************************************/

int SharedRAbundVector::getNumSeqs() {
	return numSeqs;
}

/***********************************************************************/

int SharedRAbundVector::getMaxRank() {
	return maxRank;
}
/***********************************************************************/

SharedRAbundVector SharedRAbundVector::getSharedRAbundVector() {
	return *this;
}
/***********************************************************************/
vector<SharedRAbundVector*> SharedRAbundVector::getSharedRAbundVectors() {
	SharedUtil* util;
	util = new SharedUtil();

	vector<string> Groups = settings.getGroups();
	vector<string> allGroups = m->getAllGroups();
	util->setGroups(Groups, allGroups);
	m->setGroups(Groups);

	bool remove = false;
	for (int i = 0; i < lookup.size(); i++) {
		//if this sharedrabund is not from a group the user wants then delete it.
		if (util->isValidGroup(lookup[i]->getGroup(), settings.getGroups()) == false) {
			remove = true;
			delete lookup[i]; lookup[i] = NULL;
			lookup.erase(lookup.begin() + i);
			i--;
		}
	}

	delete util;

	if (remove) { eliminateZeroOTUS(); }

	return lookup;
}
//**********************************************************************************************************************
int SharedRAbundVector::eliminateZeroOTUS() {
	try {

		vector<SharedRAbundVector*> newLookup;
		for (int i = 0; i < lookup.size(); i++) {
			SharedRAbundVector* temp = new SharedRAbundVector();
			temp->setLabel(lookup[i]->getLabel());
			temp->setGroup(lookup[i]->getGroup());
			newLookup.push_back(temp);
		}

		//for each bin
		vector<string> newBinLabels;
		string snumBins = toString(lookup[0]->getNumBins());
		for (int i = 0; i < lookup[0]->getNumBins(); i++) {
			if (ctrlc_pressed) { for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; } return 0; }

			//look at each sharedRabund and make sure they are not all zero
			bool allZero = true;
			for (int j = 0; j < lookup.size(); j++) {
				if (lookup[j]->getAbundance(i) != 0) { allZero = false;  break; }
			}

			//if they are not all zero add this bin
			if (!allZero) {
				for (int j = 0; j < lookup.size(); j++) {
					newLookup[j]->push_back(lookup[j]->getAbundance(i), lookup[j]->getGroup());
				}

				//if there is a bin label use it otherwise make one
				string binLabel = "Otu";
				string sbinNumber = toString(i + 1);
				if (sbinNumber.length() < snumBins.length()) {
					int diff = snumBins.length() - sbinNumber.length();
					for (int h = 0; h < diff; h++) { binLabel += "0"; }
				}
				binLabel += sbinNumber;
				if (i < settings.currentSharedBinLabels.size()) { binLabel = settings.currentSharedBinLabels[i]; }

				newBinLabels.push_back(binLabel);
			}
		}

		for (int j = 0; j < lookup.size(); j++) { delete lookup[j]; }

		lookup = newLookup;
		settings.currentSharedBinLabels = newBinLabels;

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SharedRAbundVector, eliminateZeroOTUS";
		exit(1);
	}
}

/***********************************************************************/
vector<SharedRAbundFloatVector*> SharedRAbundVector::getSharedRAbundFloatVectors(vector<SharedRAbundVector*> thislookup) {
	vector<SharedRAbundFloatVector*> newLookupFloat;
	for (int i = 0; i < lookup.size(); i++) {
		SharedRAbundFloatVector* temp = new SharedRAbundFloatVector();
		temp->setLabel(thislookup[i]->getLabel());
		temp->setGroup(thislookup[i]->getGroup());
		newLookupFloat.push_back(temp);
	}

	for (int i = 0; i < thislookup.size(); i++) {

		for (int j = 0; j < thislookup[i]->getNumBins(); j++) {

			if (ctrlc_pressed) { return newLookupFloat; }

			int abund = thislookup[i]->getAbundance(j);

			float relabund = abund / (float)thislookup[i]->getNumSeqs();

			newLookupFloat[i]->push_back(relabund, thislookup[i]->getGroup());
		}
	}

	return newLookupFloat;
}
/***********************************************************************/

RAbundVector SharedRAbundVector::getRAbundVector() {
	RAbundVector rav;

	for (int i = 0; i < data.size(); i++) {
		if (data[i].abundance != 0) {
			rav.push_back(data[i].abundance);
		}
	}

	rav.setLabel(label);
	return rav;
}
/***********************************************************************/

RAbundVector SharedRAbundVector::getRAbundVector2() {
	RAbundVector rav;
	for (int i = 0; i < numBins; i++)
		if (data[i].abundance != 0)
			rav.push_back(data[i].abundance - 1);
	return rav;
}
/***********************************************************************/

SharedSAbundVector SharedRAbundVector::getSharedSAbundVector() {
	SharedSAbundVector sav(maxRank + 1);

	for (int i = 0;i < data.size();i++) {
		int abund = data[i].abundance;
		sav.set(abund, sav.getAbundance(abund) + 1, group);
	}

	sav.set(0, 0, group);
	sav.setLabel(label);
	sav.setGroup(group);

	return sav;
}
/***********************************************************************/

SAbundVector SharedRAbundVector::getSAbundVector() {
	SAbundVector sav(maxRank + 1);

	for (int i = 0;i < data.size();i++) {
		int abund = data[i].abundance;
		sav.set(abund, sav.get(abund) + 1);
	}
	sav.set(0, 0);
	sav.setLabel(label);
	return sav;
}

/***********************************************************************/

SharedOrderVector SharedRAbundVector::getSharedOrderVector() {
	SharedOrderVector ov;

	for (int i = 0;i < data.size();i++) {
		for (int j = 0;j < data[i].abundance;j++) {
			ov.push_back(data[i].bin, data[i].abundance, data[i].group);
		}
	}
	random_shuffle(ov.begin(), ov.end());

	ov.setLabel(label);
	ov.updateStats();

	return ov;
}
/***********************************************************************/

OrderVector SharedRAbundVector::getOrderVector(map<string, int>* nameMap = NULL) {
	OrderVector ov;
	for (int i = 0;i < numBins;i++) {
		for (int j = 0;j < data[i].abundance;j++) {
			ov.push_back(i);
		}
	}
	random_shuffle(ov.begin(), ov.end());

	ov.setLabel(label);

	return ov;
}

/***********************************************************************/
