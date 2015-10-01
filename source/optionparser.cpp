/*
 *  optionparser.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 6/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "optionparser.h"

 /***********************************************************************/

OptionParser::OptionParser(string option) {
	if (option != "") {

		string key, value;
		//reads in parameters and values
		while ((option.find_first_of(',') != -1)) {  //while there are parameters
			m->splitAtComma(value, option);
			m->splitAtEquals(key, value);
			if ((key == "candidate") || (key == "query")) { key = "fasta"; }
			if (key == "template") { key = "reference"; }
			parameters[key] = value;
		}

		//in case there is no comma and to get last parameter after comma
		m->splitAtEquals(key, option);
		if ((key == "candidate") || (key == "query")) { key = "fasta"; }
		if (key == "template") { key = "reference"; }
		parameters[key] = option;
	}
}
/***********************************************************************/

OptionParser::OptionParser(string option, map<string, string>& copy) {
	if (option != "") {

		string key, value;
		//reads in parameters and values
		while ((option.find_first_of(',') != -1)) {  //while there are parameters
			m->splitAtComma(value, option);
			m->splitAtEquals(key, value);
			if ((key == "candidate") || (key == "query")) { key = "fasta"; }
			if (key == "template") { key = "reference"; }
			parameters[key] = value;
		}

		//in case there is no comma and to get last parameter after comma
		m->splitAtEquals(key, option);
		if ((key == "candidate") || (key == "query")) { key = "fasta"; }
		if (key == "template") { key = "reference"; }
		parameters[key] = option;
	}

	copy = parameters;
}
/***********************************************************************/

map<string, string> OptionParser::getParameters() {
	try {

		//loop through parameters and look for "current" so you can return the appropriate file
		//doing it here to avoid code duplication in each of the commands


		map<string, string>::iterator it;
		for (it = parameters.begin(); it != parameters.end();) {

			if (it->second == "current") {

				//look for file types
				if (it->first == "fasta") {
					it->second = settings.getCurrent("fasta");
				}
				else if (it->first == "qfile") {
					it->second = settings.getCurrent("qual");
				}
				else if (it->first == "phylip") {
					it->second = settings.getCurrent("phylip");
				}
				else if (it->first == "column") {
					it->second = settings.getCurrent("column");
				}
				else if (it->first == "list") {
					it->second = settings.getCurrent("list");
				}
				else if (it->first == "rabund") {
					it->second = settings.getCurrent("rabund");
				}
				else if (it->first == "sabund") {
					it->second = settings.getCurrent("sabund");
				}
				else if (it->first == "name") {
					it->second = settings.getCurrent("name");
				}
				else if (it->first == "group") {
					it->second = settings.getCurrent("group");
				}
				else if (it->first == "order") {
					it->second = settings.getCurrent("order");
				}
				else if (it->first == "ordergroup") {
					it->second = settings.getCurrent("ordergroup");
				}
				else if (it->first == "tree") {
					it->second = settings.getCurrent("tree");
				}
				else if (it->first == "shared") {
					it->second = settings.getCurrent("shared");
				}
				else if (it->first == "relabund") {
					it->second = settings.getCurrent("relabund");
				}
				else if (it->first == "design") {
					it->second = settings.getCurrent("design");
				}
				else if (it->first == "sff") {
					it->second = settings.getCurrent("sff");
				}
				else if (it->first == "flow") {
					it->second = settings.getCurrent("flow");
				}
				else if (it->first == "oligos") {
					it->second = settings.getCurrent("oligos");
				}
				else if (it->first == "accnos") {
					it->second = settings.getCurrent("accnos");
				}
				else if (it->first == "taxonomy") {
					it->second = settings.getCurrent("taxonomy");
				}
				else if (it->first == "biom") {
					it->second = settings.getCurrent("biom");
				}
				else if (it->first == "count") {
					it->second = settings.getCurrent("counttable");
				}
				else if (it->first == "summary") {
					it->second = settings.getCurrent("summary");
				}
				else if (it->first == "file") {
					it->second = settings.getCurrent("file");
				}
				else {
					LOG(INFO) << "[ERROR]: mothur does not save a current file for " + it->first << '\n';
				}

				if (it->second == "") { //no file was saved for that type, warn and remove from parameters
					LOG(WARNING) << "no file was saved for " + it->first + " parameter." << '\n';
					parameters.erase(it++);
				}
				else {
					LOG(INFO) << "Using " + it->second + " as input file for the " + it->first + " parameter." << '\n';
					it++;
				}
			}
			else { it++; }
		}

		return parameters;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OptionParser, getParameters";
		exit(1);
	}
}

/***********************************************************************/
//pass a vector of filenames that may match the current namefile.  
//this function will look at each one, if the rootnames match, mothur will warn 
//the user that they may have neglected to provide a namefile.
//stops when it finds a match.
bool OptionParser::getNameFile(vector<string> files) {
	MothurOut* m = MothurOut::getInstance();
	try {
		string namefile = settings.getCurrent("name");
		bool match = false;

		if ((namefile != "") && (!m->mothurCalling)) {
			string temp = File::getRootName(File::getSimpleName(namefile));
			vector<string> rootName;
			m->splitAtChar(temp, rootName, '.');

			for (int i = 0; i < files.size(); i++) {
				temp = File::getRootName(File::getSimpleName(files[i]));
				vector<string> root;
				m->splitAtChar(temp, root, '.');

				int smallest = rootName.size();
				if (root.size() < smallest) { smallest = root.size(); }

				int numMatches = 0;
				for (int j = 0; j < smallest; j++) {
					if (root[j] == rootName[j]) { numMatches++; }
				}

				if (smallest > 0) {
					if ((numMatches >= (smallest - 2)) && (root[0] == rootName[0])) {
						LOG(WARNING) << "This command can take a namefile and you did not provide one. The current namefile is " + namefile + " which seems to match " + files[i] + ".";
						LOG(INFO) << "";
						match = true;
						break;
					}
				}
			}

		}


		return match;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OptionParser, getNameFile";
		exit(1);
	}
}


/***********************************************************************/
