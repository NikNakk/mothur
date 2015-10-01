//
//  getotulabelscommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 5/21/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "getotulabelscommand.h"

//**********************************************************************************************************************
vector<string> GetOtuLabelsCommand::setParameters() {
	try {
		CommandParameter paccnos("accnos", "InputTypes", "", "", "none", "none", "none", "", false, true, true); parameters.push_back(paccnos);
		CommandParameter pconstaxonomy("constaxonomy", "InputTypes", "", "", "none", "FNGLT", "none", "constaxonomy", false, false, true); parameters.push_back(pconstaxonomy);
		CommandParameter plist("list", "InputTypes", "", "", "none", "FNGLT", "none", "list", false, false, true); parameters.push_back(plist);
		CommandParameter pshared("shared", "InputTypes", "", "", "none", "FNGLT", "none", "shared", false, false, true); parameters.push_back(pshared);
		CommandParameter potucorr("otucorr", "InputTypes", "", "", "none", "FNGLT", "none", "otucorr", false, false, true); parameters.push_back(potucorr);
		CommandParameter pcorraxes("corraxes", "InputTypes", "", "", "none", "FNGLT", "none", "corraxes", false, false, true); parameters.push_back(pcorraxes);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetOtuLabelsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetOtuLabelsCommand::getHelpString() {
	try {
		string helpString = "The get.otulabels command can be used to select specific otus with the output from classify.otu, otu.association, or corr.axes commands.  It can also be used to select a set of otus from a shared or list file.\n"
			"The get.otulabels parameters are: constaxonomy, otucorr, corraxes, shared, list, label and accnos.\n"
			"The constaxonomy parameter is used to input the results of the classify.otu command.\n"
			"The otucorr parameter is used to input the results of the otu.association command.\n"
			"The corraxes parameter is used to input the results of the corr.axes command.\n"
			"The label parameter is used to analyze specific labels in your input. \n"
			"The get.otulabels commmand should be in the following format: \n"
			"get.otulabels(accnos=yourListOfOTULabels, corraxes=yourCorrAxesFile)\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetOtuLabelsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string GetOtuLabelsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "constaxonomy") { pattern = "[filename],pick,[extension]"; }
	else if (type == "otucorr") { pattern = "[filename],pick,[extension]"; }
	else if (type == "corraxes") { pattern = "[filename],pick,[extension]"; }
	else if (type == "list") { pattern = "[filename],[distance],pick,[extension]"; }
	else if (type == "shared") { pattern = "[filename],[distance],pick,[extension]"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}

//**********************************************************************************************************************
GetOtuLabelsCommand::GetOtuLabelsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["constaxonomy"] = tempOutNames;
		outputTypes["otucorr"] = tempOutNames;
		outputTypes["corraxes"] = tempOutNames;
		outputTypes["shared"] = tempOutNames;
		outputTypes["list"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in GetOtuLabelsCommand, GetOtuLabelsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
GetOtuLabelsCommand::GetOtuLabelsCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		//valid paramters for this command
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter;
		map<string, string>::iterator it;
		//check to make sure all parameters are valid for command
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}


		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {

			//edit file types below to include only the types you added as parameters

			string path;
			it = parameters.find("constaxonomy");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["constaxonomy"] = inputDir + it->second; }
			}

			it = parameters.find("accnos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["accnos"] = inputDir + it->second; }
			}

			it = parameters.find("corraxes");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["corraxes"] = inputDir + it->second; }
			}

			it = parameters.find("otucorr");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["otucorr"] = inputDir + it->second; }
			}

			it = parameters.find("list");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["list"] = inputDir + it->second; }
			}

			it = parameters.find("shared");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["shared"] = inputDir + it->second; }
			}
		}

		vector<string> tempOutNames;
		outputTypes["constaxonomy"] = tempOutNames;
		outputTypes["otucorr"] = tempOutNames;
		outputTypes["corraxes"] = tempOutNames;
		outputTypes["shared"] = tempOutNames;
		outputTypes["list"] = tempOutNames;

		//check for parameters
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

		constaxonomyfile = validParameter.validFile(parameters, "constaxonomy", true);
		if (constaxonomyfile == "not open") { constaxonomyfile = ""; abort = true; }
		else if (constaxonomyfile == "not found") { constaxonomyfile = ""; }

		corraxesfile = validParameter.validFile(parameters, "corraxes", true);
		if (corraxesfile == "not open") { corraxesfile = ""; abort = true; }
		else if (corraxesfile == "not found") { corraxesfile = ""; }

		otucorrfile = validParameter.validFile(parameters, "otucorr", true);
		if (otucorrfile == "not open") { otucorrfile = ""; abort = true; }
		else if (otucorrfile == "not found") { otucorrfile = ""; }

		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { listfile = ""; abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { settings.setCurrent("list", listfile); }

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = ""; abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { settings.setCurrent("shared", sharedfile); }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		if ((constaxonomyfile == "") && (corraxesfile == "") && (otucorrfile == "") && (sharedfile == "") && (listfile == "")) { LOG(INFO) << "You must provide one of the following: constaxonomy, corraxes, otucorr, shared or list." << '\n'; abort = true; }

		if ((sharedfile != "") || (listfile != "")) {
			label = validParameter.validFile(parameters, "label", false);
			if (label == "not found") { label = ""; LOG(INFO) << "You did not provide a label, I will use the first label in your inputfile." << '\n'; label = ""; }
		}
	}

}
//**********************************************************************************************************************

int GetOtuLabelsCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//get labels you want to keep
	labels = m->readAccnos(accnosfile);
	//simplfy labels
	set<string> newLabels;
	for (set<string>::iterator it = labels.begin(); it != labels.end(); it++) { newLabels.insert(m->getSimpleLabel(*it)); }
	labels = newLabels;

	if (ctrlc_pressed) { return 0; }

	//read through the correct file and output lines you want to keep
	if (constaxonomyfile != "") { readClassifyOtu(); }
	if (corraxesfile != "") { readCorrAxes(); }
	if (otucorrfile != "") { readOtuAssociation(); }
	if (listfile != "") { readList(); }
	if (sharedfile != "") { readShared(); }

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  return 0; }

	//output files created by command
	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	string current = "";
	itTypes = outputTypes.find("list");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("list", current); }
	}

	itTypes = outputTypes.find("shared");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("shared", current); }
	}

	return 0;
}
//**********************************************************************************************************************
int GetOtuLabelsCommand::readClassifyOtu() {
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(constaxonomyfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(constaxonomyfile));
	variables["[extension]"] = m->getExtension(constaxonomyfile);
	string outputFileName = getOutputFileName("constaxonomy", variables);

	ofstream out;
	File::openOutputFile(outputFileName, out);

	ifstream in;
	File::openInputFile(constaxonomyfile, in);

	bool wroteSomething = false;
	int selectedCount = 0;

	//read headers
	string headers = File::getline(in);
	out << headers << endl;

	while (!in.eof()) {

		if (ctrlc_pressed) { break; }

		string otu = ""; string tax = "unknown";
		int size = 0;

		in >> otu >> size >> tax; File::gobble(in);

		if (labels.count(m->getSimpleLabel(otu)) != 0) {
			wroteSomething = true;
			selectedCount++;

			out << otu << '\t' << size << '\t' << tax << endl;
		}
	}
	in.close();
	out.close();

	if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any labels from the .accnos file." << '\n'; }
	outputNames.push_back(outputFileName);  outputTypes["constaxonomy"].push_back(outputFileName);

	LOG(INFO) << "Selected " + toString(selectedCount) + " otus from your constaxonomy file." << '\n';

	return 0;

}
//**********************************************************************************************************************
int GetOtuLabelsCommand::readOtuAssociation() {
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(otucorrfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(otucorrfile));
	variables["[extension]"] = m->getExtension(otucorrfile);
	string outputFileName = getOutputFileName("otucorr", variables);

	ofstream out;
	File::openOutputFile(outputFileName, out);

	ifstream in;
	File::openInputFile(otucorrfile, in);

	bool wroteSomething = false;
	int selectedCount = 0;

	//read headers
	string headers = File::getline(in);
	out << headers << endl;

	while (!in.eof()) {

		if (ctrlc_pressed) { break; }

		string otu1 = "";
		string otu2 = "";
		in >> otu1 >> otu2;
		string line = File::getline(in); File::gobble(in);

		if ((labels.count(m->getSimpleLabel(otu1)) != 0) && (labels.count(m->getSimpleLabel(otu2)) != 0)) {
			wroteSomething = true;
			selectedCount++;

			out << otu1 << '\t' << otu2 << '\t' << line << endl;
		}
	}
	in.close();
	out.close();

	if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any labels from the .accnos file." << '\n'; }
	outputNames.push_back(outputFileName);  outputTypes["otucorr"].push_back(outputFileName);

	LOG(INFO) << "Selected " + toString(selectedCount) + " lines from your otu.corr file." << '\n';

	return 0;

}
//**********************************************************************************************************************
int GetOtuLabelsCommand::readCorrAxes() {
	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(corraxesfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(corraxesfile));
	variables["[extension]"] = m->getExtension(corraxesfile);
	string outputFileName = getOutputFileName("corraxes", variables);

	ofstream out;
	File::openOutputFile(outputFileName, out);


	ifstream in;
	File::openInputFile(corraxesfile, in);

	bool wroteSomething = false;
	int selectedCount = 0;

	//read headers
	string headers = File::getline(in);
	out << headers << endl;

	while (!in.eof()) {

		if (ctrlc_pressed) { break; }

		string otu = "";
		in >> otu;
		string line = File::getline(in); File::gobble(in);

		if (labels.count(m->getSimpleLabel(otu)) != 0) {
			wroteSomething = true;
			selectedCount++;

			out << otu << '\t' << line << endl;
		}
	}
	in.close();
	out.close();

	if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any labels from the .accnos file." << '\n'; }
	outputNames.push_back(outputFileName);  outputTypes["corraxes"].push_back(outputFileName);

	LOG(INFO) << "Selected " + toString(selectedCount) + " lines from your corr.axes file." << '\n';

	return 0;

}
//**********************************************************************************************************************
int GetOtuLabelsCommand::readShared() {

	getShared();

	if (ctrlc_pressed) { for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } return 0; }

	vector<string> newLabels;

	//create new "filtered" lookup
	vector<SharedRAbundVector*> newLookup;
	for (int i = 0; i < lookup.size(); i++) {
		SharedRAbundVector* temp = new SharedRAbundVector();
		temp->setLabel(lookup[i]->getLabel());
		temp->setGroup(lookup[i]->getGroup());
		newLookup.push_back(temp);
	}

	bool wroteSomething = false;
	int numSelected = 0;
	for (int i = 0; i < lookup[0]->getNumBins(); i++) {

		if (ctrlc_pressed) { for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; } for (int j = 0; j < lookup.size(); j++) { delete lookup[j]; } return 0; }

		//is this otu on the list
		if (labels.count(m->getSimpleLabel(settings.currentSharedBinLabels[i])) != 0) {
			numSelected++; wroteSomething = true;
			newLabels.push_back(settings.currentSharedBinLabels[i]);
			for (int j = 0; j < newLookup.size(); j++) { //add this OTU to the new lookup
				newLookup[j]->push_back(lookup[j]->getAbundance(i), lookup[j]->getGroup());
			}
		}
	}

	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(sharedfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(sharedfile));
	variables["[extension]"] = m->getExtension(sharedfile);
	variables["[distance]"] = lookup[0]->getLabel();
	string outputFileName = getOutputFileName("shared", variables);
	ofstream out;
	File::openOutputFile(outputFileName, out);
	outputTypes["shared"].push_back(outputFileName);  outputNames.push_back(outputFileName);

	for (int j = 0; j < lookup.size(); j++) { delete lookup[j]; }

	settings.currentSharedBinLabels = newLabels;

	newLookup[0]->printHeaders(out);

	for (int i = 0; i < newLookup.size(); i++) {
		out << newLookup[i]->getLabel() << '\t' << newLookup[i]->getGroup() << '\t';
		newLookup[i]->print(out);
	}
	out.close();

	for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; }

	if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any OTUs from the .accnos file." << '\n'; }

	LOG(INFO) << "Selected " + toString(numSelected) + " OTUs from your shared file." << '\n';

	return 0;
}
//**********************************************************************************************************************
int GetOtuLabelsCommand::readList() {
	getListVector();

	if (ctrlc_pressed) { delete list; return 0; }

	ListVector newList;
	newList.setLabel(list->getLabel());
	int selectedCount = 0;
	bool wroteSomething = false;

	vector<string> binLabels = list->getLabels();
	vector<string> newLabels;
	for (int i = 0; i < list->getNumBins(); i++) {

		if (ctrlc_pressed) { delete list; return 0; }

		if (labels.count(m->getSimpleLabel(binLabels[i])) != 0) {
			selectedCount++;
			newList.push_back(list->get(i));
			newLabels.push_back(binLabels[i]);
		}
	}

	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(listfile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(listfile));
	variables["[extension]"] = m->getExtension(listfile);
	variables["[distance]"] = list->getLabel();
	string outputFileName = getOutputFileName("list", variables);
	ofstream out;
	File::openOutputFile(outputFileName, out);

	delete list;
	//print new listvector
	if (newList.getNumBins() != 0) {
		wroteSomething = true;
		newList.setLabels(newLabels);
		newList.printHeaders(out);
		newList.print(out);
	}
	out.close();

	if (wroteSomething == false) { LOG(INFO) << "Your file does not contain any OTUs from the .accnos file." << '\n'; }
	outputNames.push_back(outputFileName); outputTypes["list"].push_back(outputFileName);

	LOG(INFO) << "Selected " + toString(selectedCount) + " OTUs from your list file." << '\n';

	return 0;
}
catch (exception& e) {
	LOG(FATAL) << e.what() << " in GetOtuLabelsCommand, readList";
	exit(1);
}
	}
	//**********************************************************************************************************************
	int GetOtuLabelsCommand::getListVector() {
		try {
			InputData input(listfile, "list");
			list = input.getListVector();
			string lastLabel = list->getLabel();

			if (label == "") { label = lastLabel;  return 0; }

			//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
			set<string> labels; labels.insert(label);
			set<string> processedLabels;
			set<string> userLabels = labels;

			//as long as you are not at the end of the file or done wih the lines you want
			while ((list != NULL) && (userLabels.size() != 0)) {
				if (ctrlc_pressed) { return 0; }

				if (labels.count(list->getLabel()) == 1) {
					processedLabels.insert(list->getLabel());
					userLabels.erase(list->getLabel());
					break;
				}

				if ((Utility::anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
					string saveLabel = list->getLabel();

					delete list;
					list = input.getListVector(lastLabel);

					processedLabels.insert(list->getLabel());
					userLabels.erase(list->getLabel());

					//restore real lastlabel to save below
					list->setLabel(saveLabel);
					break;
				}

				lastLabel = list->getLabel();

				//get next line to process
				//prevent memory leak
				delete list;
				list = input.getListVector();
			}


			if (ctrlc_pressed) { return 0; }

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
				delete list;
				list = input.getListVector(lastLabel);
			}

			return 0;
		}
		//**********************************************************************************************************************
		int GetOtuLabelsCommand::getShared() {
			InputData input(sharedfile, "sharedfile");
			lookup = input.getSharedRAbundVectors();
			string lastLabel = lookup[0]->getLabel();

			if (label == "") { label = lastLabel;  return 0; }

			//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
			set<string> labels; labels.insert(label);
			set<string> processedLabels;
			set<string> userLabels = labels;

			//as long as you are not at the end of the file or done wih the lines you want
			while ((lookup[0] != NULL) && (userLabels.size() != 0)) {
				if (ctrlc_pressed) { return 0; }

				if (labels.count(lookup[0]->getLabel()) == 1) {
					processedLabels.insert(lookup[0]->getLabel());
					userLabels.erase(lookup[0]->getLabel());
					break;
				}

				if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
					string saveLabel = lookup[0]->getLabel();

					for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
					lookup = input.getSharedRAbundVectors(lastLabel);

					processedLabels.insert(lookup[0]->getLabel());
					userLabels.erase(lookup[0]->getLabel());

					//restore real lastlabel to save below
					lookup[0]->setLabel(saveLabel);
					break;
				}

				lastLabel = lookup[0]->getLabel();

				//get next line to process
				//prevent memory leak
				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
				lookup = input.getSharedRAbundVectors();
			}


			if (ctrlc_pressed) { return 0; }

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
			}

			return 0;
		}
		//**********************************************************************************************************************
