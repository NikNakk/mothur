/*
 *  clearmemorycommand.cpp
 *  Mothur
 *
 *  Created by westcott on 7/6/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "clearmemorycommand.h"
#include "referencedb.h"

 //**********************************************************************************************************************
vector<string> ClearMemoryCommand::setParameters() {
	try {
		vector<string> myArray;
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClearMemoryCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClearMemoryCommand::getHelpString() {
	try {
		string helpString = "The clear.memory command removes saved reference data from memory.\n"
			"The clear.memory command should be in the following format: clear.memory().\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClearMemoryCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************

ClearMemoryCommand::ClearMemoryCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }
}

//**********************************************************************************************************************

int ClearMemoryCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	ReferenceDB& rdb = ReferenceDB::getInstance();
	rdb.clearMemory();

	return 0;
}
//**********************************************************************************************************************/
