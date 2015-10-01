/*
 *  systemcommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 7/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "systemcommand.h"

 //**********************************************************************************************************************
vector<string> SystemCommand::setParameters() {
	try {
		nkParameters.add(new StringParameter("command", true, true));
		return nkParameters.getNames();
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SystemCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************

string SystemCommand::getHelpString() {
	string helpString = "The system command allows you to execute a system command from within mothur.\n"
		"The system has no parameters.\n"
		"The system command should be in the following format: system(yourCommand).\n"
		"Example system(clear).\n";
	return helpString;
}

//**********************************************************************************************************************

int SystemCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		//if command contains a redirect don't add the redirect
		string command = nkParameters["command"]->getValue();
		bool usedRedirect = false;
		if ((command.find('>')) == string::npos) {
			command += " > ./commandScreen.output 2>&1";
			usedRedirect = true;
		}

		system(command.c_str());

		if (usedRedirect) {
			ifstream in;
			string filename = "./commandScreen.output";
			File::openInputFile(filename, in, "no error");

			string output = "";
			while (char c = in.get()) {
				if (in.eof()) { break; }
				else { output += c; }
			}
			in.close();

			LOG(INFO) << output << '\n';
			File::remove(filename);
		}

		return 0;
	}

	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SystemCommand, execute";
		exit(1);
	}
}

//**********************************************************************************************************************
