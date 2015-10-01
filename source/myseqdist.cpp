/*
 *  pds.seqdist.cpp
 *
 *
 *  Created by Pat Schloss on 8/12/11.
 *  Copyright 2011 Patrick D. Schloss. All rights reserved.
 *
 */

#include "myseqdist.h"
#include "sequence.hpp"

 /**************************************************************************************************/
correctDist::correctDist(int p) : processors(p) {
}
catch (exception& e) {
	LOG(FATAL) << e.what() << " in correctDist, correctDist";
	exit(1);
}
}
/**************************************************************************************************/
correctDist::correctDist(string sequenceFileName, int p) : processors(p) {
	try {
		getSequences(sequenceFileName);
	}
	/**************************************************************************************************/
	int correctDist::addSeq(string seqName, string seqSeq) {
		names.push_back(seqName);
		sequences.push_back(fixSequence(seqSeq));
		return 0;
	}
	/**************************************************************************************************/
	int correctDist::execute(string distanceFileName) {
#if defined (UNIX)
#else
		processors = 1;
#endif
		correctMatrix.resize(4);
		for (int i = 0;i < 4;i++) { correctMatrix[i].resize(4); }

		correctMatrix[0][0] = 0.000000;		//AA
		correctMatrix[1][0] = 11.619259;	//CA
		correctMatrix[2][0] = 11.694004;	//TA
		correctMatrix[3][0] = 7.748623;		//GA

		correctMatrix[1][1] = 0.000000;		//CC
		correctMatrix[2][1] = 7.619657;		//TC
		correctMatrix[3][1] = 12.852562;	//GC

		correctMatrix[2][2] = 0.000000;		//TT
		correctMatrix[3][2] = 10.964048;	//TG

		correctMatrix[3][3] = 0.000000;		//GG

		for (int i = 0;i < 4;i++) {
			for (int j = 0;j < i;j++) {
				correctMatrix[j][i] = correctMatrix[i][j];
			}
		}

		numSeqs = names.size();

		if (processors == 1) { driver(0, numSeqs, distanceFileName); }
		else {

			for (int i = 0;i < processors;i++) {
				start.push_back(int(sqrt(float(i) / float(processors)) * numSeqs));
				end.push_back(int(sqrt(float(i + 1) / float(processors)) * numSeqs));
			}

			createProcess(distanceFileName);
		}

		return 0;
	}
	/**************************************************************************************************/
	int correctDist::getSequences(string sequenceFileName) {
		ifstream sequenceFile;
		File::openInputFile(sequenceFileName, sequenceFile);
		string seqName, seqSeq;

		while (!sequenceFile.eof()) {
			if (ctrlc_pressed) { break; }

			Sequence temp(sequenceFile); File::gobble(sequenceFile);

			if (temp.getName() != "") {
				names.push_back(temp.getName());
				sequences.push_back(fixSequence(temp.getAligned()));
			}
		}
		sequenceFile.close();
		return 0;
	}

	/**************************************************************************************************/
	vector<int> correctDist::fixSequence(string sequence) {
		int alignLength = sequence.length();

		vector<int> seqVector;

		for (int i = 0;i < alignLength;i++) {
			if (sequence[i] == 'A') { seqVector.push_back(0); }
			else if (sequence[i] == 'C') { seqVector.push_back(1); }
			else if (sequence[i] == 'T') { seqVector.push_back(2); }
			else if (sequence[i] == 'G') { seqVector.push_back(3); }
			else if (sequence[i] == 'N') { seqVector.push_back(0); }//hmmmm....
		}

		return seqVector;
	}

	/**************************************************************************************************/

	int correctDist::createProcess(string distanceFileName) {
#if defined (UNIX)
		int process = 1;
		vector<int> processIDs;
		bool recalc = false;

		while (process != processors) {

			pid_t pid = fork();

			if (pid > 0) {
				processIDs.push_back(pid);
				process++;
			}
			else if (pid == 0) {
				driver(start[process], end[process], distanceFileName + m->mothurGetpid(process) + ".temp");
				exit(0);
			}
			else {
				LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(process) + "\n"; processors = process;
				for (int i = 0; i < processIDs.size(); i++) { kill(processIDs[i], SIGINT); }
				//wait to die
				for (int i = 0;i < processIDs.size();i++) {
					int temp = processIDs[i];
					wait(&temp);
				}
				ctrlc_pressed = false;
				recalc = true;
				break;
			}
		}

		if (recalc) {
			start.clear(); end.clear();
			for (int i = 0;i < processors;i++) {
				start.push_back(int(sqrt(float(i) / float(processors)) * numSeqs));
				end.push_back(int(sqrt(float(i + 1) / float(processors)) * numSeqs));
			}

			processIDs.resize(0);
			process = 1;

			while (process != processors) {

				pid_t pid = fork();

				if (pid > 0) {
					processIDs.push_back(pid);
					process++;
				}
				else if (pid == 0) {
					driver(start[process], end[process], distanceFileName + m->mothurGetpid(process) + ".temp");
					exit(0);
				}
				else {
					LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
					for (int i = 0;i < processIDs.size();i++) { int temp = processIDs[i]; kill(temp, SIGINT); }
					exit(0);
				}
			}
		}

		driver(start[0], end[0], distanceFileName);

		for (int i = 0;i < processIDs.size();i++) {
			int temp = processIDs[i];
			wait(&temp);
		}

		for (int i = 0;i < processIDs.size();i++) {
			File::appendFiles((distanceFileName + toString(processIDs[i]) + ".temp"), distanceFileName);
			remove((distanceFileName + toString(processIDs[i]) + ".temp").c_str());
		}
#endif
		return 0;
	}

	/**************************************************************************************************/

	int correctDist::driver(int start, int end, string distFileName) {
		ofstream distFile;
		File::openOutputFile(distFileName, distFile);
		distFile << setprecision(9);

		if (start == 0) {
			distFile << numSeqs << endl;
		}

		int startTime = time(NULL);

		LOG(INFO) << "\nCalculating distances...\n";

		for (int i = start;i < end;i++) {

			distFile << i;

			for (int j = 0;j < i;j++) {

				if (ctrlc_pressed) { distFile.close(); return 0; }

				double dist = getDist(sequences[i], sequences[j]);

				distFile << ' ' << dist;
			}
			distFile << endl;

			if (i % 100 == 0) { LOG(SCREENONLY) << toString(i) + "\t" + toString(time(NULL) - startTime) + "\n"; }
		}
		distFile.close();

		if ((end - 1) % 100 != 0) { LOG(SCREENONLY) << toString(end - 1) + "\t" + toString(time(NULL) - startTime) + "\n"; }
		LOG(INFO) << "Done.\n";

		return 0;
	}
	/**************************************************************************************************/
	double correctDist::getDist(vector<int>& seqA, vector<int>& seqB) {

		int lengthA = seqA.size();
		int lengthB = seqB.size();

		vector<vector<double> > alignMatrix(lengthA + 1);
		vector<vector<char> > alignMoves(lengthA + 1);

		for (int i = 0;i <= lengthA;i++) {
			alignMatrix[i].resize(lengthB + 1, 0);
			alignMoves[i].resize(lengthB + 1, 'x');
		}

		for (int i = 0;i <= lengthA;i++) {
			alignMatrix[i][0] = 15.0 * i;
			alignMoves[i][0] = 'u';
		}
		for (int i = 0;i <= lengthB;i++) {
			alignMatrix[0][i] = 15.0 * i;
			alignMoves[0][i] = 'l';
		}

		for (int i = 1;i <= lengthA;i++) {
			for (int j = 1;j <= lengthB;j++) {

				if (ctrlc_pressed) { return 0; }

				double nogap;
				nogap = alignMatrix[i - 1][j - 1] + correctMatrix[seqA[i - 1]][seqB[j - 1]];


				double gap;
				double left;
				if (i == lengthA) { //terminal gap
					left = alignMatrix[i][j - 1];
				}
				else {
					if (seqB[j - 1] == getLastMatch('l', alignMoves, i, j, seqA, seqB)) {
						gap = 4.0;
					}
					else {
						gap = 15.0;
					}

					left = alignMatrix[i][j - 1] + gap;
				}


				double up;
				if (j == lengthB) { //terminal gap
					up = alignMatrix[i - 1][j];
				}
				else {

					if (seqA[i - 1] == getLastMatch('u', alignMoves, i, j, seqA, seqB)) {
						gap = 4.0;
					}
					else {
						gap = 15.0;
					}

					up = alignMatrix[i - 1][j] + gap;
				}



				if (nogap < left) {
					if (nogap < up) {
						alignMoves[i][j] = 'd';
						alignMatrix[i][j] = nogap;
					}
					else {
						alignMoves[i][j] = 'u';
						alignMatrix[i][j] = up;
					}
				}
				else {
					if (left < up) {
						alignMoves[i][j] = 'l';
						alignMatrix[i][j] = left;
					}
					else {
						alignMoves[i][j] = 'u';
						alignMatrix[i][j] = up;
					}
				}
			}
		}

		int i = lengthA;
		int j = lengthB;
		int count = 0;


		//	string alignA = "";
		//	string alignB = "";
		//	string bases = "ACTG";
		//	
		//	for(int i=0;i<lengthA;i++){
		//		cout << bases[seqA[i]];
		//	}cout << endl;
		//
		//	for(int i=0;i<lengthB;i++){
		//		cout << bases[seqB[i]];
		//	}cout << endl;

		while (i > 0 && j > 0) {

			if (ctrlc_pressed) { return 0; }

			if (alignMoves[i][j] == 'd') {
				//			alignA = bases[seqA[i-1]] + alignA;
				//			alignB = bases[seqB[j-1]] + alignB;

				count++;
				i--;
				j--;
			}
			else if (alignMoves[i][j] == 'u') {
				if (j != lengthB) {
					//				alignA = bases[seqA[i-1]] + alignA;
					//				alignB = '-' + alignB;
					count++;
				}

				i--;
			}
			else if (alignMoves[i][j] == 'l') {
				if (i != lengthA) {
					//				alignA = '-' + alignA;
					//				alignB = bases[seqB[j-1]] + alignB;
					count++;
				}

				j--;
			}
		}

		//	cout << alignA << endl << alignB << endl;

		return alignMatrix[lengthA][lengthB] / (double)count;
	}
	/**************************************************************************************************/
	int correctDist::getLastMatch(char direction, vector<vector<char> >& alignMoves, int i, int j, vector<int>& seqA, vector<int>& seqB) {

		char nullReturn = -1;

		while (i >= 1 && j >= 1) {

			if (ctrlc_pressed) { return nullReturn; }

			if (direction == 'd') {
				if (seqA[i - 1] == seqB[j - 1]) { return seqA[i - 1]; }
				else { return nullReturn; }
			}

			else if (direction == 'l') { j--; }
			else { i--; }

			direction = alignMoves[i][j];
		}

		return nullReturn;
	}
	/**************************************************************************************************/



