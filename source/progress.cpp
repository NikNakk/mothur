/*
 *  progress.cpp
 *
 *
 *  Created by Pat Schloss on 8/14/08.
 *  Copyright 2008 Patrick D. Schloss. All rights reserved.
 *
 */


#include "progress.hpp"

const int totalTicks = 50;
const char marker = '|';


/***********************************************************************/

Progress::Progress() {
	LOG(INFO) << "********************#****#****#****#****#****#****#****#****#****#****#";

	nTicks = 0;
	finalPos = 0;
}

/***********************************************************************/

Progress::Progress(string job, int end) {

	LOG(INFO) << "********************#****#****#****#****#****#****#****#****#****#****#\n";
	cout << setw(20) << left << job << setw(1) << marker;
	LOG(FILEONLY) << job;
	LOG(INFO) << toString(marker);
	cout.flush();

	nTicks = 0;
	finalPos = end;

}

/***********************************************************************/

void Progress::newLine(string job, int end) {
	LOG(INFO) << "";
	cout << setw(20) << left << job << setw(1) << marker;
	LOG(FILEONLY) << job;
	LOG(INFO) << toString(marker);
	cout.flush();

	nTicks = 0;
	finalPos = end;
}

/***********************************************************************/

void Progress::update(const int currentPos) {
	int ratio = int(totalTicks * (float)currentPos / finalPos);

	if (ratio > nTicks) {
		for (int i = nTicks;i < ratio;i++) {
			LOG(INFO) << toString(marker);
			cout.flush();
		}
		nTicks = ratio;
	}
}

/***********************************************************************/

void Progress::finish() {
	for (int i = nTicks;i < totalTicks;i++) {
		LOG(INFO) << toString(marker);
		cout.flush();
	}


	LOG(INFO) << '\n' << "***********************************************************************\n";
	cout.flush();
}

/***********************************************************************/
