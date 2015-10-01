/*
 *  sharedSharedSAbundVector.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 12/10/08.
 *  Copyright 2008 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "sharedsabundvector.h"
#include "sabundvector.hpp"


 /***********************************************************************/

SharedSAbundVector::SharedSAbundVector() : DataVector(), maxRank(0), numBins(0), numSeqs(0) {  }

/***********************************************************************/

SharedSAbundVector::SharedSAbundVector(int size) : DataVector(), maxRank(0), numBins(0), numSeqs(0) {
	individual newGuy;
	//initialize data
	for (int i = 0; i < size; i++) {
		newGuy.bin = i;
		newGuy.abundance = 0;
		data.push_back(newGuy);
	}
}

/***********************************************************************/

void SharedSAbundVector::set(int bin, int abundance, string groupName) {

	int initSize = data[bin].abundance;
	data[bin].abundance = abundance;
	data[bin].group = groupName;

	if (bin != 0) {
		numBins += (abundance - initSize);
	}

	numSeqs += bin * (abundance - initSize);

	if (bin > maxRank) { maxRank = bin; }
}

/***********************************************************************/

individual SharedSAbundVector::get(int index) {
	return data[index];
}
/***********************************************************************/

int SharedSAbundVector::getAbundance(int index) {
	return data[index].abundance;
}

/***********************************************************************/

void SharedSAbundVector::push_back(int abundance, int bin, string groupName) {
	individual newGuy;
	newGuy.abundance = abundance;
	newGuy.bin = bin;
	newGuy.group = groupName;

	data.push_back(newGuy);

	maxRank++;

	numBins += abundance;

	numSeqs += (maxRank * abundance);
}

/***********************************************************************/

void SharedSAbundVector::resize(int size) {
	data.resize(size);
}

/***********************************************************************/

int SharedSAbundVector::size() {
	return data.size();
}

/***********************************************************************/
void SharedSAbundVector::print(ostream& output) {
	output << label << '\t' << maxRank;

	for (int i = 1;i <= maxRank;i++) {
		output << '\t' << data[i].abundance;
	}
	output << endl;
}
/***********************************************************************/
string SharedSAbundVector::getGroup() {
	return group;
}

/***********************************************************************/

void SharedSAbundVector::setGroup(string groupName) {
	group = groupName;
}

/**********************************************************************/
int SharedSAbundVector::getNumBins() {
	return numBins;
}

/***********************************************************************/

int SharedSAbundVector::getNumSeqs() {
	return numSeqs;
}

/***********************************************************************/

int SharedSAbundVector::getMaxRank() {
	return maxRank;
}
/***********************************************************************/
RAbundVector SharedSAbundVector::getRAbundVector() {
	RAbundVector rav;

	for (int i = 1;i < data.size();i++) {
		for (int j = 0;j < data[i].abundance;j++) {
			rav.push_back(i);
		}
	}
	sort(rav.rbegin(), rav.rend());

	rav.setLabel(label);
	return rav;
}
/***********************************************************************/
SAbundVector SharedSAbundVector::getSAbundVector() {
	RAbundVector rav;
	SAbundVector sav;

	rav = getRAbundVector();
	sav = rav.getSAbundVector();
	return sav;

}

/***********************************************************************/

bool compareMembers(individual member, individual member2) {

	if (member.abundance < member2.abundance) {
		return true;
	}
	else {
		return false;
	}
}

/***********************************************************************/
SharedRAbundVector SharedSAbundVector::getSharedRAbundVector() {
	SharedRAbundVector rav;

	for (int i = 1;i < data.size();i++) {
		for (int j = 0;j < data[i].abundance;j++) {
			rav.push_back(i, data[i].group);
		}
	}
	sort(rav.rbegin(), rav.rend(), compareMembers);

	rav.setLabel(label);
	rav.setGroup(group);

	return rav;
}


/***********************************************************************/

SharedSAbundVector SharedSAbundVector::getSharedSAbundVector() {
	return *this;
}

/***********************************************************************/
SharedOrderVector SharedSAbundVector::getSharedOrderVector() {
	SharedRAbundVector rav;
	SharedOrderVector ov;

	rav = this->getSharedRAbundVector();
	ov = rav.getSharedOrderVector();

	ov.updateStats();

	return ov;
}
/***********************************************************************/

void SharedSAbundVector::clear() {
	numBins = 0;
	maxRank = 0;
	numSeqs = 0;
	data.clear();
}

/***********************************************************************/
OrderVector SharedSAbundVector::getOrderVector(map<string, int>* hold = NULL) {
	OrderVector ov;

	int binIndex = 0;

	for (int i = 1;i < data.size();i++) {
		for (int j = 0;j < data[i].abundance;j++) {
			for (int k = 0;k < i;k++) {
				ov.push_back(binIndex);
			}
			binIndex++;
		}
	}

	random_shuffle(ov.begin(), ov.end());

	ov.setLabel(label);
	ov.getNumBins();
	return ov;
}

/***********************************************************************/

