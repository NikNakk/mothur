/*
 *  otuassociationcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 1/19/12.
 *  Copyright 2012 Schloss Lab. All rights reserved.
 *
 */

#include "otuassociationcommand.h"
#include "linearalgebra.h"

 //**********************************************************************************************************************
vector<string> OTUAssociationCommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "SharedRelMeta", "SharedRelMeta", "none", "otucorr", false, false, true); parameters.push_back(pshared);
		CommandParameter prelabund("relabund", "InputTypes", "", "", "SharedRelMeta", "SharedRelMeta", "none", "otucorr", false, false); parameters.push_back(prelabund);
		CommandParameter pmetadata("metadata", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pmetadata);
		CommandParameter pcutoff("cutoff", "Number", "", "10", "", "", "", "", false, false, true); parameters.push_back(pcutoff);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new MultipleParameter("method", vector<string>{"pearson", "spearman", "kendall"}, "pearson", false, false, true));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OTUAssociationCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string OTUAssociationCommand::getHelpString() {
	try {
		string helpString = "The otu.association command reads a shared or relabund file and calculates the correlation coefficients between otus.\n"
			"If you provide a metadata file, mothur will calculate te correlation bewteen the metadata and the otus.\n"
			"The otu.association command parameters are shared, relabund, metadata, groups, method, cutoff and label.  The shared or relabund parameter is required.\n"
			"The groups parameter allows you to specify which of the groups you would like included. The group names are separated by dashes.\n"
			"The label parameter allows you to select what distances level you would like used, and are also separated by dashes.\n"
			"The cutoff parameter allows you to set a pvalue at which the otu will be reported.\n"
			"The method parameter allows you to select what method you would like to use. Options are pearson, spearman and kendall. Default=pearson.\n"
			"The otu.association command should be in the following format: otu.association(shared=yourSharedFile, method=yourMethod).\n"
			"Example otu.association(shared=genus.pool.shared, method=kendall).\n"
			"The otu.association command outputs a .otu.corr file.\n"
			"Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OTUAssociationCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string OTUAssociationCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "otucorr") { pattern = "[filename],[distance],[tag],otu.corr"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
OTUAssociationCommand::OTUAssociationCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["otucorr"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OTUAssociationCommand, OTUAssociationCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
OTUAssociationCommand::OTUAssociationCommand(Settings& settings, string option) : Command(settings, option) {
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
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		vector<string> tempOutNames;
		outputTypes["otucorr"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("shared");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["shared"] = inputDir + it->second; }
			}

			it = parameters.find("relabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["relabund"] = inputDir + it->second; }
			}

			it = parameters.find("metadata");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["metadata"] = inputDir + it->second; }
			}
		}


		//check for required parameters			
		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { inputFileName = sharedfile; settings.setCurrent("shared", sharedfile); }

		relabundfile = validParameter.validFile(parameters, "relabund", true);
		if (relabundfile == "not open") { abort = true; }
		else if (relabundfile == "not found") { relabundfile = ""; }
		else { inputFileName = relabundfile; settings.setCurrent("relabund", relabundfile); }

		metadatafile = validParameter.validFile(parameters, "metadata", true);
		if (metadatafile == "not open") { abort = true; metadatafile = ""; }
		else if (metadatafile == "not found") { metadatafile = ""; }

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = "";  pickedGroups = false; }
		else {
			pickedGroups = true;
			Utility::split(groups, '-', Groups);
		}
		m->setGroups(Groups);

		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(inputFileName); }

		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		if ((relabundfile == "") && (sharedfile == "")) {
			//is there are current file available for any of these?
			//give priority to shared, then relabund
			//if there is a current shared file, use it
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { inputFileName = sharedfile; LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else {
				relabundfile = settings.getCurrent("relabund");
				if (relabundfile != "") { inputFileName = relabundfile;  LOG(INFO) << "Using " + relabundfile + " as input file for the relabund parameter." << '\n'; }
				else {
					LOG(INFO) << "You must provide either a shared or relabund file." << '\n'; abort = true;
				}
			}
		}


		if ((relabundfile != "") && (sharedfile != "")) { LOG(INFO) << "You may only use one of the following : shared or relabund file." << '\n'; abort = true; }

		method = validParameter.validFile(parameters, "method", false);		if (method == "not found") { method = "pearson"; }

		string temp = validParameter.validFile(parameters, "cutoff", false);
		if (temp == "not found") { temp = "10"; }
		Utility::mothurConvert(temp, cutoff);

		if ((method != "pearson") && (method != "spearman") && (method != "kendall")) { LOG(INFO) << method + " is not a valid method. Valid methods are pearson, spearman, and kendall." << '\n'; abort = true; }

	}
}
//**********************************************************************************************************************

int OTUAssociationCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	if (metadatafile != "") { readMetadata(); }

	//function are identical just different datatypes
	if (sharedfile != "") { processShared(); }
	else if (relabundfile != "") { processRelabund(); }

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
//**********************************************************************************************************************
int OTUAssociationCommand::processShared() {
	InputData* input = new InputData(sharedfile, "sharedfile");
	vector<SharedRAbundVector*> lookup = input->getSharedRAbundVectors();
	string lastLabel = lookup[0]->getLabel();

	if (metadatafile != "") {
		getMetadata();
		bool error = false;
		if (metadata[0].size() != lookup.size()) { LOG(LOGERROR) << "You have selected to use " + toString(metadata[0].size()) + " data rows from the metadata file, but " + toString(lookup.size()) + " from the shared file.\n";  ctrlc_pressed = true; error = true; }
		if (error) {
			//maybe add extra info here?? compare groups in each file??
		}
	}

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	//as long as you are not at the end of the file or done wih the lines you want
	while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

		if (ctrlc_pressed) { delete input; return 0; }

		if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {
			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());

			LOG(INFO) << lookup[0]->getLabel() << '\n';
			process(lookup);
		}

		if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = lookup[0]->getLabel();

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input->getSharedRAbundVectors(lastLabel);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());

			//restore real lastlabel to save below
			lookup[0]->setLabel(saveLabel);

			LOG(INFO) << lookup[0]->getLabel() << '\n';
			process(lookup);
		}

		lastLabel = lookup[0]->getLabel();

		//get next line to process
		//prevent memory leak
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		lookup = input->getSharedRAbundVectors();
	}


	if (ctrlc_pressed) { delete input; return 0; }

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
		lookup = input->getSharedRAbundVectors(lastLabel);

		LOG(INFO) << lookup[0]->getLabel() << '\n';
		process(lookup);
	}

	delete input;

	return 0;
}
//**********************************************************************************************************************
int OTUAssociationCommand::process(vector<SharedRAbundVector*>& lookup) {
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputFileName));
	variables["[distance]"] = lookup[0]->getLabel();
	variables["[tag]"] = method;
	string outputFileName = getOutputFileName("otucorr", variables);
	outputNames.push_back(outputFileName); outputTypes["otucorr"].push_back(outputFileName);

	ofstream out;
	File::openOutputFile(outputFileName, out);
	out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);

	//column headings
	if (metadatafile == "") { out << "OTUA\tOTUB\t" << method << "Coef\tSignificance\n"; }
	else { out << "OTUA\tMetadata\t" << method << "Coef\tSignificance\n"; }


	vector< vector<double> > xy; xy.resize(lookup[0]->getNumBins());
	for (int i = 0; i < lookup[0]->getNumBins(); i++) { for (int j = 0; j < lookup.size(); j++) { xy[i].push_back(lookup[j]->getAbundance(i)); } }

	LinearAlgebra linear;
	if (metadatafile == "") {//compare otus
		for (int i = 0; i < xy.size(); i++) {

			for (int k = 0; k < i; k++) {

				if (ctrlc_pressed) { out.close(); return 0; }

				double coef = 0.0;
				double sig = 0.0;
				if (method == "spearman") { coef = linear.calcSpearman(xy[i], xy[k], sig); }
				else if (method == "pearson") { coef = linear.calcPearson(xy[i], xy[k], sig); }
				else if (method == "kendall") { coef = linear.calcKendall(xy[i], xy[k], sig); }
				else { LOG(LOGERROR) << "invalid method, choices are spearman, pearson or kendall." << '\n'; ctrlc_pressed = true; }

				if (sig < cutoff) { out << settings.currentSharedBinLabels[i] << '\t' << settings.currentSharedBinLabels[k] << '\t' << coef << '\t' << sig << endl; }
			}
		}
	}
	else { //compare otus to metadata
		for (int i = 0; i < xy.size(); i++) {

			for (int k = 0; k < metadata.size(); k++) {

				if (ctrlc_pressed) { out.close(); return 0; }

				double coef = 0.0;
				double sig = 0.0;
				if (method == "spearman") { coef = linear.calcSpearman(xy[i], metadata[k], sig); }
				else if (method == "pearson") { coef = linear.calcPearson(xy[i], metadata[k], sig); }
				else if (method == "kendall") { coef = linear.calcKendall(xy[i], metadata[k], sig); }
				else { LOG(LOGERROR) << "invalid method, choices are spearman, pearson or kendall." << '\n'; ctrlc_pressed = true; }

				if (sig < cutoff) { out << settings.currentSharedBinLabels[i] << '\t' << metadataLabels[k] << '\t' << coef << '\t' << sig << endl; }
			}
		}

	}
	out.close();


	return 0;

}
//**********************************************************************************************************************
int OTUAssociationCommand::processRelabund() {
	InputData* input = new InputData(relabundfile, "relabund");
	vector<SharedRAbundFloatVector*> lookup = input->getSharedRAbundFloatVectors();
	string lastLabel = lookup[0]->getLabel();

	if (metadatafile != "") {
		getMetadata();
		bool error = false;
		if (metadata[0].size() != lookup.size()) { LOG(LOGERROR) << "You have selected to use " + toString(metadata[0].size()) + " data rows from the metadata file, but " + toString(lookup.size()) + " from the relabund file.\n";  ctrlc_pressed = true; error = true; }
		if (error) {
			//maybe add extra info here?? compare groups in each file??
		}
	}



	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	//as long as you are not at the end of the file or done wih the lines you want
	while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

		if (ctrlc_pressed) { delete input; return 0; }

		if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {
			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());

			LOG(INFO) << lookup[0]->getLabel() << '\n';
			process(lookup);
		}

		if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = lookup[0]->getLabel();

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input->getSharedRAbundFloatVectors(lastLabel);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());

			//restore real lastlabel to save below
			lookup[0]->setLabel(saveLabel);

			LOG(INFO) << lookup[0]->getLabel() << '\n';
			process(lookup);
		}

		lastLabel = lookup[0]->getLabel();

		//get next line to process
		//prevent memory leak
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		lookup = input->getSharedRAbundFloatVectors();
	}


	if (ctrlc_pressed) { delete input; return 0; }

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
		lookup = input->getSharedRAbundFloatVectors(lastLabel);

		LOG(INFO) << lookup[0]->getLabel() << '\n';
		process(lookup);
	}

	delete input;

	return 0;
}
//**********************************************************************************************************************
int OTUAssociationCommand::process(vector<SharedRAbundFloatVector*>& lookup) {

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputFileName));
	variables["[distance]"] = lookup[0]->getLabel();
	variables["[tag]"] = method;
	string outputFileName = getOutputFileName("otucorr", variables);
	outputNames.push_back(outputFileName); outputTypes["otucorr"].push_back(outputFileName);

	ofstream out;
	File::openOutputFile(outputFileName, out);
	out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);

	//column headings
	if (metadatafile == "") { out << "OTUA\tOTUB\t" << method << "Coef\tSignificance\n"; }
	else { out << "OTUA\tMetadata\t" << method << "Coef\tSignificance\n"; }

	vector< vector<double> > xy; xy.resize(lookup[0]->getNumBins());
	for (int i = 0; i < lookup[0]->getNumBins(); i++) { for (int j = 0; j < lookup.size(); j++) { xy[i].push_back(lookup[j]->getAbundance(i)); } }

	LinearAlgebra linear;
	if (metadatafile == "") {//compare otus
		for (int i = 0; i < xy.size(); i++) {

			for (int k = 0; k < i; k++) {

				if (ctrlc_pressed) { out.close(); return 0; }

				double coef = 0.0;
				double sig = 0.0;
				if (method == "spearman") { coef = linear.calcSpearman(xy[i], xy[k], sig); }
				else if (method == "pearson") { coef = linear.calcPearson(xy[i], xy[k], sig); }
				else if (method == "kendall") { coef = linear.calcKendall(xy[i], xy[k], sig); }
				else { LOG(LOGERROR) << "invalid method, choices are spearman, pearson or kendall." << '\n'; ctrlc_pressed = true; }

				if (sig < cutoff) { out << settings.currentSharedBinLabels[i] << '\t' << settings.currentSharedBinLabels[k] << '\t' << coef << '\t' << sig << endl; }
			}
		}
	}
	else { //compare otus to metadata
		for (int i = 0; i < xy.size(); i++) {

			for (int k = 0; k < metadata.size(); k++) {

				if (ctrlc_pressed) { out.close(); return 0; }

				double coef = 0.0;
				double sig = 0.0;
				if (method == "spearman") { coef = linear.calcSpearman(xy[i], metadata[k], sig); }
				else if (method == "pearson") { coef = linear.calcPearson(xy[i], metadata[k], sig); }
				else if (method == "kendall") { coef = linear.calcKendall(xy[i], metadata[k], sig); }
				else { LOG(LOGERROR) << "invalid method, choices are spearman, pearson or kendall." << '\n'; ctrlc_pressed = true; }

				if (sig < cutoff) { out << settings.currentSharedBinLabels[i] << '\t' << metadataLabels[k] << '\t' << coef << '\t' << sig << endl; }
			}
		}

	}

	out.close();

	return 0;

}
/*****************************************************************/
int OTUAssociationCommand::readMetadata() {
	ifstream in;
	File::openInputFile(metadatafile, in);

	string headerLine = File::getline(in); File::gobble(in);
	istringstream iss(headerLine, istringstream::in);

	//read the first label, because it refers to the groups
	string columnLabel;
	iss >> columnLabel; File::gobble(iss);

	//save names of columns you are reading
	while (!iss.eof()) {
		iss >> columnLabel; File::gobble(iss);
		if (app.isDebug) { LOG(DEBUG) << "metadata column Label = " + columnLabel + "\n"; }
		metadataLabels.push_back(columnLabel);
	}
	int count = metadataLabels.size();

	//read rest of file
	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); return 0; }

		string group = "";
		in >> group; File::gobble(in);
		if (app.isDebug) { LOG(DEBUG) << "metadata group = " + group + "\n"; }

		SharedRAbundFloatVector* tempLookup = new SharedRAbundFloatVector();
		tempLookup->setGroup(group);
		tempLookup->setLabel("1");

		for (int i = 0; i < count; i++) {
			float temp = 0.0;
			in >> temp;
			if (app.isDebug) { LOG(DEBUG) << "metadata value = " + toString(temp) + "\n"; }
			tempLookup->push_back(temp, group);
		}

		metadataLookup.push_back(tempLookup);

		File::gobble(in);
	}
	in.close();

	return 0;
}
/*****************************************************************/
//eliminate groups user did not pick, remove zeroed out otus, fill metadata vector.
int OTUAssociationCommand::getMetadata() {

	vector<string> mGroups = settings.getGroups();

	bool remove = false;
	for (int i = 0; i < metadataLookup.size(); i++) {
		//if this sharedrabund is not from a group the user wants then delete it.
		if (!(m->inUsersGroups(metadataLookup[i]->getGroup(), mGroups))) {
			delete metadataLookup[i]; metadataLookup[i] = NULL;
			metadataLookup.erase(metadataLookup.begin() + i);
			i--;
			remove = true;
		}
	}

	vector<SharedRAbundFloatVector*> newLookup;
	for (int i = 0; i < metadataLookup.size(); i++) {
		SharedRAbundFloatVector* temp = new SharedRAbundFloatVector();
		temp->setLabel(metadataLookup[i]->getLabel());
		temp->setGroup(metadataLookup[i]->getGroup());
		newLookup.push_back(temp);
	}

	//for each bin
	vector<string> newBinLabels;
	for (int i = 0; i < metadataLookup[0]->getNumBins(); i++) {
		if (ctrlc_pressed) { for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; } return 0; }

		//look at each sharedRabund and make sure they are not all zero
		bool allZero = true;
		for (int j = 0; j < metadataLookup.size(); j++) {
			if (metadataLookup[j]->getAbundance(i) != 0) { allZero = false;  break; }
		}

		//if they are not all zero add this bin
		if (!allZero) {
			for (int j = 0; j < metadataLookup.size(); j++) {
				newLookup[j]->push_back(metadataLookup[j]->getAbundance(i), metadataLookup[j]->getGroup());
			}
			newBinLabels.push_back(metadataLabels[i]);
		}
	}

	metadataLabels = newBinLabels;

	for (int j = 0; j < metadataLookup.size(); j++) { delete metadataLookup[j]; }
	metadataLookup.clear();

	metadata.resize(newLookup[0]->getNumBins());
	for (int i = 0; i < newLookup[0]->getNumBins(); i++) { for (int j = 0; j < newLookup.size(); j++) { metadata[i].push_back(newLookup[j]->getAbundance(i)); } }

	for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; }

	return 0;
}
/*****************************************************************/








