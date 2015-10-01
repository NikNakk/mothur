//
//  fastqread.cpp
//  Mothur
//
//  Created by Sarah Westcott on 1/26/15.
//  Copyright (c) 2015 Schloss Lab. All rights reserved.
//

#include "fastqread.h"


/*******************************************************************************/
FastqRead::FastqRead() {
	format = "illumina1.8+"; name = ""; sequence = ""; scores.clear();
	//fill convert table - goes from solexa to sanger. Used fq_all2std.pl as a reference.
	for (int i = -64; i < 65; i++) {
		char temp = (char)((int)(33 + 10 * log(1 + pow(10, (i / 10.0))) / log(10) + 0.499));
		convertTable.push_back(temp);
	}
}
/*******************************************************************************/
FastqRead::FastqRead(string f) {
	format = f; name = ""; sequence = ""; scores.clear();
	//fill convert table - goes from solexa to sanger. Used fq_all2std.pl as a reference.
	for (int i = -64; i < 65; i++) {
		char temp = (char)((int)(33 + 10 * log(1 + pow(10, (i / 10.0))) / log(10) + 0.499));
		convertTable.push_back(temp);
	}
}
/*******************************************************************************/

FastqRead::FastqRead(string f, string n, string s, vector<int> sc) {
	format = f; name = n; sequence = s; scores = sc;
	//fill convert table - goes from solexa to sanger. Used fq_all2std.pl as a reference.
	for (int i = -64; i < 65; i++) {
		char temp = (char)((int)(33 + 10 * log(1 + pow(10, (i / 10.0))) / log(10) + 0.499));
		convertTable.push_back(temp);
	}
}
/*******************************************************************************/

FastqRead::FastqRead(ifstream& in, bool& ignore, string f) {

	ignore = false;
	format = f;
	//fill convert table - goes from solexa to sanger. Used fq_all2std.pl as a reference.
	for (int i = -64; i < 65; i++) {
		char temp = (char)((int)(33 + 10 * log(1 + pow(10, (i / 10.0))) / log(10) + 0.499));
		convertTable.push_back(temp);
	}

	//read sequence name
	string line = File::getline(in); File::gobble(in);
	vector<string> pieces = m->splitWhiteSpace(line);
	name = "";  if (pieces.size() != 0) { name = pieces[0]; }
	if (name == "") { LOG(WARNING) << "Blank fasta name, ignoring read." << '\n'; ignore = true; }
	else if (name[0] != '@') { LOG(WARNING) << "reading " + name + " expected a name with @ as a leading character, ignoring read." << '\n'; ignore = true; }
	else { name = name.substr(1); }

	//read sequence
	sequence = File::getline(in); File::gobble(in);
	if (sequence == "") { LOG(WARNING) << "missing sequence for " + name + ", ignoring."; ignore = true; }

	//read sequence name
	line = File::getline(in); File::gobble(in);
	pieces = m->splitWhiteSpace(line);
	string name2 = "";  if (pieces.size() != 0) { name2 = pieces[0]; }
	if (name2 == "") { LOG(WARNING) << "expected a name with + as a leading character, ignoring."; ignore = true; }
	else if (name2[0] != '+') { LOG(WARNING) << "reading " + name2 + " expected a name with + as a leading character, ignoring."; ignore = true; }
	else { name2 = name2.substr(1); if (name2 == "") { name2 = name; } }

	//read quality scores
	string quality = File::getline(in); File::gobble(in);
	if (quality == "") { LOG(WARNING) << "missing quality for " + name2 + ", ignoring."; ignore = true; }

	//sanity check sequence length and number of quality scores match
	if (name2 != "") { if (name != name2) { LOG(WARNING) << "names do not match. read " + name + " for fasta and " + name2 + " for quality, ignoring."; ignore = true; } }
	if (quality.length() != sequence.length()) { LOG(WARNING) << "Lengths do not match for sequence " + name + ". Read " + toString(sequence.length()) + " characters for fasta and " + toString(quality.length()) + " characters for quality scores, ignoring read."; ignore = true; }

	scores = convertQual(quality);
	m->checkName(name);

	if (app.isDebug) { LOG(DEBUG) << "" + name + " " + sequence + " " + quality + "\n"; }

}
//**********************************************************************************************************************
#ifdef USE_BOOST
FastqRead::FastqRead(boost::iostreams::filtering_istream& in, bool& ignore, string f) {

	ignore = false;
	format = f;
	//fill convert table - goes from solexa to sanger. Used fq_all2std.pl as a reference.
	for (int i = -64; i < 65; i++) {
		char temp = (char)((int)(33 + 10 * log(1 + pow(10, (i / 10.0))) / log(10) + 0.499));
		convertTable.push_back(temp);
	}

	//read sequence name
	string line = ""; std::getline(in, line); File::gobble(in);
	vector<string> pieces = m->splitWhiteSpace(line);
	name = "";  if (pieces.size() != 0) { name = pieces[0]; }
	if (name == "") { LOG(WARNING) << "Blank fasta name, ignoring read." << '\n'; ignore = true; }
	else if (name[0] != '@') { LOG(WARNING) << "reading " + name + " expected a name with @ as a leading character, ignoring read." << '\n'; ignore = true; }
	else { name = name.substr(1); }

	//read sequence
	std::getline(in, sequence); File::gobble(in);
	if (sequence == "") { LOG(WARNING) << "missing sequence for " + name + ", ignoring."; ignore = true; }

	//read sequence name
	line = ""; std::getline(in, line); File::gobble(in);
	pieces = m->splitWhiteSpace(line);
	string name2 = "";  if (pieces.size() != 0) { name2 = pieces[0]; }
	if (name2 == "") { LOG(WARNING) << "expected a name with + as a leading character, ignoring."; ignore = true; }
	else if (name2[0] != '+') { LOG(WARNING) << "reading " + name2 + " expected a name with + as a leading character, ignoring."; ignore = true; }
	else { name2 = name2.substr(1); if (name2 == "") { name2 = name; } }

	//read quality scores
	string quality = ""; std::getline(in, quality); File::gobble(in);
	if (quality == "") { LOG(WARNING) << "missing quality for " + name2 + ", ignoring."; ignore = true; }

	//sanity check sequence length and number of quality scores match
	if (name2 != "") { if (name != name2) { LOG(WARNING) << "names do not match. read " + name + " for fasta and " + name2 + " for quality, ignoring."; ignore = true; } }
	if (quality.length() != sequence.length()) { LOG(WARNING) << "Lengths do not match for sequence " + name + ". Read " + toString(sequence.length()) + " characters for fasta and " + toString(quality.length()) + " characters for quality scores, ignoring read."; ignore = true; }

	scores = convertQual(quality);
	m->checkName(name);

	if (app.isDebug) { LOG(DEBUG) << "" + name + " " + sequence + " " + quality + "\n"; }

}
#endif
//**********************************************************************************************************************
vector<int> FastqRead::convertQual(string qual) {
	vector<int> qualScores;
	bool negativeScores = false;

	for (int i = 0; i < qual.length(); i++) {

		int temp = 0;
		temp = int(qual[i]);
		if (format == "illumina") {
			temp -= 64; //char '@'
		}
		else if (format == "illumina1.8+") {
			temp -= int('!'); //char '!' //33
		}
		else if (format == "solexa") {
			temp = int(convertTable[temp]); //convert to sanger
			temp -= int('!'); //char '!' //33
		}
		else {
			temp -= int('!'); //char '!' //33
		}

		if (temp < 0) { negativeScores = true; temp = 0; }
		qualScores.push_back(temp);
	}

	if (negativeScores) { LOG(LOGERROR) << "finding negative quality scores, do you have the right format selected? http://en.wikipedia.org/wiki/FASTQ_format#Encoding \n";  ctrlc_pressed = true; }

	return qualScores;
}
//**********************************************************************************************************************
Sequence FastqRead::getSequence() {
	Sequence temp(name, sequence);
	return temp;
}
//**********************************************************************************************************************
QualityScores FastqRead::getQuality() {
	QualityScores temp(name, scores);
	return temp;
}
/*******************************************************************************/
