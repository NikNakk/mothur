/*
 *  ccode.cpp
 *  Mothur
 *
 *  Created by westcott on 8/24/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "ccode.h"
#include "ignoregaps.h"
#include "eachgapdist.h"
#include "filehandling/file.h"

 //***************************************************************************************************************
Ccode::Ccode(string filename, string temp, bool f, string mask, int win, int numW, string o) : Chimera() {
	try {

		fastafile = filename;
		outputDir = o;
		templateFileName = temp;  templateSeqs = readSeqs(temp);
		setMask(mask);
		filter = f;
		window = win;
		numWanted = numW;

		distCalc = new eachGapDist();
		decalc = new DeCalculator();

		mapInfo = outputDir + File::getRootName(File::getSimpleName(fastafile)) + "mapinfo";

		ofstream out2;
		File::openOutputFile(mapInfo, out2);

		out2 << "Place in masked, filtered and trimmed sequence\tPlace in original alignment" << endl;
		out2.close();

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in Ccode, Ccode";
		exit(1);
	}
}
//***************************************************************************************************************
Ccode::~Ccode() {
	delete distCalc;
	delete decalc;
}
//***************************************************************************************************************
Sequence Ccode::print(ostream& out, ostream& outAcc) {

	ofstream out2;
	File::openOutputFileAppend(mapInfo, out2);

	out2 << querySeq->getName() << endl;
	for (it = spotMap.begin(); it != spotMap.end(); it++) {
		out2 << it->first << '\t' << it->second << endl;
	}
	out2.close();
	out << querySeq->getName() << endl << endl << "Reference sequences used and distance to query:" << endl;

	for (int j = 0; j < closest.size(); j++) {
		out << closest[j].seq->getName() << '\t' << closest[j].dist << endl;
	}
	out << endl << endl;

	//for each window
	//window mapping info.
	out << "Mapping information: ";
	//you mask and did not filter
	if ((seqMask != "") && (!filter)) { out << "mask and trim."; }

	//you filtered and did not mask
	if ((seqMask == "") && (filter)) { out << "filter and trim."; }

	//you masked and filtered
	if ((seqMask != "") && (filter)) { out << "mask, filter and trim."; }

	out << endl << "Window\tStartPos\tEndPos" << endl;
	it = trim.begin();
	for (int k = 0; k < windows.size() - 1; k++) {
		out << k + 1 << '\t' << spotMap[windows[k] - it->first] << '\t' << spotMap[windows[k] - it->first + windowSizes] << endl;
	}

	out << windows.size() << '\t' << spotMap[windows[windows.size() - 1] - it->first] << '\t' << spotMap[it->second - it->first - 1] << endl;
	out << endl;
	out << "Window\tAvgQ\t(sdQ)\tAvgR\t(sdR)\tRatio\tAnova" << endl;
	for (int k = 0; k < windows.size(); k++) {
		float ds = averageQuery[k] / averageRef[k];
		out << k + 1 << '\t' << averageQuery[k] << '\t' << sdQuery[k] << '\t' << averageRef[k] << '\t' << sdRef[k] << '\t' << ds << '\t' << anova[k] << endl;
	}
	out << endl;

	//varRef
	//varQuery
	/* F test for differences among variances.
	* varQuery is expected to be higher or similar than varRef */
	//float fs = varQuery[query] / varRef[query];	/* F-Snedecor, test for differences of variances */

	bool results = false;

	//confidence limit, t - Student, anova
	out << "Window\tConfidenceLimit\tt-Student\tAnova" << endl;

	for (int k = 0; k < windows.size(); k++) {
		string temp = "";
		if (isChimericConfidence[k]) { temp += "*\t"; }
		else { temp += "\t"; }

		if (isChimericTStudent[k]) { temp += "*\t"; }
		else { temp += "\t"; }

		if (isChimericANOVA[k]) { temp += "*\t"; }
		else { temp += "\t"; }

		out << k + 1 << '\t' << temp << endl;

		if (temp == "*\t*\t*\t") { results = true; }
	}
	out << endl;

	if (results) {
		LOG(INFO) << querySeq->getName() + " was found have at least one chimeric window." << '\n';
		outAcc << querySeq->getName() << endl;
	}

	//free memory
	for (int i = 0; i < closest.size(); i++) { delete closest[i].seq; }

	return *querySeq;
}
//***************************************************************************************************************
int Ccode::getChimeras(Sequence* query) {

	closest.clear();
	refCombo = 0;
	sumRef.clear();
	varRef.clear();
	varQuery.clear();
	sdRef.clear();
	sdQuery.clear();
	sumQuery.clear();
	sumSquaredRef.clear();
	sumSquaredQuery.clear();
	averageRef.clear();
	averageQuery.clear();
	anova.clear();
	isChimericConfidence.clear();
	isChimericTStudent.clear();
	isChimericANOVA.clear();
	trim.clear();
	spotMap.clear();
	windowSizes = window;
	windows.clear();


	querySeq = query;

	//find closest matches to query
	closest = findClosest(query, numWanted);

	if (ctrlc_pressed) { return 0; }

	//initialize spotMap
	for (int i = 0; i < query->getAligned().length(); i++) { spotMap[i] = i; }

	//mask sequences if the user wants to 
	if (seqMask != "") {
		decalc->setMask(seqMask);

		decalc->runMask(query);

		//mask closest
		for (int i = 0; i < closest.size(); i++) { decalc->runMask(closest[i].seq); }

		spotMap = decalc->getMaskMap();
	}

	if (filter) {
		vector<Sequence*> temp;
		for (int i = 0; i < closest.size(); i++) { temp.push_back(closest[i].seq); }
		temp.push_back(query);

		createFilter(temp, 0.5);

		for (int i = 0; i < temp.size(); i++) {
			if (ctrlc_pressed) { return 0; }
			runFilter(temp[i]);
		}

		//update spotMap
		map<int, int> newMap;
		int spot = 0;

		for (int i = 0; i < filterString.length(); i++) {
			if (filterString[i] == '1') {
				//add to newMap
				newMap[spot] = spotMap[i];
				spot++;
			}
		}
		spotMap = newMap;
	}

	//trim sequences - this follows ccodes remove_extra_gaps 
	trimSequences(query);
	if (ctrlc_pressed) { return 0; }

	//windows are equivalent to words - ccode paper recommends windows are between 5% and 20% on alignment length().  
	//Our default will be 10% and we will warn if user tries to use a window above or below these recommendations
	windows = findWindows();
	if (ctrlc_pressed) { return 0; }

	//remove sequences that are more than 20% different and less than 0.5% different - may want to allow user to specify this later 
	removeBadReferenceSeqs(closest);
	if (ctrlc_pressed) { return 0; }

	//find the averages for each querys references
	getAverageRef(closest);  //fills sumRef, averageRef, sumSquaredRef and refCombo.
	getAverageQuery(closest, query);  //fills sumQuery, averageQuery, sumSquaredQuery.
	if (ctrlc_pressed) { return 0; }

	//find the averages for each querys references 
	findVarianceRef();  //fills varRef and sdRef also sets minimum error rate to 0.001 to avoid divide by 0.
	if (ctrlc_pressed) { return 0; }

	//find the averages for the query 
	findVarianceQuery();  //fills varQuery and sdQuery also sets minimum error rate to 0.001 to avoid divide by 0.
	if (ctrlc_pressed) { return 0; }

	determineChimeras();  //fills anova, isChimericConfidence, isChimericTStudent and isChimericANOVA. 
	if (ctrlc_pressed) { return 0; }

	return 0;
}
/***************************************************************************************************************/
//ccode algo says it does this to "Removes the initial and final gaps to avoid biases due to incomplete sequences."
void Ccode::trimSequences(Sequence* query) {

	int frontPos = 0;  //should contain first position in all seqs that is not a gap character
	int rearPos = query->getAligned().length();

	//********find first position in closest seqs that is a non gap character***********//
	//find first position all query seqs that is a non gap character
	for (int i = 0; i < closest.size(); i++) {

		string aligned = closest[i].seq->getAligned();
		int pos = 0;

		//find first spot in this seq
		for (int j = 0; j < aligned.length(); j++) {
			if (isalpha(aligned[j])) {
				pos = j;
				break;
			}
		}

		//save this spot if it is the farthest
		if (pos > frontPos) { frontPos = pos; }
	}

	//find first position all querySeq[query] that is a non gap character
	string aligned = query->getAligned();
	int pos = 0;

	//find first spot in this seq
	for (int j = 0; j < aligned.length(); j++) {
		if (isalpha(aligned[j])) {
			pos = j;
			break;
		}
	}

	//save this spot if it is the farthest
	if (pos > frontPos) { frontPos = pos; }


	//********find last position in closest seqs that is a non gap character***********//
	for (int i = 0; i < closest.size(); i++) {

		string aligned = closest[i].seq->getAligned();
		int pos = aligned.length();

		//find first spot in this seq
		for (int j = aligned.length() - 1; j >= 0; j--) {
			if (isalpha(aligned[j])) {
				pos = j;
				break;
			}
		}

		//save this spot if it is the farthest
		if (pos < rearPos) { rearPos = pos; }
	}

	//find last position all querySeqs[query] that is a non gap character
	aligned = query->getAligned();
	pos = aligned.length();

	//find first spot in this seq
	for (int j = aligned.length() - 1; j >= 0; j--) {
		if (isalpha(aligned[j])) {
			pos = j;
			break;
		}
	}

	//save this spot if it is the farthest
	if (pos < rearPos) { rearPos = pos; }


	//check to make sure that is not whole seq
	if ((rearPos - frontPos - 1) <= 0) { LOG(INFO) << "Error, when I trim your sequences, the entire sequence is trimmed." << '\n'; exit(1); }

	map<int, int> tempTrim;
	tempTrim[frontPos] = rearPos;

	//save trimmed locations
	trim = tempTrim;

	//update spotMask
	map<int, int> newMap;
	int spot = 0;

	for (int i = frontPos; i < rearPos; i++) {
		//add to newMap
		newMap[spot] = spotMap[i];
		spot++;
	}
	spotMap = newMap;
}
/***************************************************************************************************************/
vector<int> Ccode::findWindows() {

	vector<int> win;
	it = trim.begin();

	int length = it->second - it->first;

	//default is wanted = 10% of total length
	if (windowSizes > length) {
		LOG(INFO) << "You have slected a window larger than your sequence length after all filters, masks and trims have been done. I will use the default 10% of sequence length.";
		windowSizes = length / 10;
	}
	else if (windowSizes == 0) { windowSizes = length / 10; }
	else if (windowSizes > (length * 0.20)) {
		LOG(INFO) << "You have selected a window that is larger than 20% of your sequence length.  This is not recommended, but I will continue anyway." << '\n';
	}
	else if (windowSizes < (length * 0.05)) {
		LOG(INFO) << "You have selected a window that is smaller than 5% of your sequence length.  This is not recommended, but I will continue anyway." << '\n';
	}

	//save starting points of each window
	for (int m = it->first; m < (it->second - windowSizes); m += windowSizes) { win.push_back(m); }

	//save last window
	if (win[win.size() - 1] < (it->first + length)) {
		win.push_back(win[win.size() - 1] + windowSizes); // ex. string length is 115, window is 25, without this you would get 0, 25, 50, 75
	}																									//with this you would get 1,25,50,75,100

	return win;
}
//***************************************************************************************************************
int Ccode::getDiff(string seqA, string seqB) {

	int numDiff = 0;

	for (int i = 0; i < seqA.length(); i++) {
		//if you are both not gaps
		//if (isalpha(seqA[i]) && isalpha(seqA[i])) {
			//are you different
		if (seqA[i] != seqB[i]) {
			int ok; /* ok=1 means equivalent base. Checks for degenerate bases */

		   /* the char in base_a and base_b have been checked and they are different */
			if ((seqA[i] == 'N') && (seqB[i] != '-')) ok = 1;
			else if ((seqB[i] == 'N') && (seqA[i] != '-')) ok = 1;
			else if ((seqA[i] == 'Y') && ((seqB[i] == 'C') || (seqB[i] == 'T'))) ok = 1;
			else if ((seqB[i] == 'Y') && ((seqA[i] == 'C') || (seqA[i] == 'T'))) ok = 1;
			else if ((seqA[i] == 'R') && ((seqB[i] == 'G') || (seqB[i] == 'A'))) ok = 1;
			else if ((seqB[i] == 'R') && ((seqA[i] == 'G') || (seqA[i] == 'A'))) ok = 1;
			else if ((seqA[i] == 'S') && ((seqB[i] == 'C') || (seqB[i] == 'G'))) ok = 1;
			else if ((seqB[i] == 'S') && ((seqA[i] == 'C') || (seqA[i] == 'G'))) ok = 1;
			else if ((seqA[i] == 'W') && ((seqB[i] == 'T') || (seqB[i] == 'A'))) ok = 1;
			else if ((seqB[i] == 'W') && ((seqA[i] == 'T') || (seqA[i] == 'A'))) ok = 1;
			else if ((seqA[i] == 'M') && ((seqB[i] == 'A') || (seqB[i] == 'C'))) ok = 1;
			else if ((seqB[i] == 'M') && ((seqA[i] == 'A') || (seqA[i] == 'C'))) ok = 1;
			else if ((seqA[i] == 'K') && ((seqB[i] == 'T') || (seqB[i] == 'G'))) ok = 1;
			else if ((seqB[i] == 'K') && ((seqA[i] == 'T') || (seqA[i] == 'G'))) ok = 1;
			else if ((seqA[i] == 'V') && ((seqB[i] == 'C') || (seqB[i] == 'A') || (seqB[i] == 'G'))) ok = 1;
			else if ((seqB[i] == 'V') && ((seqA[i] == 'C') || (seqA[i] == 'A') || (seqA[i] == 'G'))) ok = 1;
			else if ((seqA[i] == 'H') && ((seqB[i] == 'T') || (seqB[i] == 'A') || (seqB[i] == 'C'))) ok = 1;
			else if ((seqB[i] == 'H') && ((seqA[i] == 'T') || (seqA[i] == 'A') || (seqA[i] == 'C'))) ok = 1;
			else if ((seqA[i] == 'D') && ((seqB[i] == 'T') || (seqB[i] == 'A') || (seqB[i] == 'G'))) ok = 1;
			else if ((seqB[i] == 'D') && ((seqA[i] == 'T') || (seqA[i] == 'A') || (seqA[i] == 'G'))) ok = 1;
			else if ((seqA[i] == 'B') && ((seqB[i] == 'C') || (seqB[i] == 'T') || (seqB[i] == 'G'))) ok = 1;
			else if ((seqB[i] == 'B') && ((seqA[i] == 'C') || (seqA[i] == 'T') || (seqA[i] == 'G'))) ok = 1;
			else ok = 0;  /* the bases are different and not equivalent */

			//check if they are both blanks
			if ((seqA[i] == '.') && (seqB[i] == '-')) ok = 1;
			else if ((seqB[i] == '.') && (seqA[i] == '-')) ok = 1;

			if (ok == 0) { numDiff++; }
		}
		//}
	}

	return numDiff;

}
//***************************************************************************************************************
//tried to make this look most like ccode original implementation
void Ccode::removeBadReferenceSeqs(vector<SeqDist>& seqs) {

	vector< vector<int> > numDiffBases;
	numDiffBases.resize(seqs.size());
	//initialize to 0
	for (int i = 0; i < numDiffBases.size(); i++) { numDiffBases[i].resize(seqs.size(), 0); }

	it = trim.begin();
	int length = it->second - it->first;

	//calc differences from each sequence to everyother seq in the set
	for (int i = 0; i < seqs.size(); i++) {

		string seqA = seqs[i].seq->getAligned().substr(it->first, length);

		//so you don't calc i to j and j to i since they are the same
		for (int j = 0; j < i; j++) {

			string seqB = seqs[j].seq->getAligned().substr(it->first, length);

			//compare strings
			int numDiff = getDiff(seqA, seqB);

			numDiffBases[i][j] = numDiff;
			numDiffBases[j][i] = numDiff;
		}
	}

	//initailize remove to 0
	vector<int> remove;  remove.resize(seqs.size(), 0);
	float top = ((20 * length) / (float)100);
	float bottom = ((0.5*length) / (float)100);

	//check each numDiffBases and if any are higher than threshold set remove to 1 so you can remove those seqs from the closest set
	for (int i = 0; i < numDiffBases.size(); i++) {
		for (int j = 0; j < i; j++) {
			//are you more than 20% different
			if (numDiffBases[i][j] > top) { remove[j] = 1; }
			//are you less than 0.5% different
			if (numDiffBases[i][j] < bottom) { remove[j] = 1; }
		}
	}

	int numSeqsLeft = 0;

	//count seqs that are not going to be removed
	for (int i = 0; i < remove.size(); i++) {
		if (remove[i] == 0) { numSeqsLeft++; }
	}

	//if you have enough then remove bad ones
	if (numSeqsLeft >= 3) {
		vector<SeqDist> goodSeqs;
		//remove bad seqs
		for (int i = 0; i < remove.size(); i++) {
			if (remove[i] == 0) {
				goodSeqs.push_back(seqs[i]);
			}
		}

		seqs = goodSeqs;

	}
	else { //warn, but dont remove any
		LOG(INFO) << querySeq->getName() + " does not have an adaquate number of reference sequences that are within 20% and 0.5% similarity.  I will continue, but please check." << '\n';
	}

}
//***************************************************************************************************************
//makes copy of templateseq for filter
vector<SeqDist>  Ccode::findClosest(Sequence* q, int numWanted) {
	try {

		vector<SeqDist>  topMatches;

		Sequence query = *(q);

		//calc distance to each sequence in template seqs
		for (int i = 0; i < templateSeqs.size(); i++) {

			Sequence ref = *(templateSeqs[i]);

			//find overall dist
			distCalc->calcDist(query, ref);
			float dist = distCalc->getDist();

			//save distance
			SeqDist temp;
			temp.seq = new Sequence(templateSeqs[i]->getName(), templateSeqs[i]->getAligned());
			temp.dist = dist;

			topMatches.push_back(temp);
		}

		sort(topMatches.begin(), topMatches.end(), compareSeqDist);

		for (int i = numWanted; i < topMatches.size(); i++) { delete topMatches[i].seq; }

		topMatches.resize(numWanted);

		return topMatches;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in Ccode, findClosestSides";
		exit(1);
	}
}
/**************************************************************************************************/
//find the distances from each reference sequence to every other reference sequence for each window for this query
void Ccode::getAverageRef(vector<SeqDist> ref) {

	vector< vector< vector<int> > >  diffs;  //diffs[0][1][2] is the number of differences between ref seq 0 and ref seq 1 at window 2.

	//initialize diffs vector
	diffs.resize(ref.size());
	for (int i = 0; i < diffs.size(); i++) {
		diffs[i].resize(ref.size());
		for (int j = 0; j < diffs[i].size(); j++) {
			diffs[i][j].resize(windows.size(), 0);
		}
	}

	it = trim.begin();

	//find the distances from each reference sequence to every other reference sequence for each window for this query		
	for (int i = 0; i < ref.size(); i++) {

		string refI = ref[i].seq->getAligned();

		//j<i, so you don't find distances from i to j and then j to i.
		for (int j = 0; j < i; j++) {

			string refJ = ref[j].seq->getAligned();

			for (int k = 0; k < windows.size(); k++) {

				string refIWindowk, refJWindowk;

				if (k < windows.size() - 1) {
					//get window strings
					refIWindowk = refI.substr(windows[k], windowSizes);
					refJWindowk = refJ.substr(windows[k], windowSizes);
				}
				else { //last window may be smaller than rest - see findwindows
				   //get window strings
					refIWindowk = refI.substr(windows[k], (it->second - windows[k]));
					refJWindowk = refJ.substr(windows[k], (it->second - windows[k]));
				}

				//find differences
				int diff = getDiff(refIWindowk, refJWindowk);

				//save differences in [i][j][k] and [j][i][k] since they are the same
				diffs[i][j][k] = diff;
				diffs[j][i][k] = diff;

			}//k

		}//j

	}//i

	//initialize sumRef for this query 
	sumRef.resize(windows.size(), 0);
	sumSquaredRef.resize(windows.size(), 0);
	averageRef.resize(windows.size(), 0);

	//find the sum of the differences for hte reference sequences
	for (int i = 0; i < diffs.size(); i++) {
		for (int j = 0; j < i; j++) {

			//increment this querys reference sequences combos
			refCombo++;

			for (int k = 0; k < diffs[i][j].size(); k++) {
				sumRef[k] += diffs[i][j][k];
				sumSquaredRef[k] += (diffs[i][j][k] * diffs[i][j][k]);
			}//k

		}//j
	}//i


	//find the average of the differences for the references for each window
	for (int i = 0; i < windows.size(); i++) {
		averageRef[i] = sumRef[i] / (float)refCombo;
	}

}
/**************************************************************************************************/
void Ccode::getAverageQuery(vector<SeqDist> ref, Sequence* query) {

	vector< vector<int> >  diffs;  //diffs[1][2] is the number of differences between querySeqs[query] and ref seq 1 at window 2.

	//initialize diffs vector
	diffs.resize(ref.size());
	for (int j = 0; j < diffs.size(); j++) {
		diffs[j].resize(windows.size(), 0);
	}

	it = trim.begin();

	string refQuery = query->getAligned();

	//j<i, so you don't find distances from i to j and then j to i.
	for (int j = 0; j < ref.size(); j++) {

		string refJ = ref[j].seq->getAligned();

		for (int k = 0; k < windows.size(); k++) {

			string QueryWindowk, refJWindowk;

			if (k < windows.size() - 1) {
				//get window strings
				QueryWindowk = refQuery.substr(windows[k], windowSizes);
				refJWindowk = refJ.substr(windows[k], windowSizes);
			}
			else { //last window may be smaller than rest - see findwindows
			   //get window strings
				QueryWindowk = refQuery.substr(windows[k], (it->second - windows[k]));
				refJWindowk = refJ.substr(windows[k], (it->second - windows[k]));
			}

			//find differences
			int diff = getDiff(QueryWindowk, refJWindowk);

			//save differences 
			diffs[j][k] = diff;

		}//k
	}//j


	//initialize sumRef for this query 
	sumQuery.resize(windows.size(), 0);
	sumSquaredQuery.resize(windows.size(), 0);
	averageQuery.resize(windows.size(), 0);

	//find the sum of the differences 
	for (int j = 0; j < diffs.size(); j++) {
		for (int k = 0; k < diffs[j].size(); k++) {
			sumQuery[k] += diffs[j][k];
			sumSquaredQuery[k] += (diffs[j][k] * diffs[j][k]);
		}//k
	}//j


	//find the average of the differences for the references for each window
	for (int i = 0; i < windows.size(); i++) {
		averageQuery[i] = sumQuery[i] / (float)ref.size();
	}
}
/**************************************************************************************************/
void Ccode::findVarianceRef() {

	varRef.resize(windows.size(), 0);
	sdRef.resize(windows.size(), 0);

	//for each window
	for (int i = 0; i < windows.size(); i++) {
		varRef[i] = (sumSquaredRef[i] - ((sumRef[i] * sumRef[i]) / (float)refCombo)) / (float)(refCombo - 1);
		sdRef[i] = sqrt(varRef[i]);

		//set minimum error rate to 0.001 - to avoid potential divide by zero - not sure if this is necessary but it follows ccode implementation
		if (averageRef[i] < 0.001) { averageRef[i] = 0.001; }
		if (sumRef[i] < 0.001) { sumRef[i] = 0.001; }
		if (varRef[i] < 0.001) { varRef[i] = 0.001; }
		if (sumSquaredRef[i] < 0.001) { sumSquaredRef[i] = 0.001; }
		if (sdRef[i] < 0.001) { sdRef[i] = 0.001; }

	}
}
/**************************************************************************************************/
void Ccode::findVarianceQuery() {
	varQuery.resize(windows.size(), 0);
	sdQuery.resize(windows.size(), 0);

	//for each window
	for (int i = 0; i < windows.size(); i++) {
		varQuery[i] = (sumSquaredQuery[i] - ((sumQuery[i] * sumQuery[i]) / (float)closest.size())) / (float)(closest.size() - 1);
		sdQuery[i] = sqrt(varQuery[i]);

		//set minimum error rate to 0.001 - to avoid potential divide by zero - not sure if this is necessary but it follows ccode implementation
		if (averageQuery[i] < 0.001) { averageQuery[i] = 0.001; }
		if (sumQuery[i] < 0.001) { sumQuery[i] = 0.001; }
		if (varQuery[i] < 0.001) { varQuery[i] = 0.001; }
		if (sumSquaredQuery[i] < 0.001) { sumSquaredQuery[i] = 0.001; }
		if (sdQuery[i] < 0.001) { sdQuery[i] = 0.001; }
	}

}
/**************************************************************************************************/
void Ccode::determineChimeras() {

	isChimericConfidence.resize(windows.size(), false);
	isChimericTStudent.resize(windows.size(), false);
	isChimericANOVA.resize(windows.size(), false);
	anova.resize(windows.size());


	//for each window
	for (int i = 0; i < windows.size(); i++) {

		//get confidence limits
		float t = getT(closest.size() - 1);  //how many seqs you are comparing to this querySeq
		float dsUpper = (averageQuery[i] + (t * sdQuery[i])) / averageRef[i];
		float dsLower = (averageQuery[i] - (t * sdQuery[i])) / averageRef[i];

		if ((dsUpper > 1.0) && (dsLower > 1.0) && (averageQuery[i] > averageRef[i])) {  /* range does not include 1 */
			isChimericConfidence[i] = true;   /* significantly higher at P<0.05 */

		}

		//student t test
		int degreeOfFreedom = refCombo + closest.size() - 2;
		float denomForT = (((refCombo - 1) * varQuery[i] + (closest.size() - 1) * varRef[i]) / (float)degreeOfFreedom) * ((refCombo + closest.size()) / (float)(refCombo * closest.size()));	/* denominator, without sqrt(), for ts calculations */

		float ts = fabs((averageQuery[i] - averageRef[i]) / (sqrt(denomForT)));  /* value of ts for t-student test */
		t = getT(degreeOfFreedom);

		if ((ts >= t) && (averageQuery[i] > averageRef[i])) {
			isChimericTStudent[i] = true;   /* significantly higher at P<0.05 */
		}

		//anova test
		float value1 = sumQuery[i] + sumRef[i];
		float value2 = sumSquaredQuery[i] + sumSquaredRef[i];
		float value3 = ((sumQuery[i] * sumQuery[i]) / (float)(closest.size())) + ((sumRef[i] * sumRef[i]) / (float)refCombo);
		float value4 = (value1 * value1) / ((float)(closest.size() + refCombo));
		float value5 = value2 - value4;
		float value6 = value3 - value4;
		float value7 = value5 - value6;
		float value8 = value7 / ((float)degreeOfFreedom);
		float anovaValue = value6 / value8;

		float f = getF(degreeOfFreedom);

		if ((anovaValue >= f) && (averageQuery[i] > averageRef[i])) {
			isChimericANOVA[i] = true;   /* significant P<0.05 */
		}

		if (isnan(anovaValue) || isinf(anovaValue)) { anovaValue = 0.0; }

		anova[i] = anovaValue;
	}

}
/**************************************************************************************************/
float Ccode::getT(int numseq) {

	float tvalue = 0;

	/* t-student critical values for different degrees of freedom and alpha 0.1 in one-tail tests (equivalent to 0.05) */
	if (numseq > 120) tvalue = 1.645f;
	else if (numseq > 60) tvalue = 1.658f;
	else if (numseq > 40) tvalue = 1.671f;
	else if (numseq > 30) tvalue = 1.684f;
	else if (numseq > 29) tvalue = 1.697f;
	else if (numseq > 28) tvalue = 1.699f;
	else if (numseq > 27) tvalue = 1.701f;
	else if (numseq > 26) tvalue = 1.703f;
	else if (numseq > 25) tvalue = 1.706f;
	else if (numseq > 24) tvalue = 1.708f;
	else if (numseq > 23) tvalue = 1.711f;
	else if (numseq > 22) tvalue = 1.714f;
	else if (numseq > 21) tvalue = 1.717f;
	else if (numseq > 20) tvalue = 1.721f;
	else if (numseq > 19) tvalue = 1.725f;
	else if (numseq > 18) tvalue = 1.729f;
	else if (numseq > 17) tvalue = 1.734f;
	else if (numseq > 16) tvalue = 1.740f;
	else if (numseq > 15) tvalue = 1.746f;
	else if (numseq > 14) tvalue = 1.753f;
	else if (numseq > 13) tvalue = 1.761f;
	else if (numseq > 12) tvalue = 1.771f;
	else if (numseq > 11) tvalue = 1.782f;
	else if (numseq > 10) tvalue = 1.796f;
	else if (numseq > 9) tvalue = 1.812f;
	else if (numseq > 8) tvalue = 1.833f;
	else if (numseq > 7) tvalue = 1.860f;
	else if (numseq > 6) tvalue = 1.895f;
	else if (numseq > 5) tvalue = 1.943f;
	else if (numseq > 4) tvalue = 2.015f;
	else if (numseq > 3) tvalue = 2.132f;
	else if (numseq > 2) tvalue = 2.353f;
	else if (numseq > 1) tvalue = 2.920f;
	else if (numseq <= 1) {
		LOG(INFO) << "Two or more reference sequences are required, your data will be flawed.\n" << '\n';
	}

	return tvalue;
}
/**************************************************************************************************/
float Ccode::getF(int numseq) {

	float fvalue = 0;

	/* F-Snedecor critical values for v1=1 and different degrees of freedom v2 and alpha 0.05 */
	if (numseq > 120) fvalue = 3.84f;
	else if (numseq > 60) fvalue = 3.92f;
	else if (numseq > 40) fvalue = 4.00f;
	else if (numseq > 30) fvalue = 4.08f;
	else if (numseq > 29) fvalue = 4.17f;
	else if (numseq > 28) fvalue = 4.18f;
	else if (numseq > 27) fvalue = 4.20f;
	else if (numseq > 26) fvalue = 4.21f;
	else if (numseq > 25) fvalue = 4.23f;
	else if (numseq > 24) fvalue = 4.24f;
	else if (numseq > 23) fvalue = 4.26f;
	else if (numseq > 22) fvalue = 4.28f;
	else if (numseq > 21) fvalue = 4.30f;
	else if (numseq > 20) fvalue = 4.32f;
	else if (numseq > 19) fvalue = 4.35f;
	else if (numseq > 18) fvalue = 4.38f;
	else if (numseq > 17) fvalue = 4.41f;
	else if (numseq > 16) fvalue = 4.45f;
	else if (numseq > 15) fvalue = 4.49f;
	else if (numseq > 14) fvalue = 4.54f;
	else if (numseq > 13) fvalue = 4.60f;
	else if (numseq > 12) fvalue = 4.67f;
	else if (numseq > 11) fvalue = 4.75f;
	else if (numseq > 10) fvalue = 4.84f;
	else if (numseq > 9) fvalue = 4.96f;
	else if (numseq > 8) fvalue = 5.12f;
	else if (numseq > 7) fvalue = 5.32f;
	else if (numseq > 6) fvalue = 5.59f;
	else if (numseq > 5) fvalue = 5.99f;
	else if (numseq > 4) fvalue = 6.61f;
	else if (numseq > 3) fvalue = 7.71f;
	else if (numseq > 2) fvalue = 10.1f;
	else if (numseq > 1) fvalue = 18.5f;
	else if (numseq > 0) fvalue = 161.0f;
	else if (numseq <= 0) {
		LOG(INFO) << "Two or more reference sequences are required, your data will be flawed.\n" << '\n';
	}

	return fvalue;
}
//***************************************************************************************************************



