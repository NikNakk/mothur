/*
 *  qualityscores.cpp
 *  Mothur
 *
 *  Created by Pat Schloss on 7/12/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "qualityscores.h"

 /**************************************************************************************************/

QualityScores::QualityScores() {
	seqName = "";
	seqLength = -1;

}
/**************************************************************************************************/

QualityScores::QualityScores(string n, vector<int> s) {
	setName(n);
	setScores(s);
}
/**************************************************************************************************/

QualityScores::QualityScores(ifstream& qFile) {

	int score;
	seqName = getSequenceName(qFile); File::gobble(qFile);

	if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "'\n."; }

	if (!ctrlc_pressed) {
		string qScoreString = File::getline(qFile); File::gobble(qFile);

		if (app.isDebug) { LOG(DEBUG) << "scores = '" + qScoreString + "'\n."; }

		while (qFile.peek() != '>' && qFile.peek() != EOF) {
			if (ctrlc_pressed) { break; }
			string temp = File::getline(qFile); File::gobble(qFile);
			//if (app.isDebug) { LOG(DEBUG) << "scores = '" + temp + "'\n.";  }
			qScoreString += ' ' + temp;
		}
		//cout << "done reading " << endl;
		istringstream qScoreStringStream(qScoreString);
		int count = 0;
		while (!qScoreStringStream.eof()) {
			if (ctrlc_pressed) { break; }
			string temp;
			qScoreStringStream >> temp;  File::gobble(qScoreStringStream);

			//if (app.isDebug) { LOG(DEBUG) << "score " + toString(qScores.size()) + " = '" + temp + "'\n.";  }

			//check temp to make sure its a number
			if (!m->isContainingOnlyDigits(temp)) { LOG(LOGERROR) << "In sequence " + seqName + "'s quality scores, expected a number and got " + temp + ", setting score to 0." << '\n'; temp = "0"; }
			convert(temp, score);

			//cout << count << '\t' << score << endl;
			qScores.push_back(score);
			count++;
		}
	}

	seqLength = qScores.size();
	//cout << "seqlength = " << seqLength  << endl;


}
/**************************************************************************************************/
#ifdef USE_BOOST
QualityScores::QualityScores(boost::iostreams::filtering_istream& qFile) {

	int score;
	seqName = getSequenceName(qFile); File::gobble(qFile);

	if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "'\n."; }

	if (!ctrlc_pressed) {
		string qScoreString = ""; std::getline(qFile, qScoreString); File::gobble(qFile);

		if (app.isDebug) { LOG(DEBUG) << "scores = '" + qScoreString + "'\n."; }

		while (qFile.peek() != '>' && qFile.peek() != EOF) {
			if (ctrlc_pressed) { break; }
			string temp = ""; std::getline(qFile, temp); File::gobble(qFile);
			//if (app.isDebug) { LOG(DEBUG) << "scores = '" + temp + "'\n.";  }
			qScoreString += ' ' + temp;
		}
		//cout << "done reading " << endl;
		istringstream qScoreStringStream(qScoreString);
		int count = 0;
		while (!qScoreStringStream.eof()) {
			if (ctrlc_pressed) { break; }
			string temp;
			qScoreStringStream >> temp;  File::gobble(qScoreStringStream);

			//if (app.isDebug) { LOG(DEBUG) << "score " + toString(qScores.size()) + " = '" + temp + "'\n.";  }

			//check temp to make sure its a number
			if (!m->isContainingOnlyDigits(temp)) { LOG(LOGERROR) << "In sequence " + seqName + "'s quality scores, expected a number and got " + temp + ", setting score to 0." << '\n'; temp = "0"; }
			convert(temp, score);

			//cout << count << '\t' << score << endl;
			qScores.push_back(score);
			count++;
		}
	}

	seqLength = qScores.size();
	//cout << "seqlength = " << seqLength  << endl;


}
#endif
/**************************************************************************************************/

int QualityScores::read(ifstream& qFile) {
	int score;
	seqName = getSequenceName(qFile); File::gobble(qFile);

	if (app.isDebug) { LOG(DEBUG) << "name = '" + seqName + "'\n."; }

	if (!ctrlc_pressed) {
		string qScoreString = File::getline(qFile); File::gobble(qFile);

		if (app.isDebug) { LOG(DEBUG) << "scores = '" + qScoreString + "'\n."; }

		while (qFile.peek() != '>' && qFile.peek() != EOF) {
			if (ctrlc_pressed) { break; }
			string temp = File::getline(qFile); File::gobble(qFile);
			//if (app.isDebug) { LOG(DEBUG) << "scores = '" + temp + "'\n.";  }
			qScoreString += ' ' + temp;
		}
		//cout << "done reading " << endl;
		istringstream qScoreStringStream(qScoreString);
		int count = 0;
		while (!qScoreStringStream.eof()) {
			if (ctrlc_pressed) { break; }
			string temp;
			qScoreStringStream >> temp;  File::gobble(qScoreStringStream);

			//if (app.isDebug) { LOG(DEBUG) << "score " + toString(qScores.size()) + " = '" + temp + "'\n.";  }

			//check temp to make sure its a number
			if (!m->isContainingOnlyDigits(temp)) { LOG(LOGERROR) << "In sequence " + seqName + "'s quality scores, expected a number and got " + temp + ", setting score to 0." << '\n'; temp = "0"; }
			convert(temp, score);

			//cout << count << '\t' << score << endl;
			qScores.push_back(score);
			count++;
		}
	}

	seqLength = qScores.size();
	//cout << "seqlength = " << seqLength  << endl;

	return seqLength;


}

//********************************************************************************************************************
string QualityScores::getSequenceName(ifstream& qFile) {
	string name = "";

	qFile >> name;
	File::getline(qFile);

	if (name.length() != 0) {

		name = name.substr(1);

		m->checkName(name);

	}
	else { LOG(INFO) << "Error in reading your qfile, at position " + toString(qFile.tellg()) + ". Blank name." << '\n'; ctrlc_pressed = true; }

	return name;
}
//********************************************************************************************************************
#ifdef USE_BOOST
string QualityScores::getSequenceName(boost::iostreams::filtering_istream& qFile) {
	string name = "";

	qFile >> name; string temp;
	std::getline(qFile, temp);

	if (name.length() != 0) {

		name = name.substr(1);

		m->checkName(name);

	}
	else { LOG(INFO) << "Error in reading your qfile, at position " + toString(qFile.tellg()) + ". Blank name." << '\n'; ctrlc_pressed = true; }

	return name;
}
#endif
//********************************************************************************************************************
void QualityScores::setName(string name) {

	m->checkName(name);
	seqName = name;
}
/**************************************************************************************************/

string QualityScores::getName() {

	try {
		return seqName;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in QualityScores, getName";
		exit(1);
	}
}

/**************************************************************************************************/

void QualityScores::printQScores(ofstream& qFile) {

	double aveQScore = calculateAverage(false);

	qFile << '>' << seqName << '\t' << aveQScore << endl;

	for (int i = 0;i < seqLength;i++) {
		qFile << qScores[i] << ' ';
	}
	qFile << endl;

}
/**************************************************************************************************/

void QualityScores::printQScores(ostream& qFile) {

	double aveQScore = calculateAverage(false);

	qFile << '>' << seqName << '\t' << aveQScore << endl;

	for (int i = 0;i < seqLength;i++) {
		qFile << qScores[i] << ' ';
	}
	qFile << endl;

}


/**************************************************************************************************/

void QualityScores::trimQScores(int start, int end) {
	vector<int> hold;


	//cout << seqName << '\t' << start << '\t' << end << '\t' << qScores.size() << endl;
	//for (int i = 0; i < qScores.size(); i++) { cout << qScores[i] << end; }
	if (end == -1) {
		hold = vector<int>(qScores.begin() + start, qScores.end());
		qScores = hold;
	}
	if (start == -1) {
		if (qScores.size() > end) {
			hold = vector<int>(qScores.begin(), qScores.begin() + end);
			qScores = hold;
		}
	}

	seqLength = qScores.size();
}

/**************************************************************************************************/

void QualityScores::flipQScores() {

	vector<int> temp = qScores;
	for (int i = 0;i < seqLength;i++) {
		qScores[seqLength - i - 1] = temp[i];
	}

}

/**************************************************************************************************/

bool QualityScores::stripQualThreshold(Sequence& sequence, double qThreshold) {
	string rawSequence = sequence.getUnaligned();
	int seqLength = sequence.getNumBases();

	if (seqName != sequence.getName()) {
		LOG(INFO) << "sequence name mismatch btwn fasta: " + sequence.getName() + " and qual file: " + seqName;
		LOG(INFO) << "";	ctrlc_pressed = true;
	}

	int end;
	for (int i = 0;i < seqLength;i++) {
		end = i;
		if (qScores[i] < qThreshold) {
			break;
		}
	}

	//every score passed
	if (end == (seqLength - 1)) { end = seqLength; }

	sequence.setUnaligned(rawSequence.substr(0, end));
	trimQScores(-1, end);

	return 1;

}

/**************************************************************************************************/

bool QualityScores::stripQualRollingAverage(Sequence& sequence, double qThreshold, bool logTransform) {
	string rawSequence = sequence.getUnaligned();
	int seqLength = sequence.getNumBases();

	if (seqName != sequence.getName()) {
		LOG(INFO) << "sequence name mismatch btwn fasta: " + sequence.getName() + " and qual file: " + seqName;
		LOG(INFO) << "";
	}

	int end = -1;
	double rollingSum = 0.0000;
	double value = 0.0;

	for (int i = 0;i < seqLength;i++) {

		if (logTransform) {
			rollingSum += (double)pow(10.0, qScores[i]);
			value = log10(rollingSum / (double)(i + 1));

		} //Sum 10^Q
		else {
			rollingSum += (double)qScores[i];
			value = rollingSum / (double)(i + 1);
		}


		if (value < qThreshold) {
			end = i;
			break;
		}
	}

	if (end == -1) { end = seqLength; }


	sequence.setUnaligned(rawSequence.substr(0, end));
	trimQScores(-1, end);


	return 1;

}

/**************************************************************************************************/

bool QualityScores::stripQualWindowAverage(Sequence& sequence, int stepSize, int windowSize, double qThreshold, bool logTransform) {
	string rawSequence = sequence.getUnaligned();
	int seqLength = sequence.getNumBases();

	if (seqName != sequence.getName()) {
		LOG(INFO) << "sequence name mismatch between fasta: " + sequence.getName() + " and qual file: " + seqName;
		LOG(INFO) << "";
	}

	int end = windowSize;
	int start = 0;

	if (seqLength < windowSize) { return 0; }

	while ((start + windowSize) < seqLength) {
		double windowSum = 0.0000;

		for (int i = start;i < end;i++) {
			if (logTransform) { windowSum += pow(10.0, qScores[i]); }
			else { windowSum += qScores[i]; }
		}
		double windowAverage = 0.0;
		if (logTransform) { windowAverage = log10(windowSum / (double)(end - start)); }
		else { windowAverage = windowSum / (double)(end - start); }

		if (windowAverage < qThreshold) {
			end = end - stepSize;
			break;
		}

		start += stepSize;
		end = start + windowSize;

		if (end >= seqLength) { end = seqLength; }

	}

	if (end == -1) { end = seqLength; }

	//failed first window
	if (end < windowSize) { return 0; }

	sequence.setUnaligned(rawSequence.substr(0, end));
	trimQScores(-1, end);

	return 1;

}

/**************************************************************************************************/

double QualityScores::calculateAverage(bool logTransform) {

	double aveQScore = 0.0000;

	for (int i = 0;i < seqLength;i++) {
		if (logTransform) { aveQScore += pow(10.0, qScores[i]); }
		else { aveQScore += qScores[i]; }
	}

	if (logTransform) { aveQScore = log10(aveQScore / (double)seqLength); }
	else { aveQScore /= (double)seqLength; }

	return aveQScore;
}

/**************************************************************************************************/

bool QualityScores::cullQualAverage(Sequence& sequence, double qAverage, bool logTransform) {
	string rawSequence = sequence.getUnaligned();
	bool success = 0;	//guilty until proven innocent

	if (seqName != sequence.getName()) {
		LOG(INFO) << "sequence name mismatch btwn fasta: " + sequence.getName() + " and qual file: " + seqName;
		LOG(INFO) << "";
	}

	double aveQScore = calculateAverage(logTransform);

	if (app.isDebug) { LOG(DEBUG) << "" + sequence.getName() + " average = " + toString(aveQScore) + "\n"; }

	if (aveQScore >= qAverage) { success = 1; }
	else { success = 0; }

	return success;
}

/**************************************************************************************************/

void QualityScores::updateQScoreErrorMap(map<char, vector<int> >& qualErrorMap, string errorSeq, int start, int stop, int weight) {

	int seqLength = errorSeq.size();

	int qIndex = start - 1;

	for (int i = 0;i < seqLength;i++) {

		if (errorSeq[i] == 'm') { qualErrorMap['m'][qScores[qIndex]] += weight; }
		else if (errorSeq[i] == 's') { qualErrorMap['s'][qScores[qIndex]] += weight; }
		else if (errorSeq[i] == 'i') { qualErrorMap['i'][qScores[qIndex]] += weight; }
		else if (errorSeq[i] == 'a') { qualErrorMap['a'][qScores[qIndex]] += weight;	/*if(qScores[qIndex] != 0){	cout << qIndex << '\t';		}*/ }
		else if (errorSeq[i] == 'd') {	/*	there are no qScores for deletions	*/ }

		if (errorSeq[i] != 'd') { qIndex++; }

		if (qIndex > stop) { break; }
	}
}

/**************************************************************************************************/

void QualityScores::updateForwardMap(vector<vector<int> >& forwardMap, int start, int stop, int weight) {

	int index = 0;
	for (int i = start - 1;i < stop;i++) {
		forwardMap[index++][qScores[i]] += weight;
	}

}

/**************************************************************************************************/

void QualityScores::updateReverseMap(vector<vector<int> >& reverseMap, int start, int stop, int weight) {
	try {

		int index = 0;
		for (int i = stop - 1;i >= start - 1;i--) {
			reverseMap[index++][qScores[i]] += weight;
		}

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in QualityScores, updateReverseMap";
		exit(1);
	}
}

/**************************************************************************************************/
