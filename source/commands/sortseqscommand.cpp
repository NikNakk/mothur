//
//  sortseqscommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 2/3/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "sortseqscommand.h"
#include "sequence.hpp"
#include "qualityscores.h"

//**********************************************************************************************************************
vector<string> SortSeqsCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "FNGLT", "none", "fasta", false, false); parameters.push_back(pfasta);
		CommandParameter pflow("flow", "InputTypes", "", "", "none", "FNGLT", "none", "flow", false, false); parameters.push_back(pflow);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "FNGLT", "none", "name", false, false); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "FNGLT", "none", "count", false, false); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "FNGLT", "none", "group", false, false); parameters.push_back(pgroup);
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "none", "FNGLT", "none", "taxonomy", false, false); parameters.push_back(ptaxonomy);
		CommandParameter pqfile("qfile", "InputTypes", "", "", "none", "FNGLT", "none", "qfile", false, false); parameters.push_back(pqfile);
		nkParameters.add(new BooleanParameter("large", false, false, false));
		CommandParameter paccnos("accnos", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(paccnos);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SortSeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string SortSeqsCommand::getHelpString() {
	try {
		string helpString = "The sort.seqs command puts the sequences in the same order for the following file types: accnos fasta, name, group, count, taxonomy, flow or quality file.\n"
			"The sort.seqs command parameters are accnos, fasta, name, group, count, taxonomy, flow, qfile and large.\n"
			"The accnos file allows you to specify the order you want the files in.  If none is provided, mothur will use the order of the first file it reads.\n"
			"The large parameters is used to indicate your files are too large to fit in RAM.\n"
			"The sort.seqs command should be in the following format: sort.seqs(fasta=yourFasta).\n"
			"Example sort.seqs(fasta=amazon.fasta).\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SortSeqsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string SortSeqsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],sorted,[extension]"; }
	else if (type == "taxonomy") { pattern = "[filename],sorted,[extension]"; }
	else if (type == "name") { pattern = "[filename],sorted,[extension]"; }
	else if (type == "group") { pattern = "[filename],sorted,[extension]"; }
	else if (type == "count") { pattern = "[filename],sorted,[extension]"; }
	else if (type == "flow") { pattern = "[filename],sorted,[extension]"; }
	else if (type == "qfile") { pattern = "[filename],sorted,[extension]"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
SortSeqsCommand::SortSeqsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["taxonomy"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;
		outputTypes["flow"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SortSeqsCommand, SortSeqsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
SortSeqsCommand::SortSeqsCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter;
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["taxonomy"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;
		outputTypes["flow"] = tempOutNames;
		outputTypes["count"] = tempOutNames;

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("fasta");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fasta"] = inputDir + it->second; }
			}

			it = parameters.find("name");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["name"] = inputDir + it->second; }
			}

			it = parameters.find("group");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["group"] = inputDir + it->second; }
			}

			it = parameters.find("taxonomy");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["taxonomy"] = inputDir + it->second; }
			}

			it = parameters.find("qfile");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["qfile"] = inputDir + it->second; }
			}

			it = parameters.find("accnos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["accnos"] = inputDir + it->second; }
			}

			it = parameters.find("flow");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["flow"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}
		}


		//check for parameters
		accnosfile = validParameter.validFile(parameters, "accnos", true);
		if (accnosfile == "not open") { accnosfile = ""; abort = true; }
		else if (accnosfile == "not found") { accnosfile = ""; }
		else { settings.setCurrent("accnos", accnosfile); }

		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not open") { fastafile = ""; abort = true; }
		else if (fastafile == "not found") { fastafile = ""; }
		else { settings.setCurrent("fasta", fastafile); }

		flowfile = validParameter.validFile(parameters, "flow", true);
		if (flowfile == "not open") { flowfile = ""; abort = true; }
		else if (flowfile == "not found") { flowfile = ""; }
		else { settings.setCurrent("flow", flowfile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { namefile = ""; abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { settings.setCurrent("name", namefile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		taxfile = validParameter.validFile(parameters, "taxonomy", true);
		if (taxfile == "not open") { abort = true; }
		else if (taxfile == "not found") { taxfile = ""; }
		else { settings.setCurrent("taxonomy", taxfile); }

		qualfile = validParameter.validFile(parameters, "qfile", true);
		if (qualfile == "not open") { abort = true; }
		else if (qualfile == "not found") { qualfile = ""; }
		else { settings.setCurrent("qual", qualfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		if ((namefile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: name or count." << '\n'; abort = true;
		}

		if ((groupfile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: group or count." << '\n'; abort = true;
		}

		string temp = validParameter.validFile(parameters, "large", false);		if (temp == "not found") { temp = "f"; }
		large = m->isTrue(temp);

		if ((fastafile == "") && (namefile == "") && (countfile == "") && (groupfile == "") && (taxfile == "") && (flowfile == "") && (qualfile == "")) { LOG(INFO) << "You must provide at least one of the following: fasta, name, group, count, taxonomy, flow or quality." << '\n'; abort = true; }

		if (countfile == "") {
			if ((fastafile != "") && (namefile == "")) {
				vector<string> files; files.push_back(fastafile);
				OptionParser::getNameFile(files);
			}
		}
	}

}
//**********************************************************************************************************************

int SortSeqsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//read through the correct file and output lines you want to keep
	if (accnosfile != "") {
		vector<string> temp;
		m->readAccnos(accnosfile, temp);
		for (int i = 0; i < temp.size(); i++) { names[temp[i]] = i; }
		LOG(INFO) << "\nUsing " + accnosfile + " to determine the order. It contains " + toString(temp.size()) + " representative sequences.\n";
	}

	if (fastafile != "") { readFasta(); }
	if (flowfile != "") { readFlow(); }
	if (qualfile != "") { readQual(); }
	if (namefile != "") { readName(); }
	if (groupfile != "") { readGroup(); }
	if (countfile != "") { readCount(); }
	if (taxfile != "") { readTax(); }

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (outputNames.size() != 0) {
		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		//set fasta file as new current fastafile
		string current = "";
		itTypes = outputTypes.find("fasta");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("fasta", current); }
		}

		itTypes = outputTypes.find("name");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("name", current); }
		}

		itTypes = outputTypes.find("group");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("group", current); }
		}


		itTypes = outputTypes.find("taxonomy");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("taxonomy", current); }
		}

		itTypes = outputTypes.find("qfile");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("qual", current); }
		}

		itTypes = outputTypes.find("flow");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("flow", current); }
		}

		itTypes = outputTypes.find("count");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
		}
	}

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in SortSeqsCommand, execute";
	exit(1);
}
}

//**********************************************************************************************************************
int SortSeqsCommand::readFasta() {
	try {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(fastafile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(fastafile));
		variables["[extension]"] = m->getExtension(fastafile);
		string outputFileName = getOutputFileName("fasta", variables);
		outputTypes["fasta"].push_back(outputFileName);  outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(fastafile, in);
		string name;

		if (names.size() != 0) {//this is not the first file we are reading so we need to use the order we already have

			if (large) { //if the file is too large to fit in memory we can still process it, but the io will be very time consuming.
				//read through the file looking for 1000 seqs at a time. Once we find them output them and start looking for the next 1000.
				//this way we only store 1000 seqs in memory at a time.

				int numNames = names.size();
				int numNamesInFile = 0;

				//to make sure we dont miss any seqs, add any seqs that are not in names but in the file to the end of names
				while (!in.eof()) {
					if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

					Sequence currSeq(in);
					name = currSeq.getName();

					if (name != "") {
						numNamesInFile++;
						map<string, int>::iterator it = names.find(name);
						if (it == names.end()) {
							names[name] = numNames; numNames++;
							LOG(INFO) << name + " was not in the contained the file which determined the order, adding it to the end.\n";
						}
					}
					File::gobble(in);
				}
				in.close();
				out.close();

				int numLeft = names.size();
				if (numNamesInFile < numLeft) { numLeft = numNamesInFile; }

				int size = 1000; //assume that user can hold 1000 seqs in memory
				if (numLeft < size) { size = numLeft; }
				int times = 0;

				vector<Sequence> seqs; seqs.resize(size);
				for (int i = 0; i < seqs.size(); i++) { seqs[i].setName(""); } //this is so if some of the seqs are missing we dont print out garbage

				while (numLeft > 0) {

					ifstream in2;
					File::openInputFile(fastafile, in2);

					if (ctrlc_pressed) { in2.close();  File::remove(outputFileName);  return 0; }

					int found = 0;
					int needToFind = size;
					if (numLeft < size) { needToFind = numLeft; }

					while (!in2.eof()) {
						if (ctrlc_pressed) { in2.close();   File::remove(outputFileName);  return 0; }

						//stop reading if we already found the seqs we are looking for
						if (found >= needToFind) { break; }

						Sequence currSeq(in2);
						name = currSeq.getName();

						if (name != "") {
							map<string, int>::iterator it = names.find(name);
							if (it != names.end()) { //we found it, so put it in the vector in the right place.
								//is it in the set of seqs we are looking for this time around
								int thisSeqsPlace = it->second;
								thisSeqsPlace -= (times * size);
								if ((thisSeqsPlace < size) && (thisSeqsPlace >= 0)) {
									seqs[thisSeqsPlace] = currSeq;
									found++;
								}
							}
							else { LOG(LOGERROR) << "in logic of readFasta function.\n"; ctrlc_pressed = true; }
						}
						File::gobble(in2);
					}
					in2.close();

					ofstream out2;
					File::openOutputFileAppend(outputFileName, out2);

					int output = seqs.size();
					if (numLeft < seqs.size()) { output = numLeft; }

					for (int i = 0; i < output; i++) {
						if (seqs[i].getName() != "") { seqs[i].printSequence(out2); }
					}
					out2.close();

					times++;
					numLeft -= output;
				}

				LOG(INFO) << "Ordered " + toString(numNamesInFile) + " sequences from " + fastafile + ".\n";
			}
			else {

				vector<Sequence> seqs; seqs.resize(names.size());
				for (int i = 0; i < seqs.size(); i++) { seqs[i].setName(""); } //this is so if some of the seqs are missing we dont print out garbage

				while (!in.eof()) {
					if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

					Sequence currSeq(in);
					name = currSeq.getName();

					if (name != "") {
						map<string, int>::iterator it = names.find(name);
						if (it != names.end()) { //we found it, so put it in the vector in the right place.
							seqs[it->second] = currSeq;
						}
						else { //if we cant find it then add it to the end
							names[name] = seqs.size();
							seqs.push_back(currSeq);
							LOG(INFO) << name + " was not in the contained the file which determined the order, adding it to the end.\n";
						}
					}
					File::gobble(in);
				}
				in.close();

				int count = 0;
				for (int i = 0; i < seqs.size(); i++) {
					if (seqs[i].getName() != "") {
						seqs[i].printSequence(out); count++;
					}
				}
				out.close();

				LOG(INFO) << "Ordered " + toString(count) + " sequences from " + fastafile + ".\n";
			}

		}
		else { //read in file to fill names
			int count = 0;

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				Sequence currSeq(in);
				name = currSeq.getName();

				if (name != "") {
					//if this name is in the accnos file
					names[name] = count;
					count++;
					currSeq.printSequence(out);
				}
				File::gobble(in);
			}
			in.close();
			out.close();

			LOG(INFO) << "\nUsing " + fastafile + " to determine the order. It contains " + toString(count) + " sequences.\n";
		}

		return 0;

	}
	//**********************************************************************************************************************
	int SortSeqsCommand::readFlow() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(flowfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(flowfile));
		variables["[extension]"] = m->getExtension(flowfile);
		string outputFileName = getOutputFileName("flow", variables);
		outputTypes["flow"].push_back(outputFileName);  outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(flowfile, in);
		int numFlows;
		string name;

		in >> numFlows; File::gobble(in);

		if (names.size() != 0) {//this is not the first file we are reading so we need to use the order we already have

			if (large) { //if the file is too large to fit in memory we can still process it, but the io will be very time consuming.
				//read through the file looking for 1000 seqs at a time. Once we find them output them and start looking for the next 1000.
				//this way we only store 1000 seqs in memory at a time.

				int numNames = names.size();
				int numNamesInFile = 0;

				//to make sure we dont miss any seqs, add any seqs that are not in names but in the file to the end of names
				while (!in.eof()) {
					if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

					in >> name;
					string rest = File::getline(in);

					if (name != "") {
						numNamesInFile++;
						map<string, int>::iterator it = names.find(name);
						if (it == names.end()) {
							names[name] = numNames; numNames++;
							LOG(INFO) << name + " was not in the contained the file which determined the order, adding it to the end.\n";
						}
					}
					File::gobble(in);
				}
				in.close();
				out.close();

				int numLeft = names.size();
				if (numNamesInFile < numLeft) { numLeft = numNamesInFile; }

				int size = 1000; //assume that user can hold 1000 seqs in memory
				if (numLeft < size) { size = numLeft; }
				int times = 0;

				vector<string> seqs; seqs.resize(size, "");

				while (numLeft > 0) {

					ifstream in2;
					File::openInputFile(flowfile, in2); in2 >> numFlows; File::gobble(in2);

					if (ctrlc_pressed) { in2.close();  File::remove(outputFileName);  return 0; }

					int found = 0;
					int needToFind = size;
					if (numLeft < size) { needToFind = numLeft; }

					while (!in2.eof()) {
						if (ctrlc_pressed) { in2.close();   File::remove(outputFileName);  return 0; }

						//stop reading if we already found the seqs we are looking for
						if (found >= needToFind) { break; }

						in2 >> name;
						string rest = File::getline(in2);

						if (name != "") {
							map<string, int>::iterator it = names.find(name);
							if (it != names.end()) { //we found it, so put it in the vector in the right place.
								//is it in the set of seqs we are looking for this time around
								int thisSeqsPlace = it->second;
								thisSeqsPlace -= (times * size);
								if ((thisSeqsPlace < size) && (thisSeqsPlace >= 0)) {
									seqs[thisSeqsPlace] = (name + '\t' + rest);
									found++;
								}
							}
							else { LOG(LOGERROR) << "in logic of readFlow function.\n"; ctrlc_pressed = true; }
						}
						File::gobble(in2);
					}
					in2.close();

					ofstream out2;
					File::openOutputFileAppend(outputFileName, out2);

					int output = seqs.size();
					if (numLeft < seqs.size()) { output = numLeft; }

					for (int i = 0; i < output; i++) {
						if (seqs[i] != "") {
							out2 << seqs[i] << endl;
						}
					}
					out2.close();

					times++;
					numLeft -= output;
				}

				LOG(INFO) << "Ordered " + toString(numNamesInFile) + " flows from " + flowfile + ".\n";
			}
			else {

				vector<string> seqs; seqs.resize(names.size(), "");

				while (!in.eof()) {
					if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

					in >> name;
					string rest = File::getline(in);

					if (name != "") {
						map<string, int>::iterator it = names.find(name);
						if (it != names.end()) { //we found it, so put it in the vector in the right place.
							seqs[it->second] = (name + '\t' + rest);
						}
						else { //if we cant find it then add it to the end
							names[name] = seqs.size();
							seqs.push_back((name + '\t' + rest));
							LOG(INFO) << name + " was not in the contained the file which determined the order, adding it to the end.\n";
						}
					}
					File::gobble(in);
				}
				in.close();

				int count = 0;
				for (int i = 0; i < seqs.size(); i++) {
					if (seqs[i] != "") {
						out << seqs[i] << endl;
						count++;
					}
				}
				out.close();

				LOG(INFO) << "Ordered " + toString(count) + " flows from " + flowfile + ".\n";
			}

		}
		else { //read in file to fill names
			int count = 0;

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				in >> name;
				string rest = File::getline(in);

				if (name != "") {
					//if this name is in the accnos file
					names[name] = count;
					count++;
					out << name << '\t' << rest << endl;
				}
				File::gobble(in);
			}
			in.close();
			out.close();

			LOG(INFO) << "\nUsing " + flowfile + " to determine the order. It contains " + toString(count) + " flows.\n";
		}

		return 0;

	}

	//**********************************************************************************************************************
	int SortSeqsCommand::readQual() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(qualfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(qualfile));
		variables["[extension]"] = m->getExtension(qualfile);
		string outputFileName = getOutputFileName("qfile", variables);
		outputTypes["qfile"].push_back(outputFileName);  outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(qualfile, in);
		string name;

		if (names.size() != 0) {//this is not the first file we are reading so we need to use the order we already have

			if (large) { //if the file is too large to fit in memory we can still process it, but the io will be very time consuming.
				//read through the file looking for 1000 seqs at a time. Once we find them output them and start looking for the next 1000.
				//this way we only store 1000 seqs in memory at a time.

				int numNames = names.size();
				int numNamesInFile = 0;

				//to make sure we dont miss any seqs, add any seqs that are not in names but in the file to the end of names
				while (!in.eof()) {
					if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

					QualityScores currQual;
					currQual = QualityScores(in);
					name = currQual.getName();

					if (name != "") {
						numNamesInFile++;
						map<string, int>::iterator it = names.find(name);
						if (it == names.end()) {
							names[name] = numNames; numNames++;
							LOG(INFO) << name + " was not in the contained the file which determined the order, adding it to the end.\n";
						}
					}
					File::gobble(in);
				}
				in.close();
				out.close();

				int numLeft = names.size();
				if (numNamesInFile < numLeft) { numLeft = numNamesInFile; }

				int size = 1000; //assume that user can hold 1000 seqs in memory
				if (numLeft < size) { size = numLeft; }
				int times = 0;


				vector<QualityScores> seqs; seqs.resize(size);
				for (int i = 0; i < seqs.size(); i++) { seqs[i].setName(""); } //this is so if some of the seqs are missing we dont print out garbage

				while (numLeft > 0) {

					ifstream in2;
					File::openInputFile(qualfile, in2);

					if (ctrlc_pressed) { in2.close();  File::remove(outputFileName);  return 0; }

					int found = 0;
					int needToFind = size;
					if (numLeft < size) { needToFind = numLeft; }

					while (!in2.eof()) {
						if (ctrlc_pressed) { in2.close();   File::remove(outputFileName);  return 0; }

						//stop reading if we already found the seqs we are looking for
						if (found >= needToFind) { break; }

						QualityScores currQual;
						currQual = QualityScores(in2);
						name = currQual.getName();

						if (name != "") {
							map<string, int>::iterator it = names.find(name);
							if (it != names.end()) { //we found it, so put it in the vector in the right place.
								//is it in the set of seqs we are looking for this time around
								int thisSeqsPlace = it->second;
								thisSeqsPlace -= (times * size);
								if ((thisSeqsPlace < size) && (thisSeqsPlace >= 0)) {
									seqs[thisSeqsPlace] = currQual;
									found++;
								}
							}
							else { LOG(LOGERROR) << "in logic of readQual function.\n"; ctrlc_pressed = true; }
						}
						File::gobble(in2);
					}
					in2.close();

					ofstream out2;
					File::openOutputFileAppend(outputFileName, out2);

					int output = seqs.size();
					if (numLeft < seqs.size()) { output = numLeft; }

					for (int i = 0; i < output; i++) {
						if (seqs[i].getName() != "") {
							seqs[i].printQScores(out2);
						}
					}
					out2.close();

					times++;
					numLeft -= output;
				}

				LOG(INFO) << "Ordered " + toString(numNamesInFile) + " sequences from " + qualfile + ".\n";

			}
			else {

				vector<QualityScores> seqs; seqs.resize(names.size());
				for (int i = 0; i < seqs.size(); i++) { seqs[i].setName(""); } //this is so if some of the seqs are missing we dont print out garbage

				while (!in.eof()) {
					if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

					QualityScores currQual;
					currQual = QualityScores(in);
					name = currQual.getName();

					if (name != "") {
						map<string, int>::iterator it = names.find(name);
						if (it != names.end()) { //we found it, so put it in the vector in the right place.
							seqs[it->second] = currQual;
						}
						else { //if we cant find it then add it to the end
							names[name] = seqs.size();
							seqs.push_back(currQual);
							LOG(INFO) << name + " was not in the contained the file which determined the order, adding it to the end.\n";
						}
					}
					File::gobble(in);
				}
				in.close();

				int count = 0;
				for (int i = 0; i < seqs.size(); i++) {
					if (seqs[i].getName() != "") { seqs[i].printQScores(out); count++; }
				}
				out.close();

				LOG(INFO) << "Ordered " + toString(count) + " sequences from " + qualfile + ".\n";
			}

		}
		else { //read in file to fill names
			int count = 0;

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				QualityScores currQual;
				currQual = QualityScores(in);

				File::gobble(in);

				if (currQual.getName() != "") {
					//if this name is in the accnos file
					names[currQual.getName()] = count;
					count++;
					currQual.printQScores(out);
				}
				File::gobble(in);
			}
			in.close();
			out.close();

			LOG(INFO) << "\nUsing " + qualfile + " to determine the order. It contains " + toString(count) + " sequences.\n";
		}

		return 0;

	}
	//**********************************************************************************************************************
	int SortSeqsCommand::readName() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(namefile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(namefile));
		variables["[extension]"] = m->getExtension(namefile);
		string outputFileName = getOutputFileName("name", variables);
		outputTypes["name"].push_back(outputFileName);  outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(namefile, in);
		string name, firstCol, secondCol;

		if (names.size() != 0) {//this is not the first file we are reading so we need to use the order we already have

			vector<string> seqs; seqs.resize(names.size(), "");

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				in >> firstCol;		File::gobble(in);
				in >> secondCol;    File::gobble(in);

				if (firstCol != "") {
					map<string, int>::iterator it = names.find(firstCol);
					if (it != names.end()) { //we found it, so put it in the vector in the right place.
						seqs[it->second] = firstCol + '\t' + secondCol;
					}
					else { //if we cant find it then add it to the end
						names[firstCol] = seqs.size();
						seqs.push_back((firstCol + '\t' + secondCol));
						LOG(INFO) << firstCol + " was not in the contained the file which determined the order, adding it to the end.\n";
					}
				}
			}
			in.close();

			int count = 0;
			for (int i = 0; i < seqs.size(); i++) {
				if (seqs[i] != "") { out << seqs[i] << endl; count++; }
			}
			out.close();

			LOG(INFO) << "Ordered " + toString(count) + " sequences from " + namefile + ".\n";

		}
		else { //read in file to fill names
			int count = 0;

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				in >> firstCol;		File::gobble(in);
				in >> secondCol;    File::gobble(in);

				if (firstCol != "") {
					//if this name is in the accnos file
					names[firstCol] = count;
					count++;
					out << firstCol << '\t' << secondCol << endl;
				}
				File::gobble(in);
			}
			in.close();
			out.close();

			LOG(INFO) << "\nUsing " + namefile + " to determine the order. It contains " + toString(count) + " representative sequences.\n";
		}

		return 0;
	}
	//**********************************************************************************************************************
	int SortSeqsCommand::readCount() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(countfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(countfile));
		variables["[extension]"] = m->getExtension(countfile);
		string outputFileName = getOutputFileName("count", variables);
		outputTypes["count"].push_back(outputFileName);  outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(countfile, in);
		string firstCol, rest;

		if (names.size() != 0) {//this is not the first file we are reading so we need to use the order we already have

			vector<string> seqs; seqs.resize(names.size(), "");

			string headers = File::getline(in); File::gobble(in);

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				in >> firstCol;		File::gobble(in);
				rest = File::getline(in);    File::gobble(in);

				if (firstCol != "") {
					map<string, int>::iterator it = names.find(firstCol);
					if (it != names.end()) { //we found it, so put it in the vector in the right place.
						seqs[it->second] = firstCol + '\t' + rest;
					}
					else { //if we cant find it then add it to the end
						names[firstCol] = seqs.size();
						seqs.push_back((firstCol + '\t' + rest));
						LOG(INFO) << firstCol + " was not in the contained the file which determined the order, adding it to the end.\n";
					}
				}
			}
			in.close();

			int count = 0;
			out << headers << endl;
			for (int i = 0; i < seqs.size(); i++) {
				if (seqs[i] != "") { out << seqs[i] << endl; count++; }
			}
			out.close();

			LOG(INFO) << "Ordered " + toString(count) + " sequences from " + countfile + ".\n";

		}
		else { //read in file to fill names
			int count = 0;

			string headers = File::getline(in); File::gobble(in);
			out << headers << endl;

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				in >> firstCol;		File::gobble(in);
				rest = File::getline(in);  File::gobble(in);

				if (firstCol != "") {
					//if this name is in the accnos file
					names[firstCol] = count;
					count++;
					out << firstCol << '\t' << rest << endl;
				}
				File::gobble(in);
			}
			in.close();
			out.close();

			LOG(INFO) << "\nUsing " + countfile + " to determine the order. It contains " + toString(count) + " representative sequences.\n";
		}

		return 0;
	}
	//**********************************************************************************************************************
	int SortSeqsCommand::readGroup() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(groupfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(groupfile));
		variables["[extension]"] = m->getExtension(groupfile);
		string outputFileName = getOutputFileName("group", variables);
		outputTypes["group"].push_back(outputFileName);  outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(groupfile, in);
		string name, group;

		if (names.size() != 0) {//this is not the first file we are reading so we need to use the order we already have

			vector<string> seqs; seqs.resize(names.size(), "");

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				in >> name;		File::gobble(in);
				in >> group;    File::gobble(in);

				if (name != "") {
					map<string, int>::iterator it = names.find(name);
					if (it != names.end()) { //we found it, so put it in the vector in the right place.
						seqs[it->second] = name + '\t' + group;
					}
					else { //if we cant find it then add it to the end
						names[name] = seqs.size();
						seqs.push_back((name + '\t' + group));
						LOG(INFO) << name + " was not in the contained the file which determined the order, adding it to the end.\n";
					}
				}
			}
			in.close();

			int count = 0;
			for (int i = 0; i < seqs.size(); i++) {
				if (seqs[i] != "") { out << seqs[i] << endl; count++; }
			}
			out.close();

			LOG(INFO) << "Ordered " + toString(count) + " sequences from " + groupfile + ".\n";

		}
		else { //read in file to fill names
			int count = 0;

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				in >> name;		File::gobble(in);
				in >> group;    File::gobble(in);

				if (name != "") {
					//if this name is in the accnos file
					names[name] = count;
					count++;
					out << name << '\t' << group << endl;
				}
				File::gobble(in);
			}
			in.close();
			out.close();

			LOG(INFO) << "\nUsing " + groupfile + " to determine the order. It contains " + toString(count) + " sequences.\n";
		}

		return 0;
	}
	//**********************************************************************************************************************
	int SortSeqsCommand::readTax() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(taxfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(taxfile));
		variables["[extension]"] = m->getExtension(taxfile);
		string outputFileName = getOutputFileName("taxonomy", variables);

		outputTypes["taxonomy"].push_back(outputFileName);  outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(taxfile, in);
		string name, tax;

		if (names.size() != 0) {//this is not the first file we are reading so we need to use the order we already have

			vector<string> seqs; seqs.resize(names.size(), "");

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				in >> name;		File::gobble(in);
				in >> tax;    File::gobble(in);

				if (name != "") {
					map<string, int>::iterator it = names.find(name);
					if (it != names.end()) { //we found it, so put it in the vector in the right place.
						seqs[it->second] = name + '\t' + tax;
					}
					else { //if we cant find it then add it to the end
						names[name] = seqs.size();
						seqs.push_back((name + '\t' + tax));
						LOG(INFO) << name + " was not in the contained the file which determined the order, adding it to the end.\n";
					}
				}
			}
			in.close();

			int count = 0;
			for (int i = 0; i < seqs.size(); i++) {
				if (seqs[i] != "") { out << seqs[i] << endl; count++; }
			}
			out.close();

			LOG(INFO) << "Ordered " + toString(count) + " sequences from " + taxfile + ".\n";

		}
		else { //read in file to fill names
			int count = 0;

			while (!in.eof()) {
				if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

				in >> name;		File::gobble(in);
				in >> tax;    File::gobble(in);

				if (name != "") {
					//if this name is in the accnos file
					names[name] = count;
					count++;
					out << name << '\t' << tax << endl;
				}
				File::gobble(in);
			}
			in.close();
			out.close();

			LOG(INFO) << "\nUsing " + taxfile + " to determine the order. It contains " + toString(count) + " sequences.\n";
		}

		return 0;
		return 0;
	}
	//**********************************************************************************************************************





