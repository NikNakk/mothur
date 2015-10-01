//
//  forest.cpp
//  Mothur
//
//  Created by Kathryn Iverson on 10/26/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "forest.h"

/***********************************************************************/
Forest::Forest(const std::vector < std::vector<int> > dataSet,
	const int numDecisionTrees,
	const string treeSplitCriterion = "gainratio",
	const bool doPruning = false,
	const float pruneAggressiveness = 0.9,
	const bool discardHighErrorTrees = true,
	const float highErrorTreeDiscardThreshold = 0.4,
	const string optimumFeatureSubsetSelectionCriteria = "log2",
	const float featureStandardDeviationThreshold = 0.0)
	: dataSet(dataSet),
	numDecisionTrees(numDecisionTrees),
	numSamples((int)dataSet.size()),
	numFeatures((int)(dataSet[0].size() - 1)),
	globalVariableImportanceList(numFeatures, 0),
	treeSplitCriterion(treeSplitCriterion),
	doPruning(doPruning),
	pruneAggressiveness(pruneAggressiveness),
	discardHighErrorTrees(discardHighErrorTrees),
	highErrorTreeDiscardThreshold(highErrorTreeDiscardThreshold),
	optimumFeatureSubsetSelectionCriteria(optimumFeatureSubsetSelectionCriteria),
	featureStandardDeviationThreshold(featureStandardDeviationThreshold)
{
	globalDiscardedFeatureIndices = getGlobalDiscardedFeatureIndices();
	// TODO: double check if the implemenatation of 'globalOutOfBagEstimates' is correct
}

/***********************************************************************/

vector<int> Forest::getGlobalDiscardedFeatureIndices() {
	//vector<int> globalDiscardedFeatureIndices;
	//globalDiscardedFeatureIndices.push_back(1);

	// calculate feature vectors
	vector< vector<int> > featureVectors(numFeatures, vector<int>(numSamples, 0));
	for (int i = 0; i < numSamples; i++) {
		if (ctrlc_pressed) { return globalDiscardedFeatureIndices; }
		for (int j = 0; j < numFeatures; j++) { featureVectors[j][i] = dataSet[i][j]; }
	}

	for (int i = 0; i < featureVectors.size(); i++) {
		if (ctrlc_pressed) { return globalDiscardedFeatureIndices; }
		double standardDeviation = m->getStandardDeviation(featureVectors[i]);
		if (standardDeviation <= featureStandardDeviationThreshold) { globalDiscardedFeatureIndices.push_back(i); }
	}

	if (app.isDebug) {
		LOG(INFO) << "number of global discarded features:  " + toString(globalDiscardedFeatureIndices.size()) + "\n";
		LOG(INFO) << "total features: " + toString(featureVectors.size()) + "\n";
	}

	return globalDiscardedFeatureIndices;
}

/***********************************************************************/

