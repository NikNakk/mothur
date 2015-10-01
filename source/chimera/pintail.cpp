/*
 *  pintail.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 7/9/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "pintail.h"
#include "ignoregaps.h"
#include "eachgapdist.h"
#include "filehandling/file.h"

 //********************************************************************************************************************
 //sorts lowest to highest
inline bool compareQuanMembers(quanMember left, quanMember right) {
	return (left.score < right.score);
}
//***************************************************************************************************************

Pintail::Pintail(string filename, string temp, bool f, int p, string mask, string cons, string q, int win, int inc, string o) : Chimera() {
	try {

		fastafile = filename;
		templateFileName = temp; templateSeqs = readSeqs(temp);
		filter = f;
		processors = p;
		setMask(mask);
		consfile = cons;
		quanfile = q;
		window = win;
		increment = inc;
		outputDir = o;

		distcalculator = new eachGapDist();
		decalc = new DeCalculator();

		doPrep();
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in Pintail, Pintail";
		exit(1);
	}

}
//***************************************************************************************************************

Pintail::~Pintail() {

	delete distcalculator;
	delete decalc;
}
//***************************************************************************************************************
int Pintail::doPrep() {

	mergedFilterString = "";
	windowSizesTemplate.resize(templateSeqs.size(), window);
	quantiles.resize(100);  //one for every percent mismatch
	quantilesMembers.resize(100);  //one for every percent mismatch

	//if the user does not enter a mask then you want to keep all the spots in the alignment
	if (seqMask.length() == 0) { decalc->setAlignmentLength(templateSeqs[0]->getAligned().length()); }
	else { decalc->setAlignmentLength(seqMask.length()); }

	decalc->setMask(seqMask);
#if defined (UNIX)
	//find breakup of templatefile for quantiles
	if (processors == 1) { templateLines.push_back(new linePair(0, templateSeqs.size())); }
	else {
		for (int i = 0; i < processors; i++) {
			templateLines.push_back(new linePair());
			templateLines[i]->start = int(sqrt(float(i) / float(processors)) * templateSeqs.size());
			templateLines[i]->end = int(sqrt(float(i + 1) / float(processors)) * templateSeqs.size());
		}
	}
#else
	templateLines.push_back(new linePair(0, templateSeqs.size()));
#endif

	LOG(INFO) << "Getting conservation... "; cout.flush();
	if (consfile == "") {
		LOG(INFO) << "Calculating probability of conservation for your template sequences.  This can take a while...  I will output the frequency of the highest base in each position to a .freq file so that you can input them using the conservation parameter next time you run this command.  Providing the .freq file will improve speed.    "; cout.flush();
		probabilityProfile = decalc->calcFreq(templateSeqs, templateFileName);
		if (ctrlc_pressed) { return 0; }
		LOG(INFO) << "Done." << '\n';
	}
	else { probabilityProfile = readFreq();	LOG(INFO) << "Done."; }
	LOG(INFO) << "";

	//make P into Q
	for (int i = 0; i < probabilityProfile.size(); i++) { probabilityProfile[i] = 1 - probabilityProfile[i]; }  //

	bool reRead = false;
	//create filter if needed for later
	if (filter) {

		//read in all query seqs
		vector<Sequence*> tempQuerySeqs = readSeqs(fastafile);

		vector<Sequence*> temp;
		//merge query seqs and template seqs
		temp = templateSeqs;
		for (int i = 0; i < tempQuerySeqs.size(); i++) { temp.push_back(tempQuerySeqs[i]); }

		if (seqMask != "") {
			reRead = true;
			//mask templates
			for (int i = 0; i < temp.size(); i++) {
				if (ctrlc_pressed) {
					for (int i = 0; i < tempQuerySeqs.size(); i++) { delete tempQuerySeqs[i]; }
					return 0;
				}
				decalc->runMask(temp[i]);
			}
		}

		mergedFilterString = createFilter(temp, 0.5);

		if (ctrlc_pressed) {
			for (int i = 0; i < tempQuerySeqs.size(); i++) { delete tempQuerySeqs[i]; }
			return 0;
		}

		//reread template seqs
		for (int i = 0; i < tempQuerySeqs.size(); i++) { delete tempQuerySeqs[i]; }
	}


	//quantiles are used to determine whether the de values found indicate a chimera
	//if you have to calculate them, its time intensive because you are finding the de and deviation values for each 
	//combination of sequences in the template
	if (quanfile != "") {
		quantiles = readQuantiles();
	}
	else {
		if ((!filter) && (seqMask != "")) { //if you didn't filter but you want to mask. if you filtered then you did mask first above.
			reRead = true;
			//mask templates
			for (int i = 0; i < templateSeqs.size(); i++) {
				if (ctrlc_pressed) { return 0; }
				decalc->runMask(templateSeqs[i]);
			}
		}

		if (filter) {
			reRead = true;
			for (int i = 0; i < templateSeqs.size(); i++) {
				if (ctrlc_pressed) { return 0; }
				runFilter(templateSeqs[i]);
			}
		}

		LOG(INFO) << "Calculating quantiles for your template.  This can take a while...  I will output the quantiles to a .quan file that you can input them using the quantiles parameter next time you run this command.  Providing the .quan file will dramatically improve speed.    "; cout.flush();
		if (processors == 1) {
			quantilesMembers = decalc->getQuantiles(templateSeqs, windowSizesTemplate, window, probabilityProfile, increment, 0, templateSeqs.size());
		}
		else { createProcessesQuan(); }

		if (ctrlc_pressed) { return 0; }

		string noOutliers, outliers;

		if ((!filter) && (seqMask == "")) {
			noOutliers = File::getRootName(File::getSimpleName(templateFileName)) + "pintail.quan";
		}
		else if ((!filter) && (seqMask != "")) {
			noOutliers = File::getRootName(File::getSimpleName(templateFileName)) + "pintail.masked.quan";
		}
		else if ((filter) && (seqMask != "")) {
			noOutliers = File::getRootName(File::getSimpleName(templateFileName)) + "pintail.filtered." + File::getSimpleName(File::getRootName(fastafile)) + "masked.quan";
		}
		else if ((filter) && (seqMask == "")) {
			noOutliers = File::getRootName(File::getSimpleName(templateFileName)) + "pintail.filtered." + File::getSimpleName(File::getRootName(fastafile)) + "quan";
		}

		decalc->removeObviousOutliers(quantilesMembers, templateSeqs.size());

		if (ctrlc_pressed) { return 0; }

		string outputString = "#" + m->getVersion() + "\n";

		//adjust quantiles
		for (int i = 0; i < quantilesMembers.size(); i++) {
			vector<float> temp;

			if (quantilesMembers[i].size() == 0) {
				//in case this is not a distance found in your template files
				for (int g = 0; g < 6; g++) {
					temp.push_back(0.0);
				}
			}
			else {

				sort(quantilesMembers[i].begin(), quantilesMembers[i].end());

				//save 10%
				temp.push_back(quantilesMembers[i][int(quantilesMembers[i].size() * 0.10)]);
				//save 25%
				temp.push_back(quantilesMembers[i][int(quantilesMembers[i].size() * 0.25)]);
				//save 50%
				temp.push_back(quantilesMembers[i][int(quantilesMembers[i].size() * 0.5)]);
				//save 75%
				temp.push_back(quantilesMembers[i][int(quantilesMembers[i].size() * 0.75)]);
				//save 95%
				temp.push_back(quantilesMembers[i][int(quantilesMembers[i].size() * 0.95)]);
				//save 99%
				temp.push_back(quantilesMembers[i][int(quantilesMembers[i].size() * 0.99)]);

			}

			//output quan value
			outputString += toString(i + 1);
			for (int u = 0; u < temp.size(); u++) { outputString += "\t" + toString(temp[u]); }
			outputString += "\n";

			quantiles[i] = temp;

		}

		printQuanFile(noOutliers, outputString);

		//free memory
		quantilesMembers.clear();

		LOG(INFO) << "Done." << '\n';
	}

	if (reRead) {
		for (int i = 0; i < templateSeqs.size(); i++) { delete templateSeqs[i]; }
		templateSeqs.clear();
		templateSeqs = readSeqs(templateFileName);
	}


	//free memory
	for (int i = 0; i < templateLines.size(); i++) { delete templateLines[i]; }

	return 0;

}
//***************************************************************************************************************
Sequence Pintail::print(ostream& out, ostream& outAcc) {

	int index = ceil(deviation);

	//is your DE value higher than the 95%
	string chimera;
	if (index != 0) {  //if index is 0 then its an exact match to a template seq
		if (quantiles[index][4] == 0.0) {
			chimera = "Your template does not include sequences that provide quantile values at distance " + toString(index);
		}
		else {
			if (DE > quantiles[index][4]) { chimera = "Yes"; }
			else { chimera = "No"; }
		}
	}
	else { chimera = "No"; }

	out << querySeq->getName() << '\t' << "div: " << deviation << "\tstDev: " << DE << "\tchimera flag: " << chimera << endl;
	if (chimera == "Yes") {
		LOG(INFO) << querySeq->getName() + "\tdiv: " + toString(deviation) + "\tstDev: " + toString(DE) + "\tchimera flag: " + chimera << '\n';
		outAcc << querySeq->getName() << endl;
	}
	out << "Observed";

	for (int j = 0; j < obsDistance.size(); j++) { out << '\t' << obsDistance[j]; }
	out << endl;

	out << "Expected";

	for (int m = 0; m < expectedDistance.size(); m++) { out << '\t' << expectedDistance[m]; }
	out << endl;

	return *querySeq;

}
//***************************************************************************************************************
int Pintail::getChimeras(Sequence* query) {
	querySeq = query;
	trimmed.clear();
	windowSizes = window;

	//find pairs has to be done before a mask
	bestfit = findPairs(query);

	if (ctrlc_pressed) { return 0; }

	//if they mask  
	if (seqMask != "") {
		decalc->runMask(query);
		decalc->runMask(bestfit);
	}

	if (filter) { //must be done after a mask
		runFilter(query);
		runFilter(bestfit);
	}


	//trim seq
	decalc->trimSeqs(query, bestfit, trimmed);

	//find windows
	it = trimmed.begin();
	windowsForeachQuery = decalc->findWindows(query, it->first, it->second, windowSizes, increment);

	//find observed distance
	obsDistance = decalc->calcObserved(query, bestfit, windowsForeachQuery, windowSizes);

	if (ctrlc_pressed) { return 0; }

	Qav = decalc->findQav(windowsForeachQuery, windowSizes, probabilityProfile);

	if (ctrlc_pressed) { return 0; }

	//find alpha			
	seqCoef = decalc->getCoef(obsDistance, Qav);

	//calculating expected distance
	expectedDistance = decalc->calcExpected(Qav, seqCoef);

	if (ctrlc_pressed) { return 0; }

	//finding de
	DE = decalc->calcDE(obsDistance, expectedDistance);

	if (ctrlc_pressed) { return 0; }

	//find distance between query and closest match
	it = trimmed.begin();
	deviation = decalc->calcDist(query, bestfit, it->first, it->second);

	delete bestfit;

	return 0;
}

//***************************************************************************************************************

vector<float> Pintail::readFreq() {
	//read in probabilities and store in vector
	int pos; float num;

	vector<float> prob;
	set<int> h = decalc->getPos();  //positions of bases in masking sequence

	ifstream in;
	File::openInputFile(consfile, in);

	//read version
	string line = File::getline(in); File::gobble(in);

	while (!in.eof()) {

		in >> pos >> num;

		if (h.count(pos) > 0) {
			float Pi;
			Pi = (num - 0.25) / 0.75;

			//cannot have probability less than 0.
			if (Pi < 0) { Pi = 0.0; }

			//do you want this spot
			prob.push_back(Pi);
		}

		File::gobble(in);
	}
	in.close();
	return prob;

}

//***************************************************************************************************************
//calculate the distances from each query sequence to all sequences in the template to find the closest sequence
Sequence* Pintail::findPairs(Sequence* q) {

	Sequence* seqsMatches;

	seqsMatches = decalc->findClosest(q, templateSeqs);
	return seqsMatches;

}
//**************************************************************************************************
void Pintail::createProcessesQuan() {
#if defined (UNIX)
	int process = 1;
	vector<int> processIDS;
	bool recalc = false;

	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);
			process++;
		}
		else if (pid == 0) {

			quantilesMembers = decalc->getQuantiles(templateSeqs, windowSizesTemplate, window, probabilityProfile, increment, templateLines[process]->start, templateLines[process]->end);

			//write out data to file so parent can read it
			ofstream out;
			string s = m->mothurGetpid(process) + ".temp";
			File::openOutputFile(s, out);

			//output observed distances
			for (int i = 0; i < quantilesMembers.size(); i++) {
				out << quantilesMembers[i].size();
				for (int j = 0; j < quantilesMembers[i].size(); j++) {
					out << '\t' << quantilesMembers[i][j];
				}
				out << endl;
			}

			out.close();

			exit(0);
		}
		else {
			LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(process) + "\n"; processors = process;
			for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
			//wait to die
			for (int i = 0;i < processIDS.size();i++) {
				int temp = processIDS[i];
				wait(&temp);
			}
			ctrlc_pressed = false;
			for (int i = 0;i < processIDS.size();i++) {
				File::remove((toString(processIDS[i]) + ".temp"));
			}
			recalc = true;
			break;
		}
	}

	if (recalc) {
		//test line, also set recalc to true.
		//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove((toString(processIDS[i]) + ".temp"));}processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

		//redo file divide
		for (int i = 0; i < templateLines.size(); i++) { delete templateLines[i]; }  templateLines.clear();
		for (int i = 0; i < processors; i++) {
			templateLines.push_back(new linePair());
			templateLines[i]->start = int(sqrt(float(i) / float(processors)) * templateSeqs.size());
			templateLines[i]->end = int(sqrt(float(i + 1) / float(processors)) * templateSeqs.size());
		}

		processIDS.resize(0);
		process = 1;

		//loop through and create all the processes you want
		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);
				process++;
			}
			else if (pid == 0) {

				quantilesMembers = decalc->getQuantiles(templateSeqs, windowSizesTemplate, window, probabilityProfile, increment, templateLines[process]->start, templateLines[process]->end);

				//write out data to file so parent can read it
				ofstream out;
				string s = m->mothurGetpid(process) + ".temp";
				File::openOutputFile(s, out);

				//output observed distances
				for (int i = 0; i < quantilesMembers.size(); i++) {
					out << quantilesMembers[i].size();
					for (int j = 0; j < quantilesMembers[i].size(); j++) {
						out << '\t' << quantilesMembers[i][j];
					}
					out << endl;
				}

				out.close();

				exit(0);
			}
			else {
				LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
				for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
				exit(0);
			}
		}
	}


	//parent does its part
	quantilesMembers = decalc->getQuantiles(templateSeqs, windowSizesTemplate, window, probabilityProfile, increment, templateLines[0]->start, templateLines[0]->end);

	//force parent to wait until all the processes are done
	for (int i = 0;i < (processors - 1);i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	//get data created by processes
	for (int i = 0;i < (processors - 1);i++) {
		ifstream in;
		string s = toString(processIDS[i]) + ".temp";
		File::openInputFile(s, in);

		vector< vector<float> > quan;
		quan.resize(100);

		//get quantiles
		for (int h = 0; h < quan.size(); h++) {
			int num;
			in >> num;

			File::gobble(in);

			vector<float> q;  float w;
			for (int j = 0; j < num; j++) {
				in >> w;
				q.push_back(w);
			}

			quan[h] = q;
			File::gobble(in);
		}


		//save quan in quantiles
		for (int j = 0; j < quan.size(); j++) {
			//put all values of q[i] into quan[i]
			for (int l = 0; l < quan[j].size(); l++) { quantilesMembers[j].push_back(quan[j][l]); }
			//quantilesMembers[j].insert(quantilesMembers[j].begin(), quan[j].begin(), quan[j].end());
		}

		in.close();
		File::remove(s);
	}

#else
	quantilesMembers = decalc->getQuantiles(templateSeqs, windowSizesTemplate, window, probabilityProfile, increment, 0, templateSeqs.size());
#endif		
}
//***************************************************************************************************************
vector< vector<float> > Pintail::readQuantiles() {
	int num;
	float ten, twentyfive, fifty, seventyfive, ninetyfive, ninetynine;

	vector< vector<float> > quan;
	vector <float> temp; temp.resize(6, 0);

	//to fill 0
	quan.push_back(temp);

	ifstream in;
	File::openInputFile(quanfile, in);

	//read version
	string line = File::getline(in); File::gobble(in);

	while (!in.eof()) {

		in >> num >> ten >> twentyfive >> fifty >> seventyfive >> ninetyfive >> ninetynine;

		temp.clear();

		temp.push_back(ten);
		temp.push_back(twentyfive);
		temp.push_back(fifty);
		temp.push_back(seventyfive);
		temp.push_back(ninetyfive);
		temp.push_back(ninetynine);

		quan.push_back(temp);

		File::gobble(in);
	}
	in.close();

	return quan;

}
//***************************************************************************************************************/

void Pintail::printQuanFile(string file, string outputString) {

	ofstream outQuan;
	File::openOutputFile(file, outQuan);

	outQuan << outputString;

	outQuan.close();
}

//***************************************************************************************************************/



