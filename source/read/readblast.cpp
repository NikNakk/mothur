/*
 *  readblast.cpp
 *  Mothur
 *
 *  Created by westcott on 12/10/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "readblast.h"
#include "progress.hpp"

 //********************************************************************************************************************
 //sorts lowest to highest
inline bool compareOverlap(seqDist left, seqDist right) {
	return (left.dist < right.dist);
}
/*********************************************************************************************/
ReadBlast::ReadBlast(string file, float c, float p, int l, bool ms, bool h) : blastfile(file), cutoff(c), penalty(p), length(l), minWanted(ms), hclusterWanted(h) {
	matrix = NULL;
}
/*********************************************************************************************/
//assumptions about the blast file: 
//1. if duplicate lines occur the first line is always best and is chosen
//2. blast scores are grouped together, ie. a a .... score, a b .... score, a c ....score...
int ReadBlast::read(NameAssignment* nameMap) {

	//if the user has not given a names file read names from blastfile
	if (nameMap->size() == 0) { readNames(nameMap); }
	int nseqs = nameMap->size();

	if (ctrlc_pressed) { return 0; }

	ifstream fileHandle;
	File::openInputFile(blastfile, fileHandle);

	string firstName, secondName, eScore, currentRow;
	string repeatName = "";
	int count = 1;
	float distance, thisoverlap, refScore;
	float percentId;
	float numBases, mismatch, gap, startQuery, endQuery, startRef, endRef, score, lengthThisSeq;

	ofstream outDist;
	ofstream outOverlap;

	//create objects needed for read
	if (!hclusterWanted) {
		matrix = new SparseDistanceMatrix();
		matrix->resize(nseqs);
	}
	else {
		overlapFile = File::getRootName(blastfile) + "overlap.dist";
		distFile = File::getRootName(blastfile) + "hclusterDists.dist";

		File::openOutputFile(overlapFile, outOverlap);
		File::openOutputFile(distFile, outDist);
	}

	if (ctrlc_pressed) {
		fileHandle.close();
		if (!hclusterWanted) { delete matrix; }
		else { outOverlap.close(); File::remove(overlapFile); outDist.close(); File::remove(distFile); }
		return 0;
	}

	Progress* reading = new Progress("Reading blast:     ", nseqs * nseqs);

	//this is used to quickly find if we already have a distance for this combo
	vector< map<int, float> > dists;  dists.resize(nseqs);  //dists[0][1] = distance from seq0 to seq1
	map<int, float> thisRowsBlastScores;

	if (!fileHandle.eof()) {
		//read in line from file
		fileHandle >> firstName >> secondName >> percentId >> numBases >> mismatch >> gap >> startQuery >> endQuery >> startRef >> endRef >> eScore >> score;
		File::gobble(fileHandle);

		currentRow = firstName;
		lengthThisSeq = numBases;
		repeatName = firstName + secondName;

		if (firstName == secondName) { refScore = score; }
		else {
			//convert name to number
			map<string, int>::iterator itA = nameMap->find(firstName);
			map<string, int>::iterator itB = nameMap->find(secondName);
			if (itA == nameMap->end()) { LOG(INFO) << "AAError: Sequence '" + firstName + "' was not found in the names file, please correct\n"; exit(1); }
			if (itB == nameMap->end()) { LOG(INFO) << "ABError: Sequence '" + secondName + "' was not found in the names file, please correct\n"; exit(1); }

			thisRowsBlastScores[itB->second] = score;

			//calc overlap score
			thisoverlap = 1.0 - (percentId * (lengthThisSeq - startQuery) / endRef / 100.0 - penalty);

			//if there is a valid overlap, add it
			if ((startRef <= length) && ((endQuery + length) >= lengthThisSeq) && (thisoverlap < cutoff)) {
				if (!hclusterWanted) {
					seqDist overlapValue(itA->second, itB->second, thisoverlap);
					overlap.push_back(overlapValue);
				}
				else {
					outOverlap << itA->first << '\t' << itB->first << '\t' << thisoverlap << endl;
				}
			}
		}
	}
	else { LOG(INFO) << "Error in your blast file, cannot read." << '\n'; exit(1); }


	//read file
	while (!fileHandle.eof()) {

		if (ctrlc_pressed) {
			fileHandle.close();
			if (!hclusterWanted) { delete matrix; }
			else { outOverlap.close(); File::remove(overlapFile); outDist.close(); File::remove(distFile); }
			delete reading;
			return 0;
		}

		//read in line from file
		fileHandle >> firstName >> secondName >> percentId >> numBases >> mismatch >> gap >> startQuery >> endQuery >> startRef >> endRef >> eScore >> score;
		//cout << firstName << '\t' << secondName << '\t' << percentId << '\t' << numBases << '\t' << mismatch << '\t' << gap << '\t' << startQuery << '\t' << endQuery << '\t' << startRef << '\t' << endRef << '\t' << eScore << '\t' << score << endl;	
		File::gobble(fileHandle);

		string temp = firstName + secondName; //to check if this file has repeat lines, ie. is this a blast instead of a blscreen file

		//if this is a new pairing
		if (temp != repeatName) {
			repeatName = temp;

			if (currentRow == firstName) {
				//cout << "first = " << firstName << " second = " << secondName << endl;	
				if (firstName == secondName) {
					refScore = score;
					reading->update((count + nseqs));
					count++;
				}
				else {
					//convert name to number
					map<string, int>::iterator itA = nameMap->find(firstName);
					map<string, int>::iterator itB = nameMap->find(secondName);
					if (itA == nameMap->end()) { LOG(INFO) << "AAError: Sequence '" + firstName + "' was not found in the names file, please correct\n"; exit(1); }
					if (itB == nameMap->end()) { LOG(INFO) << "ABError: Sequence '" + secondName + "' was not found in the names file, please correct\n"; exit(1); }

					//save score
					thisRowsBlastScores[itB->second] = score;

					//calc overlap score
					thisoverlap = 1.0 - (percentId * (lengthThisSeq - startQuery) / endRef / 100.0 - penalty);

					//if there is a valid overlap, add it
					if ((startRef <= length) && ((endQuery + length) >= lengthThisSeq) && (thisoverlap < cutoff)) {
						if (!hclusterWanted) {
							seqDist overlapValue(itA->second, itB->second, thisoverlap);
							//cout << "overlap = " << itA->second << '\t' << itB->second << '\t' << thisoverlap << endl;
							overlap.push_back(overlapValue);
						}
						else {
							outOverlap << itA->first << '\t' << itB->first << '\t' << thisoverlap << endl;
						}
					}
				} //end else
			}
			else { //end row
			   //convert blast scores to distance and add cell to sparse matrix if we can
				map<int, float>::iterator it;
				map<int, float>::iterator itDist;
				for (it = thisRowsBlastScores.begin(); it != thisRowsBlastScores.end(); it++) {
					distance = 1.0 - (it->second / refScore);


					//do we already have the distance calculated for b->a
					map<string, int>::iterator itA = nameMap->find(currentRow);
					itDist = dists[it->first].find(itA->second);

					//if we have it then compare
					if (itDist != dists[it->first].end()) {

						//if you want the minimum blast score ratio, then pick max distance
						if (minWanted) { distance = max(itDist->second, distance); }
						else { distance = min(itDist->second, distance); }

						//is this distance below cutoff
						if (distance < cutoff) {
							if (!hclusterWanted) {
								if (itA->second < it->first) {
									PDistCell value(it->first, distance);
									matrix->addCell(itA->second, value);
								}
								else {
									PDistCell value(itA->second, distance);
									matrix->addCell(it->first, value);
								}
							}
							else {
								outDist << itA->first << '\t' << nameMap->get(it->first) << '\t' << distance << endl;
							}
						}
						//not going to need this again
						dists[it->first].erase(itDist);
					}
					else { //save this value until we get the other ratio
						dists[itA->second][it->first] = distance;
					}
				}
				//clear out last rows info
				thisRowsBlastScores.clear();

				currentRow = firstName;
				lengthThisSeq = numBases;

				//add this row to thisRowsBlastScores
				if (firstName == secondName) { refScore = score; }
				else { //add this row to thisRowsBlastScores

					//convert name to number
					map<string, int>::iterator itA = nameMap->find(firstName);
					map<string, int>::iterator itB = nameMap->find(secondName);
					if (itA == nameMap->end()) { LOG(INFO) << "AAError: Sequence '" + firstName + "' was not found in the names file, please correct\n"; exit(1); }
					if (itB == nameMap->end()) { LOG(INFO) << "ABError: Sequence '" + secondName + "' was not found in the names file, please correct\n"; exit(1); }

					thisRowsBlastScores[itB->second] = score;

					//calc overlap score
					thisoverlap = 1.0 - (percentId * (lengthThisSeq - startQuery) / endRef / 100.0 - penalty);

					//if there is a valid overlap, add it
					if ((startRef <= length) && ((endQuery + length) >= lengthThisSeq) && (thisoverlap < cutoff)) {
						if (!hclusterWanted) {
							seqDist overlapValue(itA->second, itB->second, thisoverlap);
							overlap.push_back(overlapValue);
						}
						else {
							outOverlap << itA->first << '\t' << itB->first << '\t' << thisoverlap << endl;
						}
					}
				}
			}//end if current row
		}//end if repeat
	}//end while

	//get last rows info stored
	//convert blast scores to distance and add cell to sparse matrix if we can
	map<int, float>::iterator it;
	map<int, float>::iterator itDist;
	for (it = thisRowsBlastScores.begin(); it != thisRowsBlastScores.end(); it++) {
		distance = 1.0 - (it->second / refScore);

		//do we already have the distance calculated for b->a
		map<string, int>::iterator itA = nameMap->find(currentRow);
		itDist = dists[it->first].find(itA->second);

		//if we have it then compare
		if (itDist != dists[it->first].end()) {
			//if you want the minimum blast score ratio, then pick max distance
			if (minWanted) { distance = max(itDist->second, distance); }
			else { distance = min(itDist->second, distance); }

			//is this distance below cutoff
			if (distance < cutoff) {
				if (!hclusterWanted) {
					if (itA->second < it->first) {
						PDistCell value(it->first, distance);
						matrix->addCell(itA->second, value);
					}
					else {
						PDistCell value(itA->second, distance);
						matrix->addCell(it->first, value);
					}
				}
				else {
					outDist << itA->first << '\t' << nameMap->get(it->first) << '\t' << distance << endl;
				}
			}
			//not going to need this again
			dists[it->first].erase(itDist);
		}
		else { //save this value until we get the other ratio
			dists[itA->second][it->first] = distance;
		}
	}
	//clear out info
	thisRowsBlastScores.clear();
	dists.clear();

	if (ctrlc_pressed) {
		fileHandle.close();
		if (!hclusterWanted) { delete matrix; }
		else { outOverlap.close(); File::remove(overlapFile); outDist.close(); File::remove(distFile); }
		delete reading;
		return 0;
	}

	if (!hclusterWanted) {
		sort(overlap.begin(), overlap.end(), compareOverlap);
	}
	else {
		outDist.close();
		outOverlap.close();
	}

	if (ctrlc_pressed) {
		fileHandle.close();
		if (!hclusterWanted) { delete matrix; }
		else { File::remove(overlapFile);  File::remove(distFile); }
		delete reading;
		return 0;
	}

	reading->finish();
	delete reading;
	fileHandle.close();

	return 0;
}
/*********************************************************************************************/
int ReadBlast::readNames(NameAssignment* nameMap) {
	LOG(INFO) << "Reading names... "; cout.flush();

	string name, hold, prevName;
	int num = 1;

	ifstream in;
	File::openInputFile(blastfile, in);

	//ofstream outName;
	//File::openOutputFile((blastfile + ".tempOutNames"), outName);

	//read first line
	in >> prevName;

	for (int i = 0; i < 11; i++) { in >> hold; }
	File::gobble(in);

	//save name in nameMap
	nameMap->push_back(prevName);

	while (!in.eof()) {
		if (ctrlc_pressed) { in.close(); return 0; }

		//read line
		in >> name;

		for (int i = 0; i < 11; i++) { in >> hold; }
		File::gobble(in);

		//is this a new name?
		if (name != prevName) {
			prevName = name;

			if (nameMap->get(name) != -1) { LOG(LOGERROR) << "trying to exact names from blast file, and I found dups.  Are you sequence names unique? quitting.\n"; ctrlc_pressed = true; }
			else {
				nameMap->push_back(name);
			}
			//outName << name << '\t' << name << endl;
			num++;
		}
	}

	in.close();

	//write out names file
	//string outNames = File::getRootName(blastfile) + "names";
	//ofstream out;
	//File::openOutputFile(outNames, out);
	//nameMap->print(out);
	//out.close();

	if (ctrlc_pressed) { return 0; }

	LOG(INFO) << toString(num) + " names read." << '\n';

	return 0;

}
/*********************************************************************************************/



