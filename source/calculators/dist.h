#pragma once
/*
 *  dist.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 5/7/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "mothur.h"
#include "sequence.hpp"

/**************************************************************************************************/

class Dist {
	
public:
	Dist(){ dist = 0; }
	Dist(const Dist& d) : dist(d.dist) { }
	virtual ~Dist() {}
	virtual void calcDist(Sequence, Sequence) = 0;
	double getDist()	{	return dist;	}

protected:
	double dist;
};

/**************************************************************************************************/


