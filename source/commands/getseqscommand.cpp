/*
 *  getseqscommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 7/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "getseqscommand.h"
#include "sequence.hpp"
#include "listvector.hpp"
#include "counttable.h"

 //**********************************************************************************************************************
vector<string> GetSeqsCommand::setParameters() {
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "FNGLT", "none", "fasta", false, false, true); parameters.push_back(pfasta);
		CommandParameter pfastq("fastq", "InputTypes", "", "", "none", "FNGLT", "none", "fastq", false, false, true); parameters.push_back(pfastq);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "FNGLT", "none", "name", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "FNGLT", "none", "count", false, false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "FNGLT", "none", "group", false, false, true); parameters.push_back(pgroup);
		CommandParameter plist("list", "InputTypes", "", "", "none", "FNGLT", "none", "list", false, false, true); parameters.push_back(plist);
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "none", "FNGLT", "none", "taxonomy", false, false, true); parameters.push_back(ptaxonomy);
		CommandParameter palignreport("alignreport", "InputTypes", "", "", "none", "FNGLT", "none", "alignreport", false, false); parameters.push_back(palignreport);
		CommandParameter pqfile("qfile", "InputTypes", "", "", "none", "FNGLT", "none", "qfile", false, false); parameters.push_back(pqfile);
		CommandParameter paccnos("accnos", "InputTypes", "", "", "none", "none", "none", "", false, true, true); parameters.push_back(paccnos);
		nkParameters.add(new BooleanParameter("dups", true, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));
		CommandParameter paccnos2("accnos2", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(paccnos2);

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetSeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetSeqsCommand::getHelpString() {
	try {
		string helpString = "The get.seqs command reads an .accnos file and any of the following file types: fasta, name, group, count, list, taxonomy, quality, fastq or alignreport file.\n"
			"It outputs a file containing only the sequences in the .accnos file.\n"
			"The get.seqs command parameters are accnos, fasta, name, group, list, taxonomy, qfile, alignreport, fastq and dups.  You must provide accnos unless you have a valid current accnos file, and at least one of the other parameters.\n"
			"The dups parameter allows you to add the entire line from a name file if you add any name from the line. default=true. \n"
			"The get.seqs command should be in the following format: get.seqs(accnos=yourAccnos, fasta=yourFasta).\n"
			"Example get.seqs(accnos=amazon.accnos, fasta=amazon.fasta).\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetSeqsCommand, getHelpString";
		exit(1);
	}
}

//**********************************************************************************************************************
GetSeqsCommand::GetSeqsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["fastq"] = tempOutNames;
		outputTypes["taxonomy"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["alignreport"] = tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
		outputTypes["accnosreport"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetSeqsCommand, GetSeqsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetSeqsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],pick,[extension]"; }
	else if (type == "fastq") { pattern = "[filename],pick,[extension]"; }
	else if (type == "taxonomy") { pattern = "[filename],pick,[extension]"; }
	else if (type == "name") { pattern = "[filename],pick,[extension]"; }
	else if (type == "group") { pattern = "[filename],pick,[extension]"; }
	else if (type == "count") { pattern = "[filename],pick,[extension]"; }
	else if (type == "list") { pattern = "[filename],[distance],pick,[extension]"; }
	else if (type == "qfile") { pattern = "[filename],pick,[extension]"; }
	else if (type == "accnosreport") { pattern = "[filename],pick.accnos.report"; }
	else if (type == "alignreport") { pattern = "[filename],pick.align.report"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
GetSeqsCommand::GetSeqsCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["fastq"] = tempOutNames;
		outputTypes["taxonomy"] = tempOutNames;
		outputTypes["name"] = tempOutNames;
		outputTypes["group"] = tempOutNames;
		outputTypes["alignreport"] = tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;
		outputTypes["accnosreport"] = tempOutNames;
		outputTypes["count"] = tempOutNames;

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("alignreport");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["alignreport"] = inputDir + it->second; }
			}

			it = parameters.find("fasta");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fasta"] = inputDir + it->second; }
			}

			it = parameters.find("accnos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["accnos"] = inputDir + it->second; }
			}

			it = parameters.find("accnos2");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["accnos2"] = inputDir + it->second; }
			}

			it = parameters.find("list");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["list"] = inputDir + it->second; }
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

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}

			it = parameters.find("fastq");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fastq"] = inputDir + it->second; }
			}
		}


		//check for required parameters
		accnosfile = validParameter.validFile(parameters, "accnos", true);
		if (accnosfile == "not open") { abort = true; }
		else if (accnosfile == "not found") {
			accnosfile = settings.getCurrent("accnos");
			if (accnosfile != "") { LOG(INFO) << "Using " + accnosfile + " as input file for the accnos parameter." << '\n'; }
			else {
				LOG(INFO) << "You have no valid accnos file and accnos is required." << '\n';
				abort = true;
			}
		}
		else { settings.setCurrent("accnos", accnosfile); }

		if (accnosfile2 == "not found") { accnosfile2 = ""; }

		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not open") { fastafile = ""; abort = true; }
		else if (fastafile == "not found") { fastafile = ""; }
		else { settings.setCurrent("fasta", fastafile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { namefile = ""; abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { settings.setCurrent("name", namefile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		alignfile = validParameter.validFile(parameters, "alignreport", true);
		if (alignfile == "not open") { abort = true; }
		else if (alignfile == "not found") { alignfile = ""; }

		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { settings.setCurrent("list", listfile); }

		taxfile = validParameter.validFile(parameters, "taxonomy", true);
		if (taxfile == "not open") { taxfile = ""; abort = true; }
		else if (taxfile == "not found") { taxfile = ""; }
		else { settings.setCurrent("taxonomy", taxfile); }

		qualfile = validParameter.validFile(parameters, "qfile", true);
		if (qualfile == "not open") { abort = true; }
		else if (qualfile == "not found") { qualfile = ""; }
		else { settings.setCurrent("qual", qualfile); }

		fastqfile = validParameter.validFile(parameters, "fastq", true);
		if (fastqfile == "not open") { abort = true; }
		else if (fastqfile == "not found") { fastqfile = ""; }

		accnosfile2 = validParameter.validFile(parameters, "accnos2", true);
		if (accnosfile2 == "not open") { abort = true; }
		else if (accnosfile2 == "not found") { accnosfile2 = ""; }

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


		string usedDups = "true";
		string temp = validParameter.validFile(parameters, "dups", false);	if (temp == "not found") { temp = "true"; usedDups = ""; }
		dups = m->isTrue(temp);

		if ((fastqfile == "") && (fastafile == "") && (namefile == "") && (groupfile == "") && (alignfile == "") && (listfile == "") && (taxfile == "") && (qualfile == "") && (accnosfile2 == "") && (countfile == "")) { LOG(INFO) << "You must provide one of the following: fasta, name, group, count, alignreport, taxonomy, quality, fastq or listfile." << '\n'; abort = true; }

		if (countfile == "") {
			if ((namefile == "") && ((fastafile != "") || (taxfile != ""))) {
				vector<string> files; files.push_back(fastafile); files.push_back(taxfile);
				OptionParser::getNameFile(files);
			}
		}
	}

}
//**********************************************************************************************************************

int GetSeqsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//get names you want to keep
	names = m->readAccnos(accnosfile);

	if (ctrlc_pressed) { return 0; }

	if (countfile != "") {
		if ((fastafile != "") || (listfile != "") || (taxfile != "")) {
			LOG(INFO) << "\n[NOTE]: The count file should contain only unique names, so mothur assumes your fasta, list and taxonomy files also contain only uniques.\n\n";
		}
	}

	//read through the correct file and output lines you want to keep
	if (namefile != "") { readName(); }
	if (fastafile != "") { readFasta(); }
	if (fastqfile != "") { readFastq(); }
	if (groupfile != "") { readGroup(); }
	if (countfile != "") { readCount(); }
	if (alignfile != "") { readAlign(); }
	if (listfile != "") { readList(); }
	if (taxfile != "") { readTax(); }
	if (qualfile != "") { readQual(); }
	if (accnosfile2 != "") { compareAccnos(); }

	if (app.isDebug) { runSanityCheck(); }

	if (ctrlc_pressed) { outputTypes.clear(); for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }


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

		itTypes = outputTypes.find("list");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("list", current); }
		}

		itTypes = outputTypes.find("taxonomy");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("taxonomy", current); }
		}

		itTypes = outputTypes.find("qfile");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("qual", current); }
		}

		itTypes = outputTypes.find("count");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
		}
	}

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in GetSeqsCommand, execute";
	exit(1);
}
}
//**********************************************************************************************************************
int GetSeqsCommand::readFastq() {
	try {
		bool wroteSomething = false;
		int selectedCount = 0;

		ifstream in;
		File::openInputFile(fastqfile, in);

		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(fastqfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(fastqfile));
		variables["[extension]"] = m->getExtension(fastqfile);
		string outputFileName = getOutputFileName("fastq", variables);
		ofstream out;
		File::openOutputFile(outputFileName, out);

		set<string> uniqueNames;
		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); out.close(); File::remove(outputFileName); return 0; }

			//read sequence name
			string input = File::getline(in); File::gobble(in);

			string outputString = input + "\n";

			if (input[0] == '@') {
				//get rest of lines
				outputString += File::getline(in) + "\n"; File::gobble(in);
				outputString += File::getline(in) + "\n"; File::gobble(in);
				outputString += File::getline(in) + "\n"; File::gobble(in);

				vector<string> splits = m->splitWhiteSpace(input);
				string name = splits[0];
				name = name.substr(1);
				m->checkName(name);

				if (names.count(name) != 0) {
					if (uniqueNames.count(name) == 0) { //this name hasn't been seen yet
						wroteSomething = true;
						selectedCount++;
						out << outputString;
						uniqueNames.insert(name);
					}
					else {
						LOG(WARNING) << "" + name + " is in your fastq file more than once.  Mothur requires sequence names to be unique. I will only add it once.\n";
					}
				}
			}

			File::gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any sequence from the .accnos file." << '\n'; }
		outputNames.push_back(outputFileName);  outputTypes["fastq"].push_back(outputFileName);

		LOG(INFO) << "Selected " + toString(selectedCount) + " sequences from your fastq file." << '\n';

		return 0;

	}

	//**********************************************************************************************************************
	int GetSeqsCommand::readFasta() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(fastafile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(fastafile));
		variables["[extension]"] = m->getExtension(fastafile);
		string outputFileName = getOutputFileName("fasta", variables);
		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(fastafile, in);
		string name;

		bool wroteSomething = false;
		int selectedCount = 0;

		if (app.isDebug) { set<string> temp; sanity["fasta"] = temp; }

		set<string> uniqueNames;
		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); out.close(); File::remove(outputFileName);  return 0; }

			Sequence currSeq(in);
			name = currSeq.getName();

			if (!dups) {//adjust name if needed
				map<string, string>::iterator it = uniqueMap.find(name);
				if (it != uniqueMap.end()) { currSeq.setName(it->second); }
			}

			name = currSeq.getName();

			if (name != "") {
				//if this name is in the accnos file
				if (names.count(name) != 0) {
					if (uniqueNames.count(name) == 0) { //this name hasn't been seen yet
						wroteSomething = true;

						currSeq.printSequence(out);
						selectedCount++;
						uniqueNames.insert(name);

						if (app.isDebug) { sanity["fasta"].insert(name); }
					}
					else {
						LOG(WARNING) << "" + name + " is in your fasta file more than once.  Mothur requires sequence names to be unique. I will only add it once.\n";
					}
				}
			}
			File::gobble(in);
		}
		in.close();
		out.close();


		if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any sequence from the .accnos file." << '\n'; }
		outputNames.push_back(outputFileName);  outputTypes["fasta"].push_back(outputFileName);

		LOG(INFO) << "Selected " + toString(selectedCount) + " sequences from your fasta file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************
	int GetSeqsCommand::readQual() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(qualfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(qualfile));
		variables["[extension]"] = m->getExtension(qualfile);
		string outputFileName = getOutputFileName("qfile", variables);
		ofstream out;
		File::openOutputFile(outputFileName, out);


		ifstream in;
		File::openInputFile(qualfile, in);
		string name;

		bool wroteSomething = false;
		int selectedCount = 0;

		if (app.isDebug) { set<string> temp; sanity["qual"] = temp; }

		set<string> uniqueNames;
		while (!in.eof()) {
			string saveName = "";
			string name = "";
			string scores = "";

			in >> name;

			if (!dups) {//adjust name if needed
				map<string, string>::iterator it = uniqueMap.find(name);
				if (it != uniqueMap.end()) { name = it->second; }
			}

			if (name.length() != 0) {
				saveName = name.substr(1);
				while (!in.eof()) {
					char c = in.get();
					if (c == 10 || c == 13 || c == -1) { break; }
					else { name += c; }
				}
				File::gobble(in);
			}

			while (in) {
				char letter = in.get();
				if (letter == '>') { in.putback(letter);	break; }
				else { scores += letter; }
			}

			File::gobble(in);

			if (names.count(saveName) != 0) {
				if (uniqueNames.count(saveName) == 0) { //this name hasn't been seen yet
					uniqueNames.insert(saveName);
					wroteSomething = true;

					out << name << endl << scores;
					selectedCount++;
					if (app.isDebug) { sanity["qual"].insert(name); }
				}
				else {
					LOG(WARNING) << "" + saveName + " is in your qfile more than once.  Mothur requires sequence names to be unique. I will only add it once.\n";
				}
			}

			File::gobble(in);
		}
		in.close();
		out.close();


		if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any sequence from the .accnos file." << '\n'; }
		outputNames.push_back(outputFileName);  outputTypes["qfile"].push_back(outputFileName);

		LOG(INFO) << "Selected " + toString(selectedCount) + " sequences from your quality file." << '\n';


		return 0;

	}
	//**********************************************************************************************************************
	int GetSeqsCommand::readCount() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(countfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(countfile));
		variables["[extension]"] = m->getExtension(countfile);
		string outputFileName = getOutputFileName("count", variables);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(countfile, in);

		bool wroteSomething = false;
		int selectedCount = 0;

		string headers = File::getline(in); File::gobble(in);
		out << headers << endl;
		string test = headers; vector<string> pieces = m->splitWhiteSpace(test);

		string name, rest; int thisTotal; rest = "";
		set<string> uniqueNames;
		while (!in.eof()) {

			if (ctrlc_pressed) { in.close();  out.close();  File::remove(outputFileName);  return 0; }

			in >> name; File::gobble(in);
			in >> thisTotal; File::gobble(in);
			if (pieces.size() > 2) { rest = File::getline(in); File::gobble(in); }
			if (app.isDebug) { LOG(DEBUG) << "" + name + '\t' + rest + "\n"; }

			if (names.count(name) != 0) {
				if (uniqueNames.count(name) == 0) { //this name hasn't been seen yet
					uniqueNames.insert(name);

					out << name << '\t' << thisTotal << '\t' << rest << endl;
					wroteSomething = true;
					selectedCount += thisTotal;
				}
				else {
					LOG(WARNING) << "" + name + " is in your count file more than once.  Mothur requires sequence names to be unique. I will only add it once.\n";
				}
			}
		}
		in.close();
		out.close();

		//check for groups that have been eliminated
		CountTable ct;
		if (ct.testGroups(outputFileName)) {
			ct.readTable(outputFileName, true, false);
			ct.printTable(outputFileName);
		}

		if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any sequence from the .accnos file." << '\n'; }
		outputTypes["count"].push_back(outputFileName); outputNames.push_back(outputFileName);

		LOG(INFO) << "Selected " + toString(selectedCount) + " sequences from your count file." << '\n';

		return 0;
	}

	//**********************************************************************************************************************
	int GetSeqsCommand::readList() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(listfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(listfile));
		variables["[extension]"] = m->getExtension(listfile);

		ifstream in;
		File::openInputFile(listfile, in);

		bool wroteSomething = false;
		int selectedCount = 0;

		if (app.isDebug) { set<string> temp; sanity["list"] = temp; }

		set<string> uniqueNames;
		while (!in.eof()) {

			selectedCount = 0;

			//read in list vector
			ListVector list(in);

			//make a new list vector
			ListVector newList;
			newList.setLabel(list.getLabel());

			variables["[distance]"] = list.getLabel();
			string outputFileName = getOutputFileName("list", variables);

			ofstream out;
			File::openOutputFile(outputFileName, out);
			outputTypes["list"].push_back(outputFileName);  outputNames.push_back(outputFileName);

			vector<string> binLabels = list.getLabels();
			vector<string> newBinLabels;

			if (ctrlc_pressed) { in.close(); out.close();  return 0; }

			//for each bin
			for (int i = 0; i < list.getNumBins(); i++) {

				//parse out names that are in accnos file
				string binnames = list.get(i);
				vector<string> bnames;
				m->splitAtComma(binnames, bnames);

				string newNames = "";
				for (int j = 0; j < bnames.size(); j++) {
					string name = bnames[j];

					//if that name is in the .accnos file, add it
					if (names.count(name) != 0) {
						if (uniqueNames.count(name) == 0) { //this name hasn't been seen yet
							uniqueNames.insert(name);
							newNames += name + ",";
							selectedCount++;
							if (app.isDebug) { sanity["list"].insert(name); }
						}
						else {
							LOG(WARNING) << "" + name + " is in your list file more than once.  Mothur requires sequence names to be unique. I will only add it once.\n";
						}
					}
				}

				//if there are names in this bin add to new list
				if (newNames != "") {
					newNames = newNames.substr(0, newNames.length() - 1); //rip off extra comma
					newList.push_back(newNames);
					newBinLabels.push_back(binLabels[i]);
				}
			}

			//print new listvector
			if (newList.getNumBins() != 0) {
				wroteSomething = true;
				newList.setLabels(newBinLabels);
				newList.printHeaders(out);
				newList.print(out);
			}

			File::gobble(in);
			out.close();
		}
		in.close();


		if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any sequence from the .accnos file." << '\n'; }

		LOG(INFO) << "Selected " + toString(selectedCount) + " sequences from your list file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************
	int GetSeqsCommand::readName() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(namefile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(namefile));
		variables["[extension]"] = m->getExtension(namefile);
		string outputFileName = getOutputFileName("name", variables);
		ofstream out;
		File::openOutputFile(outputFileName, out);


		ifstream in;
		File::openInputFile(namefile, in);
		string name, firstCol, secondCol;

		bool wroteSomething = false;
		int selectedCount = 0;

		if (app.isDebug) { set<string> temp; sanity["name"] = temp; }
		if (app.isDebug) { set<string> temp; sanity["dupname"] = temp; }

		set<string> uniqueNames;
		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); out.close(); File::remove(outputFileName);  return 0; }

			in >> firstCol;			File::gobble(in);
			in >> secondCol;

			string hold = "";
			if (dups) { hold = secondCol; }

			vector<string> parsedNames;
			m->splitAtComma(secondCol, parsedNames);

			vector<string> validSecond; vector<string> parsedNames2;
			bool parsedError = false;
			for (int i = 0; i < parsedNames.size(); i++) {
				if (names.count(parsedNames[i]) != 0) {
					if (uniqueNames.count(parsedNames[i]) == 0) { //this name hasn't been seen yet
						uniqueNames.insert(parsedNames[i]);
						validSecond.push_back(parsedNames[i]);
						parsedNames2.push_back(parsedNames[i]);
						if (app.isDebug) { sanity["dupname"].insert(parsedNames[i]); }
					}
					else {
						LOG(WARNING) << "" + parsedNames[i] + " is in your name file more than once.  Mothur requires sequence names to be unique. I will only add it once.\n";
						parsedError = true;
					}
				}
			}
			if (parsedError) {
				parsedNames = parsedNames2;
				hold = "";
				if (parsedNames.size() != 0) {
					for (int i = 0; i < parsedNames.size() - 1; i++) { hold += parsedNames[i] + ','; }
					hold += parsedNames[parsedNames.size() - 1] + '\n';
				}
			}

			if ((dups) && (validSecond.size() != 0)) { //dups = true and we want to add someone, then add everyone
				for (int i = 0; i < parsedNames.size(); i++) { names.insert(parsedNames[i]); if (app.isDebug) { sanity["dupname"].insert(parsedNames[i]); } }
				out << firstCol << '\t' << hold << endl;
				wroteSomething = true;
				selectedCount += parsedNames.size();
				if (app.isDebug) { sanity["name"].insert(firstCol); }
			}
			else {

				if (validSecond.size() != 0) {
					selectedCount += validSecond.size();

					//if the name in the first column is in the set then print it and any other names in second column also in set
					if (names.count(firstCol) != 0) {

						wroteSomething = true;

						out << firstCol << '\t';

						//you know you have at least one valid second since first column is valid
						for (int i = 0; i < validSecond.size() - 1; i++) { out << validSecond[i] << ','; }
						out << validSecond[validSecond.size() - 1] << endl;

						if (app.isDebug) { sanity["name"].insert(firstCol); }


						//make first name in set you come to first column and then add the remaining names to second column
					}
					else {

						//you want part of this row
						if (validSecond.size() != 0) {

							wroteSomething = true;

							out << validSecond[0] << '\t';
							//we are changing the unique name in the fasta file
							uniqueMap[firstCol] = validSecond[0];

							//you know you have at least one valid second since first column is valid
							for (int i = 0; i < validSecond.size() - 1; i++) { out << validSecond[i] << ','; }
							out << validSecond[validSecond.size() - 1] << endl;

							if (app.isDebug) { sanity["name"].insert(validSecond[0]); }
						}
					}
				}
			}
			File::gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any sequence from the .accnos file." << '\n'; }
		outputNames.push_back(outputFileName); outputTypes["name"].push_back(outputFileName);

		LOG(INFO) << "Selected " + toString(selectedCount) + " sequences from your name file." << '\n';

		return 0;

	}

	//**********************************************************************************************************************
	int GetSeqsCommand::readGroup() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(groupfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(groupfile));
		variables["[extension]"] = m->getExtension(groupfile);
		string outputFileName = getOutputFileName("group", variables);
		ofstream out;
		File::openOutputFile(outputFileName, out);


		ifstream in;
		File::openInputFile(groupfile, in);
		string name, group;

		bool wroteSomething = false;
		int selectedCount = 0;

		if (app.isDebug) { set<string> temp; sanity["group"] = temp; }

		set<string> uniqueNames;
		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); out.close(); File::remove(outputFileName);  return 0; }


			in >> name;				//read from first column
			in >> group;			//read from second column


			if (names.count(name) != 0) {
				if (uniqueNames.count(name) == 0) { //this name hasn't been seen yet
					uniqueNames.insert(name);
					wroteSomething = true;

					out << name << '\t' << group << endl;
					selectedCount++;

					if (app.isDebug) { sanity["group"].insert(name); }
				}
				else {
					LOG(WARNING) << "" + name + " is in your group file more than once.  Mothur requires sequence names to be unique. I will only add it once.\n";
				}
			}

			File::gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any sequence from the .accnos file." << '\n'; }
		outputNames.push_back(outputFileName);  outputTypes["group"].push_back(outputFileName);

		LOG(INFO) << "Selected " + toString(selectedCount) + " sequences from your group file." << '\n';


		return 0;

	}
	//**********************************************************************************************************************
	int GetSeqsCommand::readTax() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(taxfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(taxfile));
		variables["[extension]"] = m->getExtension(taxfile);
		string outputFileName = getOutputFileName("taxonomy", variables);
		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(taxfile, in);
		string name, tax;

		bool wroteSomething = false;
		int selectedCount = 0;

		if (app.isDebug) { set<string> temp; sanity["tax"] = temp; }

		set<string> uniqueNames;
		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); out.close(); File::remove(outputFileName);  return 0; }

			in >> name;				//read from first column
			in >> tax;			//read from second column

			if (!dups) {//adjust name if needed
				map<string, string>::iterator it = uniqueMap.find(name);
				if (it != uniqueMap.end()) { name = it->second; }
			}

			if (names.count(name) != 0) {
				if (uniqueNames.count(name) == 0) { //this name hasn't been seen yet
					uniqueNames.insert(name);

					wroteSomething = true;

					out << name << '\t' << tax << endl;
					selectedCount++;

					if (app.isDebug) { sanity["tax"].insert(name); }
				}
				else {
					LOG(WARNING) << "" + name + " is in your taxonomy file more than once.  Mothur requires sequence names to be unique. I will only add it once.\n";
				}
			}

			File::gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any sequence from the .accnos file." << '\n'; }
		outputNames.push_back(outputFileName);  outputTypes["taxonomy"].push_back(outputFileName);

		LOG(INFO) << "Selected " + toString(selectedCount) + " sequences from your taxonomy file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************
	//alignreport file has a column header line then all other lines contain 16 columns.  we just want the first column since that contains the name
	int GetSeqsCommand::readAlign() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(alignfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(alignfile));
		string outputFileName = getOutputFileName("alignreport", variables);
		ofstream out;
		File::openOutputFile(outputFileName, out);


		ifstream in;
		File::openInputFile(alignfile, in);
		string name, junk;

		bool wroteSomething = false;
		int selectedCount = 0;

		//read column headers
		for (int i = 0; i < 16; i++) {
			if (!in.eof()) { in >> junk;	 out << junk << '\t'; }
			else { break; }
		}
		out << endl;

		set<string> uniqueNames;
		while (!in.eof()) {

			if (ctrlc_pressed) { in.close(); out.close(); File::remove(outputFileName);  return 0; }


			in >> name;				//read from first column

			if (!dups) {//adjust name if needed
				map<string, string>::iterator it = uniqueMap.find(name);
				if (it != uniqueMap.end()) { name = it->second; }
			}

			//if this name is in the accnos file
			if (names.count(name) != 0) {
				if (uniqueNames.count(name) == 0) { //this name hasn't been seen yet
					uniqueNames.insert(name);
					wroteSomething = true;
					selectedCount++;

					out << name << '\t';

					//read rest
					for (int i = 0; i < 15; i++) {
						if (!in.eof()) { in >> junk;	 out << junk << '\t'; }
						else { break; }
					}
					out << endl;
				}
				else {
					LOG(WARNING) << "" + name + " is in your alignreport file more than once.  Mothur requires sequence names to be unique. I will only add it once.\n";
				}

			}
			else {//still read just don't do anything with it
			   //read rest
				for (int i = 0; i < 15; i++) {
					if (!in.eof()) { in >> junk; }
					else { break; }
				}
			}

			File::gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any sequence from the .accnos file." << '\n'; }
		outputNames.push_back(outputFileName);  outputTypes["alignreport"].push_back(outputFileName);

		LOG(INFO) << "Selected " + toString(selectedCount) + " sequences from your alignreport file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************
	//just looking at common mistakes. 
	int GetSeqsCommand::runSanityCheck() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(fastafile); }
		string filename = outputDir + "get.seqs.debug.report";

		ofstream out;
		File::openOutputFile(filename, out);


		//compare fasta, name, qual and taxonomy if given to make sure they contain the same seqs
		if (fastafile != "") {
			if (namefile != "") { //compare with fasta
				if (sanity["fasta"] != sanity["name"]) { //create mismatch file
					createMisMatchFile(out, fastafile, namefile, sanity["fasta"], sanity["name"]);
				}
			}
			if (qualfile != "") {
				if (sanity["fasta"] != sanity["qual"]) { //create mismatch file
					createMisMatchFile(out, fastafile, qualfile, sanity["fasta"], sanity["qual"]);
				}
			}
			if (taxfile != "") {
				if (sanity["fasta"] != sanity["tax"]) { //create mismatch file
					createMisMatchFile(out, fastafile, taxfile, sanity["fasta"], sanity["tax"]);
				}
			}
		}

		//compare dupnames, groups and list if given to make sure they match
		if (namefile != "") {
			if (groupfile != "") {
				if (sanity["dupname"] != sanity["group"]) { //create mismatch file
					createMisMatchFile(out, namefile, groupfile, sanity["dupname"], sanity["group"]);
				}
			}
			if (listfile != "") {
				if (sanity["dupname"] != sanity["list"]) { //create mismatch file
					createMisMatchFile(out, namefile, listfile, sanity["dupname"], sanity["list"]);
				}
			}
		}
		else {

			if ((groupfile != "") && (fastafile != "")) {
				if (sanity["fasta"] != sanity["group"]) { //create mismatch file
					createMisMatchFile(out, fastafile, groupfile, sanity["fasta"], sanity["group"]);
				}
			}
		}

		out.close();

		if (File::isBlank(filename)) { File::remove(filename); }
		else { LOG(INFO) << "\n[DEBUG]: " + filename + " contains the file mismatches.\n";outputNames.push_back(filename); outputTypes["debug"].push_back(filename); }

		return 0;
	}
	//**********************************************************************************************************************
	//just looking at common mistakes. 
	int GetSeqsCommand::createMisMatchFile(ofstream& out, string filename1, string filename2, set<string> set1, set<string> set2) {
		out << "****************************************" << endl << endl;
		out << "Names unique to " << filename1 << ":\n";

		//remove names in set1 that are also in set2
		for (set<string>::iterator it = set1.begin(); it != set1.end();) {
			string name = *it;

			if (set2.count(name) == 0) { out << name << endl; } //name unique to set1
			else { set2.erase(name); } //you are in both so erase 
			set1.erase(it++);
		}

		out << "\nNames unique to " << filename2 << ":\n";
		//output results
		for (set<string>::iterator it = set2.begin(); it != set2.end(); it++) { out << *it << endl; }

		out << "****************************************" << endl << endl;

		return 0;
	}
	//**********************************************************************************************************************

	int GetSeqsCommand::compareAccnos() {

		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(accnosfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(accnosfile));
		string outputFileName = getOutputFileName("accnosreport", variables);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(accnosfile2, in);
		string name;

		set<string> namesAccnos2;
		set<string> namesDups;
		set<string> namesAccnos = names;

		map<string, int> nameCount;

		if (namefile != "") {
			ifstream inName;
			File::openInputFile(namefile, inName);


			while (!inName.eof()) {

				if (ctrlc_pressed) { inName.close(); return 0; }

				string thisname, repnames;

				inName >> thisname;		File::gobble(inName);		//read from first column
				inName >> repnames;			//read from second column

				int num = m->getNumNames(repnames);
				nameCount[thisname] = num;

				File::gobble(inName);
			}
			inName.close();
		}

		while (!in.eof()) {
			in >> name;

			if (namesAccnos.count(name) == 0) { //name unique to accnos2
				int pos = name.find_last_of('_');
				string tempName = name;
				if (pos != string::npos) { tempName = tempName.substr(pos + 1); cout << tempName << endl; }
				if (namesAccnos.count(tempName) == 0) {
					namesAccnos2.insert(name);
				}
				else { //you are in both so erase
					namesAccnos.erase(name);
					namesDups.insert(name);
				}
			}
			else { //you are in both so erase
				namesAccnos.erase(name);
				namesDups.insert(name);
			}

			File::gobble(in);
		}
		in.close();

		out << "Names in both files : " + toString(namesDups.size()) << endl;
		LOG(INFO) << "Names in both files : " + toString(namesDups.size()) << '\n';

		for (set<string>::iterator it = namesDups.begin(); it != namesDups.end(); it++) {
			out << (*it);
			if (namefile != "") { out << '\t' << nameCount[(*it)]; }
			out << endl;
		}

		out << "Names unique to " + accnosfile + " : " + toString(namesAccnos.size()) << endl;
		LOG(INFO) << "Names unique to " + accnosfile + " : " + toString(namesAccnos.size()) << '\n';

		for (set<string>::iterator it = namesAccnos.begin(); it != namesAccnos.end(); it++) {
			out << (*it);
			if (namefile != "") { out << '\t' << nameCount[(*it)]; }
			out << endl;
		}

		out << "Names unique to " + accnosfile2 + " : " + toString(namesAccnos2.size()) << endl;
		LOG(INFO) << "Names unique to " + accnosfile2 + " : " + toString(namesAccnos2.size()) << '\n';

		for (set<string>::iterator it = namesAccnos2.begin(); it != namesAccnos2.end(); it++) {
			out << (*it);
			if (namefile != "") { out << '\t' << nameCount[(*it)]; }
			out << endl;
		}

		out.close();

		outputNames.push_back(outputFileName);  outputTypes["accnosreport"].push_back(outputFileName);

		return 0;

	}


	//**********************************************************************************************************************

