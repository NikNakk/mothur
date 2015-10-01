/*
 *  classifyseqscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 11/2/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "classifyseqscommand.h"



 //**********************************************************************************************************************
vector<string> ClassifySeqsCommand::setParameters() {
	try {
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "none", "none", "none", "", false, true, true); parameters.push_back(ptaxonomy);
		nkParameters.add(new ReferenceFileParameter(settings, "reference", true, true));
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "taxonomy", false, true, true); parameters.push_back(pfasta);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "none", "", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "none", "none", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none", "", false, false, true); parameters.push_back(pgroup);

		CommandParameter psearch("search", "Multiple", "kmer-blast-suffix-distance-align", "kmer", "", "", "", "", false, false); parameters.push_back(psearch);
		nkParameters.add(new NumberParameter("ksize", -INFINITY, INFINITY, 8, false, false));
		CommandParameter pmethod("method", "Multiple", "wang-knn-zap", "wang", "", "", "", "", false, false); parameters.push_back(pmethod);
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new NumberParameter("match", -INFINITY, INFINITY, 1.0, false, false));
		nkParameters.add(new NumberParameter("mismatch", -INFINITY, INFINITY, -1.0, false, false));
		nkParameters.add(new NumberParameter("gapopen", -INFINITY, INFINITY, -2.0, false, false));
		nkParameters.add(new NumberParameter("gapextend", -INFINITY, INFINITY, -1.0, false, false));
		nkParameters.add(new NumberParameter("cutoff", -INFINITY, INFINITY, 0, false, true));
		nkParameters.add(new BooleanParameter("probs", true, false, false));
		nkParameters.add(new NumberParameter("iters", -INFINITY, INFINITY, 100, false, true));
		nkParameters.add(new BooleanParameter("save", false, false, false));
		nkParameters.add(new BooleanParameter("shortcuts", true, false, false));
		nkParameters.add(new BooleanParameter("relabund", false, false, false));
		nkParameters.add(new NumberParameter("numwanted", -INFINITY, INFINITY, 10, false, true));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifySeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClassifySeqsCommand::getHelpString() {
	try {
		string helpString = "The classify.seqs command reads a fasta file containing sequences and creates a .taxonomy file and a .tax.summary file.\n"
			"The classify.seqs command parameters are reference, fasta, name, group, count, search, ksize, method, taxonomy, processors, match, mismatch, gapopen, gapextend, numwanted, relabund and probs.\n"
			"The reference, fasta and taxonomy parameters are required. You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amzon.fasta \n"
			"The search parameter allows you to specify the method to find most similar template.  Your options are: suffix, kmer, blast, align and distance. The default is kmer.\n"
			"The name parameter allows you add a names file with your fasta file, if you enter multiple fasta files, you must enter matching names files for them.\n"
			"The group parameter allows you add a group file so you can have the summary totals broken up by group.\n"
			"The count parameter allows you add a count file so you can have the summary totals broken up by group.\n"
			"The method parameter allows you to specify classification method to use.  Your options are: wang, knn and zap. The default is wang.\n"
			"The ksize parameter allows you to specify the kmer size for finding most similar template to candidate.  The default is 8.\n"
			"The processors parameter allows you to specify the number of processors to use. The default is 1.\n"
			"If the save parameter is set to true the reference sequences will be saved in memory, to clear them later you can use the clear.memory command. Default=f."
			"The match parameter allows you to specify the bonus for having the same base. The default is 1.0.\n"
			"The mistmatch parameter allows you to specify the penalty for having different bases.  The default is -1.0.\n"
			"The gapopen parameter allows you to specify the penalty for opening a gap in an alignment. The default is -2.0.\n"
			"The gapextend parameter allows you to specify the penalty for extending a gap in an alignment.  The default is -1.0.\n"
			"The numwanted parameter allows you to specify the number of sequence matches you want with the knn method.  The default is 10.\n"
			"The cutoff parameter allows you to specify a bootstrap confidence threshold for your taxonomy.  The default is 0.\n"
			"The probs parameter shuts off the bootstrapping results for the wang and zap method. The default is true, meaning you want the bootstrapping to be shown.\n"
			"The relabund parameter allows you to indicate you want the summary file values to be relative abundances rather than raw abundances. Default=F. \n"
			"The iters parameter allows you to specify how many iterations to do when calculating the bootstrap confidence score for your taxonomy with the wang method.  The default is 100.\n";
		//helpString += "The flip parameter allows you shut off mothur's   The default is T.\n"
		"The classify.seqs command should be in the following format: \n"
			"classify.seqs(reference=yourTemplateFile, fasta=yourFastaFile, method=yourClassificationMethod, search=yourSearchmethod, ksize=yourKmerSize, taxonomy=yourTaxonomyFile, processors=yourProcessors) \n"
			"Example classify.seqs(fasta=amazon.fasta, reference=core.filtered, method=knn, search=gotoh, ksize=8, processors=2)\n"
			"The .taxonomy file consists of 2 columns: 1 = your sequence name, 2 = the taxonomy for your sequence. \n"
			"The .tax.summary is a summary of the different taxonomies represented in your fasta file. \n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifySeqsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClassifySeqsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "taxonomy") { pattern = "[filename],[tag],[tag2],taxonomy"; }
	else if (type == "taxsummary") { pattern = "[filename],[tag],[tag2],tax.summary"; }
	else if (type == "accnos") { pattern = "[filename],[tag],[tag2],flip.accnos"; }
	else if (type == "matchdist") { pattern = "[filename],[tag],[tag2],match.dist"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ClassifySeqsCommand::ClassifySeqsCommand() :
	Command(), rdb(ReferenceDB::getInstance())
{
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["taxonomy"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["taxsummary"] = tempOutNames;
		outputTypes["matchdist"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifySeqsCommand, ClassifySeqsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
ClassifySeqsCommand::ClassifySeqsCommand(string option) :
	rdb(ReferenceDB::getInstance())
{
	try {
		abort = false; calledHelp = false;
		hasName = false; hasCount = false;

		//allow user to run help
		if (option == "help") { help(); abort = true; calledHelp = true; }
		else if (option == "citation") { citation(); abort = true; calledHelp = true; }

		else {
			vector<string> myArray = setParameters();

			OptionParser parser(option);
			map<string, string> parameters = parser.getParameters();

			ValidParameters validParameter("classify.seqs");
			map<string, string>::iterator it;

			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) {
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
			}

			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["taxonomy"] = tempOutNames;
			outputTypes["taxsummary"] = tempOutNames;
			outputTypes["matchdist"] = tempOutNames;
			outputTypes["accnos"] = tempOutNames;

			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);
			if (inputDir == "not found") { inputDir = ""; }
			else {
				string path;
				it = parameters.find("reference");
				//user has given a template file
				if (it != parameters.end()) {
					path = File::getPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") { parameters["reference"] = inputDir + it->second; }
				}

				it = parameters.find("taxonomy");
				//user has given a template file
				if (it != parameters.end()) {
					path = File::getPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") { parameters["taxonomy"] = inputDir + it->second; }
				}
			}

			fastaFileName = validParameter.validFile(parameters, "fasta", false);
			if (fastaFileName == "not found") {
				//if there is a current fasta file, use it
				string filename = settings.getCurrent("fasta");
				if (filename != "") { fastaFileNames.push_back(filename); LOG(INFO) << "Using " + filename + " as input file for the fasta parameter." << '\n'; }
				else { LOG(INFO) << "You have no current fastafile and the fasta parameter is required." << '\n'; abort = true; }
			}
			else {
				Utility::split(fastaFileName, '-', fastaFileNames);

				//go through files and make sure they are good, if not, then disregard them
				for (int i = 0; i < fastaFileNames.size(); i++) {

					bool ignore = false;
					if (fastaFileNames[i] == "current") {
						fastaFileNames[i] = settings.getCurrent("fasta");
						if (fastaFileNames[i] != "") { LOG(INFO) << "Using " + fastaFileNames[i] + " as input file for the fasta parameter where you had given current." << '\n'; }
						else {
							LOG(INFO) << "You have no current fastafile, ignoring current." << '\n'; ignore = true;
							//erase from file list
							fastaFileNames.erase(fastaFileNames.begin() + i);
							i--;
						}
					}

					if (!ignore) {

						if (inputDir != "") {
							string path = File::getPath(fastaFileNames[i]);
							//if the user has not given a path then, add inputdir. else leave path alone.
							if (path == "") { fastaFileNames[i] = inputDir + fastaFileNames[i]; }
						}

						int ableToOpen;

						ifstream in;
						ableToOpen = File::openInputFile(fastaFileNames[i], in, "noerror");

						//if you can't open it, try default location
						if (ableToOpen == 1) {
							if (settings.getDefaultPath() != "") { //default path is set
								string tryPath = settings.getDefaultPath() + File::getSimpleName(fastaFileNames[i]);
								LOG(INFO) << "Unable to open " + fastaFileNames[i] + ". Trying default " + tryPath << '\n';
								ifstream in2;
								ableToOpen = File::openInputFile(tryPath, in2, "noerror");
								in2.close();
								fastaFileNames[i] = tryPath;
							}
						}

						if (ableToOpen == 1) {
							if (settings.getOutputDir() != "") { //default path is set
								string tryPath = settings.getOutputDir() + File::getSimpleName(fastaFileNames[i]);
								LOG(INFO) << "Unable to open " + fastaFileNames[i] + ". Trying output directory " + tryPath << '\n';
								ifstream in2;
								ableToOpen = File::openInputFile(tryPath, in2, "noerror");
								in2.close();
								fastaFileNames[i] = tryPath;
							}
						}

						in.close();

						if (ableToOpen == 1) {
							LOG(INFO) << "Unable to open " + fastaFileNames[i] + ". It will be disregarded." << '\n';
							//erase from file list
							fastaFileNames.erase(fastaFileNames.begin() + i);
							i--;
						}
						else {
							settings.setCurrent("fasta", fastaFileNames[i]);
						}
					}

				}

				//make sure there is at least one valid file left
				if (fastaFileNames.size() == 0) { LOG(INFO) << "no valid files." << '\n'; abort = true; }
			}

			namefile = validParameter.validFile(parameters, "name", false);
			if (namefile == "not found") { namefile = ""; }
			else {
				Utility::split(namefile, '-', namefileNames);

				//go through files and make sure they are good, if not, then disregard them
				for (int i = 0; i < namefileNames.size(); i++) {
					bool ignore = false;
					if (namefileNames[i] == "current") {
						namefileNames[i] = settings.getCurrent("name");
						if (namefileNames[i] != "") { LOG(INFO) << "Using " + namefileNames[i] + " as input file for the name parameter where you had given current." << '\n'; }
						else {
							LOG(INFO) << "You have no current namefile, ignoring current." << '\n'; ignore = true;
							//erase from file list
							namefileNames.erase(namefileNames.begin() + i);
							i--;
						}
					}

					if (!ignore) {

						if (inputDir != "") {
							string path = File::getPath(namefileNames[i]);
							//if the user has not given a path then, add inputdir. else leave path alone.
							if (path == "") { namefileNames[i] = inputDir + namefileNames[i]; }
						}
						int ableToOpen;

						ifstream in;
						ableToOpen = File::openInputFile(namefileNames[i], in, "noerror");

						//if you can't open it, try default location
						if (ableToOpen == 1) {
							if (settings.getDefaultPath() != "") { //default path is set
								string tryPath = settings.getDefaultPath() + File::getSimpleName(namefileNames[i]);
								LOG(INFO) << "Unable to open " + namefileNames[i] + ". Trying default " + tryPath << '\n';
								ifstream in2;
								ableToOpen = File::openInputFile(tryPath, in2, "noerror");
								in2.close();
								namefileNames[i] = tryPath;
							}
						}

						if (ableToOpen == 1) {
							if (settings.getOutputDir() != "") { //default path is set
								string tryPath = settings.getOutputDir() + File::getSimpleName(namefileNames[i]);
								LOG(INFO) << "Unable to open " + namefileNames[i] + ". Trying output directory " + tryPath << '\n';
								ifstream in2;
								ableToOpen = File::openInputFile(tryPath, in2, "noerror");
								in2.close();
								namefileNames[i] = tryPath;
							}
						}
						in.close();

						if (ableToOpen == 1) {
							LOG(INFO) << "Unable to open " + namefileNames[i] + ". It will be disregarded." << '\n';  abort = true;
							//erase from file list
							namefileNames.erase(namefileNames.begin() + i);
							i--;
						}
						else {
							settings.setCurrent("name", namefileNames[i]);
						}
					}
				}
			}

			if (namefileNames.size() != 0) { hasName = true; }

			if (namefile != "") {
				if (namefileNames.size() != fastaFileNames.size()) { abort = true; LOG(INFO) << "If you provide a name file, you must have one for each fasta file." << '\n'; }
			}

			//check for required parameters
			countfile = validParameter.validFile(parameters, "count", false);
			if (countfile == "not found") {
				countfile = "";
			}
			else {
				Utility::split(countfile, '-', countfileNames);

				//go through files and make sure they are good, if not, then disregard them
				for (int i = 0; i < countfileNames.size(); i++) {

					bool ignore = false;
					if (countfileNames[i] == "current") {
						countfileNames[i] = settings.getCurrent("counttable");
						if (countfileNames[i] != "") { LOG(INFO) << "Using " + countfileNames[i] + " as input file for the count parameter where you had given current." << '\n'; }
						else {
							LOG(INFO) << "You have no current count file, ignoring current." << '\n'; ignore = true;
							//erase from file list
							countfileNames.erase(countfileNames.begin() + i);
							i--;
						}
					}

					if (!ignore) {

						if (inputDir != "") {
							string path = File::getPath(countfileNames[i]);
							//if the user has not given a path then, add inputdir. else leave path alone.
							if (path == "") { countfileNames[i] = inputDir + countfileNames[i]; }
						}

						int ableToOpen;
						ifstream in;

						ableToOpen = File::openInputFile(countfileNames[i], in, "noerror");

						//if you can't open it, try default location
						if (ableToOpen == 1) {
							if (settings.getDefaultPath() != "") { //default path is set
								string tryPath = settings.getDefaultPath() + File::getSimpleName(countfileNames[i]);
								LOG(INFO) << "Unable to open " + countfileNames[i] + ". Trying default " + tryPath << '\n';
								ifstream in2;
								ableToOpen = File::openInputFile(tryPath, in2, "noerror");
								in2.close();
								countfileNames[i] = tryPath;
							}
						}

						if (ableToOpen == 1) {
							if (settings.getOutputDir() != "") { //default path is set
								string tryPath = settings.getOutputDir() + File::getSimpleName(countfileNames[i]);
								LOG(INFO) << "Unable to open " + countfileNames[i] + ". Trying output directory " + tryPath << '\n';
								ifstream in2;
								ableToOpen = File::openInputFile(tryPath, in2, "noerror");
								in2.close();
								countfileNames[i] = tryPath;
							}
						}

						in.close();

						if (ableToOpen == 1) {
							LOG(INFO) << "Unable to open " + countfileNames[i] + ". It will be disregarded." << '\n';
							//erase from file list
							countfileNames.erase(countfileNames.begin() + i);
							i--;
						}
						else {
							settings.setCurrent("counttable", countfileNames[i]);
						}
					}
				}
			}

			if (countfileNames.size() != 0) { hasCount = true; if (countfileNames.size() != fastaFileNames.size()) { LOG(INFO) << "If you provide a count file, you must have one for each fasta file." << '\n'; } }

			//make sure there is at least one valid file left
			if (hasName && hasCount) { LOG(LOGERROR) << "You must enter ONLY ONE of the following: count or name." << '\n'; abort = true; }

			groupfile = validParameter.validFile(parameters, "group", false);
			if (groupfile == "not found") { groupfile = ""; }
			else {
				Utility::split(groupfile, '-', groupfileNames);

				//go through files and make sure they are good, if not, then disregard them
				for (int i = 0; i < groupfileNames.size(); i++) {

					bool ignore = false;
					if (groupfileNames[i] == "current") {
						groupfileNames[i] = settings.getCurrent("group");
						if (groupfileNames[i] != "") { LOG(INFO) << "Using " + groupfileNames[i] + " as input file for the group parameter where you had given current." << '\n'; }
						else {
							LOG(INFO) << "You have no current group file, ignoring current." << '\n'; ignore = true;
							//erase from file list
							groupfileNames.erase(groupfileNames.begin() + i);
							i--;
						}
					}

					if (!ignore) {

						if (inputDir != "") {
							string path = File::getPath(groupfileNames[i]);
							//cout << path << '\t' << inputDir << endl;
							//if the user has not given a path then, add inputdir. else leave path alone.
							if (path == "") { groupfileNames[i] = inputDir + groupfileNames[i]; }
						}

						int ableToOpen;

						ifstream in;
						ableToOpen = File::openInputFile(groupfileNames[i], in, "noerror");

						//if you can't open it, try default location
						if (ableToOpen == 1) {
							if (settings.getDefaultPath() != "") { //default path is set
								string tryPath = settings.getDefaultPath() + File::getSimpleName(groupfileNames[i]);
								LOG(INFO) << "Unable to open " + groupfileNames[i] + ". Trying default " + tryPath << '\n';
								ifstream in2;
								ableToOpen = File::openInputFile(tryPath, in2, "noerror");
								in2.close();
								groupfileNames[i] = tryPath;
							}
						}

						if (ableToOpen == 1) {
							if (settings.getOutputDir() != "") { //default path is set
								string tryPath = settings.getOutputDir() + File::getSimpleName(groupfileNames[i]);
								LOG(INFO) << "Unable to open " + groupfileNames[i] + ". Trying output directory " + tryPath << '\n';
								ifstream in2;
								ableToOpen = File::openInputFile(tryPath, in2, "noerror");
								in2.close();
								groupfileNames[i] = tryPath;
							}
						}

						in.close();

						if (ableToOpen == 1) {
							LOG(INFO) << "Unable to open " + groupfileNames[i] + ". It will be disregarded." << '\n';
							//erase from file list
							groupfileNames.erase(groupfileNames.begin() + i);
							i--;
						}
						else {
							settings.setCurrent("group", groupfileNames[i]);
						}
					}

				}
			}

			if (groupfile != "") {
				if (groupfileNames.size() != fastaFileNames.size()) { abort = true; LOG(INFO) << "If you provide a group file, you must have one for each fasta file." << '\n'; }
				if (hasCount) { LOG(LOGERROR) << "You must enter ONLY ONE of the following: count or group." << '\n'; abort = true; }
			}
			else {
				for (int i = 0; i < fastaFileNames.size(); i++) { groupfileNames.push_back(""); }
			}

			//check for optional parameter and set defaults
			// ...at some point should added some additional type checking...
			string temp;
			temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
			settings.setProcessors(temp);
			Utility::mothurConvert(temp, processors);

			temp = validParameter.validFile(parameters, "save", false);			if (temp == "not found") { temp = "f"; }
			save = m->isTrue(temp);
			rdb.save = save;
			if (save) { //clear out old references
				rdb.clearMemory();
			}

			//this has to go after save so that if the user sets save=t and provides no reference we abort
			templateFileName = validParameter.validFile(parameters, "reference", true);
			if (templateFileName == "not found") {
				//check for saved reference sequences
				if (rdb.referenceSeqs.size() != 0) {
					templateFileName = "saved";
				}
				else {
					LOG(LOGERROR) << "You don't have any saved reference sequences and the reference parameter is a required for the classify.seqs command.";
					LOG(INFO) << "";
					abort = true;
				}
			}
			else if (templateFileName == "not open") { abort = true; }
			else { if (save) { rdb.setSavedReference(templateFileName); } }

			//this has to go after save so that if the user sets save=t and provides no reference we abort
			taxonomyFileName = validParameter.validFile(parameters, "taxonomy", true);
			if (taxonomyFileName == "not found") {
				//check for saved reference sequences
				if (rdb.wordGenusProb.size() != 0) {
					taxonomyFileName = "saved";
				}
				else {
					LOG(LOGERROR) << "You don't have any saved taxonomy information and the taxonomy parameter is a required for the classify.seqs command.";
					LOG(INFO) << "";
					abort = true;
				}
			}
			else if (taxonomyFileName == "not open") { abort = true; }
			else { if (save) { rdb.setSavedTaxonomy(taxonomyFileName); } }

			search = validParameter.validFile(parameters, "search", false);		if (search == "not found") { search = "kmer"; }

			method = validParameter.validFile(parameters, "method", false);		if (method == "not found") { method = "wang"; }

			temp = validParameter.validFile(parameters, "ksize", false);		if (temp == "not found") {
				temp = "8";
				if (method == "zap") { temp = "7"; }
			}
			Utility::mothurConvert(temp, kmerSize);

			temp = validParameter.validFile(parameters, "match", false);		if (temp == "not found") { temp = "1.0"; }
			Utility::mothurConvert(temp, match);

			temp = validParameter.validFile(parameters, "mismatch", false);		if (temp == "not found") { temp = "-1.0"; }
			Utility::mothurConvert(temp, misMatch);

			temp = validParameter.validFile(parameters, "gapopen", false);		if (temp == "not found") { temp = "-2.0"; }
			Utility::mothurConvert(temp, gapOpen);

			temp = validParameter.validFile(parameters, "gapextend", false);	if (temp == "not found") { temp = "-1.0"; }
			Utility::mothurConvert(temp, gapExtend);

			temp = validParameter.validFile(parameters, "numwanted", false);	if (temp == "not found") { temp = "10"; }
			Utility::mothurConvert(temp, numWanted);

			temp = validParameter.validFile(parameters, "cutoff", false);		if (temp == "not found") { temp = "0"; }
			Utility::mothurConvert(temp, cutoff);

			temp = validParameter.validFile(parameters, "probs", false);		if (temp == "not found") { temp = "true"; }
			probs = m->isTrue(temp);

			temp = validParameter.validFile(parameters, "relabund", false);		if (temp == "not found") { temp = "false"; }
			relabund = m->isTrue(temp);

			temp = validParameter.validFile(parameters, "shortcuts", false);	if (temp == "not found") { temp = "true"; }
			writeShortcuts = m->isTrue(temp);

			//temp = validParameter.validFile(parameters, "flip", false);			if (temp == "not found"){	temp = "T";				}
			//flip = m->isTrue(temp); 
			flip = true;

			temp = validParameter.validFile(parameters, "iters", false);		if (temp == "not found") { temp = "100"; }
			Utility::mothurConvert(temp, iters);


			if ((method == "wang") && (search != "kmer")) {
				LOG(INFO) << "The wang method requires the kmer search. " + search + " will be disregarded, and kmer will be used." << '\n';
				search = "kmer";
			}

			if ((method == "zap") && ((search != "kmer") && (search != "align"))) {
				LOG(INFO) << "The zap method requires the kmer or align search. " + search + " will be disregarded, and kmer will be used." << '\n';
				search = "kmer";
			}

			if (!abort) {
				if (!hasCount) {
					if (namefileNames.size() == 0) {
						if (fastaFileNames.size() != 0) {
							vector<string> files; files.push_back(fastaFileNames[fastaFileNames.size() - 1]);
							OptionParser::getNameFile(files);
						}
					}
				}
			}
		}
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifySeqsCommand, ClassifySeqsCommand";
		exit(1);
	}
}

//**********************************************************************************************************************
ClassifySeqsCommand::~ClassifySeqsCommand() {
	if (abort == false) {
		for (int i = 0; i < lines.size(); i++) { delete lines[i]; }  lines.clear();
	}
}
//**********************************************************************************************************************

int ClassifySeqsCommand::execute() {
	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	string outputMethodTag = method;
	if (method == "wang") { classify = new Bayesian(taxonomyFileName, templateFileName, search, kmerSize, cutoff, iters, rand(), flip, writeShortcuts); }
	else if (method == "knn") { classify = new Knn(taxonomyFileName, templateFileName, search, kmerSize, gapOpen, gapExtend, match, misMatch, numWanted, rand()); }
	else if (method == "zap") {
		outputMethodTag = search + "_" + outputMethodTag;
		if (search == "kmer") { classify = new KmerTree(templateFileName, taxonomyFileName, kmerSize, cutoff); }
		else { classify = new AlignTree(templateFileName, taxonomyFileName, cutoff); }
	}
	else {
		LOG(INFO) << search + " is not a valid method option. I will run the command using wang.";
		LOG(INFO) << "";
		classify = new Bayesian(taxonomyFileName, templateFileName, search, kmerSize, cutoff, iters, rand(), flip, writeShortcuts);
	}

	if (ctrlc_pressed) { delete classify; return 0; }

	for (int s = 0; s < fastaFileNames.size(); s++) {

		LOG(INFO) << "Classifying sequences from " + fastaFileNames[s] + " ..." << '\n';

		string baseTName = File::getSimpleName(taxonomyFileName);
		if (taxonomyFileName == "saved") { baseTName = rdb.getSavedTaxonomy(); }

		//set rippedTaxName to 
		string RippedTaxName = "";
		bool foundDot = false;
		for (int i = baseTName.length() - 1; i >= 0; i--) {
			if (foundDot && (baseTName[i] != '.')) { RippedTaxName = baseTName[i] + RippedTaxName; }
			else if (foundDot && (baseTName[i] == '.')) { break; }
			else if (!foundDot && (baseTName[i] == '.')) { foundDot = true; }
		}
		//if (RippedTaxName != "") { RippedTaxName +=  "."; }   

		if (outputDir == "") { outputDir += File::getPath(fastaFileNames[s]); }
		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[s]));
		variables["[tag]"] = RippedTaxName;
		variables["[tag2]"] = outputMethodTag;
		string newTaxonomyFile = getOutputFileName("taxonomy", variables);
		string newaccnosFile = getOutputFileName("accnos", variables);
		string tempTaxonomyFile = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[s])) + "taxonomy.temp";
		string taxSummary = getOutputFileName("taxsummary", variables);

		if ((method == "knn") && (search == "distance")) {
			string DistName = getOutputFileName("matchdist", variables);
			classify->setDistName(DistName);  outputNames.push_back(DistName); outputTypes["matchdist"].push_back(DistName);
		}

		outputNames.push_back(newTaxonomyFile); outputTypes["taxonomy"].push_back(newTaxonomyFile);
		outputNames.push_back(taxSummary);	outputTypes["taxsummary"].push_back(taxSummary);

		int start = time(NULL);
		int numFastaSeqs = 0;
		for (int i = 0; i < lines.size(); i++) { delete lines[i]; }  lines.clear();

		vector<unsigned long long> positions;
#if defined (UNIX)
		positions = File::divideFile(fastaFileNames[s], processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(new linePair(positions[i], positions[(i + 1)])); }
#else
		if (processors == 1) {
			lines.push_back(new linePair(0, 1000));
		}
		else {
			positions = m->setFilePosFasta(fastaFileNames[s], numFastaSeqs);
			if (positions.size() < processors) { processors = positions.size(); }

			//figure out how many sequences you have to process
			int numSeqsPerProcessor = numFastaSeqs / processors;
			for (int i = 0; i < processors; i++) {
				int startIndex = i * numSeqsPerProcessor;
				if (i == (processors - 1)) { numSeqsPerProcessor = numFastaSeqs - i * numSeqsPerProcessor; }
				lines.push_back(new linePair(positions[startIndex], numSeqsPerProcessor));
			}
		}
#endif
		if (processors == 1) {
			numFastaSeqs = driver(lines[0], newTaxonomyFile, tempTaxonomyFile, newaccnosFile, fastaFileNames[s]);
		}
		else {
			numFastaSeqs = createProcesses(newTaxonomyFile, tempTaxonomyFile, newaccnosFile, fastaFileNames[s]);
		}

		if (!File::isBlank(newaccnosFile)) {
			LOG(WARNING) << "mothur reversed some your sequences for a better classification.  If you would like to take a closer look, please check " + newaccnosFile + " for the list of the sequences\n.";
			outputNames.push_back(newaccnosFile); outputTypes["accnos"].push_back(newaccnosFile);
		}
		else { File::remove(newaccnosFile); }

		LOG(INFO) << std::endl << "It took " + toString(time(NULL) - start) + " secs to classify " + toString(numFastaSeqs) + " sequences." << '\n' << '\n';
		start = time(NULL);

		//read namefile
		if (namefile != "") {

			LOG(INFO) << "Reading " + namefileNames[s] + "..."; cout.flush();
			nameMap.clear(); //remove old names
			m->readNames(namefileNames[s], nameMap);
			LOG(INFO) << "  Done." << '\n';
		}

		string group = "";
		GroupMap* groupMap = NULL;
		CountTable* ct = NULL;
		PhyloSummary* taxaSum;
		if (hasCount) {
			ct = new CountTable();
			ct->readTable(countfileNames[s], true, false);
			taxaSum = new PhyloSummary(taxonomyFileName, ct, relabund);
			taxaSum->summarize(tempTaxonomyFile);
		}
		else {
			if (groupfile != "") { group = groupfileNames[s]; groupMap = new GroupMap(group); groupMap->readMap(); }

			taxaSum = new PhyloSummary(taxonomyFileName, groupMap, relabund);

			if (ctrlc_pressed) { outputTypes.clear(); if (ct != NULL) { delete ct; }  if (groupMap != NULL) { delete groupMap; } delete taxaSum; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } delete classify; return 0; }

			if (namefile == "") { taxaSum->summarize(tempTaxonomyFile); }
			else {
				ifstream in;
				File::openInputFile(tempTaxonomyFile, in);

				//read in users taxonomy file and add sequences to tree
				string name, taxon;

				while (!in.eof()) {
					if (ctrlc_pressed) { outputTypes.clear();  if (ct != NULL) { delete ct; } if (groupMap != NULL) { delete groupMap; } delete taxaSum; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } delete classify; return 0; }

					in >> name >> taxon; File::gobble(in);

					itNames = nameMap.find(name);

					if (itNames == nameMap.end()) {
						LOG(INFO) << name + " is not in your name file please correct." << '\n'; exit(1);
					}
					else {
						for (int i = 0; i < itNames->second.size(); i++) {
							taxaSum->addSeqToTree(itNames->second[i], taxon);  //add it as many times as there are identical seqs
						}
						itNames->second.clear();
						nameMap.erase(itNames->first);
					}
				}
				in.close();
			}
		}
		File::remove(tempTaxonomyFile);

		if (ctrlc_pressed) { outputTypes.clear(); if (ct != NULL) { delete ct; } if (groupMap != NULL) { delete groupMap; } for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } delete classify; return 0; }

		//print summary file
		ofstream outTaxTree;
		File::openOutputFile(taxSummary, outTaxTree);
		taxaSum->print(outTaxTree);
		outTaxTree.close();

		//output taxonomy with the unclassified bins added
		ifstream inTax;
		File::openInputFile(newTaxonomyFile, inTax);

		ofstream outTax;
		string unclass = newTaxonomyFile + ".unclass.temp";
		File::openOutputFile(unclass, outTax);

		//get maxLevel from phylotree so you know how many 'unclassified's to add
		int maxLevel = taxaSum->getMaxLevel();

		//read taxfile - this reading and rewriting is done to preserve the confidence scores.
		string name, taxon;
		while (!inTax.eof()) {
			if (ctrlc_pressed) { outputTypes.clear(); if (ct != NULL) { delete ct; }  if (groupMap != NULL) { delete groupMap; } delete taxaSum; for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } File::remove(unclass); delete classify; return 0; }

			inTax >> name >> taxon; File::gobble(inTax);

			string newTax = addUnclassifieds(taxon, maxLevel);

			outTax << name << '\t' << newTax << endl;
		}
		inTax.close();
		outTax.close();

		if (ct != NULL) { delete ct; }
		if (groupMap != NULL) { delete groupMap; } delete taxaSum;
		File::remove(newTaxonomyFile);
		rename(unclass.c_str(), newTaxonomyFile.c_str());

		LOG(INFO) << std::endl << "It took " + toString(time(NULL) - start) + " secs to create the summary file for " + toString(numFastaSeqs) + " sequences." << '\n' << '\n';

	}
	delete classify;

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	//set taxonomy file as new current taxonomyfile
	string current = "";
	itTypes = outputTypes.find("taxonomy");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("taxonomy", current); }
	}

	current = "";
	itTypes = outputTypes.find("accnos");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("accnos", current); }
	}



	return 0;
}

/**************************************************************************************************/
string ClassifySeqsCommand::addUnclassifieds(string tax, int maxlevel) {
	try {
		string newTax, taxon;
		int level = 0;

		//keep what you have counting the levels
		while (tax.find_first_of(';') != -1) {
			//get taxon
			taxon = tax.substr(0, tax.find_first_of(';')) + ';';
			tax = tax.substr(tax.find_first_of(';') + 1, tax.length());
			newTax += taxon;
			level++;
		}

		//add "unclassified" until you reach maxLevel
		while (level < maxlevel) {
			newTax += "unclassified;";
			level++;
		}

		return newTax;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifySeqsCommand, addUnclassifieds";
		exit(1);
	}
}

/**************************************************************************************************/

int ClassifySeqsCommand::createProcesses(string taxFileName, string tempTaxFile, string accnos, string filename) {

	int num = 0;
	processIDS.clear();
	bool recalc = false;

#if defined (UNIX)
	int process = 1;

	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
			process++;
		}
		else if (pid == 0) {
			num = driver(lines[process], taxFileName + m->mothurGetpid(process) + ".temp", tempTaxFile + m->mothurGetpid(process) + ".temp", accnos + m->mothurGetpid(process) + ".temp", filename);

			//pass numSeqs to parent
			ofstream out;
			string tempFile = filename + m->mothurGetpid(process) + ".num.temp";
			File::openOutputFile(tempFile, out);
			out << num << endl;
			out.close();

			exit(0);
		}
		else {
			LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(process) + "\n"; processors = process;
			for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
			//wait to die
			for (int i = 0;i < processIDS.size();i++) {
				int temp = processIDS[i];
				wait(&temp);
			}
			ctrlc_pressed = false;
			recalc = true;
			break;
		}
	}

	if (recalc) {
		//test line, also set recalc to true.
		//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";
		for (int i = 0; i < lines.size(); i++) { delete lines[i]; }  lines.clear();
		vector<unsigned long long> positions = File::divideFile(filename, processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(new linePair(positions[i], positions[(i + 1)])); }

		num = 0;
		processIDS.resize(0);
		process = 1;

		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				num = driver(lines[process], taxFileName + m->mothurGetpid(process) + ".temp", tempTaxFile + m->mothurGetpid(process) + ".temp", accnos + m->mothurGetpid(process) + ".temp", filename);

				//pass numSeqs to parent
				ofstream out;
				string tempFile = filename + m->mothurGetpid(process) + ".num.temp";
				File::openOutputFile(tempFile, out);
				out << num << endl;
				out.close();

				exit(0);
			}
			else {
				LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
				for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
				exit(0);
			}
		}
	}


	//parent does its part
	num = driver(lines[0], taxFileName, tempTaxFile, accnos, filename);

	//force parent to wait until all the processes are done
	for (int i = 0;i < processIDS.size();i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	for (int i = 0; i < processIDS.size(); i++) {
		ifstream in;
		string tempFile = filename + toString(processIDS[i]) + ".num.temp";
		File::openInputFile(tempFile, in);
		if (!in.eof()) { int tempNum = 0; in >> tempNum; num += tempNum; }
		in.close(); File::remove(m->getFullPathName(tempFile));
	}
#else
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Windows version shared memory, so be careful when passing variables through the alignData struct. 
	//Above fork() will clone, so memory is separate, but that's not the case with windows, 
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<classifyData*> pDataArray;
	vector<DWORD> dwThreadIdArray(processors - 1);
	vector<HANDLE> hThreadArray(processors - 1);

	//Create processor worker threads.
	for (int i = 0; i < processors - 1; i++) {
		// Allocate memory for thread data.
		string extension = "";
		if (i != 0) { extension = toString(i) + ".temp"; processIDS.push_back(i); }

		classifyData* tempclass = new classifyData((accnos + extension), probs, method, templateFileName, taxonomyFileName, (taxFileName + extension), (tempTaxFile + extension), filename, search, kmerSize, iters, numWanted, m, lines[i]->start, lines[i]->end, match, misMatch, gapOpen, gapExtend, cutoff, i, flip, writeShortcuts);
		pDataArray.push_back(tempclass);

		//MySeqSumThreadFunction is in header. It must be global or static to work with the threads.
		//default security attributes, thread function name, argument to thread function, use default creation flags, returns the thread identifier
		hThreadArray[i] = CreateThread(NULL, 0, MyClassThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]);

	}

	//parent does its part
	num = driver(lines[processors - 1], taxFileName + toString(processors - 1) + ".temp", tempTaxFile + toString(processors - 1) + ".temp", accnos + toString(processors - 1) + ".temp", filename);
	processIDS.push_back((processors - 1));

	//Wait until all threads have terminated.
	WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

	//Close all thread handles and free memory allocations.
	for (int i = 0; i < pDataArray.size(); i++) {
		num += pDataArray[i]->count;
		if (pDataArray[i]->count != pDataArray[i]->end) {
			LOG(LOGERROR) << "process " + toString(i) + " only processed " + toString(pDataArray[i]->count) + " of " + toString(pDataArray[i]->end) + " sequences assigned to it, quitting. \n"; ctrlc_pressed = true;
		}
		CloseHandle(hThreadArray[i]);
		delete pDataArray[i];
	}

#endif	
	vector<string> nonBlankAccnosFiles;
	if (!(File::isBlank(accnos))) { nonBlankAccnosFiles.push_back(accnos); }
	else { File::remove(accnos); } //remove so other files can be renamed to it

	for (int i = 0;i < processIDS.size();i++) {
		File::appendFiles((taxFileName + toString(processIDS[i]) + ".temp"), taxFileName);
		File::appendFiles((tempTaxFile + toString(processIDS[i]) + ".temp"), tempTaxFile);
		if (!(File::isBlank(accnos + toString(processIDS[i]) + ".temp"))) {
			nonBlankAccnosFiles.push_back(accnos + toString(processIDS[i]) + ".temp");
		}
		else { File::remove((accnos + toString(processIDS[i]) + ".temp")); }

		File::remove((m->getFullPathName(taxFileName) + toString(processIDS[i]) + ".temp"));
		File::remove((m->getFullPathName(tempTaxFile) + toString(processIDS[i]) + ".temp"));
	}

	//append accnos files
	if (nonBlankAccnosFiles.size() != 0) {
		rename(nonBlankAccnosFiles[0].c_str(), accnos.c_str());

		for (int h = 1; h < nonBlankAccnosFiles.size(); h++) {
			File::appendFiles(nonBlankAccnosFiles[h], accnos);
			File::remove(nonBlankAccnosFiles[h]);
		}
	}
	else { //recreate the accnosfile if needed
		ofstream out;
		File::openOutputFile(accnos, out);
		out.close();
	}

	return num;

}
//**********************************************************************************************************************

int ClassifySeqsCommand::driver(linePair* filePos, string taxFName, string tempTFName, string accnos, string filename) {
	ofstream outTax;
	File::openOutputFile(taxFName, outTax);

	ofstream outTaxSimple;
	File::openOutputFile(tempTFName, outTaxSimple);

	ofstream outAcc;
	File::openOutputFile(accnos, outAcc);

	ifstream inFASTA;
	File::openInputFile(filename, inFASTA);

	string taxonomy;

	inFASTA.seekg(filePos->start);

	bool done = false;
	int count = 0;

	while (!done) {
		if (ctrlc_pressed) {
			inFASTA.close();
			outTax.close();
			outTaxSimple.close();
			outAcc.close(); return 0;
		}

		Sequence* candidateSeq = new Sequence(inFASTA); File::gobble(inFASTA);

		if (candidateSeq->getName() != "") {

			taxonomy = classify->getTaxonomy(candidateSeq);

			if (ctrlc_pressed) { delete candidateSeq; return 0; }

			if (taxonomy == "unknown;") { LOG(WARNING) << candidateSeq->getName() + " could not be classified. You can use the remove.lineage command with taxon=unknown; to remove such sequences.\n"; }

			//output confidence scores or not
			if (probs) {
				outTax << candidateSeq->getName() << '\t' << taxonomy << endl;
			}
			else {
				outTax << candidateSeq->getName() << '\t' << classify->getSimpleTax() << endl;
			}

			if (classify->getFlipped()) { outAcc << candidateSeq->getName() << endl; }

			outTaxSimple << candidateSeq->getName() << '\t' << classify->getSimpleTax() << endl;

			count++;
		}
		delete candidateSeq;

#if defined (UNIX)
		unsigned long long pos = inFASTA.tellg();
		if ((pos == -1) || (pos >= filePos->end)) { break; }
#else
		if (inFASTA.eof()) { break; }
#endif

		//report progress
		if ((count) % 100 == 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(count) + "\n"; }

	}
	//report progress
	if ((count) % 100 != 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(count) + "\n"; }

	inFASTA.close();
	outTax.close();
	outTaxSimple.close();
	outAcc.close();

	return count;
}
/**************************************************************************************************/
