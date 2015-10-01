/*
 *  getcurrentcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 3/16/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "getcurrentcommand.h"

 //**********************************************************************************************************************
vector<string> GetCurrentCommand::setParameters() {
	try {
		nkParameters.add(new StringParameter("clear"));
		nkParameters.addStandardParameters();
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetCurrentCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetCurrentCommand::getHelpString() {
	try {
		string helpString = ""
			"The get.current command outputs the current files saved by mothur.\n"
			"The get.current command has one parameter: clear.\n"
			"The clear parameter is used to indicate which file types you would like to clear values for, multiple types can be separated by dashes.\n"
			"The get.current command should be in the following format: \n"
			"get.current() or get.current(clear=fasta-name-accnos)\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetCurrentCommand, getHelpString";
		exit(1);
	}
}

int GetCurrentCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		//user wants to clear a type
		if (types.size() != 0) {
			for (int i = 0; i < types.size(); i++) {

				if (ctrlc_pressed) { break; }

				//look for file types
				if (types[i] == "fasta") {
					settings.setCurrent("fasta", "");
				}
				else if (types[i] == "qfile") {
					settings.setCurrent("qual", "");
				}
				else if (types[i] == "phylip") {
					settings.setCurrent("phylip", "");
				}
				else if (types[i] == "column") {
					settings.setCurrent("column", "");
				}
				else if (types[i] == "list") {
					settings.setCurrent("list", "");
				}
				else if (types[i] == "rabund") {
					settings.setCurrent("rabund", "");
				}
				else if (types[i] == "sabund") {
					settings.setCurrent("sabund", "");
				}
				else if (types[i] == "name") {
					settings.setCurrent("name", "");
				}
				else if (types[i] == "group") {
					settings.setCurrent("group", "");
				}
				else if (types[i] == "order") {
					settings.setCurrent("order", "");
				}
				else if (types[i] == "ordergroup") {
					settings.setCurrent("ordergroup", "");
				}
				else if (types[i] == "tree") {
					settings.setCurrent("tree", "");
				}
				else if (types[i] == "shared") {
					settings.setCurrent("shared", "");
				}
				else if (types[i] == "relabund") {
					settings.setCurrent("relabund", "");
				}
				else if (types[i] == "design") {
					settings.setCurrent("design", "");
				}
				else if (types[i] == "sff") {
					settings.setCurrent("sff", "");
				}
				else if (types[i] == "oligos") {
					settings.setCurrent("oligos", "");
				}
				else if (types[i] == "accnos") {
					settings.setCurrent("accnos", "");
				}
				else if (types[i] == "taxonomy") {
					settings.setCurrent("taxonomy", "");
				}
				else if (types[i] == "flow") {
					settings.setCurrent("flow", "");
				}
				else if (types[i] == "biom") {
					settings.setCurrent("biom", "");
				}
				else if (types[i] == "count") {
					settings.setCurrent("counttable", "");
				}
				else if (types[i] == "summary") {
					settings.setCurrent("summary", "");
				}
				else if (types[i] == "file") {
					settings.setCurrent("file", "");
				}
				else if (types[i] == "processors") {
					settings.setProcessors("1");
				}
				else if (types[i] == "all") {
					m->clearCurrentFiles();
				}
				else {
					LOG(INFO) << "[ERROR]: mothur does not save a current file for " + types[i] << '\n';
				}
			}
		}

		if (settings.hasCurrentFiles()) {
			LOG(INFO) << '\n' << "Current files saved by mothur:" << '\n';
			m->printCurrentFiles();
		}

		string inputDir = m->getInputDir();
		if (inputDir != "") {
			LOG(INFO) << '\n' << "Current input directory saved by mothur: " + inputDir << '\n';
		}

		string outputDir = settings.getOutputDir;
		if (outputDir != "") {
			LOG(INFO) << '\n' << "Current output directory saved by mothur: " + outputDir << '\n';
		}
		string defaultPath = settings.getDefaultPath();
		if (defaultPath != "") {
			LOG(INFO) << '\n' << "Current default directory saved by mothur: " + defaultPath << '\n';
		}


		string temp = "./";
#if defined (UNIX)
#else
		temp = ".\\";
#endif
		temp = m->getFullPathName(temp);
		LOG(INFO) << '\n' << "Current working directory: " + temp << '\n';

		return 0;
	}

	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetCurrentCommand, execute";
		exit(1);
	}
}

//**********************************************************************************************************************



