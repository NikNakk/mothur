/*
 *  raredisplay.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 11/18/08.
 *  Copyright 2008 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "raredisplay.h"

 /***********************************************************************/

void RareDisplay::init(string label) {
	this->label = label;
}

/***********************************************************************/

void RareDisplay::update(SAbundVector* rank) {
	int newNSeqs = rank->getNumSeqs();
	vector<double> data = estimate->getValues(rank);

	map<int, vector<double> >::iterator it = results.find(newNSeqs);
	if (it == results.end()) { //first iter for this count
		vector<double> temp;
		temp.push_back(data[0]);
		results[newNSeqs] = temp;
	}
	else {
		it->second.push_back(data[0]);
	}
}

/***********************************************************************/
void RareDisplay::update(vector<SharedRAbundVector*> shared, int numSeqs, int numGroupComb) {
	vector<double> data = estimate->getValues(shared);

	map<int, vector<double> >::iterator it = results.find(numSeqs);
	if (it == results.end()) { //first iter for this count
		vector<double> temp;
		temp.push_back(data[0]);
		results[numSeqs] = temp;
	}
	else {
		it->second.push_back(data[0]);
	}
}

/***********************************************************************/

void RareDisplay::reset() {
	nIters++;
}

/***********************************************************************/

void RareDisplay::close() {
	output->initFile(label);

	for (map<int, vector<double> >::iterator it = results.begin(); it != results.end(); it++) {

		vector<double> data(3, 0);

		sort((it->second).begin(), (it->second).end());

		vector<double> thisResults = it->second;
		double meanResults = m->getAverage(thisResults);
		data[0] = meanResults;
		data[1] = (it->second)[(int)(0.025*(nIters - 1))];
		data[2] = (it->second)[(int)(0.975*(nIters - 1))];
		//cout << nIters << '\t' << (int)(0.025*(nIters-1)) << '\t' << (int)(0.975*(nIters-1)) << endl;

		//cout << it->first << '\t' << data[1] << '\t' << data[2] << endl;

		output->output(it->first, data);
	}

	nIters = 1;
	results.clear();

	output->resetFile();
}
/***********************************************************************/

void RareDisplay::inputTempFiles(string filename) {
	ifstream in;
	File::openInputFile(filename, in);

	int thisIters, size;
	in >> thisIters >> size; File::gobble(in);
	nIters += thisIters;

	for (int i = 0; i < size; i++) {
		int tempCount;
		in >> tempCount; File::gobble(in);
		map<int, vector<double> >::iterator it = results.find(tempCount);
		if (it != results.end()) {
			for (int j = 0; j < thisIters; j++) {
				double value;
				in >> value; File::gobble(in);
				(it->second).push_back(value);
			}
		}
		else {
			vector<double> tempValues;
			for (int j = 0; j < thisIters; j++) {
				double value;
				in >> value; File::gobble(in);
				tempValues.push_back(value);
			}
			results[tempCount] = tempValues;
		}
	}

	in.close();
}

/***********************************************************************/

void RareDisplay::outputTempFiles(string filename) {
	ofstream out;
	File::openOutputFile(filename, out);

	out << nIters - 1 << '\t' << results.size() << endl;

	for (map<int, vector<double> >::iterator it = results.begin(); it != results.end(); it++) {
		out << it->first;
		for (int i = 0; i < (it->second).size(); i++) {
			out << '\t' << (it->second)[i];
		}
		out << endl;
	}

	out.close();
}


/***********************************************************************/

