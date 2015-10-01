/*
 *  chimerauchimecommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/13/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "chimerauchimecommand.h"
#include "deconvolutecommand.h"
 //#include "uc.h"
#include "sequence.hpp"
#include "referencedb.h"
#include "systemcommand.h"
#include <thread>
#include <iterator>
#include "utility.h"

//**********************************************************************************************************************
vector<string> ChimeraUchimeCommand::setParameters() {
	try {
		nkParameters.add(new ReferenceFileParameter(settings, "reference", true, true));
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "chimera-accnos", false, true, true); parameters.push_back(pfasta);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "none", "", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "none", "none", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none", "", false, false, true); parameters.push_back(pgroup);
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new StringParameter("strand", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));
		nkParameters.add(new NumberParameter("abskew", -INFINITY, INFINITY, 1.9, false, false));
		CommandParameter pchimealns("chimealns", "Boolean", "", "F", "", "", "", "alns", false, false); parameters.push_back(pchimealns);
		nkParameters.add(new NumberParameter("minh", -INFINITY, INFINITY, 0.3, false, false));
		nkParameters.add(new NumberParameter("mindiv", -INFINITY, INFINITY, 0.5, false, false));
		nkParameters.add(new NumberParameter("xn", -INFINITY, INFINITY, 8.0, false, false));
		nkParameters.add(new NumberParameter("dn", -INFINITY, INFINITY, 1.4, false, false));
		nkParameters.add(new NumberParameter("xa", -INFINITY, INFINITY, 1, false, false));
		nkParameters.add(new NumberParameter("chunks", -INFINITY, INFINITY, 4, false, false));
		nkParameters.add(new NumberParameter("minchunk", -INFINITY, INFINITY, 64, false, false));
		nkParameters.add(new NumberParameter("idsmoothwindow", -INFINITY, INFINITY, 32, false, false));
		nkParameters.add(new BooleanParameter("dereplicate", false, false, false));

		//CommandParameter pminsmoothid("minsmoothid", "Number", "", "0.95", "", "", "",false,false); parameters.push_back(pminsmoothid);
		nkParameters.add(new NumberParameter("maxp", -INFINITY, INFINITY, 2, false, false));
		nkParameters.add(new BooleanParameter("skipgaps", true, false, false));
		nkParameters.add(new BooleanParameter("skipgaps2", true, false, false));
		nkParameters.add(new NumberParameter("minlen", -INFINITY, INFINITY, 10, false, false));
		nkParameters.add(new NumberParameter("maxlen", -INFINITY, INFINITY, 10000, false, false));
		nkParameters.add(new BooleanParameter("ucl", false, false, false));
		nkParameters.add(new NumberParameter("queryfract", -INFINITY, INFINITY, 0.5, false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraUchimeCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraUchimeCommand::getHelpString() {
	try {
		string helpString = "The chimera.uchime command reads a fastafile and referencefile and outputs potentially chimeric sequences.\n"
			"This command is a wrapper for uchime written by Robert C. Edgar.\n"
			"The chimera.uchime command parameters are fasta, name, count, reference, processors, dereplicate, abskew, chimealns, minh, mindiv, xn, dn, xa, chunks, minchunk, idsmoothwindow, minsmoothid, maxp, skipgaps, skipgaps2, minlen, maxlen, ucl, strand and queryfact.\n"
			"The fasta parameter allows you to enter the fasta file containing your potentially chimeric sequences, and is required, unless you have a valid current fasta file. \n"
			"The name parameter allows you to provide a name file, if you are using template=self. \n"
			"The count parameter allows you to provide a count file, if you are using template=self. When you use a count file with group info and dereplicate=T, mothur will create a *.pick.count_table file containing seqeunces after chimeras are removed. \n"
			"You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amazon.fasta \n"
			"The group parameter allows you to provide a group file. The group file can be used with a namesfile and reference=self. When checking sequences, only sequences from the same group as the query sequence will be used as the reference. \n"
			"If the dereplicate parameter is false, then if one group finds the seqeunce to be chimeric, then all groups find it to be chimeric, default=f.\n"
			"The reference parameter allows you to enter a reference file containing known non-chimeric sequences, and is required. You may also set template=self, in this case the abundant sequences will be used as potential parents. \n"
			"The processors parameter allows you to specify how many processors you would like to use.  The default is 1. \n"
			"The abskew parameter can only be used with template=self. Minimum abundance skew. Default 1.9. Abundance skew is: min [ abund(parent1), abund(parent2) ] / abund(query).\n"
			"The chimealns parameter allows you to indicate you would like a file containing multiple alignments of query sequences to parents in human readable format. Alignments show columns with differences that support or contradict a chimeric model.\n"
			"The minh parameter - mininum score to report chimera. Default 0.3. Values from 0.1 to 5 might be reasonable. Lower values increase sensitivity but may report more false positives. If you decrease xn you may need to increase minh, and vice versa.\n"
			"The mindiv parameter - minimum divergence ratio, default 0.5. Div ratio is 100%% - %%identity between query sequence and the closest candidate for being a parent. If you don't care about very close chimeras, then you could increase mindiv to, say, 1.0 or 2.0, and also decrease minh, say to 0.1, to increase sensitivity. How well this works will depend on your data. Best is to tune parameters on a good benchmark.\n"
			"The xn parameter - weight of a no vote. Default 8.0. Decreasing this weight to around 3 or 4 may give better performance on denoised data.\n"
			"The dn parameter - pseudo-count prior on number of no votes. Default 1.4. Probably no good reason to change this unless you can retune to a good benchmark for your data. Reasonable values are probably in the range from 0.2 to 2.\n"
			"The xa parameter - weight of an abstain vote. Default 1. So far, results do not seem to be very sensitive to this parameter, but if you have a good training set might be worth trying. Reasonable values might range from 0.1 to 2.\n"
			"The chunks parameter is the number of chunks to extract from the query sequence when searching for parents. Default 4.\n"
			"The minchunk parameter is the minimum length of a chunk. Default 64.\n"
			"The idsmoothwindow parameter is the length of id smoothing window. Default 32.\n";
		//helpString += "The minsmoothid parameter - minimum factional identity over smoothed window of candidate parent. Default 0.95.\n"
		"The maxp parameter - maximum number of candidate parents to consider. Default 2. In tests so far, increasing maxp gives only a very small improvement in sensivity but tends to increase the error rate quite a bit.\n"
			"The skipgaps parameter controls how gapped columns affect counting of diffs. If skipgaps is set to T, columns containing gaps do not found as diffs. Default = T.\n"
			"The skipgaps2 parameter controls how gapped columns affect counting of diffs. If skipgaps2 is set to T, if column is immediately adjacent to a column containing a gap, it is not counted as a diff. Default = T.\n"
			"The minlen parameter is the minimum unaligned sequence length. Defaults 10. Applies to both query and reference sequences.\n"
			"The maxlen parameter is the maximum unaligned sequence length. Defaults 10000. Applies to both query and reference sequences.\n"
			"The ucl parameter - use local-X alignments. Default is global-X or false. On tests so far, global-X is always better; this option is retained because it just might work well on some future type of data.\n"
			"The queryfract parameter - minimum fraction of the query sequence that must be covered by a local-X alignment. Default 0.5. Applies only when ucl is true.\n"
			"The chimera.uchime command should be in the following format: \n"
			"chimera.uchime(fasta=yourFastaFile, reference=yourTemplate) \n"
			"Example: chimera.uchime(fasta=AD.align, reference=silva.gold.align) \n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraUchimeCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraUchimeCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "chimera") { pattern = "[filename],[tag],uchime.chimeras"; }
	else if (type == "accnos") { pattern = "[filename],[tag],uchime.accnos"; }
	else if (type == "alns") { pattern = "[filename],[tag],uchime.alns"; }
	else if (type == "count") { pattern = "[filename],[tag],uchime.pick.count_table"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ChimeraUchimeCommand::ChimeraUchimeCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["alns"] = tempOutNames;
		outputTypes["count"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ChimeraUchimeCommand, ChimeraUchimeCommand";
		exit(1);
	}
}
//***************************************************************************************************************
ChimeraUchimeCommand::ChimeraUchimeCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false; hasName = false; hasCount = false;
	ReferenceDB& rdb = ReferenceDB::getInstance();

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter("chimera.uchime");
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
		outputTypes["alns"] = tempOutNames;
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


		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		string path;
		it = parameters.find("reference");
		//user has given a template file
		if (it != parameters.end()) {
			if (it->second == "self") { templatefile = "self"; }
			else {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["reference"] = inputDir + it->second; }

				templatefile = validParameter.validFile(parameters, "reference", true);
				if (templatefile == "not open") { abort = true; }
				else if (templatefile == "not found") { //check for saved reference sequences
					if (rdb.getSavedReference() != "") {
						templatefile = rdb.getSavedReference();
						LOG(INFO) << '\n' << "Using sequences from " + rdb.getSavedReference() + "." << '\n';
					}
					else {
						LOG(LOGERROR) << "You don't have any saved reference sequences and the reference parameter is a required." << '\n';
						abort = true;
					}
				}
			}
		}
		else if (hasName) { templatefile = "self"; }
		else if (hasCount) { templatefile = "self"; }
		else {
			if (rdb.getSavedReference() != "") {
				templatefile = rdb.getSavedReference();
				LOG(INFO) << '\n' << "Using sequences from " + rdb.getSavedReference() + "." << '\n';
			}
			else {
				LOG(LOGERROR) << "You don't have any saved reference sequences and the reference parameter is a required.";
				LOG(INFO) << "";
				templatefile = ""; abort = true;
			}
		}

		string temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		abskew = validParameter.validFile(parameters, "abskew", false);	if (abskew == "not found") { useAbskew = false;  abskew = "1.9"; }
		else { useAbskew = true; }
		if (useAbskew && templatefile != "self") { LOG(INFO) << "The abskew parameter is only valid with template=self, ignoring." << '\n'; useAbskew = false; }

		temp = validParameter.validFile(parameters, "chimealns", false);			if (temp == "not found") { temp = "f"; }
		chimealns = m->isTrue(temp);

		minh = validParameter.validFile(parameters, "minh", false);						if (minh == "not found") { useMinH = false; minh = "0.3"; }
		else { useMinH = true; }
		mindiv = validParameter.validFile(parameters, "mindiv", false);					if (mindiv == "not found") { useMindiv = false; mindiv = "0.5"; }
		else { useMindiv = true; }
		xn = validParameter.validFile(parameters, "xn", false);							if (xn == "not found") { useXn = false; xn = "8.0"; }
		else { useXn = true; }
		dn = validParameter.validFile(parameters, "dn", false);							if (dn == "not found") { useDn = false; dn = "1.4"; }
		else { useDn = true; }
		xa = validParameter.validFile(parameters, "xa", false);							if (xa == "not found") { useXa = false; xa = "1"; }
		else { useXa = true; }
		chunks = validParameter.validFile(parameters, "chunks", false);					if (chunks == "not found") { useChunks = false; chunks = "4"; }
		else { useChunks = true; }
		minchunk = validParameter.validFile(parameters, "minchunk", false);				if (minchunk == "not found") { useMinchunk = false; minchunk = "64"; }
		else { useMinchunk = true; }
		idsmoothwindow = validParameter.validFile(parameters, "idsmoothwindow", false);	if (idsmoothwindow == "not found") { useIdsmoothwindow = false; idsmoothwindow = "32"; }
		else { useIdsmoothwindow = true; }
		//minsmoothid = validParameter.validFile(parameters, "minsmoothid", false);		if (minsmoothid == "not found")		{ useMinsmoothid = false; minsmoothid = "0.95";		}	else{ useMinsmoothid = true;	}
		maxp = validParameter.validFile(parameters, "maxp", false);						if (maxp == "not found") { useMaxp = false; maxp = "2"; }
		else { useMaxp = true; }
		minlen = validParameter.validFile(parameters, "minlen", false);					if (minlen == "not found") { useMinlen = false; minlen = "10"; }
		else { useMinlen = true; }
		maxlen = validParameter.validFile(parameters, "maxlen", false);					if (maxlen == "not found") { useMaxlen = false; maxlen = "10000"; }
		else { useMaxlen = true; }

		strand = validParameter.validFile(parameters, "strand", false);	if (strand == "not found") { strand = ""; }

		temp = validParameter.validFile(parameters, "ucl", false);						if (temp == "not found") { temp = "f"; }
		ucl = m->isTrue(temp);

		queryfract = validParameter.validFile(parameters, "queryfract", false);			if (queryfract == "not found") { useQueryfract = false; queryfract = "0.5"; }
		else { useQueryfract = true; }
		if (!ucl && useQueryfract) { LOG(INFO) << "queryfact may only be used when ucl=t, ignoring." << '\n'; useQueryfract = false; }

		temp = validParameter.validFile(parameters, "skipgaps", false);					if (temp == "not found") { temp = "t"; }
		skipgaps = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "skipgaps2", false);				if (temp == "not found") { temp = "t"; }
		skipgaps2 = m->isTrue(temp);


		temp = validParameter.validFile(parameters, "dereplicate", false);
		if (temp == "not found") { temp = "false"; }
		dups = m->isTrue(temp);


		if (hasName && (templatefile != "self")) { LOG(INFO) << "You have provided a namefile and the reference parameter is not set to self. I am not sure what reference you are trying to use, aborting." << '\n'; abort = true; }
		if (hasCount && (templatefile != "self")) { LOG(INFO) << "You have provided a countfile and the reference parameter is not set to self. I am not sure what reference you are trying to use, aborting." << '\n'; abort = true; }
		if (hasGroup && (templatefile != "self")) { LOG(INFO) << "You have provided a group file and the reference parameter is not set to self. I am not sure what reference you are trying to use, aborting." << '\n'; abort = true; }

		//look for uchime exe
		path = m->argv;
		string tempPath = path;
		for (int i = 0; i < path.length(); i++) { tempPath[i] = tolower(path[i]); }
		path = path.substr(0, (tempPath.find_last_of('m')));

		string uchimeCommand;
#if defined (UNIX)
		uchimeCommand = path + "uchime";	//	format the database, -o option gives us the ability
		if (app.isDebug) {
			LOG(DEBUG) << "Uchime location using \"which uchime\" = ";
			SystemCommand newCommand("which uchime"); LOG(DEBUG) << '\n';
			newCommand.execute();
			LOG(DEBUG) << "Mothur's location using \"which mothur\" = ";
			newCommand = SystemCommand("which mothur"); LOG(DEBUG) << '\n';
			newCommand.execute();
		}
#else
		uchimeCommand = path + "uchime.exe";
#endif

		//test to make sure uchime exists
		ifstream in;
		uchimeCommand = m->getFullPathName(uchimeCommand);
		int ableToOpen = File::openInputFile(uchimeCommand, in, "no error"); in.close();
		if (ableToOpen == 1) {
			LOG(INFO) << uchimeCommand + " file does not exist. Checking path... \n";
			//check to see if uchime is in the path??

			string uLocation = m->findProgramPath("uchime");


			ifstream in2;
#if defined (UNIX)
			ableToOpen = File::openInputFile(uLocation, in2, "no error"); in2.close();
#else
			ableToOpen = File::openInputFile((uLocation + ".exe"), in2, "no error"); in2.close();
#endif

			if (ableToOpen == 1) { LOG(LOGERROR) << "" + uLocation + " file does not exist. mothur requires the uchime executable." << '\n'; abort = true; }
			else { LOG(INFO) << "Found uchime in your path, using " + uLocation + "\n";uchimeLocation = uLocation; }
		}
		else { uchimeLocation = uchimeCommand; }

		uchimeLocation = m->getFullPathName(uchimeLocation);
	}
}
//***************************************************************************************************************

int ChimeraUchimeCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		LOG(INFO) << "\nuchime by Robert C. Edgar\nhttp://drive5.com/uchime\nThis code is donated to the public domain.\n\n";

		for (int s = 0; s < fastaFileNames.size(); s++) {

			LOG(INFO) << "Checking sequences from " + fastaFileNames[s] + " ..." << '\n';

			int start = time(NULL);
			string nameFile = "";
			if (outputDir == "") { outputDir = File::getPath(fastaFileNames[s]); }//if user entered a file with a path then preserve it				
			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastaFileNames[s]));
			variables["[tag]"] = "denovo";
			if (templatefile != "self") { variables["[tag]"] = "ref"; }
			string outputFileName = getOutputFileName("chimera", variables);
			string accnosFileName = getOutputFileName("accnos", variables);
			string alnsFileName = getOutputFileName("alns", variables);
			string newFasta = File::getRootName(fastaFileNames[s]) + "temp";
			string newCountFile = "";

			//you provided a groupfile
			string groupFile = "";
			bool hasGroup = false;
			if (groupFileNames.size() != 0) { groupFile = groupFileNames[s]; hasGroup = true; }
			else if (hasCount) {
				CountTable ct;
				if (ct.testGroups(nameFileNames[s])) { hasGroup = true; }
				variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(nameFileNames[s]));
				newCountFile = getOutputFileName("count", variables);
			}

			if ((templatefile == "self") && (!hasGroup)) { //you want to run uchime with a template=self and no groups

				if (processors != 1) { LOG(INFO) << "When using template=self, mothur can only use 1 processor, continuing." << '\n'; processors = 1; }
				if (nameFileNames.size() != 0) { //you provided a namefile and we don't need to create one
					nameFile = nameFileNames[s];
				}
				else { nameFile = getNamesFile(fastaFileNames[s]); }

				map<string, string> seqs;
				readFasta(fastaFileNames[s], seqs);  if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

				//read namefile
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
				else {
					error = m->readNames(nameFile, nameMapCount, seqs); if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }
				}
				if (error == 1) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }
				if (seqs.size() != nameMapCount.size()) { LOG(INFO) << "The number of sequences in your fastafile does not match the number of sequences in your namefile, aborting." << '\n'; for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

				printFile(nameMapCount, newFasta);
				fastaFileNames[s] = newFasta;
			}

			if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

			if (hasGroup) {
				if (nameFileNames.size() != 0) { //you provided a namefile and we don't need to create one
					nameFile = nameFileNames[s];
				}
				else { nameFile = getNamesFile(fastaFileNames[s]); }

				//Parse sequences by group
				vector<string> groups;
				map<string, string> uniqueNames;
				if (hasCount) {
					sparser = make_shared<SequenceCountParser>(nameFile, fastaFileNames[s]);
				}
				else {
					sparser = make_shared<SequenceParser>(groupFile, fastaFileNames[s], nameFile);
				}
				groups = sparser->getNamesOfGroups();
				uniqueNames = sparser->getAllSeqsMap();

				if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

				//clears files
				ofstream out, out1, out2;
				File::openOutputFile(outputFileName, out); out.close();
				File::openOutputFile(accnosFileName, out1); out1.close();
				if (chimealns) { File::openOutputFile(alnsFileName, out2); out2.close(); }
				int totalSeqs = 0;

				if (processors == 1) {
					totalSeqs = driverGroups(outputFileName, newFasta, accnosFileName, alnsFileName, newCountFile, 0, groups.size(), groups);

					if (hasCount && dups) {
						CountTable c; c.readTable(nameFile, true, false);
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
				else { totalSeqs = createProcessesGroups(outputFileName, newFasta, accnosFileName, alnsFileName, newCountFile, groups, nameFile, groupFile, fastaFileNames[s]); }

				if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }


				if (!dups) {
					int totalChimeras = deconvoluteResults(uniqueNames, outputFileName, accnosFileName, alnsFileName);

					LOG(INFO) << '\n' << "It took " + toString(time(NULL) - start) + " secs to check " + toString(totalSeqs) + " sequences. " + toString(totalChimeras) + " chimeras were found." << '\n';
					LOG(INFO) << "The number of sequences checked may be larger than the number of unique sequences because some sequences are found in several samples." << '\n';
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

				if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

			}
			else {
				if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); }  return 0; }

				int numSeqs = 0;
				int numChimeras = 0;

				if (processors == 1) { numSeqs = driver(outputFileName, fastaFileNames[s], accnosFileName, alnsFileName, numChimeras); }
				else { numSeqs = createProcesses(outputFileName, fastaFileNames[s], accnosFileName, alnsFileName, numChimeras); }

				//add headings
				ofstream out;
				File::openOutputFile(outputFileName + ".temp", out);
				out << "Score\tQuery\tParentA\tParentB\tIdQM\tIdQA\tIdQB\tIdAB\tIdQT\tLY\tLN\tLA\tRY\tRN\tRA\tDiv\tYN\n";
				out.close();

				File::appendFiles(outputFileName, outputFileName + ".temp");
				File::remove(outputFileName); rename((outputFileName + ".temp").c_str(), outputFileName.c_str());

				if (ctrlc_pressed) { for (int j = 0; j < outputNames.size(); j++) { File::remove(outputNames[j]); } return 0; }

				//remove file made for uchime
				if (templatefile == "self") { File::remove(fastaFileNames[s]); }

				LOG(INFO) << '\n' << "It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences. " + toString(numChimeras) + " chimeras were found." << '\n';
			}

			outputNames.push_back(outputFileName); outputTypes["chimera"].push_back(outputFileName);
			outputNames.push_back(accnosFileName); outputTypes["accnos"].push_back(accnosFileName);
			if (chimealns) { outputNames.push_back(alnsFileName); outputTypes["alns"].push_back(alnsFileName); }
		}

		//set accnos file as new current accnosfile
		string current = "";
		itTypes = outputTypes.find("accnos");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("accnos", current); }
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
		LOG(FATAL) << e.what() << " in ChimeraUchimeCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************
int ChimeraUchimeCommand::deconvoluteResults(map<string, string>& uniqueNames, string outputFileName, string accnosFileName, string alnsFileName) {
	map<string, string>::iterator itUnique;
	int total = 0;

	ofstream out2;
	File::openOutputFile(accnosFileName + ".temp", out2);

	string name;
	set<string> namesInFile; //this is so if a sequence is found to be chimera in several samples we dont write it to the results file more than once
	set<string>::iterator itNames;
	set<string> chimerasInFile;
	set<string>::iterator itChimeras;

	if (!File::isBlank(accnosFileName)) {
		//edit accnos file
		ifstream in2;
		File::openInputFile(accnosFileName, in2);

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
	}
	out2.close();

	File::remove(accnosFileName);
	rename((accnosFileName + ".temp").c_str(), accnosFileName.c_str());



	//edit chimera file
	ifstream in;
	File::openInputFile(outputFileName, in);

	ofstream out;
	File::openOutputFile(outputFileName + ".temp", out); out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);
	out << "Score\tQuery\tParentA\tParentB\tIdQM\tIdQA\tIdQB\tIdAB\tIdQT\tLY\tLN\tLA\tRY\tRN\tRA\tDiv\tYN\n";

	float temp1;
	string parent1, parent2, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10, temp11, temp12, temp13, flag;
	name = "";
	namesInFile.clear();
	//assumptions - in file each read will always look like - if uchime source is updated, revisit this code.
	/*										1	2	3	4	5	6	7	8	9	10	11	12	13	14	15
	 0.000000	F11Fcsw_33372/ab=18/		*	*	*	*	*	*	*	*	*	*	*	*	*	*	N
	 0.018300	F11Fcsw_14980/ab=16/		F11Fcsw_1915/ab=35/	F11Fcsw_6032/ab=42/	79.9	78.7	78.2	78.7	79.2	3	0	5	11	10	20	1.46	N
	*/

	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); out.close(); File::remove((outputFileName + ".temp")); return 0; }

		bool print = false;
		in >> temp1;	File::gobble(in);
		in >> name;		File::gobble(in);
		in >> parent1;	File::gobble(in);
		in >> parent2;	File::gobble(in);
		in >> temp2 >> temp3 >> temp4 >> temp5 >> temp6 >> temp7 >> temp8 >> temp9 >> temp10 >> temp11 >> temp12 >> temp13 >> flag;
		File::gobble(in);

		//parse name - name will look like U68590/ab=1/
		string restOfName = "";
		int pos = name.find_first_of('/');
		if (pos != string::npos) {
			restOfName = name.substr(pos);
			name = name.substr(0, pos);
		}

		//find unique name
		itUnique = uniqueNames.find(name);

		if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find " + name + "." << '\n'; ctrlc_pressed = true; }
		else {
			name = itUnique->second;
			//is this name already in the file
			itNames = namesInFile.find((name));

			if (itNames == namesInFile.end()) { //no not in file
				if (flag == "N") { //are you really a no??
					//is this sequence really not chimeric??
					itChimeras = chimerasInFile.find(name);

					//then you really are a no so print, otherwise skip
					if (itChimeras == chimerasInFile.end()) { print = true; }
				}
				else { print = true; }
			}
		}

		if (print) {
			out << temp1 << '\t' << name << restOfName << '\t';
			namesInFile.insert(name);

			//parse parent1 names
			if (parent1 != "*") {
				restOfName = "";
				pos = parent1.find_first_of('/');
				if (pos != string::npos) {
					restOfName = parent1.substr(pos);
					parent1 = parent1.substr(0, pos);
				}

				itUnique = uniqueNames.find(parent1);
				if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find parentA " + parent1 + "." << '\n'; ctrlc_pressed = true; }
				else { out << itUnique->second << restOfName << '\t'; }
			}
			else { out << parent1 << '\t'; }

			//parse parent2 names
			if (parent2 != "*") {
				restOfName = "";
				pos = parent2.find_first_of('/');
				if (pos != string::npos) {
					restOfName = parent2.substr(pos);
					parent2 = parent2.substr(0, pos);
				}

				itUnique = uniqueNames.find(parent2);
				if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing chimera results. Cannot find parentB " + parent2 + "." << '\n'; ctrlc_pressed = true; }
				else { out << itUnique->second << restOfName << '\t'; }
			}
			else { out << parent2 << '\t'; }

			out << temp2 << '\t' << temp3 << '\t' << temp4 << '\t' << temp5 << '\t' << temp6 << '\t' << temp7 << '\t' << temp8 << '\t' << temp9 << '\t' << temp10 << '\t' << temp11 << '\t' << temp12 << temp13 << '\t' << flag << endl;
		}
	}
	in.close();
	out.close();

	File::remove(outputFileName);
	rename((outputFileName + ".temp").c_str(), outputFileName.c_str());


	//edit anls file
	//assumptions - in file each read will always look like - if uchime source is updated, revisit this code.
	/*
	 ------------------------------------------------------------------------
	 Query   (  179 nt) F21Fcsw_11639/ab=591/
	 ParentA (  179 nt) F11Fcsw_6529/ab=1625/
	 ParentB (  181 nt) F21Fcsw_12128/ab=1827/

	 A     1 AAGgAAGAtTAATACaagATGgCaTCatgAGtccgCATgTtcAcatGATTAAAG--gTaTtcCGGTagacGATGGGGATG 78
	 Q     1 AAGTAAGACTAATACCCAATGACGTCTCTAGAAGACATCTGAAAGAGATTAAAG--ATTTATCGGTGATGGATGGGGATG 78
	 B     1 AAGgAAGAtTAATcCaggATGggaTCatgAGttcACATgTccgcatGATTAAAGgtATTTtcCGGTagacGATGGGGATG 80
	 Diffs      N    N    A N?N   N N  NNN  N?NB   N ?NaNNN          B B NN    NNNN
	 Votes      0    0    + 000   0 0  000  000+   0 00!000            + 00    0000
	 Model   AAAAAAAAAAAAAAAAAAAAAAxBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

	 A    79 CGTtccATTAGaTaGTaGGCGGGGTAACGGCCCACCtAGtCttCGATggaTAGGGGTTCTGAGAGGAAGGTCCCCCACAT 158
	 Q    79 CGTCTGATTAGCTTGTTGGCGGGGTAACGGCCCACCAAGGCAACGATCAGTAGGGGTTCTGAGAGGAAGGTCCCCCACAT 158
	 B    81 CGTtccATTAGaTaGTaGGCGGGGTAACGGCCCACCtAGtCAACGATggaTAGGGGTTCTGAGAGGAAGGTCCCCCACAT 160
	 Diffs      NNN     N N  N                   N  N BB    NNN
	 Votes      000     0 0  0                   0  0 ++    000
	 Model   BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

	 A   159 TGGAACTGAGACACGGTCCAA 179
	 Q   159 TGGAACTGAGACACGGTCCAA 179
	 B   161 TGGAACTGAGACACGGTCCAA 181
	 Diffs
	 Votes
	 Model   BBBBBBBBBBBBBBBBBBBBB

	 Ids.  QA 76.6%, QB 77.7%, AB 93.7%, QModel 78.9%, Div. +1.5%
	 Diffs Left 7: N 0, A 6, Y 1 (14.3%); Right 35: N 1, A 30, Y 4 (11.4%), Score 0.0047
	*/
	if (chimealns) {
		ifstream in3;
		File::openInputFile(alnsFileName, in3);

		ofstream out3;
		File::openOutputFile(alnsFileName + ".temp", out3); out3.setf(ios::fixed, ios::floatfield); out3.setf(ios::showpoint);

		name = "";
		namesInFile.clear();
		string line = "";

		while (!in3.eof()) {
			if (ctrlc_pressed) { in3.close(); out3.close(); File::remove(outputFileName); File::remove((accnosFileName)); File::remove((alnsFileName + ".temp")); return 0; }

			line = "";
			line = File::getline(in3);
			string temp = "";

			if (line != "") {
				istringstream iss(line);
				iss >> temp;

				//are you a name line
				if ((temp == "Query") || (temp == "ParentA") || (temp == "ParentB")) {
					int spot = 0;
					for (int i = 0; i < line.length(); i++) {
						spot = i;
						if (line[i] == ')') { break; }
						else { out3 << line[i]; }
					}

					if (spot == (line.length() - 1)) { LOG(LOGERROR) << "could not line sequence name in line " + line + "." << '\n'; ctrlc_pressed = true; }
					else if ((spot + 2) > (line.length() - 1)) { LOG(LOGERROR) << "could not line sequence name in line " + line + "." << '\n'; ctrlc_pressed = true; }
					else {
						out << line[spot] << line[spot + 1];

						name = line.substr(spot + 2);

						//parse name - name will either look like U68590/ab=1/ or U68590
						string restOfName = "";
						int pos = name.find_first_of('/');
						if (pos != string::npos) {
							restOfName = name.substr(pos);
							name = name.substr(0, pos);
						}

						//find unique name
						itUnique = uniqueNames.find(name);

						if (itUnique == uniqueNames.end()) { LOG(LOGERROR) << "trouble parsing alns results. Cannot find " + name + "." << '\n';ctrlc_pressed = true; }
						else {
							//only limit repeats on query names
							if (temp == "Query") {
								itNames = namesInFile.find((itUnique->second));

								if (itNames == namesInFile.end()) {
									out << itUnique->second << restOfName << endl;
									namesInFile.insert((itUnique->second));
								}
							}
							else { out << itUnique->second << restOfName << endl; }
						}

					}

				}
				else { //not need to alter line
					out3 << line << endl;
				}
			}
			else { out3 << endl; }
		}
		in3.close();
		out3.close();

		File::remove(alnsFileName);
		rename((alnsFileName + ".temp").c_str(), alnsFileName.c_str());
	}

	return total;
}
//**********************************************************************************************************************
int ChimeraUchimeCommand::printFile(vector<seqPriorityNode>& nameMapCount, string filename) {

	sort(nameMapCount.begin(), nameMapCount.end(), compareSeqPriorityNodes);

	ofstream out;
	File::openOutputFile(filename, out);

	//print new file in order of
	for (int i = 0; i < nameMapCount.size(); i++) {
		out << ">" << nameMapCount[i].name << "/ab=" << nameMapCount[i].numIdentical << "/" << endl << nameMapCount[i].seq << endl;
	}
	out.close();

	return 0;
}
//**********************************************************************************************************************
int ChimeraUchimeCommand::readFasta(string filename, map<string, string>& seqs) {
	//create input file for uchime
	//read through fastafile and store info
	ifstream in;
	File::openInputFile(filename, in);

	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); return 0; }

		Sequence seq(in); File::gobble(in);
		seqs[seq.getName()] = seq.getAligned();
	}
	in.close();

	return 0;
}
//**********************************************************************************************************************

string ChimeraUchimeCommand::getNamesFile(string& inputFile) {
	string nameFile = "";

	LOG(INFO) << std::endl << "No namesfile given, running unique.seqs command to generate one." << '\n' << '\n';

	//use unique.seqs to create new name and fastafile
	string inputString = "fasta=" + inputFile;
	LOG(INFO) << "/******************************************/" << '\n';
	LOG(INFO) << "Running command: unique.seqs(" + inputString + ")" << '\n';
	m->mothurCalling = true;

	DeconvoluteCommand uniqueCommand(inputString);
	uniqueCommand.execute();


	map<string, vector<string> > filenames = uniqueCommand.getOutputFiles();

	m->mothurCalling = false;
	LOG(INFO) << "/******************************************/" << '\n';

	nameFile = filenames["name"][0];
	inputFile = filenames["fasta"][0];

	return nameFile;
}
//**********************************************************************************************************************
void ChimeraUchimeCommand::driverGroupsWithCount(string outputFName, string filename, string accnos, string alns, string countlist, unsigned long long start, unsigned long long end, vector<string> groups, int& numSeqs) {
	numSeqs = driverGroups(outputFName, filename, accnos, alns, countlist, start, end, groups);
}

int ChimeraUchimeCommand::driverGroups(string outputFName, string filename, string accnos, string alns, string countlist, unsigned long long start, unsigned long long end, vector<string> groups) {

	int totalSeqs = 0;
	int numChimeras = 0;


	ofstream outCountList;
	if (hasCount && dups) { File::openOutputFile(countlist, outCountList); }

	for (int i = start; i < end; i++) {
		int start = time(NULL);	 if (ctrlc_pressed) { outCountList.close(); File::remove(countlist); return 0; }

		int error = sparser->getSeqs(groups[i], filename, true);
		if ((error == 1) || ctrlc_pressed) { return 0; }

		int numSeqs = driver((outputFName + groups[i]), filename, (accnos + groups[i]), (alns + groups[i]), numChimeras);
		totalSeqs += numSeqs;

		if (ctrlc_pressed) { return 0; }

		//remove file made for uchime
		if (!app.isDebug) { File::remove(filename); }
		else { LOG(DEBUG) << "saving file: " + filename + ".\n"; }

		//if we provided a count file with group info and set dereplicate=t, then we want to create a *.pick.count_table
		//This table will zero out group counts for seqs determined to be chimeric by that group.
		if (dups) {
			if (!File::isBlank(accnos + groups[i])) {
				ifstream in;
				File::openInputFile(accnos + groups[i], in);
				string name;
				if (hasCount) {
					while (!in.eof()) {
						in >> name; File::gobble(in);
						outCountList << name << '\t' << groups[i] << endl;
					}
					in.close();
				}
				else {
					map<string, string> thisnamemap = dynamic_pointer_cast<SequenceParser>(sparser)->getNameMap(groups[i]);
					map<string, string>::iterator itN;
					ofstream out;
					File::openOutputFile(accnos + groups[i] + ".temp", out);
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
					m->renameFile(accnos + groups[i] + ".temp", accnos + groups[i]);
				}

			}
		}

		//append files
		File::appendFiles((outputFName + groups[i]), outputFName); File::remove((outputFName + groups[i]));
		File::appendFiles((accnos + groups[i]), accnos); File::remove((accnos + groups[i]));
		if (chimealns) { File::appendFiles((alns + groups[i]), alns); File::remove((alns + groups[i])); }

		LOG(INFO) << '\n' << "It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences from group " + groups[i] + "." << '\n';
	}

	if (hasCount && dups) { outCountList.close(); }

	return totalSeqs;

}
//**********************************************************************************************************************

void ChimeraUchimeCommand::driverWithCount(string outputFName, string filename, string accnos, string alns, int& numChimeras, int& numSeqs) {
	numSeqs = driver(outputFName, filename, accnos, alns, numChimeras);
}

int ChimeraUchimeCommand::driver(string outputFName, string filename, string accnos, string alns, int& numChimeras) {

	outputFName = m->getFullPathName(outputFName);
	filename = m->getFullPathName(filename);
	alns = m->getFullPathName(alns);

	vector<string> cPara;

	string uchimeCommand = uchimeLocation;
	uchimeCommand = "\"" + uchimeCommand + "\" ";

	cPara.push_back(uchimeCommand);

	//are you using a reference file
	if (templatefile != "self") {
		string outputFileName = filename + ".uchime_formatted";
		prepFile(filename, outputFileName);
		cPara.push_back("--db");
		cPara.push_back("\"" + templatefile + "\"");
	}

	cPara.push_back("--input");
	cPara.push_back("\"" + filename + "\"");

	cPara.push_back("--uchimeout");
	cPara.push_back("\"" + outputFName + "\"");

	if (chimealns) {
		cPara.push_back("--uchimealns");
		cPara.push_back("\"" + alns + "\"");
	}

	if (strand != "") {
		cPara.push_back("--strand");
		cPara.push_back(strand);
	}

	if (useAbskew) {
		cPara.push_back("--abskew");
		cPara.push_back(abskew);
	}

	if (useMinH) {
		cPara.push_back("--minh");
		cPara.push_back(minh);
	}

	if (useMindiv) {
		cPara.push_back("--mindiv");
		cPara.push_back(mindiv);
	}

	if (useXn) {
		cPara.push_back("--xn");
		cPara.push_back(xn);
	}

	if (useDn) {
		cPara.push_back("--dn");
		cPara.push_back(dn);
	}

	if (useXa) {
		cPara.push_back("--xa");
		cPara.push_back(xa);
	}

	if (useChunks) {
		cPara.push_back("--chunks");
		cPara.push_back(chunks);
	}

	if (useMinchunk) {
		cPara.push_back("--minchunk");
		cPara.push_back(minchunk);
	}

	if (useIdsmoothwindow) {
		cPara.push_back("--idsmoothwindow");
		cPara.push_back(idsmoothwindow);
	}

	if (useMaxp) {
		cPara.push_back("--maxp");
		cPara.push_back(maxp);
	}

	if (!skipgaps) {
		cPara.push_back("--noskipgaps");
	}

	if (!skipgaps2) {
		cPara.push_back("--noskipgaps2");
	}

	if (useMinlen) {
		cPara.push_back("--minlen");
		cPara.push_back(minlen);
	}

	if (useMaxlen) {
		cPara.push_back("--maxlen");
		cPara.push_back(maxlen);
	}

	if (ucl) {
		cPara.push_back("--ucl");
	}

	if (useQueryfract) {
		cPara.push_back("--queryfract");
		cPara.push_back(queryfract);
	}

	string commandString = Utility::join(cPara, " ");

#if defined (WIN32)
	commandString = "\"" + commandString + "\"";
#endif
	if (app.isDebug) { LOG(DEBUG) << "uchime command = " + commandString + ".\n"; }
	system(commandString.c_str());

	if (ctrlc_pressed) { return 0; }

	//create accnos file from uchime results
	ifstream in;
	File::openInputFile(outputFName, in);

	ofstream out;
	File::openOutputFile(accnos, out);

	int num = 0;
	numChimeras = 0;
	while (!in.eof()) {

		if (ctrlc_pressed) { break; }

		string name = "";
		string chimeraFlag = "";
		//in >> chimeraFlag >> name;

		string line = File::getline(in);
		vector<string> pieces = m->splitWhiteSpace(line);
		if (pieces.size() > 2) {
			name = pieces[1];
			//fix name if needed
			if (templatefile == "self") {
				name = name.substr(0, name.length() - 1); //rip off last /
				name = name.substr(0, name.find_last_of('/'));
			}

			chimeraFlag = pieces[pieces.size() - 1];
		}
		//for (int i = 0; i < 15; i++) {  in >> chimeraFlag; }
		File::gobble(in);

		if (chimeraFlag == "Y") { out << name << endl; numChimeras++; }
		num++;
	}
	in.close();
	out.close();

	//if (templatefile != "self") {  File::remove(filename); }

	return num;
}
/**************************************************************************************************/
//uchime can't handle some of the things allowed in mothurs fasta files. This functions "cleans up" the file.
int ChimeraUchimeCommand::prepFile(string filename, string output) {

	ifstream in;
	File::openInputFile(filename, in);

	ofstream out;
	File::openOutputFile(output, out);

	while (!in.eof()) {
		if (ctrlc_pressed) { break; }

		Sequence seq(in); File::gobble(in);

		if (seq.getName() != "") { seq.printSequence(out); }
	}
	in.close();
	out.close();

	return 0;
}
/**************************************************************************************************/

int ChimeraUchimeCommand::createProcesses(string outputFileName, string filename, string accnos, string alns, int& numChimeras) {

	int num = 0;
	vector<string> files;

	//break up file into multiple files
	File::divideFile(filename, processors, files);

	if (ctrlc_pressed) { return 0; }

	vector<thread> thrds(files.size() - 1);
	vector<int> tNumSeqs(files.size() - 1);
	vector<int> tNumChimeras(files.size() - 1);

	//loop through and create all the processes you want
	for (int i = 1; i < files.size(); i++) {
		thrds[i - 1] = thread(&ChimeraUchimeCommand::driverWithCount, this, outputFileName + toString(i) + ".temp", files[i], accnos + toString(i) + ".temp", alns + toString(i) + ".temp", ref(tNumChimeras[i - 1]), ref(tNumSeqs[i - 1]));
	}

	//do my part
	num = driver(outputFileName, files[0], accnos, alns, numChimeras);

	//force parent to wait until all the processes are done
	for (int i = 0; i < thrds.size(); i++) {
		thrds[i].join();
		num += tNumSeqs[i];
		numChimeras += tNumChimeras[i];
	}

	//append output files
	for (int i = 0;i < thrds.size();i++) {
		File::appendFiles((outputFileName + toString(processIDS[i]) + ".temp"), outputFileName);
		File::remove((outputFileName + toString(processIDS[i]) + ".temp"));

		File::appendFiles((accnos + toString(processIDS[i]) + ".temp"), accnos);
		File::remove((accnos + toString(processIDS[i]) + ".temp"));

		if (chimealns) {
			File::appendFiles((alns + toString(processIDS[i]) + ".temp"), alns);
			File::remove((alns + toString(processIDS[i]) + ".temp"));
		}
	}

	//get rid of the file pieces.
	for (int i = 0; i < files.size(); i++) { File::remove(files[i]); }
	return num;
}
/**************************************************************************************************/

int ChimeraUchimeCommand::createProcessesGroups(string outputFName, string filename, string accnos, string alns, string newCountFile, vector<string> groups, string nameFile, string groupFile, string fastaFile) {

	int num = 0;

	CountTable newCount;
	if (hasCount && dups) { newCount.readTable(nameFile, true, false); }

	//sanity check
	if (groups.size() < processors) { processors = groups.size(); }

	//divide the groups between the processors
	vector<linePair> lines;
	int remainingPairs = groups.size();
	int startIndex = 0;
	for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
		int numPairs = remainingPairs; //case for last processor
		if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
		lines.push_back(linePair(startIndex, (startIndex + numPairs))); //startIndex, endIndex
		startIndex = startIndex + numPairs;
		remainingPairs = remainingPairs - numPairs;
	}

	vector<thread> thrds(lines.size() - 1);
	vector<int> nums(lines.size() - 1);

	//loop through and create all the processes you want
	for (int i = 1; i < lines.size(); i++) {
		thrds[i - 1] = thread(&ChimeraUchimeCommand::driverGroupsWithCount, this, outputFName + toString(i) + ".temp", filename + toString(i) + ".temp", accnos + toString(i) + ".temp", alns + toString(i) + ".temp", accnos + ".byCount." + toString(i) + ".temp", lines[i].start, lines[i].end, groups, ref(nums[i - 1]));
	}

	//do my part
	num = driverGroups(outputFName, filename, accnos, alns, accnos + ".byCount.0.temp", lines[0].start, lines[0].end, groups);

	//force parent to wait until all the processes are done
	for (int i = 0; i < thrds.size(); i++) {
		thrds[i].join();
		num += nums[i];
	}

	//append output files
	for (int i = 0; i < lines.size(); i++) {
		if (i > 0) {
			File::appendFiles((outputFName + toString(i) + ".temp"), outputFName);
			File::remove((outputFName + toString(i) + ".temp"));

			File::appendFiles((accnos + toString(i) + ".temp"), accnos);
			File::remove((accnos + toString(i) + ".temp"));

			if (chimealns) {
				File::appendFiles((alns + toString(i) + ".temp"), alns);
				File::remove((alns + toString(i) + ".temp"));
			}
		}

		if (hasCount && dups) {
			if (!File::isBlank(accnos + ".byCount." + toString(i) + ".temp")) {
				ifstream in2;
				File::openInputFile(accnos + ".byCount." + toString(i) + ".temp", in2);

				string name, group;
				while (!in2.eof()) {
					in2 >> name >> group; File::gobble(in2);
					newCount.setAbund(name, group, 0);
				}
				in2.close();
			}
			File::remove(accnos + ".byCount." + toString(i) + ".temp");
		}
	}

	//print new *.pick.count_table
	if (hasCount && dups) { newCount.printTable(newCountFile); }

	return num;

}
/**************************************************************************************************/

