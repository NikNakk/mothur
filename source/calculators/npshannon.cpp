/*
 *  npshannon.cpp
 *  Dotur
 *
 *  Created by John Westcott on 1/7/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "npshannon.h"

 /***********************************************************************/


EstOutput NPShannon::getValues(SAbundVector* rank) {
	data.resize(1, 0);
	float npShannon = 0.0000;

	double maxRank = static_cast<double>(rank->getMaxRank());
	double sampled = rank->getNumSeqs();

	double Chat = 1.0000 - static_cast<double>(rank->get(1)) / static_cast<double>(sampled);

	if (Chat > 0) {
		for (int i = 1;i <= maxRank;i++) {
			double pi = (static_cast<double>(i)) / (static_cast<double>(sampled));
			double ChatPi = Chat*pi;
			if (ChatPi > 0) {
				npShannon += static_cast<float>(rank->get(i) * ChatPi*log(ChatPi) / (1 - pow(1 - ChatPi, static_cast<double>(sampled))));
			}
		}
		npShannon = -npShannon;
	}
	else {
		npShannon = 0.000;
	}

	data[0] = npShannon;

	if (isnan(data[0]) || isinf(data[0])) { data[0] = 0; }

	return data;
}

/***********************************************************************/
