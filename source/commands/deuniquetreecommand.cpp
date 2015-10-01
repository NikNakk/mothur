/*
 *  deuniquetreecommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/27/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "deuniquetreecommand.h"
#include "treereader.h"

 //**********************************************************************************************************************
vector<string> DeuniqueTreeCommand::setParameters() {
	try {
		CommandParameter ptree("tree", "InputTypes", "", "", "none", "none", "none", "tree", false, true, true); parameters.push_back(ptree);
		CommandParameter pname("name", "InputTypes", "", "", "none", "none", "none", "", false, true, true); parameters.push_back(pname);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DeuniqueTreeCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string DeuniqueTreeCommand::getHelpString() {
	try {
		string helpString = "The deunique.tree command parameters are tree and name.  Both parameters are required unless you have valid current files.\n"
			"The deunique.tree command should be in the following format: deunique.tree(tree=yourTreeFile, name=yourNameFile).\n"
			"Example deunique.tree(tree=abrecovery.tree, name=abrecovery.names).\n"
			"Note: No spaces between parameter labels (i.e. tree), '=' and parameters (i.e.yourTreeFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DeuniqueTreeCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string DeuniqueTreeCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "tree") { pattern = "[filename],deunique.tre"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
DeuniqueTreeCommand::DeuniqueTreeCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["tree"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in DeuniqueTreeCommand, DeuniqueTreeCommand";
		exit(1);
	}
}
/***********************************************************/
DeuniqueTreeCommand::DeuniqueTreeCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();
		map<string, string>::iterator it;

		ValidParameters validParameter;

		//check to make sure all parameters are valid for command
		for (map<string, string>::iterator it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["tree"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("tree");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["tree"] = inputDir + it->second; }
			}

			it = parameters.find("name");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["name"] = inputDir + it->second; }
			}
		}

		//check for required parameters
		treefile = validParameter.validFile(parameters, "tree", true);
		if (treefile == "not open") { abort = true; }
		else if (treefile == "not found") { 				//if there is a current design file, use it
			treefile = settings.getCurrent("tree");
			if (treefile != "") { LOG(INFO) << "Using " + treefile + " as input file for the tree parameter." << '\n'; }
			else { LOG(INFO) << "You have no current tree file and the tree parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("tree", treefile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { abort = true; }
		else if (namefile == "not found") { 				//if there is a current design file, use it
			namefile = settings.getCurrent("name");
			if (namefile != "") { LOG(INFO) << "Using " + namefile + " as input file for the name parameter." << '\n'; }
			else { LOG(INFO) << "You have no current name file and the name parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("name", namefile); }

		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(treefile); }
	}

}

/***********************************************************/
int DeuniqueTreeCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	settings.setCurrent("tree", treefile);

	TreeReader* reader = new TreeReader(treefile, "", namefile);
	vector<Tree*> T = reader->getTrees();
	map<string, string> nameMap;
	m->readNames(namefile, nameMap);
	delete reader;

	//print new Tree
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(treefile));
	string outputFile = getOutputFileName("tree", variables);
	outputNames.push_back(outputFile); outputTypes["tree"].push_back(outputFile);
	ofstream out;
	File::openOutputFile(outputFile, out);
	T[0]->print(out, nameMap);
	out.close();

	delete (T[0]->getCountTable());
	for (int i = 0; i < T.size(); i++) { delete T[i]; }

	//set phylip file as new current phylipfile
	string current = "";
	itTypes = outputTypes.find("tree");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("tree", current); }
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;

}
/***********************************************************/




