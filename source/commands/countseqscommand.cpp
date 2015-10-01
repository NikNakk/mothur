/*
 *  countseqscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 6/1/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "countseqscommand.h"
#include "sharedutilities.h"
#include "counttable.h"
#include "inputdata.h"
#include <thread>

 //**********************************************************************************************************************
vector<string> CountSeqsCommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "NameSHared-sharedGroup", "NameSHared", "none", "count", false, false, true); parameters.push_back(pshared);
		CommandParameter pname("name", "InputTypes", "", "", "NameSHared", "NameSHared", "none", "count", false, false, true); parameters.push_back(pname);
		CommandParameter pgroup("group", "InputTypes", "", "", "sharedGroup", "none", "none", "", false, false, true); parameters.push_back(pgroup);
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new BooleanParameter("large", false, false, false));
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CountSeqsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string CountSeqsCommand::getHelpString() {
	try {
		string helpString = "The count.seqs aka. make.table command reads a name or shared file and outputs a .count_table file.  You may also provide a group with the names file to get the counts broken down by group.\n"
			"The groups parameter allows you to indicate which groups you want to include in the counts, by default all groups in your groupfile are used.\n"
			"The large parameter indicates the name and group files are too large to fit in RAM.\n"
			"When you use the groups parameter and a sequence does not represent any sequences from the groups you specify it is not included in the .count.summary file.\n"
			"The processors parameter allows you to specify the number of processors to use. The default is 1.\n"
			"The count.seqs command should be in the following format: count.seqs(name=yourNameFile).\n"
			"Example count.seqs(name=amazon.names) or make.table(name=amazon.names).\n"
			"Note: No spaces between parameter labels (i.e. name), '=' and parameters (i.e.yourNameFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CountSeqsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string CountSeqsCommand::getOutputPattern(string type) {
	string pattern = "";
	if (type == "count") { pattern = "[filename],count_table-[filename],[distance],count_table"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
CountSeqsCommand::CountSeqsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["count"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CountSeqsCommand, CountSeqsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

CountSeqsCommand::CountSeqsCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	allLines = 1;

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
		for (map<string, string>::iterator it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["count"] = tempOutNames;


		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
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

			it = parameters.find("shared");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["shared"] = inputDir + it->second; }
			}
		}

		//check for required parameters
		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { namefile = ""; abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { settings.setCurrent("name", namefile); }

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = ""; abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { settings.setCurrent("shared", sharedfile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		if ((namefile == "") && (sharedfile == "")) {
			namefile = settings.getCurrent("name");
			if (namefile != "") { LOG(INFO) << "Using " + namefile + " as input file for the name parameter." << '\n'; }
			else {
				sharedfile = settings.getCurrent("shared");
				if (sharedfile != "") { LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
				else {
					LOG(INFO) << "You have no current namefile or sharedfile and the name or shared parameter is required." << '\n'; abort = true;
				}
			}
		}

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = "all"; }
		Utility::split(groups, '-', Groups);
		m->setGroups(Groups);

		string temp = validParameter.validFile(parameters, "large", false);		if (temp == "not found") { temp = "F"; }
		large = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

	}

}
//**********************************************************************************************************************

int CountSeqsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	map<string, string> variables;

	if (namefile != "") {
		unsigned long long total = 0;
		int start = time(NULL);
		if (outputDir == "") { outputDir = File::getPath(namefile); }
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(namefile));
		string outputFileName = getOutputFileName("count", variables);

		if (!large) { total = processSmall(outputFileName); }
		else { total = processLarge(outputFileName); }

		if (ctrlc_pressed) { File::remove(outputFileName); return 0; }

		LOG(INFO) << "It took " + toString(time(NULL) - start) + " secs to create a table for " + toString(total) + " sequences.";
		LOG(INFO) << "" << '\n';

		LOG(INFO) << '\n' << "Total number of sequences: " + toString(total) << '\n';

	}
	else {
		if (outputDir == "") { outputDir = File::getPath(sharedfile); }
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(sharedfile));

		InputData input(sharedfile, "sharedfile");
		vector<SharedRAbundVector*> lookup = input.getSharedRAbundVectors();
		string lastLabel = lookup[0]->getLabel();

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		//as long as you are not at the end of the file or done wih the lines you want
		while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

			if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

				LOG(INFO) << lookup[0]->getLabel() << '\n';

				processShared(lookup, variables);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());
			}

			if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = lookup[0]->getLabel();

				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
				lookup = input.getSharedRAbundVectors(lastLabel);
				LOG(INFO) << lookup[0]->getLabel() << '\n';

				processShared(lookup, variables);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());

				//restore real lastlabel to save below
				lookup[0]->setLabel(saveLabel);
			}

			lastLabel = lookup[0]->getLabel();
			//prevent memory leak
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }

			if (ctrlc_pressed) { return 0; }

			//get next line to process
			lookup = input.getSharedRAbundVectors();
		}

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }

		//output error messages about any remaining user labels
		set<string>::iterator it;
		bool needToRun = false;
		for (it = userLabels.begin(); it != userLabels.end(); it++) {
			LOG(INFO) << "Your file does not include the label " + *it;
			if (processedLabels.count(lastLabel) != 1) {
				LOG(INFO) << ". I will use " + lastLabel + "." << '\n';
				needToRun = true;
			}
			else {
				LOG(INFO) << ". Please refer to " + lastLabel + "." << '\n';
			}
		}

		//run last label if you need to
		if (needToRun == true) {
			for (int i = 0; i < lookup.size(); i++) { if (lookup[i] != NULL) { delete lookup[i]; } }
			lookup = input.getSharedRAbundVectors(lastLabel);

			LOG(INFO) << lookup[0]->getLabel() << '\n';

			processShared(lookup, variables);

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		}

	}

	//set rabund file as new current rabundfile
	itTypes = outputTypes.find("count");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { string current = (itTypes->second)[0]; settings.setCurrent("counttable", current); }
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in CountSeqsCommand, execute";
	exit(1);
}
}
//**********************************************************************************************************************

unsigned long long CountSeqsCommand::processShared(vector<SharedRAbundVector*>& lookup, map<string, string> variables) {
	try {
		variables["[distance]"] = lookup[0]->getLabel();
		string outputFileName = getOutputFileName("count", variables);
		outputNames.push_back(outputFileName); outputTypes["count"].push_back(outputFileName);

		ofstream out;
		File::openOutputFile(outputFileName, out);

		out << "OTU_Label\ttotal";
		for (int i = 0; i < lookup.size(); i++) { out << '\t' << lookup[i]->getGroup(); } out << endl;

		for (int j = 0; j < lookup[0]->getNumBins(); j++) {
			if (ctrlc_pressed) { break; }

			int total = 0;
			string output = "";
			for (int i = 0; i < lookup.size(); i++) {
				total += lookup[i]->getAbundance(j);
				output += '\t' + toString(lookup[i]->getAbundance(j));
			}
			out << settings.currentSharedBinLabels[j] << '\t' << total << output << endl;
		}
		out.close();

		return 0;
	}
	//**********************************************************************************************************************

	unsigned long long CountSeqsCommand::processSmall(string outputFileName) {
		ofstream out;
		File::openOutputFile(outputFileName, out); outputTypes["count"].push_back(outputFileName);
		outputNames.push_back(outputFileName); outputTypes["count"].push_back(outputFileName);
		out << "Representative_Sequence\ttotal";

		shared_ptr<GroupMap> groupMap;
		if (groupfile != "") {
			groupMap = make_shared<GroupMap>(groupfile);
			groupMap->readMap();

			//make sure groups are valid. takes care of user setting groupNames that are invalid or setting groups=all
			SharedUtil util = SharedUtil();
			vector<string> nameGroups = groupMap->getNamesOfGroups();
			util.setGroups(Groups, nameGroups);

			//sort groupNames so that the group title match the counts below, this is needed because the map object automatically sorts
			sort(Groups.begin(), Groups.end());

			//print groupNames
			for (int i = 0; i < Groups.size(); i++) {
				out << '\t' << Groups[i];
			}
		}
		out << endl;
		out.close();

		unsigned long long total = createProcesses(*groupMap.get(), outputFileName);

		return total;
	}
	/**************************************************************************************************/
	unsigned long long CountSeqsCommand::createProcesses(GroupMap& groupMap, string outputFileName) {

		vector<unsigned long long> positions;
		vector<linePair> lines;
		unsigned long long numSeqs = 0;

		positions = File::divideFilePerLine(namefile, processors);
		for (int i = 0; i < (positions.size() - 1); i++) { lines.push_back(linePair(positions[i], positions[(i + 1)])); }

		vector<thread> thrds(lines.size() - 1);
		vector<unsigned long long> tNumSeqs(lines.size() - 1);

		//loop through and create all the processes you want
		for (int i = 1; i < lines.size(); i++) {
			string filename = outputFileName + toString(i) + ".temp";
			thrds[i - 1] = thread(&CountSeqsCommand::driverWithCount, this, lines[i].start, lines[i].end, filename, ref(groupMap), ref(tNumSeqs[i - 1]));
		}

		numSeqs = driver(lines[0].start, lines[0].end, outputFileName + toString(0) + ".temp", groupMap);

		//force parent to wait until all the processes are done
		for (int i = 0; i < thrds.size(); i++) {
			thrds[i].join();
			numSeqs += tNumSeqs[i];
		}

		//append output files
		for (int i = 0; i < lines.size(); i++) {
			File::appendFiles(outputFileName + toString(i) + ".temp", outputFileName);
			File::remove(outputFileName + toString(i) + ".temp");
		}

		//sanity check
		if (groupfile != "") {
			if (numSeqs != groupMap.getNumSeqs()) {
				LOG(LOGERROR) << "processes reported processing " + toString(numSeqs) + " sequences, but group file indicates you have " + toString(groupMap.getNumSeqs()) + " sequences.";
				if (processors == 1) { LOG(INFO) << " Could you have a file mismatch?\n"; }
				else { LOG(INFO) << " Either you have a file mismatch or a process failed to complete the task assigned to it.\n"; ctrlc_pressed = true; }
			}
		}
		return numSeqs;
	}
	/**************************************************************************************************/
	unsigned long long CountSeqsCommand::driver(unsigned long long start, unsigned long long end, string outputFileName, GroupMap& groupMap) {

		ofstream out;
		File::openOutputFile(outputFileName, out);

		ifstream in;
		File::openInputFile(namefile, in);
		in.seekg(start);

		//adjust start if null strings
		if (start == 0) { m->zapGremlins(in); File::gobble(in); }


		bool done = false;
		unsigned long long total = 0;
		while (!done) {
			if (ctrlc_pressed) { break; }

			string firstCol, secondCol;
			in >> firstCol; File::gobble(in); in >> secondCol; File::gobble(in);
			//cout << firstCol << '\t' << secondCol << endl;
			m->checkName(firstCol);
			m->checkName(secondCol);
			//cout << firstCol << '\t' << secondCol << endl;

			vector<string> names;
			m->splitAtChar(secondCol, names, ',');

			if (groupfile != "") {
				//set to 0
				map<string, int> groupCounts;
				int total = 0;
				for (int i = 0; i < Groups.size(); i++) { groupCounts[Groups[i]] = 0; }

				//get counts for each of the users groups
				for (int i = 0; i < names.size(); i++) {
					string group = groupMap.getGroup(names[i]);

					if (group == "not found") { LOG(LOGERROR) << "" + names[i] + " is not in your groupfile, please correct." << '\n'; }
					else {
						map<string, int>::iterator it = groupCounts.find(group);

						//if not found, then this sequence is not from a group we care about
						if (it != groupCounts.end()) {
							it->second++;
							total++;
						}
					}
				}

				if (total != 0) {
					out << firstCol << '\t' << total;
					for (map<string, int>::iterator it = groupCounts.begin(); it != groupCounts.end(); it++) {
						out << '\t' << it->second;
					}
					out << endl;
				}
			}
			else {
				out << firstCol << '\t' << names.size() << endl;
			}

			total += names.size();

			unsigned long long pos = in.tellg();
			if ((pos == -1) || (pos >= end)) { break; }

		}
		in.close();
		out.close();

		return total;

	}

	void CountSeqsCommand::driverWithCount(unsigned long long start, unsigned long long end, string outputFileName, GroupMap& groupMap, unsigned long long& numSeqs) {
		numSeqs = driver(start, end, outputFileName, groupMap);
	}

	//**********************************************************************************************************************

	unsigned long long CountSeqsCommand::processLarge(string outputFileName) {
		set<string> namesOfGroups;
		map<string, int> initial;
		for (set<string>::iterator it = namesOfGroups.begin(); it != namesOfGroups.end(); it++) { initial[(*it)] = 0; }
		ofstream out;
		File::openOutputFile(outputFileName, out);
		outputNames.push_back(outputFileName); outputTypes["count"].push_back(outputFileName);
		out << "Representative_Sequence\ttotal";
		if (groupfile == "") { out << endl; }

		map<string, unsigned long long> namesToIndex;
		string outfile = File::getRootName(groupfile) + "sorted.groups.temp";
		string outName = File::getRootName(namefile) + "sorted.name.temp";
		map<int, string> indexToName;
		map<int, string> indexToGroup;
		if (groupfile != "") {
			time_t estart = time(NULL);
			//convert name file to redundant -> unique.  set unique name equal to index so we can use vectors, save name for later.
			string newNameFile = File::getRootName(namefile) + ".name.temp";
			string newGroupFile = File::getRootName(groupfile) + ".group.temp";
			indexToName = processNameFile(newNameFile);
			indexToGroup = getGroupNames(newGroupFile, namesOfGroups);

			//sort file by first column so the names of sequences will be easier to find
			//use the unix sort 
#if defined (UNIX)
			string command = "sort -n " + newGroupFile + " -o " + outfile;
			system(command.c_str());
			command = "sort -n " + newNameFile + " -o " + outName;
			system(command.c_str());
#else //sort using windows sort
			string command = "sort " + newGroupFile + " /O " + outfile;
			system(command.c_str());
			command = "sort " + newNameFile + " /O " + outName;
			system(command.c_str());
#endif
			File::remove(newNameFile);
			File::remove(newGroupFile);

			LOG(INFO) << "It took " + toString(time(NULL) - estart) + " seconds to sort and index the group and name files. " << '\n';
		}
		else { outName = namefile; }

		time_t estart = time(NULL);
		//open input file
		ifstream in;
		File::openInputFile(outName, in);

		//open input file
		ifstream in2;

		unsigned long long total = 0;
		vector< vector<int> > nameMapCount;
		if (groupfile != "") {
			File::openInputFile(outfile, in2);
			nameMapCount.resize(indexToName.size());
			for (int i = 0; i < nameMapCount.size(); i++) {
				nameMapCount[i].resize(indexToGroup.size(), 0);
			}
		}

		while (!in.eof()) {
			if (ctrlc_pressed) { break; }

			string firstCol;
			in >> firstCol;  File::gobble(in);

			if (groupfile != "") {
				int uniqueIndex;
				in >> uniqueIndex; File::gobble(in);

				string name; int groupIndex;
				in2 >> name >> groupIndex; File::gobble(in2);

				if (name != firstCol) { LOG(LOGERROR) << "found " + name + " in your groupfile, but " + firstCol + " was in your namefile, please correct.\n"; ctrlc_pressed = true; }

				nameMapCount[uniqueIndex][groupIndex]++;
				total++;
			}
			else {
				string secondCol;
				in >> secondCol; File::gobble(in);
				int num = m->getNumNames(secondCol);
				out << firstCol << '\t' << num << endl;
				total += num;
			}
		}
		in.close();

		if (groupfile != "") {
			File::remove(outfile);
			File::remove(outName);
			in2.close();
			for (map<int, string>::iterator it = indexToGroup.begin(); it != indexToGroup.end(); it++) { out << '\t' << it->second; }
			out << endl;
			for (int i = 0; i < nameMapCount.size(); i++) {
				string totalsLine = "";
				int seqTotal = 0;
				for (int j = 0; j < nameMapCount[i].size(); j++) {
					seqTotal += nameMapCount[i][j];
					totalsLine += '\t' + toString(nameMapCount[i][j]);
				}
				out << indexToName[i] << '\t' << seqTotal << totalsLine << endl;
			}
		}

		out.close();

		LOG(INFO) << "It took " + toString(time(NULL) - estart) + " seconds to create the count table file. " << '\n';

		return total;
	}
	/**************************************************************************************************/
	map<int, string> CountSeqsCommand::processNameFile(string name) {
		map<int, string> indexToNames;

		ofstream out;
		File::openOutputFile(name, out);

		//open input file
		ifstream in;
		File::openInputFile(namefile, in);

		string rest = "";
		char buffer[4096];
		bool pairDone = false;
		bool columnOne = true;
		string firstCol, secondCol;
		int count = 0;

		while (!in.eof()) {
			if (ctrlc_pressed) { break; }

			in.read(buffer, 4096);
			vector<string> pieces = m->splitWhiteSpace(rest, buffer, in.gcount());

			for (int i = 0; i < pieces.size(); i++) {
				if (columnOne) { firstCol = pieces[i]; columnOne = false; }
				else { secondCol = pieces[i]; pairDone = true; columnOne = true; }

				if (pairDone) {
					m->checkName(firstCol);
					m->checkName(secondCol);
					//parse names into vector
					vector<string> theseNames;
					m->splitAtComma(secondCol, theseNames);
					for (int i = 0; i < theseNames.size(); i++) { out << theseNames[i] << '\t' << count << endl; }
					indexToNames[count] = firstCol;
					pairDone = false;
					count++;
				}
			}
		}
		in.close();


		if (rest != "") {
			vector<string> pieces = m->splitWhiteSpace(rest);

			for (int i = 0; i < pieces.size(); i++) {
				if (columnOne) { firstCol = pieces[i]; columnOne = false; }
				else { secondCol = pieces[i]; pairDone = true; columnOne = true; }

				if (pairDone) {
					m->checkName(firstCol);
					m->checkName(secondCol);
					//parse names into vector
					vector<string> theseNames;
					m->splitAtComma(secondCol, theseNames);
					for (int i = 0; i < theseNames.size(); i++) { out << theseNames[i] << '\t' << count << endl; }
					indexToNames[count] = firstCol;
					pairDone = false;
					count++;
				}
			}

		}
		out.close();

		return indexToNames;
	}
	/**************************************************************************************************/
	map<int, string> CountSeqsCommand::getGroupNames(string filename, set<string>& namesOfGroups) {
		map<int, string> indexToGroups;
		map<string, int> groupIndex;
		map<string, int>::iterator it;

		ofstream out;
		File::openOutputFile(filename, out);

		//open input file
		ifstream in;
		File::openInputFile(groupfile, in);

		string rest = "";
		char buffer[4096];
		bool pairDone = false;
		bool columnOne = true;
		string firstCol, secondCol;
		int count = 0;

		while (!in.eof()) {
			if (ctrlc_pressed) { break; }

			in.read(buffer, 4096);
			vector<string> pieces = m->splitWhiteSpace(rest, buffer, in.gcount());

			for (int i = 0; i < pieces.size(); i++) {
				if (columnOne) { firstCol = pieces[i]; columnOne = false; }
				else { secondCol = pieces[i]; pairDone = true; columnOne = true; }

				if (pairDone) {
					m->checkName(firstCol);
					it = groupIndex.find(secondCol);
					if (it == groupIndex.end()) { //add group, assigning the group and number so we can use vectors above
						groupIndex[secondCol] = count;
						count++;
					}
					out << firstCol << '\t' << groupIndex[secondCol] << endl;
					namesOfGroups.insert(secondCol);
					pairDone = false;
				}
			}
		}
		in.close();


		if (rest != "") {
			vector<string> pieces = m->splitWhiteSpace(rest);

			for (int i = 0; i < pieces.size(); i++) {
				if (columnOne) { firstCol = pieces[i]; columnOne = false; }
				else { secondCol = pieces[i]; pairDone = true; columnOne = true; }

				if (pairDone) {
					m->checkName(firstCol);
					it = groupIndex.find(secondCol);
					if (it == groupIndex.end()) { //add group, assigning the group and number so we can use vectors above
						groupIndex[secondCol] = count;
						count++;
					}
					out << firstCol << '\t' << groupIndex[secondCol] << endl;
					namesOfGroups.insert(secondCol);
					pairDone = false;
				}
			}
		}
		out.close();

		for (it = groupIndex.begin(); it != groupIndex.end(); it++) { indexToGroups[it->second] = it->first; }

		return indexToGroups;
	}
	//**********************************************************************************************************************



