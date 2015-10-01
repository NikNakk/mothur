/*
 *  chimeraslayercommand.cpp
 *  Mothur
 *
 *  Created by westcott on 3/31/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "chimeraslayercommand.h"
#include "deconvolutecommand.h"
#include "referencedb.h"
#include "sequenceparser.h"
#include "counttable.h"

 //**********************************************************************************************************************
vector<string> ChimeraSlayerCommand::setParameters() {
	try {
		nkParameters.add(new ReferenceFileParameter(settings, "reference", true, true));
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "chimera-accnos", false, true, true); parameters.push_back(pfasta);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "none", "", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "none", "none", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none", "", false, false, true); parameters.push_back(pgroup);
		nkParameters.add(new NumberParameter("window", -INFINITY, INFINITY, 50, false, false));
		nkParameters.add(new NumberParameter("ksize", -INFINITY, INFINITY, 7, false, false));
		nkParameters.add(new NumberParameter("match", -INFINITY, INFINITY, 5.0, false, false));
		nkParameters.add(new NumberParameter("mismatch", -INFINITY, INFINITY, -4.0, false, false));
		nkParameters.add(new NumberParameter("minsim", -INFINITY, INFINITY, 90, false, false));
		nkParameters.add(new NumberParameter("mincov", -INFINITY, INFINITY, 70, false, false));
		nkParameters.add(new NumberParameter("minsnp", -INFINITY, INFINITY, 10, false, false));
		nkParameters.add(new NumberParameter("minbs", -INFINITY, INFINITY, 90, false, false));
		CommandParameter psearch("search", "Multiple", "kmer-blast", "blast", "", "", "", "", false, false); parameters.push_back(psearch);
		nkParameters.add(new ProcessorsParameter(settings));

		nkParameters.add(new BooleanParameter("realign", true, false, false));
		CommandParameter ptrim("trim", "Boolean", "", "F", "", "", "", "fasta", false, false); parameters.push_back(ptrim);
		nkParameters.add(new BooleanParameter("split", false, false, false));
		nkParameters.add(new NumberParameter("numwanted", -INFINITY, INFINITY, 15, false, false));
		nkParameters.add(new NumberParameter("iters", -INFINITY, INFINITY, 1000, false, false));
		nkParameters.add(new NumberParameter("divergence", -INFINITY, INFINITY, 1.007, false, false));
		nkParameters.add(new BooleanParameter("dereplicate", false, false, false));
		nkParameters.add(new NumberParameter("parents", -INFINITY, INFINITY, 3, false, false));
		nkParameters.add(new NumberParameter("increment", -INFINITY, INFINITY, 5, false, false));
		nkParameters.add(new StringParameter("blastlocation", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));
		nkParameters.add(new BooleanParameter("save", false, false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraSlayerCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraSlayerCommand::getHelpString() {
	try {
		string helpString = "The chimera.slayer command reads a fastafile and referencefile and outputs potentially chimeric sequences.\n"
			"This command was modeled after the chimeraSlayer written by the Broad Institute.\n"
			"The chimera.slayer command parameters are fasta, name, group, template, processors, dereplicate, trim, ksize, window, match, mismatch, divergence. minsim, mincov, minbs, minsnp, parents, search, iters, increment, numwanted, blastlocation and realign.\n"
			"The fasta parameter allows you to enter the fasta file containing your potentially chimeric sequences, and is required, unless you have a valid current fasta file. \n"
			"The name parameter allows you to provide a name file, if you are using reference=self. \n"
			"The group parameter allows you to provide a group file. The group file can be used with a namesfile and reference=self. When checking sequences, only sequences from the same group as the query sequence will be used as the reference. \n"
			"The count parameter allows you to provide a count file. The count file reference=self. If your count file contains group information, when checking sequences, only sequences from the same group as the query sequence will be used as the reference. When you use a count file with group info and dereplicate=T, mothur will create a *.pick.count_table file containing seqeunces after chimeras are removed. \n"
			"You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amazon.fasta \n"
			"The reference parameter allows you to enter a reference file containing known non-chimeric sequences, and is required. You may also set template=self, in this case the abundant sequences will be used as potential parents. \n"
			"The processors parameter allows you to specify how many processors you would like to use.  The default is 1. \n"
			"If the dereplicate parameter is false, then if one group finds the seqeunce to be chimeric, then all groups find it to be chimeric, default=f.\n"
			"The trim parameter allows you to output a new fasta file containing your sequences with the chimeric ones trimmed to include only their longest piece, default=F. \n"
			"The split parameter allows you to check both pieces of non-chimeric sequence for chimeras, thus looking for trimeras and quadmeras. default=F. \n"
			"The window parameter allows you to specify the window size for searching for chimeras, default=50. \n"
			"The increment parameter allows you to specify how far you move each window while finding chimeric sequences, default=5.\n"
			"The numwanted parameter allows you to specify how many sequences you would each query sequence compared with, default=15.\n"
			"The ksize parameter allows you to input kmersize, default is 7, used if search is kmer. \n"
			"The match parameter allows you to reward matched bases in blast search, default is 5. \n"
			"The parents parameter allows you to select the number of potential parents to investigate from the numwanted best matches after rating them, default is 3. \n"
			"The mismatch parameter allows you to penalize mismatched bases in blast search, default is -4. \n"
			"The divergence parameter allows you to set a cutoff for chimera determination, default is 1.007. \n"
			"The iters parameter allows you to specify the number of bootstrap iters to do with the chimeraslayer method, default=1000.\n"
			"The minsim parameter allows you to specify a minimum similarity with the parent fragments, default=90. \n"
			"The mincov parameter allows you to specify minimum coverage by closest matches found in template. Default is 70, meaning 70%. \n"
			"The minbs parameter allows you to specify minimum bootstrap support for calling a sequence chimeric. Default is 90, meaning 90%. \n"
			"The minsnp parameter allows you to specify percent of SNPs to sample on each side of breakpoint for computing bootstrap support (default: 10) \n"
			"The search parameter allows you to specify search method for finding the closest parent. Choices are blast, and kmer, default blast. \n"
			"The realign parameter allows you to realign the query to the potential parents. Choices are true or false, default true.  \n"
			"The blastlocation parameter allows you to specify the location of your blast executable. By default mothur will look in ./blast/bin relative to mothur's executable.  \n"
			"If the save parameter is set to true the reference sequences will be saved in memory, to clear them later you can use the clear.memory command. Default=f."
			"The chimera.slayer command should be in the following format: \n"
			"chimera.slayer(fasta=yourFastaFile, reference=yourTemplate, search=yourSearch) \n"
			"Example: chimera.slayer(fasta=AD.align, reference=core_set_aligned.imputed.fasta, search=kmer) \n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraSlayerCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraSlayerCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "chimera") { pattern = "[filename],slayer.chimeras"; }
	else if (type == "accnos") { pattern = "[filename],slayer.accnos"; }
	else if (type == "fasta") { pattern = "[filename],slayer.fasta"; }
	else if (type == "count") { pattern = "[filename],slayer.pick.count_table"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ChimeraSlayerCommand::ChimeraSlayerCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraSlayerCommand, ChimeraSlayerCommand";
		exit(1);
	}
}
//***************************************************************************************************************
ChimeraSlayerCommand::ChimeraSlayerCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	ReferenceDB& rdb = ReferenceDB::getInstance();
	hasCount = false;
	hasName = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter("chimera.slayer");
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["count"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }

		//check for required parameters
		fastafile = validParameter.validFile(parameters, "fasta", false);
		if (fastafile == "not found") {
			//if there is a current fasta file, use it
			string filename = settings.getCurrent("fasta");
			if (filename != "") { fastaFileNames.push_back(filename); LOG(INFO) << "Using " + filename + " as input file for the fasta parameter." << '\n'; }
			else { LOG(INFO) << "You have no current fastafile and the fasta parameter is required." << '\n'; abort = true; }
		}
		else {
			Utility::split(fastafile, '-', fastaFileNames);

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
			if (fastaFileNames.size() == 0) { LOG(LOGERROR) << "no valid files." << '\n'; abort = true; }
		}


		//check for required parameters
		namefile = validParameter.validFile(parameters, "name", false);
		if (namefile == "not found") { namefile = ""; }
		else {
			Utility::split(namefile, '-', nameFileNames);

			//go through files and make sure they are good, if not, then disregard them
			for (int i = 0; i < nameFileNames.size(); i++) {

				bool ignore = false;
				if (nameFileNames[i] == "current") {
					nameFileNames[i] = settings.getCurrent("name");
					if (nameFileNames[i] != "") { LOG(INFO) << "Using " + nameFileNames[i] + " as input file for the name parameter where you had given current." << '\n'; }
					else {
						LOG(INFO) << "You have no current namefile, ignoring current." << '\n'; ignore = true;
						//erase from file list
						nameFileNames.erase(nameFileNames.begin() + i);
						i--;
					}
				}

				if (!ignore) {

					if (inputDir != "") {
						string path = File::getPath(nameFileNames[i]);
						//if the user has not given a path then, add inputdir. else leave path alone.
						if (path == "") { nameFileNames[i] = inputDir + nameFileNames[i]; }
					}

					int ableToOpen;
					ifstream in;

					ableToOpen = File::openInputFile(nameFileNames[i], in, "noerror");

					//if you can't open it, try default location
					if (ableToOpen == 1) {
						if (settings.getDefaultPath() != "") { //default path is set
							string tryPath = settings.getDefaultPath() + File::getSimpleName(nameFileNames[i]);
							LOG(INFO) << "Unable to open " + nameFileNames[i] + ". Trying default " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							nameFileNames[i] = tryPath;
						}
					}

					if (ableToOpen == 1) {
						if (settings.getOutputDir() != "") { //default path is set
							string tryPath = settings.getOutputDir() + File::getSimpleName(nameFileNames[i]);
							LOG(INFO) << "Unable to open " + nameFileNames[i] + ". Trying output directory " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							nameFileNames[i] = tryPath;
						}
					}

					in.close();

					if (ableToOpen == 1) {
						LOG(INFO) << "Unable to open " + nameFileNames[i] + ". It will be disregarded." << '\n';
						//erase from file list
						nameFileNames.erase(nameFileNames.begin() + i);
						i--;
					}
					else {
						settings.setCurrent("name", nameFileNames[i]);
					}
				}
			}
		}

		if (nameFileNames.size() != 0) { hasName = true; }

		//check for required parameters
		vector<string> countfileNames;
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
					if (nameFileNames[i] != "") { LOG(INFO) << "Using " + countfileNames[i] + " as input file for the count parameter where you had given current." << '\n'; }
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

		if (countfileNames.size() != 0) { hasCount = true; }

		//make sure there is at least one valid file left
		if (hasName && hasCount) { LOG(LOGERROR) << "You must enter ONLY ONE of the following: count or name." << '\n'; abort = true; }

		if (!hasName && hasCount) { nameFileNames = countfileNames; }

		if ((hasCount || hasName) && (nameFileNames.size() != fastaFileNames.size())) { LOG(LOGERROR) << "The number of name or count files does not match the number of fastafiles, please correct." << '\n'; abort = true; }

		bool hasGroup = true;
		groupfile = validParameter.validFile(parameters, "group", false);
		if (groupfile == "not found") { groupfile = "";  hasGroup = false; }
		else {
			Utility::split(groupfile, '-', groupFileNames);

			//go through files and make sure they are good, if not, then disregard them
			for (int i = 0; i < groupFileNames.size(); i++) {

				bool ignore = false;
				if (groupFileNames[i] == "current") {
					groupFileNames[i] = settings.getCurrent("group");
					if (groupFileNames[i] != "") { LOG(INFO) << "Using " + groupFileNames[i] + " as input file for the group parameter where you had given current." << '\n'; }
					else {
						LOG(INFO) << "You have no current namefile, ignoring current." << '\n'; ignore = true;
						//erase from file list
						groupFileNames.erase(groupFileNames.begin() + i);
						i--;
					}
				}

				if (!ignore) {

					if (inputDir != "") {
						string path = File::getPath(groupFileNames[i]);
						//if the user has not given a path then, add inputdir. else leave path alone.
						if (path == "") { groupFileNames[i] = inputDir + groupFileNames[i]; }
					}

					int ableToOpen;
					ifstream in;

					ableToOpen = File::openInputFile(groupFileNames[i], in, "noerror");

					//if you can't open it, try default location
					if (ableToOpen == 1) {
						if (settings.getDefaultPath() != "") { //default path is set
							string tryPath = settings.getDefaultPath() + File::getSimpleName(groupFileNames[i]);
							LOG(INFO) << "Unable to open " + groupFileNames[i] + ". Trying default " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							groupFileNames[i] = tryPath;
						}
					}

					if (ableToOpen == 1) {
						if (settings.getOutputDir() != "") { //default path is set
							string tryPath = settings.getOutputDir() + File::getSimpleName(groupFileNames[i]);
							LOG(INFO) << "Unable to open " + groupFileNames[i] + ". Trying output directory " + tryPath << '\n';
							ifstream in2;
							ableToOpen = File::openInputFile(tryPath, in2, "noerror");
							in2.close();
							groupFileNames[i] = tryPath;
						}
					}

					in.close();

					if (ableToOpen == 1) {
						LOG(INFO) << "Unable to open " + groupFileNames[i] + ". It will be disregarded." << '\n';
						//erase from file list
						groupFileNames.erase(groupFileNames.begin() + i);
						i--;
					}
					else {
						settings.setCurrent("group", groupFileNames[i]);
					}
				}
			}

			//make sure there is at least one valid file left
			if (groupFileNames.size() == 0) { LOG(LOGERROR) << "no valid group files." << '\n'; abort = true; }
		}

		if (hasGroup && (groupFileNames.size() != fastaFileNames.size())) { LOG(LOGERROR) << "The number of groupfiles does not match the number of fastafiles, please correct." << '\n'; abort = true; }

		if (hasGroup && hasCount) { LOG(LOGERROR) << "You must enter ONLY ONE of the following: count or group." << '\n'; abort = true; }
		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		string temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "save", false);			if (temp == "not found") { temp = "f"; }
		save = m->isTrue(temp);
		rdb.save = save;
		if (save) { //clear out old references
			rdb.clearMemory();
		}

		string path;
		it = parameters.find("reference");
		//user has given a template file
		if (it != parameters.end()) {
			if (it->second == "self") {
				templatefile = "self";
				if (save) {
					LOG(WARNING) << "You can't save reference=self, ignoring save.";
					LOG(INFO) << "";
					save = false;
				}
			}
			else {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["reference"] = inputDir + it->second; }

				templatefile = validParameter.validFile(parameters, "reference", true);
				if (templatefile == "not open") { abort = true; }
				else if (templatefile == "not found") { //check for saved reference sequences
					if (rdb.referenceSeqs.size() != 0) {
						templatefile = "saved";
					}
					else {
						LOG(LOGERROR) << "You don't have any saved reference sequences and the reference parameter is a required.";
						LOG(INFO) << "";
						abort = true;
					}
				}
				else { if (save) { rdb.setSavedReference(templatefile); } }
			}
		}
		else if (hasName) {
			templatefile = "self";
			if (save) {
				LOG(WARNING) << "You can't save reference=self, ignoring save.";
				LOG(INFO) << "";
				save = false;
			}
		}
		else if (hasCount) {
			templatefile = "self";
			if (save) {
				LOG(WARNING) << "You can't save reference=self, ignoring save.";
				LOG(INFO) << "";
				save = false;
			}
		}
		else {
			if (rdb.referenceSeqs.size() != 0) {
				templatefile = "saved";
			}
			else {
				LOG(LOGERROR) << "You don't have any saved reference sequences and the reference parameter is a required.";
				LOG(INFO) << "";
				templatefile = ""; abort = true;
			}
		}



		temp = validParameter.validFile(parameters, "ksize", false);			if (temp == "not found") { temp = "7"; }
		Utility::mothurConvert(temp, ksize);

		temp = validParameter.validFile(parameters, "window", false);			if (temp == "not found") { temp = "50"; }
		Utility::mothurConvert(temp, window);

		temp = validParameter.validFile(parameters, "match", false);			if (temp == "not found") { temp = "5"; }
		Utility::mothurConvert(temp, match);

		temp = validParameter.validFile(parameters, "mismatch", false);			if (temp == "not found") { temp = "-4"; }
		Utility::mothurConvert(temp, mismatch);

		temp = validParameter.validFile(parameters, "divergence", false);		if (temp == "not found") { temp = "1.007"; }
		Utility::mothurConvert(temp, divR);

		temp = validParameter.validFile(parameters, "minsim", false);			if (temp == "not found") { temp = "90"; }
		Utility::mothurConvert(temp, minSimilarity);

		temp = validParameter.validFile(parameters, "mincov", false);			if (temp == "not found") { temp = "70"; }
		Utility::mothurConvert(temp, minCoverage);

		temp = validParameter.validFile(parameters, "minbs", false);			if (temp == "not found") { temp = "90"; }
		Utility::mothurConvert(temp, minBS);

		temp = validParameter.validFile(parameters, "minsnp", false);			if (temp == "not found") { temp = "10"; }
		Utility::mothurConvert(temp, minSNP);

		temp = validParameter.validFile(parameters, "parents", false);			if (temp == "not found") { temp = "3"; }
		Utility::mothurConvert(temp, parents);

		temp = validParameter.validFile(parameters, "realign", false);			if (temp == "not found") { temp = "t"; }
		realign = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "trim", false);				if (temp == "not found") { temp = "f"; }
		trim = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "split", false);			if (temp == "not found") { temp = "f"; }
		trimera = m->isTrue(temp);

		search = validParameter.validFile(parameters, "search", false);			if (search == "not found") { search = "blast"; }

		temp = validParameter.validFile(parameters, "iters", false);			if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, iters);

		temp = validParameter.validFile(parameters, "increment", false);		if (temp == "not found") { temp = "5"; }
		Utility::mothurConvert(temp, increment);

		temp = validParameter.validFile(parameters, "numwanted", false);		if (temp == "not found") { temp = "15"; }
		Utility::mothurConvert(temp, numwanted);

		temp = validParameter.validFile(parameters, "dereplicate", false);
		if (temp == "not found") { temp = "false"; }
		dups = m->isTrue(temp);

		blastlocation = validParameter.validFile(parameters, "blastlocation", false);
		if (blastlocation == "not found") { blastlocation = ""; }
		else {
			//add / to name if needed
			string lastChar = blastlocation.substr(blastlocation.length() - 1);
#if defined (UNIX)
			if (lastChar != "/") { blastlocation += "/"; }
#else
			if (lastChar != "\\") { blastlocation += "\\"; }
#endif
			blastlocation = m->getFullPathName(blastlocation);
			string formatdbCommand = "";
#if defined (UNIX)
			formatdbCommand = blastlocation + "formatdb";
#else
			formatdbCommand = blastlocation + "formatdb.exe";
#endif

			//test to make sure formatdb exists
			ifstream in;
			formatdbCommand = m->getFullPathName(formatdbCommand);
			int ableToOpen = File::openInputFile(formatdbCommand, in, "no error"); in.close();
			if (ableToOpen == 1) { LOG(LOGERROR) << "" + formatdbCommand + " file does not exist. mothur requires formatdb.exe to run chimera.slayer." << '\n'; abort = true; }

			string blastCommand = "";
#if defined (UNIX)
			blastCommand = blastlocation + "megablast";
#else
			blastCommand = blastlocation + "megablast.exe";
#endif
			//test to make sure formatdb exists
			ifstream in2;
			blastCommand = m->getFullPathName(blastCommand);
			ableToOpen = File::openInputFile(blastCommand, in2, "no error"); in2.close();
			if (ableToOpen == 1) { LOG(LOGERROR) << "" + blastCommand + " file does not exist. mothur requires blastall.exe to run chimera.slayer." << '\n'; abort = true; }
		}

		if ((search != "blast") && (search != "kmer")) { LOG(INFO) << search + " is not a valid search." << '\n'; abort = true; }

		if ((hasName || hasCount) && (templatefile != "self")) { LOG(INFO) << "You have provided a namefile or countfile and the reference parameter is not set to self. I am not sure what reference you are trying to use, aborting." << '\n'; abort = true; }
		if (hasGroup && (templatefile != "self")) { LOG(INFO) << "You have provided a group file and the reference parameter is not set to self. I am not sure what reference you are trying to use, aborting." << '\n'; abort = true; }

		//until we resolve the issue 10-18-11
#if defined (UNIX)
#else
			//processors=1;
#endif
	}
}
//***************************************************************************************************************

int ChimeraSlayerCommand::execute() {
	try {
		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		for (int s = 0; s < fastaFileNames.size(); s++) {

			LOG(INFO) << "Checking sequences from " + fastaFileNames[s] + " ..." << '\n';

			int start = time(NULL);
			if (outputDir == "") { outputDir = File::getPath(fastaFileNames[s]); }//if user entered a file with a path then preserve it	
			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[s]));
			string outputFileName = getOutputFileName("chimera", variables);
			string accnosFileName = getOutputFileName("accnos", variables);
			string trimFastaFileName = getOutputFileName("fasta", variables);
			string newCountFile = "";

			//clears files
			ofstream out, out1, out2;
			File::openOutputFile(outputFileName, out); out.close();
			File::openOutputFile(accnosFileName, out1); out1.close();
			if (trim) { File::openOutputFile(trimFastaFileName, out2); out2.close(); }
			outputNames.push_back(outputFileName); outputTypes["chimera"].push_back(outputFileName);
			outputNames.push_back(accnosFileName); outputTypes["accnos"].push_back(accnosFileName);
			if (trim) { outputNames.push_back(trimFastaFileName); outputTypes["fasta"].push_back(trimFastaFileName); }

			//maps a filename to priority map. 
			//if no groupfile this is fastafileNames[s] -> prioirity
			//if groupfile then this is each groups seqs -> priority
			map<string, map<string, int> > fileToPriority;
			map<string, map<string, int> >::iterator itFile;
			map<string, string> fileGroup;
			fileToPriority[fastaFileNames[s]] = priority; //default
			fileGroup[fastaFileNames[s]] = "noGroup";
			map<string, string> uniqueNames;
			int totalChimeras = 0;
			lines.clear();

			if (templatefile == "self") {
				if (hasCount) {
					SequenceCountParser* parser = NULL;
					setUpForSelfReference(parser, fileGroup, fileToPriority, s);
					if (parser != NULL) { uniqueNames = parser->getAllSeqsMap(); delete parser; }
				}
				else {
					SequenceParser* parser = NULL;
					setUpForSelfReference(parser, fileGroup, fileToPriority, s);
					if (parser != NULL) { uniqueNames = parser->getAllSeqsMap(); delete parser; }
				}
			}

			if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

			if (fileToPriority.size() == 1) { //you running without a groupfile
				itFile = fileToPriority.begin();
				string thisFastaName = itFile->first;
				map<string, int> thisPriority = itFile->second;
				//break up file
				vector<unsigned long long> positions;
#if defined (UNIX)
				positions = File::divideFile(thisFastaName, processors);
				for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }
#else
				if (processors == 1) { lines.push_back(linePair(0, 1000)); }
				else {
					positions = m->setFilePosFasta(thisFastaName, numSeqs);
					if (positions.size() < processors) { processors = positions.size(); }

					//figure out how many sequences you have to process
					int numSeqsPerProcessor = numSeqs / processors;
					for (int i = 0; i < processors; i++) {
						int startIndex = i * numSeqsPerProcessor;
						if (i == (processors - 1)) { numSeqsPerProcessor = numSeqs - i * numSeqsPerProcessor; }
						lines.push_back(linePair(positions[startIndex], numSeqsPerProcessor));
					}
				}
#endif
				if (processors == 1) { numSeqs = driver(lines[0], outputFileName, thisFastaName, accnosFileName, trimFastaFileName, thisPriority); }
				else { numSeqs = createProcesses(outputFileName, thisFastaName, accnosFileName, trimFastaFileName, thisPriority); }

				if (ctrlc_pressed) { outputTypes.clear(); if (trim) { File::remove(trimFastaFileName); } File::remove(outputFileName); File::remove(accnosFileName); for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

			}
			else { //you have provided a groupfile
				string countFile = "";
				if (hasCount) {
					countFile = nameFileNames[s];
					variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(nameFileNames[s]));
					newCountFile = getOutputFileName("count", variables);
				}

				if (processors == 1) {
					numSeqs = driverGroups(outputFileName, accnosFileName, trimFastaFileName, fileToPriority, fileGroup, newCountFile);
					if (hasCount && dups) {
						CountTable c; c.readTable(nameFileNames[s], true, false);
						if (!File::isBlank(newCountFile)) {
							ifstream in2;
							File::openInputFile(newCountFile, in2);

							string name, group;
							while (!in2.eof()) {
								in2 >> name >> group; File::gobble(in2);
								c.setAbund(name, group, 0);
							}
							in2.close();
						}
						File::remove(newCountFile);
						c.printTable(newCountFile);
					}

				}
				else { numSeqs = createProcessesGroups(outputFileName, accnosFileName, trimFastaFileName, fileToPriority, fileGroup, newCountFile, countFile); } //destroys fileToPriority


				if (!dups) {
					totalChimeras = deconvoluteResults(uniqueNames, outputFileName, accnosFileName, trimFastaFileName);
					LOG(INFO) << '\n' << toString(totalChimeras) + " chimera found." << '\n';
				}
				else {
					if (hasCount) {
						set<string> doNotRemove;
						CountTable c; c.readTable(newCountFile, true, true);
						vector<string> namesInTable = c.getNamesOfSeqs();
						for (int i = 0; i < namesInTable.size(); i++) {
							int temp = c.getNumSeqs(namesInTable[i]);
							if (temp == 0) { c.remove(namesInTable[i]); }
							else { doNotRemove.insert((namesInTable[i])); }
						}
						//remove names we want to keep from accnos file.
						set<string> accnosNames = m->readAccnos(accnosFileName);
						ofstream out2;
						File::openOutputFile(accnosFileName, out2);
						for (set<string>::iterator it = accnosNames.begin(); it != accnosNames.end(); it++) {
							if (doNotRemove.count(*it) == 0) { out2 << (*it) << endl; }
						}
						out2.close();
						c.printTable(newCountFile);
						outputNames.push_back(newCountFile); outputTypes["count"].push_back(newCountFile);
					}
				}
			}

			LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences." << '\n';
		}

		//set accnos file as new current accnosfile
		string current = "";
		itTypes = outputTypes.find("accnos");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("accnos", current); }
		}

		if (trim) {
			itTypes = outputTypes.find("fasta");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("fasta", current); }
			}
		}

		itTypes = outputTypes.find("count");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
		}

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraSlayerCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************
int ChimeraSlayerCommand::deconvoluteResults(map<string, string>& uniqueNames, string outputFileName, string accnosFileName, string trimFileName) {
	map<string, string>::iterator itUnique;
	int total = 0;

	if (trimera) { //add in more potential uniqueNames
		map<string, string> newUniqueNames = uniqueNames;
		for (map<string, string>::iterator it = uniqueNames.begin(); it != uniqueNames.end(); it++) {
			newUniqueNames[(it->first) + "_LEFT"] = (it->first) + "_LEFT";
			newUniqueNames[(it->first) + "_RIGHT"] = (it->first) + "_RIGHT";
		}
		uniqueNames = newUniqueNames;
		newUniqueNames.clear();
	}

	//edit accnos file
	ifstream in2;
	File::openInputFile(accnosFileName, in2, "no error");

	ofstream out2;
	File::openOutputFile(accnosFileName + ".temp", out2);

	string name; name = "";
	set<string> chimerasInFile;
	set<string>::iterator itChimeras;

	while (!in2.eof()) {
		if (ctrlc_pressed) { in2.close(); out2.close(); File::remove(outputFileName); File::remove((accnosFileName + ".temp")); return 0; }

		in2 >> name; File::gobble(in2);

		//find unique name
		itUnique = uniqueNames.find(name);

		if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing accnos results. Cannot find " + name + "." << '\n'; ctrlc_pressed = true; }
		else {
			itChimeras = chimerasInFile.find((itUnique->second));

			if (itChimeras == chimerasInFile.end()) {
				out2 << itUnique->second << endl;
				chimerasInFile.insert((itUnique->second));
				total++;
			}
		}
	}
	in2.close();
	out2.close();

	File::remove(accnosFileName);
	rename((accnosFileName + ".temp").c_str(), accnosFileName.c_str());


	//edit chimera file
	ifstream in;
	File::openInputFile(outputFileName, in);

	ofstream out;
	File::openOutputFile(outputFileName + ".temp", out); out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);

	string rest, parent1, parent2, line;
	set<string> namesInFile; //this is so if a sequence is found to be chimera in several samples we dont write it to the results file more than once
	set<string>::iterator itNames;

	//assumptions - in file each read will always look like...
	/*
	 F11Fcsw_92754	no
	 F11Fcsw_63104	F11Fcsw_33372	F11Fcsw_37007	0.89441	80.4469	0.2	1.03727	93.2961	52.2	no	0-241	243-369
	 */

	 //get header line
	if (!in.eof()) {
		line = File::getline(in); File::gobble(in);
		out << line << endl;
	}

	//for the chimera file, we want to make sure if any group finds a sequence to be chimeric then all groups do, 
	//so if this is a report that did not find it to be chimeric, but it appears in the accnos file, 
	//then ignore this report and continue until we find the report that found it to be chimeric

	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); out.close(); File::remove((outputFileName + ".temp")); return 0; }

		in >> name;		File::gobble(in);
		in >> parent1;	File::gobble(in);

		if (name == "Name") { //name = "Name" because we append the header line each time we add results from the groups
			line = File::getline(in); File::gobble(in);
		}
		else {
			if (parent1 == "no") {
				//find unique name
				itUnique = uniqueNames.find(name);

				if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find " + name + "." << '\n'; ctrlc_pressed = true; }
				else {
					//is this sequence really not chimeric??
					itChimeras = chimerasInFile.find(itUnique->second);

					if (itChimeras == chimerasInFile.end()) {
						//is this sequence not already in the file
						itNames = namesInFile.find((itUnique->second));

						if (itNames == namesInFile.end()) { out << itUnique->second << '\t' << "no" << endl; namesInFile.insert(itUnique->second); }
					}
				}
			}
			else { //read the rest of the line
				double DivQLAQRB, PerIDQLAQRB, BootStrapA, DivQLBQRA, PerIDQLBQRA, BootStrapB;
				string flag, range1, range2;
				bool print = false;
				in >> parent2 >> DivQLAQRB >> PerIDQLAQRB >> BootStrapA >> DivQLBQRA >> PerIDQLBQRA >> BootStrapB >> flag >> range1 >> range2;	File::gobble(in);

				//find unique name
				itUnique = uniqueNames.find(name);

				if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find " + name + "." << '\n'; ctrlc_pressed = true; }
				else {
					name = itUnique->second;
					//is this name already in the file
					itNames = namesInFile.find((name));

					if (itNames == namesInFile.end()) { //no not in file
						if (flag == "no") { //are you really a no??
							//is this sequence really not chimeric??
							itChimeras = chimerasInFile.find(name);

							//then you really are a no so print, otherwise skip
							if (itChimeras == chimerasInFile.end()) { print = true; }

						}
						else { print = true; }
					}
				}

				if (print) {
					out << name << '\t';

					namesInFile.insert(name);

					//output parent1's name
					itUnique = uniqueNames.find(parent1);
					if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find parentA " + parent1 + "." << '\n'; ctrlc_pressed = true; }
					else { out << itUnique->second << '\t'; }

					//output parent2's name
					itUnique = uniqueNames.find(parent2);
					if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find parentA " + parent2 + "." << '\n'; ctrlc_pressed = true; }
					else { out << itUnique->second << '\t'; }

					out << DivQLAQRB << '\t' << PerIDQLAQRB << '\t' << BootStrapA << '\t' << DivQLBQRA << '\t' << PerIDQLBQRA << '\t' << BootStrapB << '\t' << flag << '\t' << range1 << '\t' << range2 << endl;
				}
			}
		}
	}
	in.close();
	out.close();

	File::remove(outputFileName);
	rename((outputFileName + ".temp").c_str(), outputFileName.c_str());

	//edit fasta file
	if (trim) {
		ifstream in3;
		File::openInputFile(trimFileName, in3);

		ofstream out3;
		File::openOutputFile(trimFileName + ".temp", out3);

		namesInFile.clear();

		while (!in3.eof()) {
			if (ctrlc_pressed) { in3.close(); out3.close(); File::remove(outputFileName); File::remove(accnosFileName); File::remove((trimFileName + ".temp")); return 0; }

			Sequence seq(in3); File::gobble(in3);

			if (seq.getName() != "") {
				//find unique name
				itUnique = uniqueNames.find(seq.getName());

				if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing accnos results. Cannot find " + seq.getName() + "." << '\n'; ctrlc_pressed = true; }
				else {
					itNames = namesInFile.find((itUnique->second));

					if (itNames == namesInFile.end()) {
						seq.printSequence(out3);
					}
				}
			}
		}
		in3.close();
		out3.close();

		File::remove(trimFileName);
		rename((trimFileName + ".temp").c_str(), trimFileName.c_str());
	}

	return total;
}
//**********************************************************************************************************************
int ChimeraSlayerCommand::setUpForSelfReference(SequenceParser*& parser, map<string, string>& fileGroup, map<string, map<string, int> >& fileToPriority, int s) {
	fileGroup.clear();
	fileToPriority.clear();

	string nameFile = "";
	if (nameFileNames.size() != 0) { //you provided a namefile and we don't need to create one
		nameFile = nameFileNames[s];
	}
	else { nameFile = getNamesFile(fastaFileNames[s]); }

	//you provided a groupfile
	string groupFile = "";
	if (groupFileNames.size() != 0) { groupFile = groupFileNames[s]; }

	if (groupFile == "") {
		if (processors != 1) { LOG(INFO) << "When using template=self, mothur can only use 1 processor, continuing." << '\n'; processors = 1; }

		//sort fastafile by abundance, returns new sorted fastafile name
		LOG(INFO) << "Sorting fastafile according to abundance..."; cout.flush();
		priority = sortFastaFile(fastaFileNames[s], nameFile);
		LOG(INFO) << "Done." << '\n';

		fileToPriority[fastaFileNames[s]] = priority;
		fileGroup[fastaFileNames[s]] = "noGroup";
	}
	else {
		//Parse sequences by group
		parser = new SequenceParser(groupFile, fastaFileNames[s], nameFile);
		vector<string> groups = parser->getNamesOfGroups();

		for (int i = 0; i < groups.size(); i++) {
			vector<Sequence> thisGroupsSeqs = parser->getSeqs(groups[i]);
			map<string, string> thisGroupsMap = parser->getNameMap(groups[i]);
			group2NameMap[groups[i]] = thisGroupsMap;
			string newFastaFile = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[s])) + groups[i] + "-sortedTemp.fasta";
			priority = sortFastaFile(thisGroupsSeqs, thisGroupsMap, newFastaFile);
			fileToPriority[newFastaFile] = priority;
			fileGroup[newFastaFile] = groups[i];
		}
	}


	return 0;
}
//**********************************************************************************************************************
int ChimeraSlayerCommand::setUpForSelfReference(SequenceCountParser*& parser, map<string, string>& fileGroup, map<string, map<string, int> >& fileToPriority, int s) {
	fileGroup.clear();
	fileToPriority.clear();

	string nameFile = "";
	if (nameFileNames.size() != 0) { //you provided a namefile and we don't need to create one
		nameFile = nameFileNames[s];
	}
	else { ctrlc_pressed = true; return 0; }

	CountTable ct;
	if (!ct.testGroups(nameFile)) {
		if (processors != 1) { LOG(INFO) << "When using template=self, mothur can only use 1 processor, continuing." << '\n'; processors = 1; }

		//sort fastafile by abundance, returns new sorted fastafile name
		LOG(INFO) << "Sorting fastafile according to abundance..."; cout.flush();
		priority = sortFastaFile(fastaFileNames[s], nameFile);
		LOG(INFO) << "Done." << '\n';

		fileToPriority[fastaFileNames[s]] = priority;
		fileGroup[fastaFileNames[s]] = "noGroup";
	}
	else {
		//Parse sequences by group
		parser = new SequenceCountParser(nameFile, fastaFileNames[s]);
		vector<string> groups = parser->getNamesOfGroups();

		for (int i = 0; i < groups.size(); i++) {
			vector<Sequence> thisGroupsSeqs = parser->getSeqs(groups[i]);
			map<string, int> thisGroupsMap = parser->getCountTable(groups[i]);
			string newFastaFile = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[s])) + groups[i] + "-sortedTemp.fasta";
			sortFastaFile(thisGroupsSeqs, thisGroupsMap, newFastaFile);
			fileToPriority[newFastaFile] = thisGroupsMap;
			fileGroup[newFastaFile] = groups[i];
		}
	}


	return 0;
}
//**********************************************************************************************************************
string ChimeraSlayerCommand::getNamesFile(string& inputFile) {
	string nameFile = "";

	LOG(INFO) << std::endl << "No namesfile given, running unique.seqs command to generate one." << '\n' << '\n';

	//use unique.seqs to create new name and fastafile
	string inputString = "fasta=" + inputFile;
	LOG(INFO) << "/******************************************/" << '\n';
	LOG(INFO) << "Running command: unique.seqs(" + inputString + ")" << '\n';
	m->mothurCalling = true;

	Command* uniqueCommand = new DeconvoluteCommand(inputString);
	uniqueCommand->execute();

	map<string, vector<string> > filenames = uniqueCommand->getOutputFiles();

	delete uniqueCommand;
	m->mothurCalling = false;
	LOG(INFO) << "/******************************************/" << '\n';

	nameFile = filenames["name"][0];
	inputFile = filenames["fasta"][0];

	return nameFile;
}
//**********************************************************************************************************************

int ChimeraSlayerCommand::driverGroups(string outputFName, string accnos, string fasta, map<string, map<string, int> >& fileToPriority, map<string, string>& fileGroup, string countlist) {
	int totalSeqs = 0;
	ofstream outCountList;

	if (hasCount && dups) { File::openOutputFile(countlist, outCountList); }

	for (map<string, map<string, int> >::iterator itFile = fileToPriority.begin(); itFile != fileToPriority.end(); itFile++) {

		if (ctrlc_pressed) { return 0; }

		int start = time(NULL);
		string thisFastaName = itFile->first;
		map<string, int> thisPriority = itFile->second;
		string thisoutputFileName = outputDir + File::getRootName(File::getSimpleName(thisFastaName)) + fileGroup[thisFastaName] + "slayer.chimera";
		string thisaccnosFileName = outputDir + File::getRootName(File::getSimpleName(thisFastaName)) + fileGroup[thisFastaName] + "slayer.accnos";
		string thistrimFastaFileName = outputDir + File::getRootName(File::getSimpleName(thisFastaName)) + fileGroup[thisFastaName] + "slayer.fasta";

		LOG(INFO) << '\n' << "Checking sequences from group: " + fileGroup[thisFastaName] + "." << '\n';

		lines.clear();
#if defined (UNIX)
		int proc = 1;
		vector<unsigned long long> positions = File::divideFile(thisFastaName, proc);
		lines.push_back(linePair(positions[0], positions[1]));
#else
		lines.push_back(linePair(0, 1000));
#endif			
		int numSeqs = driver(lines[0], thisoutputFileName, thisFastaName, thisaccnosFileName, thistrimFastaFileName, thisPriority);

		//if we provided a count file with group info and set dereplicate=t, then we want to create a *.pick.count_table
		//This table will zero out group counts for seqs determined to be chimeric by that group.
		if (dups) {
			if (!File::isBlank(thisaccnosFileName)) {
				ifstream in;
				File::openInputFile(thisaccnosFileName, in);
				string name;
				if (hasCount) {
					while (!in.eof()) {
						in >> name; File::gobble(in);
						outCountList << name << '\t' << fileGroup[thisFastaName] << endl;
					}
					in.close();
				}
				else {
					map<string, map<string, string> >::iterator itGroupNameMap = group2NameMap.find(fileGroup[thisFastaName]);
					if (itGroupNameMap != group2NameMap.end()) {
						map<string, string> thisnamemap = itGroupNameMap->second;
						map<string, string>::iterator itN;
						ofstream out;
						File::openOutputFile(thisaccnosFileName + ".temp", out);
						while (!in.eof()) {
							in >> name; File::gobble(in);
							itN = thisnamemap.find(name);
							if (itN != thisnamemap.end()) {
								vector<string> tempNames; m->splitAtComma(itN->second, tempNames);
								for (int j = 0; j < tempNames.size(); j++) { out << tempNames[j] << endl; }

							}
							else { LOG(LOGERROR) << "parsing cannot find " + name + ".\n"; ctrlc_pressed = true; }
						}
						out.close();
						in.close();
						m->renameFile(thisaccnosFileName + ".temp", thisaccnosFileName);
					}
					else { LOG(LOGERROR) << "parsing cannot find " + fileGroup[thisFastaName] + ".\n"; ctrlc_pressed = true; }
				}

			}
		}

		//append files
		File::appendFiles(thisoutputFileName, outputFName); File::remove(thisoutputFileName);
		File::appendFiles(thisaccnosFileName, accnos); File::remove(thisaccnosFileName);
		if (trim) { File::appendFiles(thistrimFastaFileName, fasta); File::remove(thistrimFastaFileName); }
		File::remove(thisFastaName);

		totalSeqs += numSeqs;

		LOG(INFO) << '\n' << "It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences from group " + fileGroup[thisFastaName] + "." << '\n';
	}

	if (hasCount && dups) { outCountList.close(); }

	return totalSeqs;
}
/**************************************************************************************************/
int ChimeraSlayerCommand::createProcessesGroups(string outputFName, string accnos, string fasta, map<string, map<string, int> >& fileToPriority, map<string, string>& fileGroup, string countlist, string countFile) {
	int process = 1;
	int num = 0;
	processIDS.clear();
	bool recalc = false;
	map<string, map<string, int> > copyFileToPriority;
	copyFileToPriority = fileToPriority;

	if (fileToPriority.size() < processors) { processors = fileToPriority.size(); }

	CountTable newCount;
	if (hasCount && dups) { newCount.readTable(countFile, true, false); }

	int groupsPerProcessor = fileToPriority.size() / processors;
	int remainder = fileToPriority.size() % processors;

	vector< map<string, map<string, int> > > breakUp;

	for (int i = 0; i < processors; i++) {
		map<string, map<string, int> > thisFileToPriority;
		map<string, map<string, int> >::iterator itFile;
		int count = 0;
		int enough = groupsPerProcessor;
		if (i == 0) { enough = groupsPerProcessor + remainder; }

		for (itFile = fileToPriority.begin(); itFile != fileToPriority.end();) {
			thisFileToPriority[itFile->first] = itFile->second;
			fileToPriority.erase(itFile++);
			count++;
			if (count == enough) { break; }
		}
		breakUp.push_back(thisFileToPriority);
	}

#if defined (UNIX)
	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
			process++;
		}
		else if (pid == 0) {
			num = driverGroups(outputFName + toString(m->mothurGetpid(process)) + ".temp", accnos + m->mothurGetpid(process) + ".temp", fasta + toString(m->mothurGetpid(process)) + ".temp", breakUp[process], fileGroup, accnos + toString(m->mothurGetpid(process)) + ".byCount");

			//pass numSeqs to parent
			ofstream out;
			string tempFile = outputFName + toString(m->mothurGetpid(process)) + ".num.temp";
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
		groupsPerProcessor = copyFileToPriority.size() / processors;
		remainder = copyFileToPriority.size() % processors;
		breakUp.clear();

		for (int i = 0; i < processors; i++) {
			map<string, map<string, int> > thisFileToPriority;
			map<string, map<string, int> >::iterator itFile;
			int count = 0;
			int enough = groupsPerProcessor;
			if (i == 0) { enough = groupsPerProcessor + remainder; }

			for (itFile = copyFileToPriority.begin(); itFile != copyFileToPriority.end();) {
				thisFileToPriority[itFile->first] = itFile->second;
				copyFileToPriority.erase(itFile++);
				count++;
				if (count == enough) { break; }
			}
			breakUp.push_back(thisFileToPriority);
		}

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
				num = driverGroups(outputFName + toString(m->mothurGetpid(process)) + ".temp", accnos + m->mothurGetpid(process) + ".temp", fasta + toString(m->mothurGetpid(process)) + ".temp", breakUp[process], fileGroup, accnos + toString(m->mothurGetpid(process)) + ".byCount");

				//pass numSeqs to parent
				ofstream out;
				string tempFile = outputFName + toString(m->mothurGetpid(process)) + ".num.temp";
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

	num = driverGroups(outputFName, accnos, fasta, breakUp[0], fileGroup, accnos + ".byCount");

	//force parent to wait until all the processes are done
	for (int i = 0;i < processors;i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	for (int i = 0; i < processIDS.size(); i++) {
		ifstream in;
		string tempFile = outputFName + toString(processIDS[i]) + ".num.temp";
		File::openInputFile(tempFile, in);
		if (!in.eof()) { int tempNum = 0;  in >> tempNum; num += tempNum; }
		in.close(); File::remove(tempFile);
	}
#else

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Windows version shared memory, so be careful when passing variables through the slayerData struct. 
	//Above fork() will clone, so memory is separate, but that's not the case with windows, 
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<slayerData*> pDataArray;
	vector<DWORD> dwThreadIdArray(processors - 1);
	vector<HANDLE> hThreadArray(processors - 1);

	//Create processor worker threads.
	for (int i = 1; i < processors; i++) {
		string extension = toString(i) + ".temp";
		slayerData* tempslayer = new slayerData(group2NameMap, hasCount, dups, (accnos + toString(i) + ".byCount"), (outputFName + extension), (fasta + extension), (accnos + extension), templatefile, search, blastlocation, trimera, trim, realign, m, breakUp[i], fileGroup, ksize, match, mismatch, window, minSimilarity, minCoverage, minBS, minSNP, parents, iters, increment, numwanted, divR, priority, i);
		pDataArray.push_back(tempslayer);
		processIDS.push_back(i);

		//MySlayerThreadFunction is in header. It must be global or static to work with the threads.
		//default security attributes, thread function name, argument to thread function, use default creation flags, returns the thread identifier
		hThreadArray[i - 1] = CreateThread(NULL, 0, MySlayerGroupThreadFunction, pDataArray[i - 1], 0, &dwThreadIdArray[i - 1]);
	}

	num = driverGroups(outputFName, accnos, fasta, breakUp[0], fileGroup, accnos + ".byCount");

	//Wait until all threads have terminated.
	WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

	//Close all thread handles and free memory allocations.
	for (int i = 0; i < pDataArray.size(); i++) {
		if (pDataArray[i]->fileToPriority.size() != pDataArray[i]->end) {
			LOG(LOGERROR) << "process " + toString(i) + " only processed " + toString(pDataArray[i]->end) + " of " + toString(pDataArray[i]->fileToPriority.size()) + " groups assigned to it, quitting. \n"; ctrlc_pressed = true;
		}
		num += pDataArray[i]->count;
		CloseHandle(hThreadArray[i]);
		delete pDataArray[i];
	}
#endif	
	//read my own
	if (hasCount && dups) {
		if (!File::isBlank(accnos + ".byCount")) {
			ifstream in2;
			File::openInputFile(accnos + ".byCount", in2);

			string name, group;
			while (!in2.eof()) {
				in2 >> name >> group; File::gobble(in2);
				newCount.setAbund(name, group, 0);
			}
			in2.close();
		}
		File::remove(accnos + ".byCount");
	}


	//append output files
	for (int i = 0;i < processIDS.size();i++) {
		File::appendFiles((outputFName + toString(processIDS[i]) + ".temp"), outputFName);
		File::remove((outputFName + toString(processIDS[i]) + ".temp"));

		File::appendFiles((accnos + toString(processIDS[i]) + ".temp"), accnos);
		File::remove((accnos + toString(processIDS[i]) + ".temp"));

		if (trim) {
			File::appendFiles((fasta + toString(processIDS[i]) + ".temp"), fasta);
			File::remove((fasta + toString(processIDS[i]) + ".temp"));
		}

		if (hasCount && dups) {
			if (!File::isBlank(accnos + toString(processIDS[i]) + ".byCount")) {
				ifstream in2;
				File::openInputFile(accnos + toString(processIDS[i]) + ".byCount", in2);

				string name, group;
				while (!in2.eof()) {
					in2 >> name >> group; File::gobble(in2);
					newCount.setAbund(name, group, 0);
				}
				in2.close();
			}
			File::remove(accnos + toString(processIDS[i]) + ".byCount");
		}

	}

	//print new *.pick.count_table
	if (hasCount && dups) { newCount.printTable(countlist); }

	return num;
}
//**********************************************************************************************************************

int ChimeraSlayerCommand::driver(linePair filePos, string outputFName, string filename, string accnos, string fasta, map<string, int>& priority) {

	if (app.isDebug) { LOG(DEBUG) << "filename = " + filename + "\n"; }

	Chimera* chimera;
	if (templatefile != "self") { //you want to run slayer with a reference template
		chimera = new ChimeraSlayer(filename, templatefile, trim, search, ksize, match, mismatch, window, divR, minSimilarity, minCoverage, minBS, minSNP, parents, iters, increment, numwanted, realign, blastlocation, rand());
	}
	else {
		chimera = new ChimeraSlayer(filename, templatefile, trim, priority, search, ksize, match, mismatch, window, divR, minSimilarity, minCoverage, minBS, minSNP, parents, iters, increment, numwanted, realign, blastlocation, rand());
	}

	if (ctrlc_pressed) { delete chimera; return 0; }

	if (chimera->getUnaligned()) { delete chimera; LOG(INFO) << "Your template sequences are different lengths, please correct." << '\n'; ctrlc_pressed = true; return 0; }
	templateSeqsLength = chimera->getLength();

	ofstream out;
	File::openOutputFile(outputFName, out);

	ofstream out2;
	File::openOutputFile(accnos, out2);

	ofstream out3;
	if (trim) { File::openOutputFile(fasta, out3); }

	ifstream inFASTA;
	File::openInputFile(filename, inFASTA);

	inFASTA.seekg(filePos.start);

	if (filePos.start == 0) { chimera->printHeader(out); }

	bool done = false;
	int count = 0;

	while (!done) {

		if (ctrlc_pressed) { delete chimera; out.close(); out2.close(); if (trim) { out3.close(); } inFASTA.close(); return 1; }

		Sequence* candidateSeq = new Sequence(inFASTA);  File::gobble(inFASTA);
		string candidateAligned = candidateSeq->getAligned();

		if (candidateSeq->getName() != "") { //incase there is a commented sequence at the end of a file
			if (candidateSeq->getAligned().length() != templateSeqsLength) {
				LOG(INFO) << candidateSeq->getName() + " is not the same length as the template sequences. Skipping." << '\n';
			}
			else {
				//find chimeras
				chimera->getChimeras(candidateSeq);

				if (ctrlc_pressed) { delete chimera; delete candidateSeq; return 1; }

				//if you are not chimeric, then check each half
				data_results wholeResults = chimera->getResults();

				//determine if we need to split
				bool isChimeric = false;

				if (wholeResults.flag == "yes") {
					string chimeraFlag = "no";
					if ((wholeResults.results[0].bsa >= minBS && wholeResults.results[0].divr_qla_qrb >= divR)
						||
						(wholeResults.results[0].bsb >= minBS && wholeResults.results[0].divr_qlb_qra >= divR)) {
						chimeraFlag = "yes";
					}


					if (chimeraFlag == "yes") {
						if ((wholeResults.results[0].bsa >= minBS) || (wholeResults.results[0].bsb >= minBS)) { isChimeric = true; }
					}
				}

				if ((!isChimeric) && trimera) {

					//split sequence in half by bases
					string leftQuery, rightQuery;
					Sequence tempSeq(candidateSeq->getName(), candidateAligned);
					divideInHalf(tempSeq, leftQuery, rightQuery);

					//run chimeraSlayer on each piece
					Sequence* left = new Sequence(candidateSeq->getName(), leftQuery);
					Sequence* right = new Sequence(candidateSeq->getName(), rightQuery);

					//find chimeras
					chimera->getChimeras(left);
					data_results leftResults = chimera->getResults();

					chimera->getChimeras(right);
					data_results rightResults = chimera->getResults();

					//if either piece is chimeric then report
					Sequence trimmed = chimera->print(out, out2, leftResults, rightResults);
					if (trim) { trimmed.printSequence(out3); }

					delete left; delete right;

				}
				else { //already chimeric
				   //print results
					Sequence trimmed = chimera->print(out, out2);
					if (trim) { trimmed.printSequence(out3); }
				}


			}
			count++;
		}

#if defined (UNIX)
		unsigned long long pos = inFASTA.tellg();
		if ((pos == -1) || (pos >= filePos.end)) { break; }
#else
		if (inFASTA.eof()) { break; }
#endif

		delete candidateSeq;
		//report progress
		if ((count) % 100 == 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(count) + "\n"; }
	}
	//report progress
	if ((count) % 100 != 0) { LOG(SCREENONLY) << "Processing sequence: " + toString(count) + "\n"; }

	int numNoParents = chimera->getNumNoParents();
	if (numNoParents == count) { LOG(WARNING) << "megablast returned 0 potential parents for all your sequences. This could be due to formatdb.exe not being setup properly, please check formatdb.log for errors." << '\n'; }

	out.close();
	out2.close();
	if (trim) { out3.close(); }
	inFASTA.close();
	delete chimera;

	return count;


}
/**************************************************************************************************/

int ChimeraSlayerCommand::createProcesses(string outputFileName, string filename, string accnos, string fasta, map<string, int>& thisPriority) {
	int process = 0;
	int num = 0;
	processIDS.clear();
	bool recalc = false;

	if (app.isDebug) { LOG(DEBUG) << "filename = " + filename + "\n"; }

#if defined (UNIX)
	//loop through and create all the processes you want
	while (process != processors) {
		pid_t pid = fork();

		if (pid > 0) {
			processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
			process++;
		}
		else if (pid == 0) {
			num = driver(lines[process], outputFileName + toString(m->mothurGetpid(process)) + ".temp", filename, accnos + toString(m->mothurGetpid(process)) + ".temp", fasta + toString(m->mothurGetpid(process)) + ".temp", thisPriority);

			//pass numSeqs to parent
			ofstream out;
			string tempFile = outputFileName + toString(m->mothurGetpid(process)) + ".num.temp";
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
		lines.clear();
		vector<unsigned long long> positions = File::divideFile(filename, processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }

		num = 0;
		processIDS.resize(0);
		process = 0;

		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				num = driver(lines[process], outputFileName + toString(m->mothurGetpid(process)) + ".temp", filename, accnos + toString(m->mothurGetpid(process)) + ".temp", fasta + toString(m->mothurGetpid(process)) + ".temp", thisPriority);

				//pass numSeqs to parent
				ofstream out;
				string tempFile = outputFileName + toString(m->mothurGetpid(process)) + ".num.temp";
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


	//force parent to wait until all the processes are done
	for (int i = 0;i < processors;i++) {
		int temp = processIDS[i];
		wait(&temp);
	}

	for (int i = 0; i < processIDS.size(); i++) {
		ifstream in;
		string tempFile = outputFileName + toString(processIDS[i]) + ".num.temp";
		File::openInputFile(tempFile, in);
		if (!in.eof()) { int tempNum = 0;  in >> tempNum; num += tempNum; }
		in.close(); File::remove(tempFile);
	}
#else

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Windows version shared memory, so be careful when passing variables through the slayerData struct. 
	//Above fork() will clone, so memory is separate, but that's not the case with windows, 
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	vector<slayerData*> pDataArray;
	vector<DWORD> dwThreadIdArray(processors);
	vector<HANDLE> hThreadArray(processors);

	//Create processor worker threads.
	for (int i = 0; i < processors; i++) {
		string extension = toString(i) + ".temp";
		slayerData* tempslayer = new slayerData((outputFileName + extension), (fasta + extension), (accnos + extension), filename, templatefile, search, blastlocation, trimera, trim, realign, m, lines[i].start, lines[i].end, ksize, match, mismatch, window, minSimilarity, minCoverage, minBS, minSNP, parents, iters, increment, numwanted, divR, priority, i);
		pDataArray.push_back(tempslayer);
		processIDS.push_back(i);

		//MySlayerThreadFunction is in header. It must be global or static to work with the threads.
		//default security attributes, thread function name, argument to thread function, use default creation flags, returns the thread identifier
		hThreadArray[i] = CreateThread(NULL, 0, MySlayerThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]);
	}

	//Wait until all threads have terminated.
	WaitForMultipleObjects(processors, &(hThreadArray[0]), TRUE, INFINITE);

	//Close all thread handles and free memory allocations.
	for (int i = 0; i < pDataArray.size(); i++) {
		num += pDataArray[i]->count;
		CloseHandle(hThreadArray[i]);
		delete pDataArray[i];
	}
#endif	

	rename((outputFileName + toString(processIDS[0]) + ".temp").c_str(), outputFileName.c_str());
	rename((accnos + toString(processIDS[0]) + ".temp").c_str(), accnos.c_str());
	if (trim) { rename((fasta + toString(processIDS[0]) + ".temp").c_str(), fasta.c_str()); }

	//append output files
	for (int i = 1;i < processIDS.size();i++) {
		File::appendFiles((outputFileName + toString(processIDS[i]) + ".temp"), outputFileName);
		File::remove((outputFileName + toString(processIDS[i]) + ".temp"));

		File::appendFiles((accnos + toString(processIDS[i]) + ".temp"), accnos);
		File::remove((accnos + toString(processIDS[i]) + ".temp"));

		if (trim) {
			File::appendFiles((fasta + toString(processIDS[i]) + ".temp"), fasta);
			File::remove((fasta + toString(processIDS[i]) + ".temp"));
		}
	}


	return num;
}

/**************************************************************************************************/

int ChimeraSlayerCommand::divideInHalf(Sequence querySeq, string& leftQuery, string& rightQuery) {

	string queryUnAligned = querySeq.getUnaligned();
	int numBases = int(queryUnAligned.length() * 0.5);

	string queryAligned = querySeq.getAligned();
	leftQuery = querySeq.getAligned();
	rightQuery = querySeq.getAligned();

	int baseCount = 0;
	int leftSpot = 0;
	for (int i = 0; i < queryAligned.length(); i++) {
		//if you are a base
		if (isalpha(queryAligned[i])) {
			baseCount++;
		}

		//if you have half
		if (baseCount >= numBases) { leftSpot = i; break; } //first half
	}

	//blank out right side
	for (int i = leftSpot; i < leftQuery.length(); i++) { leftQuery[i] = '.'; }

	//blank out left side
	for (int i = 0; i < leftSpot; i++) { rightQuery[i] = '.'; }

	return 0;

}
/**************************************************************************************************/
map<string, int> ChimeraSlayerCommand::sortFastaFile(string fastaFile, string nameFile) {
	map<string, int> nameAbund;

	//read through fastafile and store info
	map<string, string> seqs;
	ifstream in;
	File::openInputFile(fastaFile, in);

	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); return nameAbund; }

		Sequence seq(in); File::gobble(in);
		seqs[seq.getName()] = seq.getAligned();
	}

	in.close();

	//read namefile or countfile
	vector<seqPriorityNode> nameMapCount;
	int error;
	if (hasCount) {
		CountTable ct;
		ct.readTable(nameFile, true, false);

		for (map<string, string>::iterator it = seqs.begin(); it != seqs.end(); it++) {
			int num = ct.getNumSeqs(it->first);
			if (num == 0) { error = 1; }
			else {
				seqPriorityNode temp(num, it->second, it->first);
				nameMapCount.push_back(temp);
			}
		}
	}
	else { error = m->readNames(nameFile, nameMapCount, seqs); }

	if (ctrlc_pressed) { return nameAbund; }

	if (error == 1) { ctrlc_pressed = true; return nameAbund; }
	if (seqs.size() != nameMapCount.size()) { LOG(INFO) << "The number of sequences in your fastafile does not match the number of sequences in your namefile, aborting." << '\n'; ctrlc_pressed = true; return nameAbund; }

	sort(nameMapCount.begin(), nameMapCount.end(), compareSeqPriorityNodes);

	string newFasta = fastaFile + ".temp";
	ofstream out;
	File::openOutputFile(newFasta, out);

	//print new file in order of
	for (int i = 0; i < nameMapCount.size(); i++) {
		out << ">" << nameMapCount[i].name << endl << nameMapCount[i].seq << endl;
		nameAbund[nameMapCount[i].name] = nameMapCount[i].numIdentical;
	}
	out.close();

	rename(newFasta.c_str(), fastaFile.c_str());

	return nameAbund;

}
/**************************************************************************************************/
map<string, int> ChimeraSlayerCommand::sortFastaFile(vector<Sequence>& thisseqs, map<string, string>& nameMap, string newFile) {
	map<string, int> nameAbund;
	vector<seqPriorityNode> nameVector;

	//read through fastafile and store info
	map<string, string> seqs;

	for (int i = 0; i < thisseqs.size(); i++) {

		if (ctrlc_pressed) { return nameAbund; }

		map<string, string>::iterator itNameMap = nameMap.find(thisseqs[i].getName());

		if (itNameMap == nameMap.end()) {
			ctrlc_pressed = true;
			LOG(LOGERROR) << "" + thisseqs[i].getName() + " is in your fastafile, but is not in your namesfile, please correct." << '\n';
		}
		else {
			int num = m->getNumNames(itNameMap->second);

			seqPriorityNode temp(num, thisseqs[i].getAligned(), thisseqs[i].getName());
			nameVector.push_back(temp);
		}
	}

	//sort by num represented
	sort(nameVector.begin(), nameVector.end(), compareSeqPriorityNodes);

	if (ctrlc_pressed) { return nameAbund; }

	if (thisseqs.size() != nameVector.size()) { LOG(INFO) << "The number of sequences in your fastafile does not match the number of sequences in your namefile, aborting." << '\n'; ctrlc_pressed = true; return nameAbund; }

	ofstream out;
	File::openOutputFile(newFile, out);

	//print new file in order of
	for (int i = 0; i < nameVector.size(); i++) {
		out << ">" << nameVector[i].name << endl << nameVector[i].seq << endl;
		nameAbund[nameVector[i].name] = nameVector[i].numIdentical;
	}
	out.close();

	return nameAbund;

}
/**************************************************************************************************/
int ChimeraSlayerCommand::sortFastaFile(vector<Sequence>& thisseqs, map<string, int>& countMap, string newFile) {
	vector<seqPriorityNode> nameVector;

	//read through fastafile and store info
	map<string, string> seqs;

	for (int i = 0; i < thisseqs.size(); i++) {

		if (ctrlc_pressed) { return 0; }

		map<string, int>::iterator itCountMap = countMap.find(thisseqs[i].getName());

		if (itCountMap == countMap.end()) {
			ctrlc_pressed = true;
			LOG(LOGERROR) << "" + thisseqs[i].getName() + " is in your fastafile, but is not in your count file, please correct." << '\n';
		}
		else {
			seqPriorityNode temp(itCountMap->second, thisseqs[i].getAligned(), thisseqs[i].getName());
			nameVector.push_back(temp);
		}
	}

	//sort by num represented
	sort(nameVector.begin(), nameVector.end(), compareSeqPriorityNodes);

	if (ctrlc_pressed) { return 0; }

	if (thisseqs.size() != nameVector.size()) { LOG(INFO) << "The number of sequences in your fastafile does not match the number of sequences in your count file, aborting." << '\n'; ctrlc_pressed = true; return 0; }

	ofstream out;
	File::openOutputFile(newFile, out);

	//print new file in order of
	for (int i = 0; i < nameVector.size(); i++) {
		out << ">" << nameVector[i].name << endl << nameVector[i].seq << endl;
	}
	out.close();

	return 0;

}
/**************************************************************************************************/

