#pragma once

/*
 *  memeuclidean.h
 *  Mothur
 *
 *  Created by westcott on 12/17/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */


#include "calculator.h"

/***********************************************************************/

class MemEuclidean : public Calculator  {
	
public:
	MemEuclidean() :  Calculator("memeuclidean", 1, false) {};  
	EstOutput getValues(SAbundVector*) {return data;};
	EstOutput getValues(vector<SharedRAbundVector*>);
	string getCitation() { return "http://www.mothur.org/wiki/Memeuclidean"; }
private:
	
};

/***********************************************************************/


