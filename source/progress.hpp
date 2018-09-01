#pragma once

#include "mothur.h"

class Progress {

public:
	Progress();
	Progress(string, int);
	void update(int);
	void newLine(string, int);
	void finish();

private:
	int nTicks;
	int finalPos;
};


