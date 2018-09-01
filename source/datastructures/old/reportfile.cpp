/*
 *  reportfile.cpp
 *  Mothur
 *
 *  Created by Pat Schloss on 12/19/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "mothur.h"
#include "reportfile.h"

 /**************************************************************************************************/

ReportFile::ReportFile() {
}
catch (exception& e) {
	LOG(FATAL) << e.what() << " in ReportFile, ReportFile";
	exit(1);
}
}

/**************************************************************************************************/

int ReportFile::readHeaders(ifstream& repFile, string repFileName) {
	try {
		File::openInputFile(repFileName, repFile);
		File::getline(repFile);
		return 0;
	}


	/**************************************************************************************************/

	int ReportFile::read(ifstream& repFile) {

		repFile >> queryName;
		repFile >> queryLength;
		repFile >> templateName;
		repFile >> templateLength;
		repFile >> searchMethod;
		repFile >> dummySearchScore;
		repFile >> alignmentMethod;
		repFile >> queryStart;
		repFile >> queryEnd;
		repFile >> templateStart;
		repFile >> templateEnd;
		repFile >> pairwiseAlignmentLength;
		repFile >> gapsInQuery;
		repFile >> gapsInTemplate;
		repFile >> longestInsert;
		repFile >> simBtwnQueryAndTemplate;

		if (dummySearchScore != "nan") {
			istringstream stream(dummySearchScore);
			stream >> searchScore;
		}
		else {
			searchScore = 0;
		}

		File::gobble(repFile);
		return 0;

	}

	/**************************************************************************************************/
