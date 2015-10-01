//
//  oligos.cpp
//  Mothur
//
//  Created by Sarah Westcott on 4/4/14.
//  Copyright (c) 2014 Schloss Lab. All rights reserved.
//

#include "oligos.h"

/**************************************************************************************************/

Oligos::Oligos(string o) {
	hasPPrimers = false; hasPBarcodes = false; pairedOligos = false; reversePairs = true;
	indexBarcode = 0; indexPairedBarcode = 0; indexPrimer = 0; indexPairedPrimer = 0;
	oligosfile = o;
	readOligos();
	if (pairedOligos) {
		numBarcodes = pairedBarcodes.size();
		numFPrimers = pairedPrimers.size();
	}
	else {
		numBarcodes = barcodes.size();
		numFPrimers = primers.size();
	}
}
/**************************************************************************************************/

Oligos::Oligos() {
	hasPPrimers = false; hasPBarcodes = false; pairedOligos = false; reversePairs = true;
	indexBarcode = 0; indexPairedBarcode = 0; indexPrimer = 0; indexPairedPrimer = 0;
	numFPrimers = 0; numBarcodes = 0;
}
/**************************************************************************************************/
int Oligos::read(string o) {
	oligosfile = o;
	readOligos();
	if (pairedOligos) {
		numBarcodes = pairedBarcodes.size();
		numFPrimers = pairedPrimers.size();
	}
	else {
		numBarcodes = barcodes.size();
		numFPrimers = primers.size();
	}
	return 0;
}
/**************************************************************************************************/
int Oligos::read(string o, bool reverse) {
	oligosfile = o;
	reversePairs = reverse;
	readOligos();
	if (pairedOligos) {
		numBarcodes = pairedBarcodes.size();
		numFPrimers = pairedPrimers.size();
	}
	else {
		numBarcodes = barcodes.size();
		numFPrimers = primers.size();
	}
	return 0;
}
//***************************************************************************************************************

int Oligos::readOligos() {
	ifstream inOligos;
	File::openInputFile(oligosfile, inOligos);

	string type, oligo, roligo, group;

	while (!inOligos.eof()) {

		inOligos >> type;

		if (app.isDebug) { LOG(DEBUG) << "reading type - " + type + ".\n"; }

		if (type[0] == '#') {
			while (!inOligos.eof()) { char c = inOligos.get();  if (c == 10 || c == 13) { break; } } // get rest of line if there's any crap there
			File::gobble(inOligos);
		}
		else {
			File::gobble(inOligos);
			//make type case insensitive
			for (int i = 0;i < type.length();i++) { type[i] = toupper(type[i]); }

			inOligos >> oligo;

			if (app.isDebug) { LOG(DEBUG) << "reading - " + oligo + ".\n"; }

			for (int i = 0;i < oligo.length();i++) {
				oligo[i] = toupper(oligo[i]);
				if (oligo[i] == 'U') { oligo[i] = 'T'; }
			}

			if (type == "FORWARD") {
				group = "";

				// get rest of line in case there is a primer name
				while (!inOligos.eof()) {
					char c = inOligos.get();
					if (c == 10 || c == 13 || c == -1) { break; }
					else if (c == 32 || c == 9) { ; } //space or tab
					else { group += c; }
				}

				//check for repeat barcodes
				map<string, int>::iterator itPrime = primers.find(oligo);
				if (itPrime != primers.end()) { LOG(WARNING) << "primer " + oligo + " is in your oligos file already, disregarding." << '\n'; }
				else {
					if (app.isDebug) { if (group != "") { LOG(DEBUG) << "reading group " + group + ".\n"; } else { LOG(DEBUG) << "no group for primer " + oligo + ".\n"; } }

					primers[oligo] = indexPrimer; indexPrimer++;
					primerNameVector.push_back(group);
				}
			}
			else if (type == "PRIMER") {
				File::gobble(inOligos);

				inOligos >> roligo;

				for (int i = 0;i < roligo.length();i++) {
					roligo[i] = toupper(roligo[i]);
					if (roligo[i] == 'U') { roligo[i] = 'T'; }
				}
				if (reversePairs) { roligo = reverseOligo(roligo); }
				group = "";

				// get rest of line in case there is a primer name
				while (!inOligos.eof()) {
					char c = inOligos.get();
					if (c == 10 || c == 13 || c == -1) { break; }
					else if (c == 32 || c == 9) { ; } //space or tab
					else { group += c; }
				}

				oligosPair newPrimer(oligo, roligo);

				if (app.isDebug) { LOG(DEBUG) << "primer pair " + newPrimer.forward + " " + newPrimer.reverse + ", and group = " + group + ".\n"; }

				//check for repeat barcodes
				string tempPair = oligo + roligo;
				if (uniquePrimers.count(tempPair) != 0) { LOG(INFO) << "primer pair " + newPrimer.forward + " " + newPrimer.reverse + " is in your oligos file already, disregarding." << '\n'; }
				else {
					uniquePrimers.insert(tempPair);

					if (app.isDebug) { if (group != "") { LOG(DEBUG) << "reading group " + group + ".\n"; } else { LOG(DEBUG) << "no group for primer pair " + newPrimer.forward + " " + newPrimer.reverse + ".\n"; } }

					pairedPrimers[indexPairedPrimer] = newPrimer; indexPairedPrimer++;
					primerNameVector.push_back(group);
					hasPPrimers = true;
				}
			}
			else if (type == "REVERSE") {
				string oligoRC = reverseOligo(oligo);
				revPrimer.push_back(oligoRC);
			}
			else if (type == "BARCODE") {
				inOligos >> group;

				//barcode lines can look like   BARCODE   atgcatgc   groupName  - for 454 seqs
				//or                            BARCODE   atgcatgc   atgcatgc    groupName  - for illumina data that has forward and reverse info

				string temp = "";
				while (!inOligos.eof()) {
					char c = inOligos.get();
					if (c == 10 || c == 13 || c == -1) { break; }
					else if (c == 32 || c == 9) { ; } //space or tab
					else { temp += c; }
				}

				//then this is illumina data with 4 columns
				if (temp != "") {
					hasPBarcodes = true;
					string reverseBarcode = group; //reverseOligo(group); //reverse barcode
					group = temp;

					for (int i = 0;i < reverseBarcode.length();i++) {
						reverseBarcode[i] = toupper(reverseBarcode[i]);
						if (reverseBarcode[i] == 'U') { reverseBarcode[i] = 'T'; }
					}

					if (reversePairs) { reverseBarcode = reverseOligo(reverseBarcode); }
					oligosPair newPair(oligo, reverseBarcode);

					if (app.isDebug) { LOG(DEBUG) << "barcode pair " + newPair.forward + " " + newPair.reverse + ", and group = " + group + ".\n"; }

					//check for repeat barcodes
					string tempPair = oligo + reverseBarcode;
					if (uniqueBarcodes.count(tempPair) != 0) { LOG(INFO) << "barcode pair " + newPair.forward + " " + newPair.reverse + " is in your oligos file already, disregarding." << '\n'; }
					else {
						uniqueBarcodes.insert(tempPair);
						pairedBarcodes[indexPairedBarcode] = newPair; indexPairedBarcode++;
						barcodeNameVector.push_back(group);
					}
				}
				else {
					//check for repeat barcodes
					map<string, int>::iterator itBar = barcodes.find(oligo);
					if (itBar != barcodes.end()) { LOG(WARNING) << "barcode " + oligo + " is in your oligos file already, disregarding." << '\n'; }
					else {
						barcodes[oligo] = indexBarcode; indexBarcode++;
						barcodeNameVector.push_back(group);
					}
				}
			}
			else if (type == "LINKER") {
				linker.push_back(oligo);
			}
			else if (type == "SPACER") {
				spacer.push_back(oligo);
			}
			else { LOG(WARNING) << "" + type + " is not recognized as a valid type. Choices are forward, reverse, and barcode. Ignoring " + oligo + "." << '\n'; }
		}
		File::gobble(inOligos);
	}
	inOligos.close();

	if ((linker.size() == 0) && (spacer.size() == 0) && (pairedBarcodes.size() == 0) && (barcodes.size() == 0) && (pairedPrimers.size() == 0) && (primers.size() == 0) && (revPrimer.size() == 0)) { LOG(LOGERROR) << "invalid oligos file, quitting.\n"; ctrlc_pressed = true; return 0; }

	if (hasPBarcodes || hasPPrimers) {
		pairedOligos = true;
		if ((primers.size() != 0) || (barcodes.size() != 0) || (linker.size() != 0) || (spacer.size() != 0) || (revPrimer.size() != 0)) { ctrlc_pressed = true;  LOG(LOGERROR) << "cannot mix paired primers and barcodes with non paired or linkers and spacers, quitting." << '\n';  return 0; }
	}


	//add in potential combos
	if (barcodeNameVector.size() == 0) {
		if (pairedOligos) {
			oligosPair newPair("", "");
			pairedBarcodes[0] = newPair;
		}
		else {
			barcodes[""] = 0;
		}
		barcodeNameVector.push_back("");
	}

	if (primerNameVector.size() == 0) {
		if (pairedOligos) {
			oligosPair newPair("", "");
			pairedPrimers[0] = newPair;
		}
		else {
			primers[""] = 0;
		}
		primerNameVector.push_back("");
	}


	if (pairedOligos) {
		for (map<int, oligosPair>::iterator itBar = pairedBarcodes.begin();itBar != pairedBarcodes.end();itBar++) {
			for (map<int, oligosPair>::iterator itPrimer = pairedPrimers.begin();itPrimer != pairedPrimers.end(); itPrimer++) {

				string primerName = primerNameVector[itPrimer->first];
				string barcodeName = barcodeNameVector[itBar->first];

				if (app.isDebug) { LOG(DEBUG) << "primerName = " + primerName + " barcodeName = " + barcodeName + "\n"; }

				if ((primerName == "ignore") || (barcodeName == "ignore")) { if (app.isDebug) { LOG(DEBUG) << "in ignore. \n"; } } //do nothing
				else if ((primerName == "") && (barcodeName == "")) { if (app.isDebug) { LOG(DEBUG) << "in blank. \n"; } } //do nothing
				else {
					string comboGroupName = "";
					string fastqFileName = "";

					if (primerName == "") {
						comboGroupName = barcodeNameVector[itBar->first];
					}
					else {
						if (barcodeName == "") {
							comboGroupName = primerNameVector[itPrimer->first];
						}
						else {
							comboGroupName = barcodeNameVector[itBar->first] + "." + primerNameVector[itPrimer->first];
						}
					}

					if (app.isDebug) { LOG(DEBUG) << "comboGroupName = " + comboGroupName + "\n"; }

					uniqueNames.insert(comboGroupName);

					map<string, vector<string> >::iterator itGroup2Barcode = Group2Barcode.find(comboGroupName);
					if (itGroup2Barcode == Group2Barcode.end()) {
						vector<string> tempBarcodes; tempBarcodes.push_back((itBar->second).forward + "." + (itBar->second).reverse);
						Group2Barcode[comboGroupName] = tempBarcodes;
					}
					else {
						Group2Barcode[comboGroupName].push_back((itBar->second).forward + "." + (itBar->second).reverse);
					}

					itGroup2Barcode = Group2Primer.find(comboGroupName);
					if (itGroup2Barcode == Group2Primer.end()) {
						vector<string> tempPrimers; tempPrimers.push_back((itPrimer->second).forward + "." + (itPrimer->second).reverse);
						Group2Primer[comboGroupName] = tempPrimers;
					}
					else {
						Group2Primer[comboGroupName].push_back((itPrimer->second).forward + "." + (itPrimer->second).reverse);
					}
				}
			}
		}
	}
	else {
		for (map<string, int>::iterator itBar = barcodes.begin();itBar != barcodes.end();itBar++) {
			for (map<string, int>::iterator itPrimer = primers.begin();itPrimer != primers.end(); itPrimer++) {

				string primerName = primerNameVector[itPrimer->second];
				string barcodeName = barcodeNameVector[itBar->second];

				if ((primerName == "ignore") || (barcodeName == "ignore")) {} //do nothing
				else if ((primerName == "") && (barcodeName == "")) {} //do nothing
				else {
					string comboGroupName = "";
					string fastqFileName = "";

					if (primerName == "") {
						comboGroupName = barcodeNameVector[itBar->second];
					}
					else {
						if (barcodeName == "") {
							comboGroupName = primerNameVector[itPrimer->second];
						}
						else {
							comboGroupName = barcodeNameVector[itBar->second] + "." + primerNameVector[itPrimer->second];
						}
					}
					uniqueNames.insert(comboGroupName);

					map<string, vector<string> >::iterator itGroup2Barcode = Group2Barcode.find(comboGroupName);
					if (itGroup2Barcode == Group2Barcode.end()) {
						vector<string> tempBarcodes; tempBarcodes.push_back(itBar->first);
						Group2Barcode[comboGroupName] = tempBarcodes;
					}
					else {
						Group2Barcode[comboGroupName].push_back(itBar->first);
					}

					itGroup2Barcode = Group2Primer.find(comboGroupName);
					if (itGroup2Barcode == Group2Primer.end()) {
						vector<string> tempPrimers; tempPrimers.push_back(itPrimer->first);
						Group2Primer[comboGroupName] = tempPrimers;
					}
					else {
						Group2Primer[comboGroupName].push_back(itPrimer->first);
					}
				}
			}
		}
	}


	if (app.isDebug) { int count = 0; for (set<string>::iterator it = uniqueNames.begin(); it != uniqueNames.end(); it++) { LOG(DEBUG) << "" + toString(count) + " groupName = " + *it + "\n"; count++; } }

	Groups.clear();
	for (set<string>::iterator it = uniqueNames.begin(); it != uniqueNames.end(); it++) { Groups.push_back(*it); }

	return 0;
}
//********************************************************************/
vector<string> Oligos::getBarcodes(string groupName) {
	vector<string> thisGroupsBarcodes;

	map<string, vector<string> >::iterator it = Group2Barcode.find(groupName);

	if (it == Group2Barcode.end()) {
		LOG(LOGERROR) << "no barcodes found for group " + groupName + ".\n"; ctrlc_pressed = true;
	}
	else { thisGroupsBarcodes = it->second; }

	return thisGroupsBarcodes;
}
//********************************************************************/
vector<string> Oligos::getPrimers(string groupName) {
	vector<string> thisGroupsPrimers;

	map<string, vector<string> >::iterator it = Group2Primer.find(groupName);

	if (it == Group2Primer.end()) {
		LOG(LOGERROR) << "no primers found for group " + groupName + ".\n"; ctrlc_pressed = true;
	}
	else { thisGroupsPrimers = it->second; }

	return thisGroupsPrimers;
}
//********************************************************************/
//can't have paired and unpaired so this function will either run the paired map or the unpaired
map<int, oligosPair> Oligos::getReorientedPairedPrimers() {
	map<int, oligosPair> rpairedPrimers;

	for (map<int, oligosPair>::iterator it = pairedPrimers.begin(); it != pairedPrimers.end(); it++) {
		string forward = (it->second).reverse;
		if (reversePairs) { forward = reverseOligo(forward); }
		string reverse = (it->second).forward;
		if (reversePairs) { reverse = reverseOligo(reverse); }
		oligosPair tempPair(forward, reverse); //reversePrimer, rc ForwardPrimer
		rpairedPrimers[it->first] = tempPair;
	}


	for (map<string, int>::iterator it = primers.begin(); it != primers.end(); it++) {
		oligosPair tempPair("", reverseOligo((it->first))); //reverseBarcode, rc ForwardBarcode
		rpairedPrimers[it->second] = tempPair;
	}

	return rpairedPrimers;
}
//********************************************************************/
//can't have paired and unpaired so this function will either run the paired map or the unpaired
map<int, oligosPair> Oligos::getReorientedPairedBarcodes() {
	map<int, oligosPair> rpairedBarcodes;

	for (map<int, oligosPair>::iterator it = pairedBarcodes.begin(); it != pairedBarcodes.end(); it++) {
		string forward = (it->second).reverse;
		if (reversePairs) { forward = reverseOligo(forward); }
		string reverse = (it->second).forward;
		if (reversePairs) { reverse = reverseOligo(reverse); }
		oligosPair tempPair(forward, reverse); //reversePrimer, rc ForwardPrimer
		rpairedBarcodes[it->first] = tempPair;
	}

	for (map<string, int>::iterator it = barcodes.begin(); it != barcodes.end(); it++) {
		oligosPair tempPair("", reverseOligo((it->first))); //reverseBarcode, rc ForwardBarcode
		rpairedBarcodes[it->second] = tempPair;
	}

	return rpairedBarcodes;
}

//********************************************************************/
string Oligos::reverseOligo(string oligo) {

	if (oligo == "NONE") { return "NONE"; }

	string reverse = "";

	for (int i = oligo.length() - 1;i >= 0;i--) {

		if (oligo[i] == 'A') { reverse += 'T'; }
		else if (oligo[i] == 'T') { reverse += 'A'; }
		else if (oligo[i] == 'U') { reverse += 'A'; }

		else if (oligo[i] == 'G') { reverse += 'C'; }
		else if (oligo[i] == 'C') { reverse += 'G'; }

		else if (oligo[i] == 'R') { reverse += 'Y'; }
		else if (oligo[i] == 'Y') { reverse += 'R'; }

		else if (oligo[i] == 'M') { reverse += 'K'; }
		else if (oligo[i] == 'K') { reverse += 'M'; }

		else if (oligo[i] == 'W') { reverse += 'W'; }
		else if (oligo[i] == 'S') { reverse += 'S'; }

		else if (oligo[i] == 'B') { reverse += 'V'; }
		else if (oligo[i] == 'V') { reverse += 'B'; }

		else if (oligo[i] == 'D') { reverse += 'H'; }
		else if (oligo[i] == 'H') { reverse += 'D'; }

		else { reverse += 'N'; }
	}


	return reverse;
}
//********************************************************************/
string Oligos::getBarcodeName(int index) {
	string name = "";

	if ((index >= 0) && (index < barcodeNameVector.size())) { name = barcodeNameVector[index]; }

	return name;
}
//********************************************************************/
string Oligos::getPrimerName(int index) {
	string name = "";

	if ((index >= 0) && (index < primerNameVector.size())) { name = primerNameVector[index]; }

	return name;
}
//********************************************************************/
string Oligos::getGroupName(int barcodeIndex, int primerIndex) {

	string thisGroup = "";
	if (numBarcodes != 0) {
		thisGroup = getBarcodeName(barcodeIndex);
		if (numFPrimers != 0) {
			if (getPrimerName(primerIndex) != "") {
				if (thisGroup != "") {
					thisGroup += "." + getPrimerName(primerIndex);
				}
				else {
					thisGroup = getPrimerName(primerIndex);
				}
			}
		}
	}

	return thisGroup;
}

/**************************************************************************************************/

