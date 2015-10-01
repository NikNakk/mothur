#pragma once

/*
 *  unweighted.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 2/9/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "treecalculator.h"
#include "counttable.h"


 /***********************************************************************/

class Unweighted : public TreeCalculator {

public:
	Unweighted(bool r) : includeRoot(r) {};
	~Unweighted() {};
	EstOutput getValues(Tree*, int, string);
	EstOutput getValues(Tree*, string, string, int, string);

private:
	struct linePair {
		int start;
		int num;
		linePair(int i, int j) : start(i), num(j) {}
	};
	vector<linePair> lines;

	EstOutput data;
	int processors;
	string outputDir;
	map< vector<string>, set<int> > rootForGrouping;  //maps a grouping combo to the roots for that combo
	bool includeRoot;

	void driverWithData(Tree * t, vector<vector<string>> namesOfGroupCombos, int start, int num, CountTable * ct, EstOutput & output);

	EstOutput driver(Tree * t, vector<vector<string>> namesOfGroupCombos, int start, int num, CountTable * ct);

	EstOutput createProcesses(Tree*, vector< vector<string> >, CountTable*);
	EstOutput driver(Tree*, vector< vector<string> >, int, int, bool, CountTable*);
	int getRoot(Tree*, int, vector<string>);
	EstOutput createProcesses(Tree * t, vector<vector<string>> namesOfGroupCombos, bool usingGroups, CountTable * ct);
};

/***********************************************************************/
