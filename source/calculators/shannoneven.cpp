/*
 *  shannoneven.cpp
 *  Mothur
 *
 *  Created by Pat Schloss on 8/21/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "shannoneven.h"
#include "shannon.h"

 /***********************************************************************/

EstOutput ShannonEven::getValues(SAbundVector* rank) {
	//vector<double> simpsonData(3,0);
	data.resize(1, 0);
	vector<double> shanData(3, 0);
	Shannon* shannon = new Shannon();
	shanData = shannon->getValues(rank);

	long int sobs = rank->getNumBins();
	if (sobs > 1) {
		data[0] = shanData[0] / log(sobs);
	}
	else {
		data[0] = 1;
	}

	delete shannon;
	return data;
}

/***********************************************************************/
