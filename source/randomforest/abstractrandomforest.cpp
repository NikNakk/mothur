//
//  abstractrandomforest.cpp
//  Mothur
//
//  Created by Sarah Westcott on 10/1/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "abstractrandomforest.hpp"

/***********************************************************************/
AbstractRandomForest::AbstractRandomForest(const std::vector < std::vector<int> > dataSet,
	const int numDecisionTrees,
	const string treeSplitCriterion = "informationGain")
	: dataSet(dataSet),
	numDecisionTrees(numDecisionTrees),
	numSamples((int)dataSet.size()),
	numFeatures((int)(dataSet[0].size() - 1)),
	globalDiscardedFeatureIndices(getGlobalDiscardedFeatureIndices()),
	globalVariableImportanceList(numFeatures, 0),
	treeSplitCriterion(treeSplitCriterion) {
	// TODO: double check if the implemenatation of 'globalOutOfBagEstimates' is correct
}

/***********************************************************************/

vector<int> AbstractRandomForest::getGlobalDiscardedFeatureIndices() {
	vector<int> globalDiscardedFeatureIndices;

	// calculate feature vectors
	vector< vector<int> > featureVectors(numFeatures, vector<int>(numSamples, 0));
	for (int i = 0; i < numSamples; i++) {
		if (ctrlc_pressed) { return globalDiscardedFeatureIndices; }
		for (int j = 0; j < numFeatures; j++) { featureVectors[j][i] = dataSet[i][j]; }
	}

	for (int i = 0; i < featureVectors.size(); i++) {
		if (ctrlc_pressed) { return globalDiscardedFeatureIndices; }
		double standardDeviation = m->getStandardDeviation(featureVectors[i]);
		if (standardDeviation <= 0) { globalDiscardedFeatureIndices.push_back(i); }
	}

	if (app.isDebug) {
		LOG(INFO) << "number of global discarded features:  " + toString(globalDiscardedFeatureIndices.size()) + "\n";
		LOG(INFO) << "total features: " + toString(featureVectors.size()) + "\n";
	}

	return globalDiscardedFeatureIndices;
}

/***********************************************************************/

