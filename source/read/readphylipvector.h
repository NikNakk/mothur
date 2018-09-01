#pragma once

/*
 *  readphylipvector.h
 *  mothur
 *
 *  Created by westcott on 1/11/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */


#include "mothur.h"

 /******************************************************/

class ReadPhylipVector {

public:
	ReadPhylipVector(string); //phylipfile - lt or square
	~ReadPhylipVector() {}
	vector<string> read(vector< vector<double> >&); //pass in matrix to fill with values, returns vector of strings containing names in phylipfile
	vector<string> read(vector<seqDist>&); //pass in matrix to fill with values, returns vector of strings containing names in phylipfile

private:
	string distFile;
};

/******************************************************/


