//
//  getdistscommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 1/28/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#include "getdistscommand.h"

//**********************************************************************************************************************
vector<string> GetDistsCommand::setParameters() {
	try {
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "PhylipColumn", "none", "phylip", false, false, true); parameters.push_back(pphylip);
		CommandParameter pcolumn("column", "InputTypes", "", "", "none", "PhylipColumn", "none", "column", false, false, true); parameters.push_back(pcolumn);
		CommandParameter paccnos("accnos", "InputTypes", "", "", "none", "none", "none", "", false, true, true); parameters.push_back(paccnos);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetDistsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetDistsCommand::getHelpString() {
	try {
		string helpString = "The get.dists command selects distances from a phylip or column file related to groups or sequences listed in an accnos file.\n"
			"The get.dists command parameters are accnos, phylip and column.\n"
			"The get.dists command should be in the following format: get.dists(accnos=yourAccnos, phylip=yourPhylip).\n"
			"Example get.dists(accnos=final.accnos, phylip=final.an.thetayc.0.03.lt.ave.dist).\n"
			"Note: No spaces between parameter labels (i.e. accnos), '=' and parameters (i.e.final.accnos).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetDistsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetDistsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "phylip") { pattern = "[filename],pick,[extension]"; }
	else if (type == "column") { pattern = "[filename],pick,[extension]"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
GetDistsCommand::GetDistsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["phylip"] = tempOutNames;
		outputTypes["column"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetDistsCommand, GetDistsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
GetDistsCommand::GetDistsCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["column"] = tempOutNames;
		outputTypes["phylip"] = tempOutNames;

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("phylip");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["phylip"] = inputDir + it->second; }
			}

			it = parameters.find("column");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["column"] = inputDir + it->second; }
			}

			it = parameters.find("accnos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["accnos"] = inputDir + it->second; }
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

		phylipfile = validParameter.validFile(parameters, "phylip", true);
		if (phylipfile == "not open") { phylipfile = ""; abort = true; }
		else if (phylipfile == "not found") { phylipfile = ""; }
		else { settings.setCurrent("phylip", phylipfile); }

		columnfile = validParameter.validFile(parameters, "column", true);
		if (columnfile == "not open") { columnfile = ""; abort = true; }
		else if (columnfile == "not found") { columnfile = ""; }
		else { settings.setCurrent("column", columnfile); }

		if ((phylipfile == "") && (columnfile == "")) {
			//is there are current file available for either of these?
			//give priority to column, then phylip
			columnfile = settings.getCurrent("column");
			if (columnfile != "") { LOG(INFO) << "Using " + columnfile + " as input file for the column parameter." << '\n'; }
			else {
				phylipfile = settings.getCurrent("phylip");
				if (phylipfile != "") { LOG(INFO) << "Using " + phylipfile + " as input file for the phylip parameter." << '\n'; }
				else {
					LOG(INFO) << "No valid current files. You must provide a phylip or column file." << '\n';
					abort = true;
				}
			}
		}
	}

}
//**********************************************************************************************************************

int GetDistsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//get names you want to keep
	names = m->readAccnos(accnosfile);

	if (ctrlc_pressed) { return 0; }

	//read through the correct file and output lines you want to keep
	if (phylipfile != "") { readPhylip(); }
	if (columnfile != "") { readColumn(); }

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }


	if (outputNames.size() != 0) {
		LOG(INFO) << '\n' << "Output File names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		//set fasta file as new current fastafile
		string current = "";
		itTypes = outputTypes.find("phylip");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("phylip", current); }
		}

		itTypes = outputTypes.find("column");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("column", current); }
		}
	}

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in GetDistsCommand, execute";
	exit(1);
}
}

//**********************************************************************************************************************
int GetDistsCommand::readPhylip() {
	try {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(phylipfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(phylipfile));
		variables["[extension]"] = m->getExtension(phylipfile);
		string outputFileName = getOutputFileName("phylip", variables);

		ifstream in;
		File::openInputFile(phylipfile, in);

		float distance;
		int square, nseqs;
		string name;
		unsigned int row;
		set<unsigned int> rows; //converts names in names to a index
		row = 0;

		string numTest;
		in >> numTest >> name;

		if (!m->isContainingOnlyDigits(numTest)) { LOG(LOGERROR) << "expected a number and got " + numTest + ", quitting." << '\n'; exit(1); }
		else { convert(numTest, nseqs); }

		if (names.count(name) != 0) { rows.insert(row); }
		row++;

		//is the matrix square?
		char d;
		while ((d = in.get()) != EOF) {

			if (isalnum(d)) {
				square = 1;
				in.putback(d);
				for (int i = 0;i < nseqs;i++) {
					in >> distance;
				}
				break;
			}
			if (d == '\n') {
				square = 0;
				break;
			}
		}

		//map name to row/column        
		if (square == 0) {
			for (int i = 1;i < nseqs;i++) {
				in >> name;
				if (names.count(name) != 0) { rows.insert(row); }
				row++;

				for (int j = 0;j < i;j++) {
					if (ctrlc_pressed) { in.close(); return 0; }
					in >> distance;
				}
			}
		}
		else {
			for (int i = 1;i < nseqs;i++) {
				in >> name;
				if (names.count(name) != 0) { rows.insert(row); }
				row++;
				for (int j = 0;j < nseqs;j++) {
					if (ctrlc_pressed) { in.close(); return 0; }
					in >> distance;
				}
			}
		}
		in.close();

		if (ctrlc_pressed) { return 0; }

		//read through file only printing rows and columns of seqs in names
		ifstream inPhylip;
		File::openInputFile(phylipfile, inPhylip);

		inPhylip >> numTest;

		ofstream out;
		File::openOutputFile(outputFileName, out);
		outputTypes["phylip"].push_back(outputFileName);  outputNames.push_back(outputFileName);
		out << names.size() << endl;

		unsigned int count = 0;
		if (square == 0) {
			for (int i = 0;i < nseqs;i++) {
				inPhylip >> name;
				bool ignoreRow = false;

				if (names.count(name) == 0) { ignoreRow = true; }
				else { out << name << '\t'; count++; }

				for (int j = 0;j < i;j++) {
					if (ctrlc_pressed) { inPhylip.close(); out.close();  return 0; }
					inPhylip >> distance;
					if (!ignoreRow) {
						//is this a column we want
						if (rows.count(j) != 0) { out << distance << '\t'; }
					}
				}
				if (!ignoreRow) { out << endl; }
			}
		}
		else {
			for (int i = 0;i < nseqs;i++) {
				inPhylip >> name;

				bool ignoreRow = false;

				if (names.count(name) == 0) { ignoreRow = true; }
				else { out << name << '\t'; count++; }

				for (int j = 0;j < nseqs;j++) {
					if (ctrlc_pressed) { inPhylip.close(); out.close(); return 0; }
					inPhylip >> distance;
					if (!ignoreRow) {
						//is this a column we want
						if (rows.count(j) != 0) { out << distance << '\t'; }
					}
				}
				if (!ignoreRow) { out << endl; }
			}
		}
		inPhylip.close();
		out.close();

		if (count == 0) { LOG(INFO) << "Your file does NOT contain distances related to groups or sequences listed in the accnos file." << '\n'; }
		else if (count != names.size()) {
			LOG(WARNING) << "Your accnos file contains " + toString(names.size()) + " groups or sequences, but I only found " + toString(count) + " of them in the phylip file." << '\n';
			//rewrite with new number
			m->renameFile(outputFileName, outputFileName + ".temp");
			ofstream out2;
			File::openOutputFile(outputFileName, out2);
			out2 << count << endl;

			ifstream in3;
			File::openInputFile(outputFileName + ".temp", in3);
			in3 >> nseqs; File::gobble(in3);
			char buffer[4096];
			while (!in3.eof()) {
				in3.read(buffer, 4096);
				out2.write(buffer, in3.gcount());
			}
			in3.close();
			out2.close();
			File::remove(outputFileName + ".temp");
		}

		LOG(INFO) << "Selected " + toString(count) + " groups or sequences from your phylip file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************
	int GetDistsCommand::readColumn() {
		string thisOutputDir = outputDir;
		if (outputDir == "") { thisOutputDir += File::getPath(columnfile); }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(columnfile));
		variables["[extension]"] = m->getExtension(columnfile);
		string outputFileName = getOutputFileName("column", variables);
		outputTypes["column"].push_back(outputFileName);  outputNames.push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(columnfile, in);

		set<string> foundNames;
		string firstName, secondName;
		float distance;
		while (!in.eof()) {

			if (ctrlc_pressed) { out.close(); in.close(); return 0; }

			in >> firstName >> secondName >> distance; File::gobble(in);

			//are both names in the accnos file
			if ((names.count(firstName) != 0) && (names.count(secondName) != 0)) {
				out << firstName << '\t' << secondName << '\t' << distance << endl;
				foundNames.insert(firstName);
				foundNames.insert(secondName);
			}
		}
		in.close();
		out.close();

		if (foundNames.size() == 0) { LOG(INFO) << "Your file does NOT contain distances related to groups or sequences listed in the accnos file." << '\n'; }
		else if (foundNames.size() != names.size()) {
			LOG(WARNING) << "Your accnos file contains " + toString(names.size()) + " groups or sequences, but I only found " + toString(foundNames.size()) + " of them in the column file." << '\n';
		}

		LOG(INFO) << "Selected " + toString(foundNames.size()) + " groups or sequences from your column file." << '\n';

		return 0;

	}
	//**********************************************************************************************************************


