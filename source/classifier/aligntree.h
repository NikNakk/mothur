//
//  alignTree.h
//  pdsBayesian
//
//  Created by Patrick Schloss on 4/3/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#pragma once

#include "classify.h"

class AlignNode;

class AlignTree : public Classify {

public:
	AlignTree(string, string, int);
	~AlignTree();
	string getTaxonomy(Sequence*);

private:
	int addTaxonomyToTree(string, string&, string&);
	double getOutlierLogProbability(string&);
	int getMinRiskIndexAlign(string&, vector<int>&, vector<double>&);
	int aggregateThetas();
	int sanityCheck(vector<vector<int> >&, vector<int>&);

	int numSeqs, confidenceThreshold, length;
	vector<AlignNode*> tree;
};


