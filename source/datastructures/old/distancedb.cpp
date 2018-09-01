/*
 *  distancedb.cpp
 *
 *
 *  Created by Pat Schloss on 12/29/08.
 *  Copyright 2008 Patrick D. Schloss. All rights reserved.
 *
 */


#include "database.hpp"
#include "sequence.hpp"
#include "distancedb.hpp"
#include "onegapignore.h"


 /**************************************************************************************************/
DistanceDB::DistanceDB() : Database() {
	try {
		templateAligned = true;
		templateSeqsLength = 0;
		distCalculator = new oneGapIgnoreTermGapDist();
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DistanceDB, DistanceDB";
		exit(1);
	}
}
/**************************************************************************************************/
void DistanceDB::addSequence(Sequence seq) {
	//are the template sequences aligned
	if (!isAligned(seq.getAligned())) {
		templateAligned = false;
		LOG(INFO) << seq.getName() + " is not aligned. Sequences must be aligned to use the distance method.";
		LOG(INFO) << "";
	}

	if (templateSeqsLength == 0) { templateSeqsLength = seq.getAligned().length(); }

	data.push_back(seq);
}
/**************************************************************************************************/
//returns indexes to top matches
vector<int> DistanceDB::findClosestSequences(Sequence* query, int numWanted) {
	vector<int> topMatches;
	Scores.clear();
	bool templateSameLength = true;
	string sequence = query->getAligned();
	vector<seqDist> dists;

	searchScore = -1.0;

	if (numWanted > data.size()) {
		LOG(INFO) << "numwanted is larger than the number of template sequences, using " + toString(data.size()) + ".";
		LOG(INFO) << "";
		numWanted = data.size();
	}

	if (sequence.length() != templateSeqsLength) { templateSameLength = false; }

	if (templateSameLength && templateAligned) {
		if (numWanted != 1) {

			dists.resize(data.size());

			//calc distance from this sequence to every sequence in the template
			for (int i = 0; i < data.size(); i++) {
				distCalculator->calcDist(*query, data[i]);
				float dist = distCalculator->getDist();

				//save distance to each template sequence
				dists[i].seq1 = -1;
				dists[i].seq2 = i;
				dists[i].dist = dist;
			}

			sort(dists.begin(), dists.end(), compareSequenceDistance);  //sorts by distance lowest to highest

			//save distance of best match
			searchScore = dists[0].dist;

			//fill topmatches with numwanted closest sequences indexes
			for (int i = 0; i < numWanted; i++) {
				topMatches.push_back(dists[i].seq2);
				Scores.push_back(dists[i].dist);
			}
		}
		else {
			int bestIndex = 0;
			float smallDist = 100000;
			for (int i = 0; i < data.size(); i++) {
				distCalculator->calcDist(*query, data[i]);
				float dist = distCalculator->getDist();

				//are you smaller?
				if (dist < smallDist) {
					bestIndex = i;
					smallDist = dist;
				}
			}
			searchScore = smallDist;
			topMatches.push_back(bestIndex);
			Scores.push_back(smallDist);
		}

	}
	else {
		LOG(INFO) << "cannot find closest matches using distance method for " + query->getName() + " without aligned template sequences of the same length.";
		LOG(INFO) << "";
		exit(1);
	}

	return topMatches;
}
/**************************************************************************************************/
bool DistanceDB::isAligned(string seq) {
	bool aligned;

	int pos = seq.find_first_of(".-");

	if (pos != seq.npos) {
		aligned = true;
	}
	else { aligned = false; }


	return aligned;
}

/**************************************************************************************************/
