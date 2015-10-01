/*
 *  list.cpp
 *
 *
 *  Created by Pat Schloss on 8/8/08.
 *  Copyright 2008 Patrick D. Schloss. All rights reserved.
 *
 */


#include "sabundvector.hpp"
#include "rabundvector.hpp"
#include "ordervector.hpp"
#include "listvector.hpp"

 //sorts highest to lowest
 /***********************************************************************/
inline bool abundNamesSort(string left, string right) {

	int countLeft = 0;
	if (left != "") {
		countLeft = 1;
		for (int i = 0;i < left.size();i++) { if (left[i] == ',') { countLeft++; } }
	}

	int countRight = 0;
	if (right != "") {
		countRight = 1;
		for (int i = 0;i < right.size();i++) { if (right[i] == ',') { countRight++; } }
	}

	if (countLeft > countRight) {
		return true;
	}
	return false;
}
//sorts highest to lowest
/***********************************************************************/
inline bool abundNamesSort2(listCt left, listCt right) {
	if (left.bin == "") { return false; }
	if (right.bin == "") { return true; }
	if (left.binSize > right.binSize) {
		return true;
	}
	return false;
}


/***********************************************************************/

ListVector::ListVector() : DataVector(), maxRank(0), numBins(0), numSeqs(0) {}

/***********************************************************************/

ListVector::ListVector(int n) : DataVector(), data(n, ""), maxRank(0), numBins(0), numSeqs(0) {}

/***********************************************************************/

ListVector::ListVector(string id, vector<string> lv) : DataVector(id), data(lv) {
	for (int i = 0;i < data.size();i++) {
		if (data[i] != "") {
			int binSize = m->getNumNames(data[i]);
			numBins = i + 1;
			if (binSize > maxRank) { maxRank = binSize; }
			numSeqs += binSize;
		}
	}
}

/**********************************************************************/

ListVector::ListVector(ifstream& f) : DataVector(), maxRank(0), numBins(0), numSeqs(0) {
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

void ListVector::set(int binNumber, string seqNames) {
	int nNames_old = m->getNumNames(data[binNumber]);
	data[binNumber] = seqNames;
	int nNames_new = m->getNumNames(seqNames);

	if (nNames_old == 0) { numBins++; }
	if (nNames_new == 0) { numBins--; }
	if (nNames_new > maxRank) { maxRank = nNames_new; }

	numSeqs += (nNames_new - nNames_old);
}

/***********************************************************************/

string ListVector::get(int index) {
	return data[index];
}
/***********************************************************************/

void ListVector::setLabels(vector<string> labels) {
	binLabels = labels;
}

/***********************************************************************/
//could potentially end up with duplicate binlabel names with code below.
//we don't currently use them in a way that would do that.
//if you had a listfile that had been subsampled and then added to it, dup names would be possible.
vector<string> ListVector::getLabels() {

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

void ListVector::push_back(string seqNames) {
	data.push_back(seqNames);
	int nNames = m->getNumNames(seqNames);

	numBins++;

	if (nNames > maxRank) { maxRank = nNames; }

	numSeqs += nNames;
}

/***********************************************************************/

void ListVector::resize(int size) {
	data.resize(size);
}

/***********************************************************************/

int ListVector::size() {
	return data.size();
}
/***********************************************************************/

void ListVector::clear() {
	numBins = 0;
	maxRank = 0;
	numSeqs = 0;
	return data.clear();

}

/***********************************************************************/
void ListVector::printHeaders(ostream& output) {
	string snumBins = toString(numBins);
	string tagHeader = "Otu";
	if (m->sharedHeaderMode == "tax") { tagHeader = "PhyloType"; }
	output << "label\tnum" + tagHeader + "s";

	vector<string> theseLabels = getLabels();

	for (int i = 0; i < theseLabels.size(); i++) { //print original label for sorted by abundance otu
		output << '\t' << theseLabels[i];
	}

	output << endl;

	m->printedListHeaders = true;
}

/***********************************************************************/

void ListVector::print(ostream& output, map<string, int>& ct) {
	output << label << '\t' << numBins;


	vector<listCt> hold;
	for (int i = 0; i < data.size(); i++) {
		if (data[i] != "") {
			vector<string> binNames;
			string bin = data[i];
			m->splitAtComma(bin, binNames);
			int total = 0;
			for (int j = 0; j < binNames.size(); j++) {
				map<string, int>::iterator it = ct.find(binNames[j]);
				if (it == ct.end()) {
					LOG(LOGERROR) << "" + binNames[j] + " is not in your count table. Please correct.\n"; ctrlc_pressed = true;
				}
				else { total += ct[binNames[j]]; }
			}
			listCt temp(data[i], total);
			hold.push_back(temp);
		}
	}
	sort(hold.begin(), hold.end(), abundNamesSort2);

	for (int i = 0;i < hold.size();i++) {
		if (hold[i].bin != "") {
			output << '\t' << hold[i].bin;
		}
	}
	output << endl;

}

/***********************************************************************/

void ListVector::print(ostream& output) {
	output << label << '\t' << numBins;

	vector<string> hold = data;
	sort(hold.begin(), hold.end(), abundNamesSort);

	//find first non blank otu
	int start = 0;
	for (int i = 0;i < hold.size();i++) { if (hold[i] != "") { start = i; break; } }

	for (int i = start;i < hold.size();i++) {
		if (hold[i] != "") {
			output << '\t' << hold[i];
		}
	}
	output << endl;
}

/***********************************************************************/

RAbundVector ListVector::getRAbundVector() {
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

SAbundVector ListVector::getSAbundVector() {
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

OrderVector ListVector::getOrderVector(map<string, int>* orderMap = NULL) {

	try {
		if (orderMap == NULL) {
			OrderVector ov;

			for (int i = 0;i < data.size();i++) {
				int binSize = m->getNumNames(data[i]);
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
				int length = listOTU.size();

				string seqName = "";

				for (int j = 0;j < length;j++) {

					if (listOTU[j] != ',') {
						seqName += listOTU[j];
					}
					else {
						if (orderMap->count(seqName) == 0) {
							LOG(INFO) << seqName + " not found, check *.names file\n";
							exit(1);
						}

						ov.set((*orderMap)[seqName], i);
						seqName = "";
					}
				}

				if (orderMap->count(seqName) == 0) {
					LOG(INFO) << seqName + " not found, check *.names file\n";
					exit(1);
				}
				ov.set((*orderMap)[seqName], i);
			}

			ov.setLabel(label);
			ov.getNumBins();

			return ov;
		}
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ListVector, getOrderVector";
		exit(1);
	}
}

/***********************************************************************/
