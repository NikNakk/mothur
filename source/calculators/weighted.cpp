/*
 *  weighted.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 2/9/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "weighted.h"

 /**************************************************************************************************/

EstOutput Weighted::getValues(Tree* t, int p, string o) {
	data.clear(); //clear out old values
	int numGroups;
	vector<double> D;
	processors = p;
	outputDir = o;

	CountTable* ct = t->getCountTable();

	numGroups = settings.getNumGroups();

	if (ctrlc_pressed) { return data; }

	//calculate number of comparisons i.e. with groups A,B,C = AB, AC, BC = 3;
	vector< vector<string> > namesOfGroupCombos;
	for (int i = 0; i < numGroups; i++) {
		for (int l = 0; l < i; l++) {
			//initialize weighted scores
			//WScore[globaldata->Groups[i]+globaldata->Groups[l]] = 0.0;
			vector<string> groups; groups.push_back((settings.getGroups())[i]); groups.push_back((settings.getGroups())[l]);
			namesOfGroupCombos.push_back(groups);
		}
	}

	int remainingPairs = namesOfGroupCombos.size();
	int startIndex = 0;
	for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
		int numPairs = remainingPairs; //case for last processor
		if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
		lines.push_back(linePair(startIndex, numPairs)); //startIndex, numPairs
		startIndex = startIndex + numPairs;
		remainingPairs = remainingPairs - numPairs;
	}

	data = createProcesses(t, namesOfGroupCombos, ct);

	lines.clear();

	return data;
}
/**************************************************************************************************/

EstOutput Weighted::createProcesses(Tree* t, vector< vector<string> > namesOfGroupCombos, CountTable* ct) {
	vector<int> processIDS;
	EstOutput results;
	bool recalc = false;

	int process = 1;

	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
			process++;
		}
		else if (pid == 0) {
			EstOutput Myresults;
			Myresults = driver(t, namesOfGroupCombos, lines[process].start, lines[process].num, ct);

			//pass numSeqs to parent
			ofstream out;
			string tempFile = outputDir + m->mothurGetpid(process) + ".weighted.results.temp";
			File::openOutputFile(tempFile, out);

			out << Myresults.size() << endl;
			for (int i = 0; i < Myresults.size(); i++) { out << Myresults[i] << '\t'; } out << endl;
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
				File::remove(outputDir + (toString(processIDS[i]) + ".weightedcommand.results.temp"));
			}
			recalc = true;
			break;
		}
	}

	if (recalc) {
		//test line, also set recalc to true.
		//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove(outputDir + (toString(processIDS[i]) + ".weightedcommand.results.temp"));}processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

		lines.clear();

		//calculate number of comparisons i.e. with groups A,B,C = AB, AC, BC = 3;
		int numGroups = settings.getNumGroups();
		vector< vector<string> > namesOfGroupCombos;
		for (int i = 0; i < numGroups; i++) {
			for (int l = 0; l < i; l++) {
				//initialize weighted scores
				vector<string> groups; groups.push_back((settings.getGroups())[i]); groups.push_back((settings.getGroups())[l]);
				namesOfGroupCombos.push_back(groups);
			}
		}

		int remainingPairs = namesOfGroupCombos.size();
		int startIndex = 0;
		for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
			int numPairs = remainingPairs; //case for last processor
			if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
			lines.push_back(linePair(startIndex, numPairs)); //startIndex, numPairs
			startIndex = startIndex + numPairs;
			remainingPairs = remainingPairs - numPairs;
		}

		results.clear();
		processIDS.resize(0);
		process = 1;

		//loop through and create all the processes you want
		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				EstOutput Myresults;
				Myresults = driver(t, namesOfGroupCombos, lines[process].start, lines[process].num, ct);

				//pass numSeqs to parent
				ofstream out;
				string tempFile = outputDir + m->mothurGetpid(process) + ".weighted.results.temp";
				File::openOutputFile(tempFile, out);

				out << Myresults.size() << endl;
				for (int i = 0; i < Myresults.size(); i++) { out << Myresults[i] << '\t'; } out << endl;
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

	results = driver(t, namesOfGroupCombos, lines[0].start, lines[0].num, ct);

	//force parent to wait until all the processes are done
	for (int i = 0;i < (processors - 1);i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	if (ctrlc_pressed) { return results; }

	//get data created by processes
	for (int i = 0;i < (processors - 1);i++) {
		ifstream in;
		string s = outputDir + toString(processIDS[i]) + ".weighted.results.temp";
		File::openInputFile(s, in);

		//get quantiles
		while (!in.eof()) {
			int num;
			in >> num; File::gobble(in);

			if (ctrlc_pressed) { break; }

			double w;
			for (int j = 0; j < num; j++) {
				in >> w;
				results.push_back(w);
			}
			File::gobble(in);
		}
		in.close();
		File::remove(s);
	}

	return results;
}
/**************************************************************************************************/
EstOutput Weighted::driver(Tree* t, vector< vector<string> > namesOfGroupCombos, int start, int num, CountTable* ct) {
	try {
		EstOutput results;
		vector<double> D;

		int count = 0;
		for (int h = start; h < (start + num); h++) {

			if (ctrlc_pressed) { return results; }

			//initialize weighted score
			string groupA = namesOfGroupCombos[h][0];
			string groupB = namesOfGroupCombos[h][1];

			set<int> validBranches;
			WScore[groupA + groupB] = 0.0;
			D.push_back(0.0000); //initialize a spot in D for each combination

			//adding the wieghted sums from group i
			for (int j = 0; j < t->groupNodeInfo[groupA].size(); j++) { //the leaf nodes that have seqs from group i
				map<string, int>::iterator it = t->tree[t->groupNodeInfo[groupA][j]].pcount.find(groupA);
				int numSeqsInGroupI = it->second;

				double sum = getLengthToRoot(t, t->groupNodeInfo[groupA][j], groupA, groupB);
				double weightedSum = ((numSeqsInGroupI * sum) / (double)ct->getGroupCount(groupA));

				D[count] += weightedSum;
			}

			//adding the wieghted sums from group l
			for (int j = 0; j < t->groupNodeInfo[groupB].size(); j++) { //the leaf nodes that have seqs from group l
				map<string, int>::iterator it = t->tree[t->groupNodeInfo[groupB][j]].pcount.find(groupB);
				int numSeqsInGroupL = it->second;

				double sum = getLengthToRoot(t, t->groupNodeInfo[groupB][j], groupA, groupB);
				double weightedSum = ((numSeqsInGroupL * sum) / (double)ct->getGroupCount(groupB));

				D[count] += weightedSum;
			}
			count++;
		}

		//calculate u for the group comb 
		for (int h = start; h < (start + num); h++) {
			//report progress
			//LOG(INFO) << "Processing combo: " + toString(h) << '\n';

			string groupA = namesOfGroupCombos[h][0];
			string groupB = namesOfGroupCombos[h][1];

			//calculate u for the group comb 
			for (int i = 0;i < t->getNumNodes();i++) {

				if (ctrlc_pressed) { return data; }

				double u;
				//int pcountSize = 0;
				//does this node have descendants from groupA
				it = t->tree[i].pcount.find(groupA);
				//if it does u = # of its descendants with a certain group / total number in tree with a certain group
				if (it != t->tree[i].pcount.end()) {
					u = (double)t->tree[i].pcount[groupA] / (double)ct->getGroupCount(groupA);
				}
				else { u = 0.00; }


				//does this node have descendants from group l
				it = t->tree[i].pcount.find(groupB);

				//if it does subtract their percentage from u
				if (it != t->tree[i].pcount.end()) {
					u -= (double)t->tree[i].pcount[groupB] / (double)ct->getGroupCount(groupB);
				}

				if (includeRoot) {
					if (t->tree[i].getBranchLength() != -1) {
						u = abs(u * t->tree[i].getBranchLength());
						WScore[(groupA + groupB)] += u;
					}
				}
				else {
					//if this is not the root then add it
					if (rootForGrouping[namesOfGroupCombos[h]].count(i) == 0) {
						if (t->tree[i].getBranchLength() != -1) {
							u = abs(u * t->tree[i].getBranchLength());
							WScore[(groupA + groupB)] += u;
						}
					}
				}
			}

		}

		/********************************************************/
		//calculate weighted score for the group combination
		double UN;
		count = 0;
		for (int h = start; h < (start + num); h++) {
			UN = (WScore[namesOfGroupCombos[h][0] + namesOfGroupCombos[h][1]] / D[count]);
			if (isnan(UN) || isinf(UN)) { UN = 0; }
			results.push_back(UN);
			count++;
		}

		return results;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in Weighted, driver";
		exit(1);
	}
}
/**************************************************************************************************/
EstOutput Weighted::getValues(Tree* t, string groupA, string groupB) {
	try {

		data.clear(); //clear out old values

		CountTable* ct = t->getCountTable();

		if (ctrlc_pressed) { return data; }

		//initialize weighted score
		WScore[(groupA + groupB)] = 0.0;
		double D = 0.0;
		set<int> validBranches;

		vector<string> groups; groups.push_back(groupA); groups.push_back(groupB);

		//adding the wieghted sums from group i
		for (int j = 0; j < t->groupNodeInfo[groups[0]].size(); j++) { //the leaf nodes that have seqs from group i
			map<string, int>::iterator it = t->tree[t->groupNodeInfo[groups[0]][j]].pcount.find(groups[0]);
			int numSeqsInGroupI = it->second;

			double sum = getLengthToRoot(t, t->groupNodeInfo[groups[0]][j], groups[0], groups[1]);
			double weightedSum = ((numSeqsInGroupI * sum) / (double)ct->getGroupCount(groups[0]));

			D += weightedSum;
		}

		//adding the wieghted sums from group l
		for (int j = 0; j < t->groupNodeInfo[groups[1]].size(); j++) { //the leaf nodes that have seqs from group l
			map<string, int>::iterator it = t->tree[t->groupNodeInfo[groups[1]][j]].pcount.find(groups[1]);
			int numSeqsInGroupL = it->second;

			double sum = getLengthToRoot(t, t->groupNodeInfo[groups[1]][j], groups[0], groups[1]);
			double weightedSum = ((numSeqsInGroupL * sum) / (double)ct->getGroupCount(groups[1]));

			D += weightedSum;
		}

		//calculate u for the group comb 
		for (int i = 0;i < t->getNumNodes();i++) {

			if (ctrlc_pressed) { return data; }

			double u;
			//int pcountSize = 0;
			//does this node have descendants from groupA
			it = t->tree[i].pcount.find(groupA);
			//if it does u = # of its descendants with a certain group / total number in tree with a certain group
			if (it != t->tree[i].pcount.end()) {
				u = (double)t->tree[i].pcount[groupA] / (double)ct->getGroupCount(groupA);
			}
			else { u = 0.00; }


			//does this node have descendants from group l
			it = t->tree[i].pcount.find(groupB);
			//if it does subtract their percentage from u
			if (it != t->tree[i].pcount.end()) {
				u -= (double)t->tree[i].pcount[groupB] / (double)ct->getGroupCount(groupB);
			}

			if (includeRoot) {
				if (t->tree[i].getBranchLength() != -1) {
					u = abs(u * t->tree[i].getBranchLength());
					WScore[(groupA + groupB)] += u;
				}
			}
			else {
				//if this is not the root then add it
				if (rootForGrouping[groups].count(i) == 0) {
					if (t->tree[i].getBranchLength() != -1) {
						u = abs(u * t->tree[i].getBranchLength());
						WScore[(groupA + groupB)] += u;
					}
				}
			}
		}
		/********************************************************/

		//calculate weighted score for the group combination
		double UN;
		UN = (WScore[(groupA + groupB)] / D);

		if (isnan(UN) || isinf(UN)) { UN = 0; }
		data.push_back(UN);

		return data;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in Weighted, getValues";
		exit(1);
	}
}
/**************************************************************************************************/
double Weighted::getLengthToRoot(Tree* t, int v, string groupA, string groupB) {
	try {

		double sum = 0.0;
		int index = v;

		//you are a leaf
		if (t->tree[index].getBranchLength() != -1) { sum += abs(t->tree[index].getBranchLength()); }
		double tempTotal = 0.0;
		index = t->tree[index].getParent();

		vector<string> grouping; grouping.push_back(groupA); grouping.push_back(groupB);

		rootForGrouping[grouping].insert(index);

		//while you aren't at root
		while (t->tree[index].getParent() != -1) {

			if (ctrlc_pressed) { return sum; }

			int parent = t->tree[index].getParent();

			if (includeRoot) { //add everyone
				if (t->tree[index].getBranchLength() != -1) { sum += abs(t->tree[index].getBranchLength()); }
			}
			else {

				//am I the root for this grouping? if so I want to stop "early"
				//does my sibling have descendants from the users groups? 
				int lc = t->tree[parent].getLChild();
				int rc = t->tree[parent].getRChild();

				int sib = lc;
				if (lc == index) { sib = rc; }

				map<string, int>::iterator itGroup;
				int pcountSize = 0;
				itGroup = t->tree[sib].pcount.find(groupA);
				if (itGroup != t->tree[sib].pcount.end()) { pcountSize++; }
				itGroup = t->tree[sib].pcount.find(groupB);
				if (itGroup != t->tree[sib].pcount.end()) { pcountSize++; }

				//if yes, I am not the root so add me
				if (pcountSize != 0) {
					if (t->tree[index].getBranchLength() != -1) {
						sum += abs(t->tree[index].getBranchLength()) + tempTotal;
						tempTotal = 0.0;
					}
					else {
						sum += tempTotal;
						tempTotal = 0.0;
					}
					rootForGrouping[grouping].clear();
					rootForGrouping[grouping].insert(parent);
				}
				else { //if no, I may be the root so add my br to tempTotal until I am proven innocent
					if (t->tree[index].getBranchLength() != -1) {
						tempTotal += abs(t->tree[index].getBranchLength());
					}
				}
			}

			index = parent;
		}

		//get all nodes above the root to add so we don't add their u values above
		index = *(rootForGrouping[grouping].begin());

		while (t->tree[index].getParent() != -1) {
			int parent = t->tree[index].getParent();
			rootForGrouping[grouping].insert(parent);
			index = parent;
		}
		return sum;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in Weighted, getBranchLengthSums";
		exit(1);
	}
}
/**************************************************************************************************/


