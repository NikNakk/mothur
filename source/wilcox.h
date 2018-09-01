//
//  wilcox.h
//  Mothur
//
//  Created by SarahsWork on 8/6/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#pragma once


class PWilcox {
public:
	PWilcox() { mout = MothurOut::getInstance(); }
	~PWilcox() { }

	double pwilcox(double q, double m, double n, bool lower_tail);

private:

	MothurOut* mout;
	int allocated_m, allocated_n;
	double gammln(const double xx);
	double choose(double n, double k);
	double cwilcox(int k, int m, int n, double*** w);
};


