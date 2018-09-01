#pragma once

/*
 *  mothurmetastats.h
 *  Mothur
 *
 *  Created by westcott on 7/6/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */



class MothurMetastats {

public:
	MothurMetastats(double, int); //threshold, numPermutations
	~MothurMetastats();

	int runMetastats(string, vector< vector<double> >&, int); //outputFileName, data, secondGroupingStart

private:
	int row, column, numPermutations, secondGroupingStart;
	double threshold;

	vector<double> permuted_pvalues(vector< vector<double> >&, vector<double>&, vector< vector<double> >&);
	vector<double> permute_and_calc_ts(vector< vector<double> >&);

	int start(vector<double>&, int, vector<double>&, vector< vector<double> >&); //Find the initial values for the matrix
	int meanvar(vector<double>&, int, vector<double>&);
	int testp(vector<double>&, vector<double>&, vector<double>&, int, vector<double>&, vector<double>&);
	int permute_matrix(vector<double>&, vector<double>&, int, vector<double>&, vector<double>&, vector<double>&);
	int permute_array(vector<int>&);
	int calc_twosample_ts(vector<double>&, int, vector<double>&, vector<double>&, vector<double>&);
	int OrderPValues(int, int, vector<double>&, vector<int>&);
	int swapElements(int, int, vector<double>&, vector<int>&);
	vector<int> getSequence(int, int, int);

};



