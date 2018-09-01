//
//  regularizedrandomforest.h
//  Mothur
//
//  Created by Kathryn Iverson on 11/16/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#pragma once

#include "forest.h"
#include "decisiontree.hpp"

class RegularizedRandomForest : public Forest {
public:
	//
	RegularizedRandomForest(const vector <vector<int> > dataSet, const int numDecisionTrees, const string);

	int calcForrestErrorRate();
	int calcForrestVariableImportance(string);
	int populateDecisionTrees();
	int updateGlobalOutOfBagEstimates(DecisionTree* decisionTree);

private:
	//
};

 /* defined(__Mothur__regularizedrandomforest__) */
