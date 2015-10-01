/*
 *  phylotypecommand.cpp
 *  Mothur
 *
 *  Created by westcott on 11/20/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "phylotypecommand.h"
#include "phylotree.h"
#include "listvector.hpp"
#include "rabundvector.hpp"
#include "sabundvector.hpp"
#include "counttable.h"

 //**********************************************************************************************************************
vector<string> PhylotypeCommand::setParameters() {
	try {
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "none", "none", "none", "list-rabund-sabund", false, true, true); parameters.push_back(ptaxonomy);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "ColumnName", "rabund-sabund", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount", "none", "none", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pcutoff("cutoff", "Number", "", "-1", "", "", "", "", false, false, true); parameters.push_back(pcutoff);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PhylotypeCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string PhylotypeCommand::getHelpString() {
	try {
		string helpString = "The phylotype command reads a taxonomy file and outputs a .list, .rabund and .sabund file. \n"
			"The phylotype command parameter options are taxonomy, name, count, cutoff and label. The taxonomy parameter is required.\n"
			"The cutoff parameter allows you to specify the level you want to stop at.  The default is the highest level in your taxonomy file. \n"
			"For example: taxonomy = Bacteria;Bacteroidetes-Chlorobi;Bacteroidetes; - cutoff=2, would truncate the taxonomy to Bacteria;Bacteroidetes-Chlorobi; \n"
			"For the cutoff parameter levels count up from the root of the phylotree. This enables you to look at the grouping down to a specific resolution, say the genus level.\n"
			"The label parameter allows you to specify which level you would like, and are separated by dashes.  The default all levels in your taxonomy file. \n"
			"For the label parameter, levels count down from the root to keep the output similar to mothur's other commands which report information from finer resolution to coarser resolutions.\n"
			"The phylotype command should be in the following format: \n"
			"phylotype(taxonomy=yourTaxonomyFile, cutoff=yourCutoff, label=yourLabels) \n"
			"Eaxample: phylotype(taxonomy=amazon.taxonomy, cutoff=5, label=1-3-5).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PhylotypeCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string PhylotypeCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "list") { pattern = "[filename],[clustertag],list-[filename],[clustertag],[tag2],list"; }
	else if (type == "rabund") { pattern = "[filename],[clustertag],rabund"; }
	else if (type == "sabund") { pattern = "[filename],[clustertag],sabund"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
PhylotypeCommand::PhylotypeCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["list"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
		outputTypes["rabund"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PhylotypeCommand, PhylotypeCommand";
		exit(1);
	}
}
/**********************************************************************************************************************/
PhylotypeCommand::PhylotypeCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["list"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
		outputTypes["rabund"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("taxonomy");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["taxonomy"] = inputDir + it->second; }
			}

			it = parameters.find("name");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["name"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}
		}

		taxonomyFileName = validParameter.validFile(parameters, "taxonomy", true);
		if (taxonomyFileName == "not found") {
			taxonomyFileName = settings.getCurrent("taxonomy");
			if (taxonomyFileName != "") { LOG(INFO) << "Using " + taxonomyFileName + " as input file for the taxonomy parameter." << '\n'; }
			else {
				LOG(INFO) << "No valid current files. taxonomy is a required parameter." << '\n';
				abort = true;
			}
		}
		else if (taxonomyFileName == "not open") { taxonomyFileName = ""; abort = true; }
		else { settings.setCurrent("taxonomy", taxonomyFileName); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { namefile = ""; abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { readNamesFile(); settings.setCurrent("name", namefile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { abort = true; countfile = ""; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = "";
			outputDir += File::getPath(taxonomyFileName); //if user entered a file with a path then preserve it	
		}

		if ((countfile != "") && (namefile != "")) { LOG(INFO) << "You must enter ONLY ONE of the following: count or name." << '\n'; abort = true; }

		string temp = validParameter.validFile(parameters, "cutoff", false);
		if (temp == "not found") { temp = "-1"; }
		Utility::mothurConvert(temp, cutoff);

		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; allLines = 1; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		if (countfile == "") {
			if (namefile == "") {
				vector<string> files; files.push_back(taxonomyFileName);
				OptionParser::getNameFile(files);
			}
		}
	}
}
/**********************************************************************************************************************/

int PhylotypeCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//reads in taxonomy file and makes all the taxonomies the same length 
	//by appending the last taxon to a given taxonomy as many times as needed to 
	//make it as long as the longest taxonomy in the file 
	TaxEqualizer* taxEqual = new TaxEqualizer(taxonomyFileName, cutoff, outputDir);

	if (ctrlc_pressed) { delete taxEqual; return 0; }

	string equalizedTaxFile = taxEqual->getEqualizedTaxFile();

	delete taxEqual;

	//build taxonomy tree from equalized file
	PhyloTree* tree = new PhyloTree(equalizedTaxFile);
	vector<int> leaves = tree->getGenusNodes();

	//store leaf nodes in current map
	for (int i = 0; i < leaves.size(); i++) { currentNodes[leaves[i]] = leaves[i]; }

	bool done = false;
	if (tree->get(leaves[0]).parent == -1) { LOG(INFO) << "Empty Tree" << '\n';	done = true; }

	if (ctrlc_pressed) { delete tree; return 0; }

	ofstream outList, outRabund, outSabund;
	map<string, string> variables;
	string fileroot = outputDir + File::getRootName(File::getSimpleName(taxonomyFileName));
	variables["[filename]"] = fileroot;
	variables["[clustertag]"] = "tx";
	string sabundFileName = getOutputFileName("sabund", variables);
	string rabundFileName = getOutputFileName("rabund", variables);
	if (countfile != "") { variables["[tag2]"] = "unique_list"; }
	string listFileName = getOutputFileName("list", variables);

	map<string, int> counts;
	if (countfile == "") {
		File::openOutputFile(sabundFileName, outSabund);
		File::openOutputFile(rabundFileName, outRabund);
		outputNames.push_back(sabundFileName); outputTypes["sabund"].push_back(sabundFileName);
		outputNames.push_back(rabundFileName); outputTypes["rabund"].push_back(rabundFileName);

	}
	else {
		CountTable ct;
		ct.readTable(countfile, false, false);
		counts = ct.getNameMap();
	}
	File::openOutputFile(listFileName, outList);
	outputNames.push_back(listFileName); outputTypes["list"].push_back(listFileName);


	int count = 1;
	//start at leaves of tree and work towards root, processing the labels the user wants
	while ((!done) && ((allLines == 1) || (labels.size() != 0))) {

		string level = toString(count);
		count++;

		if (ctrlc_pressed) {
			if (countfile == "") { outRabund.close(); outSabund.close(); } outList.close();
			for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }
			delete tree; return 0;
		}

		//is this a level the user want output for
		if (allLines == 1 || labels.count(level) == 1) {

			//output level
			LOG(INFO) << level << '\n';

			ListVector list;
			list.setLabel(level);

			//go through nodes and build listvector 
			for (itCurrent = currentNodes.begin(); itCurrent != currentNodes.end(); itCurrent++) {

				//get parents
				TaxNode node = tree->get(itCurrent->first);
				parentNodes[node.parent] = node.parent;

				vector<string> names = node.accessions;

				//make the names compatable with listvector
				string name = "";
				for (int i = 0; i < names.size(); i++) {

					if (names[i] != "unknown") {
						if (namefile != "") {
							map<string, string>::iterator itNames = namemap.find(names[i]);  //make sure this name is in namefile

							if (itNames != namemap.end()) { name += namemap[names[i]] + ","; } //you found it in namefile
							else { LOG(LOGERROR) << "" + names[i] + " is not in your namefile, please correct." << '\n'; ctrlc_pressed = true; }

						}
						else { name += names[i] + ","; }
					}
				}

				if (ctrlc_pressed) { break; }

				name = name.substr(0, name.length() - 1);  //rip off extra ','
				//add bin to list vector
				if (name != "") { list.push_back(name); } //caused by unknown
			}

			//print listvector
			if (!m->printedListHeaders) { list.printHeaders(outList); }
			if (countfile == "") { list.print(outList); }
			else { list.print(outList, counts); }

			if (countfile == "") {
				//print rabund
				list.getRAbundVector().print(outRabund);
				//print sabund
				list.getSAbundVector().print(outSabund);
			}
			labels.erase(level);

		}
		else {

			//just get parents
			for (itCurrent = currentNodes.begin(); itCurrent != currentNodes.end(); itCurrent++) {
				int parent = tree->get(itCurrent->first).parent;
				parentNodes[parent] = parent;
			}
		}

		//move up a level
		currentNodes = parentNodes;
		parentNodes.clear();

		//have we reached the rootnode
		if (tree->get(currentNodes.begin()->first).parent == -1) { done = true; }
	}

	outList.close();
	if (countfile == "") {
		outSabund.close();
		outRabund.close();
	}

	delete tree;

	if (ctrlc_pressed) {
		for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }
		return 0;
	}

	//set list file as new current listfile
	string current = "";
	itTypes = outputTypes.find("list");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("list", current); }
	}

	//set rabund file as new current rabundfile
	itTypes = outputTypes.find("rabund");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("rabund", current); }
	}

	//set sabund file as new current sabundfile
	itTypes = outputTypes.find("sabund");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("sabund", current); }
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}

catch (exception& e) {
	LOG(FATAL) << e.what() << " in PhylotypeCommand, execute";
	exit(1);
}
}
/*****************************************************************/
int PhylotypeCommand::readNamesFile() {
	try {

		ifstream in;
		File::openInputFile(namefile, in);

		string first, second;
		map<string, string>::iterator itNames;

		while (!in.eof()) {
			in >> first >> second; File::gobble(in);

			itNames = namemap.find(first);
			if (itNames == namemap.end()) {
				namemap[first] = second;
			}
			else { LOG(INFO) << first + " has already been seen in namefile, disregarding names file." << '\n'; in.close(); namemap.clear(); namefile = ""; return 1; }
		}
		in.close();

		return 0;
	}

	/**********************************************************************************************************************/
