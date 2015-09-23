#ifndef FILTERS_H
#define FILTERS_H

/*
 *  filters.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 6/29/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "mothur.h"
#include "sequence.hpp"


/***********************************************************************/

class Filters {

public:
	Filters():
		numSeqs(0)
	{m = MothurOut::getInstance(); };
	~Filters(){};
		
	string getFilter()			{	return filter;		}
	void setFilter(string s)	{  filter = s;			}
	void setLength(int l)		{ alignmentLength = l;	}
	void setSoft(float s)		{		soft = s;		}
	void setTrump(float t)		{		trump = t;		}
	void setNumSeqs(int num)	{	numSeqs = num;		}
	int  getNumSeqs()			{ return numSeqs;		}
	vector<int> a, t, g, c, gap;
	
	
	void initialize() {
		a.assign(alignmentLength, 0);
		t.assign(alignmentLength, 0);
		g.assign(alignmentLength, 0);
		c.assign(alignmentLength, 0);
		gap.assign(alignmentLength, 0);
	}

	void doSoft() { 
		int threshold = int (soft * numSeqs);
	
		for(int i=0;i<alignmentLength;i++){
			if(a[i] < threshold && t[i] < threshold && g[i] < threshold && c[i] < threshold){	filter[i] = 0;	}
		}
	}

	void mergeFilter(string newFilter){
		for(int i=0;i<alignmentLength;i++){
			if(newFilter[i] == '0'){
				filter[i] = '0';
			}
		}
	}
	
	void doVertical() {

		for(int i=0;i<alignmentLength;i++){
			if(gap[i] == numSeqs)	{	filter[i] = '0';	}
		}
	
	}
	
	void doTrump(Sequence seq) {
	
		string curAligned = seq.getAligned();

		for(int j = 0; j < alignmentLength; j++) {
			if(curAligned[j] == trump){
				filter[j] = '0';
			}
		}

	}

	void doHard(string hard) {
		ifstream fileHandle;
		m->openInputFile(hard, fileHandle);
	
		fileHandle >> filter;
	
		fileHandle.close();
        
        if (filter.length() != alignmentLength) {  m->mothurOut("[ERROR]: Sequences are not all the same length as the filter, please correct.\n");  m->control_pressed = true; }
	}

	void getFreqs(Sequence seq) {
	
		string curAligned = seq.getAligned();
	
		for(int j=0;j<alignmentLength;j++){
			if(toupper(curAligned[j]) == 'A')										{	a[j]++;		}
			else if(toupper(curAligned[j]) == 'T' || toupper(curAligned[j]) == 'U')	{	t[j]++;		}
			else if(toupper(curAligned[j]) == 'G')									{	g[j]++;		}
			else if(toupper(curAligned[j]) == 'C')									{	c[j]++;		}
			else if(curAligned[j] == '-' || curAligned[j] == '.')					{	gap[j]++;	}
		}
	}
		
protected:
	string filter;
	int alignmentLength, numSeqs;
	float soft;
	char trump;
	MothurOut* m;

};

/***********************************************************************/

#endif

