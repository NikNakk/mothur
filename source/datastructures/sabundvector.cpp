/*
 *  sabund.cpp
 *
 *
 *  Created by Pat Schloss on 8/8/08.
 *  Copyright 2008 Patrick D. Schloss. All rights resesaved.
 *
 */

#include "sabundvector.hpp"
#include "calculator.h"

 /***********************************************************************/

SAbundVector::SAbundVector() : DataVector(), maxRank(0), numBins(0), numSeqs(0) {}

/***********************************************************************/

SAbundVector::SAbundVector(int size) : DataVector(), data(size, 0), maxRank(0), numBins(0), numSeqs(0) {}

/***********************************************************************/

SAbundVector::SAbundVector(string id, vector<int> sav) : DataVector(id), data(sav) {
	for (int i = 0;i < sav.size();i++) {
		if (data[i] != 0) { maxRank = i; }
		numSeqs += i*data[i];
		numBins += data[i];
	}
}

/***********************************************************************/

SAbundVector::SAbundVector(vector <int> dataVec, int mr, int nb, int ns) {
	data = dataVec;
	maxRank = mr;
	numBins = nb;
	numSeqs = ns;
}
/***********************************************************************/

SAbundVector::SAbundVector(ifstream& f) : DataVector(), maxRank(0), numBins(0), numSeqs(0) {
	int hold;
	f >> label >> hold;

	data.assign(hold + 1, 0);
	int inputData;

	for (int i = 1;i <= hold;i++) {
		f >> inputData;
		set(i, inputData);
	}

}


/***********************************************************************/

void SAbundVector::set(int sabund, int abundance) {

	int initSize = data[sabund];
	data[sabund] = abundance;

	if (sabund != 0) {
		numBins += (abundance - initSize);
	}

	numSeqs += sabund * (abundance - initSize);

	if (sabund > maxRank) { maxRank = sabund; }
}


/***********************************************************************/

int SAbundVector::get(int index) {
	return data[index];
}

/***********************************************************************/

void SAbundVector::push_back(int abundance) {
	data.push_back(abundance);

	maxRank++;

	numBins += abundance;

	numSeqs += (maxRank * abundance);
}
/***********************************************************************/

void SAbundVector::quicksort() {
	sort(data.rbegin(), data.rend());
}

/***********************************************************************/

int SAbundVector::sum() {
	VecCalc vecCalc;
	return vecCalc.sumElements(data);
}

/***********************************************************************/

void SAbundVector::resize(int size) {
	data.resize(size);
}

/***********************************************************************/

int SAbundVector::size() {
	return data.size();
}

/***********************************************************************/
void SAbundVector::print(string prefix, ostream& output) {

	output << prefix << '\t' << maxRank;

	for (int i = 1;i <= maxRank;i++) {
		output << '\t' << data[i];
	}
	output << endl;
}
/***********************************************************************/
void SAbundVector::clear() {
	numBins = 0;
	maxRank = 0;
	numSeqs = 0;
	data.clear();
}
/***********************************************************************/
void SAbundVector::print(ostream& output) {
	output << label << '\t' << maxRank;

	for (int i = 1;i <= maxRank;i++) {
		output << '\t' << data[i];
	}
	output << endl;
}

/**********************************************************************/
int SAbundVector::getNumBins() {
	//	if(needToUpdate == 1){	updateStats();	}
	return numBins;
}

/***********************************************************************/

int SAbundVector::getNumSeqs() {
	//	if(needToUpdate == 1){	updateStats();	}
	return numSeqs;
}

/***********************************************************************/

int SAbundVector::getMaxRank() {
	//	if(needToUpdate == 1){	updateStats();	}
	return maxRank;
}
/***********************************************************************/
RAbundVector SAbundVector::getRAbundVector() {
	RAbundVector rav;

	for (int i = 1;i < data.size();i++) {
		for (int j = 0;j < data[i];j++) {
			rav.push_back(i);
		}
	}
	sort(rav.rbegin(), rav.rend());

	rav.setLabel(label);
	return rav;
}

/***********************************************************************/

SAbundVector SAbundVector::getSAbundVector() {
	return *this;
}

/***********************************************************************/

OrderVector SAbundVector::getOrderVector(map<string, int>* hold = NULL) {
	OrderVector ov;

	int binIndex = 0;

	for (int i = 1;i < data.size();i++) {
		for (int j = 0;j < data[i];j++) {
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
