/*
 *  pcacommand.cpp
 *  mothur
 *
 *  Created by westcott on 1/7/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "pcacommand.h"
#include "inputdata.h"

 //**********************************************************************************************************************
vector<string> PCACommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "LRSS", "LRSS", "none", "pca-loadings", false, false, true); parameters.push_back(pshared);
		CommandParameter prelabund("relabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "pca-loadings", false, false, true); parameters.push_back(prelabund);
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new BooleanParameter("metric", true, false, false));
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PCACommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string PCACommand::getHelpString() {
	try {
		string helpString = "The pca command parameters are shared, relabund, label, groups and metric.  shared or relabund is required unless you have a valid current file.";
		helpString += "The label parameter is used to analyze specific labels in your input. Default is the first label in your shared or relabund file. Multiple labels may be separated by dashes.\n"
			"The groups parameter allows you to specify which groups you would like analyzed. Groupnames are separated by dashes.\n"
			"The metric parameter allows you to indicate if would like the pearson correlation coefficient calculated. Default=True"
			"Example pca(groups=yourGroups).\n"
			"Example pca(groups=A-B-C).\n"
			"Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PCACommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string PCACommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "pca") { pattern = "[filename],[distance],pca.axes"; }
	else if (type == "loadings") { pattern = "[filename],[distance],pca.loadings"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}

//**********************************************************************************************************************
PCACommand::PCACommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["pca"] = tempOutNames;
		outputTypes["loadings"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PCACommand, PCACommand";
		exit(1);
	}
}
//**********************************************************************************************************************

PCACommand::PCACommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["pca"] = tempOutNames;
		outputTypes["loadings"] = tempOutNames;

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
		}

		//check for required parameters
		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = ""; abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { mode = "sharedfile"; inputFile = sharedfile; settings.setCurrent("shared", sharedfile); }

		relabundfile = validParameter.validFile(parameters, "relabund", true);
		if (relabundfile == "not open") { relabundfile = ""; abort = true; }
		else if (relabundfile == "not found") { relabundfile = ""; }
		else { mode = "relabund"; inputFile = relabundfile; settings.setCurrent("relabund", relabundfile); }


		if ((sharedfile == "") && (relabundfile == "")) {
			//is there are current file available for any of these?
			//give priority to shared, then list, then rabund, then sabund
			//if there is a current shared file, use it
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { inputFile = sharedfile; mode = "sharedfile"; LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else {
				relabundfile = settings.getCurrent("relabund");
				if (relabundfile != "") { inputFile = relabundfile; mode = "relabund"; LOG(INFO) << "Using " + relabundfile + " as input file for the relabund parameter." << '\n'; }
				else {
					LOG(INFO) << "No valid current files. You must provide a relabund or shared file." << '\n';
					abort = true;
				}
			}
		}

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = "";
			outputDir += File::getPath(inputFile); //if user entered a file with a path then preserve it	
		}

		string temp = validParameter.validFile(parameters, "metric", false);	if (temp == "not found") { temp = "T"; }
		metric = m->isTrue(temp);

		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; if (labels.size() == 0) { LOG(INFO) << "You did not provide a label, I will use the first label in your inputfile." << '\n'; } }
		else { Utility::split(label, '-', labels); }

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else { Utility::split(groups, '-', Groups); }
		m->setGroups(Groups);

	}

}
//**********************************************************************************************************************
int PCACommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::showpoint);
	cerr.setf(ios::fixed, ios::floatfield);
	cerr.setf(ios::showpoint);

	//get first line of shared file
	vector< vector<double> > matrix;
	InputData* input;
	if (mode == "sharedfile") {
		input = new InputData(inputFile, "sharedfile");
	}
	else if (mode == "relabund") {
		input = new InputData(inputFile, "relabund");
	}
	else { LOG(LOGERROR) << "filetype not recognized." << '\n';  return 0; }

	vector<SharedRAbundFloatVector*> lookupFloat = input->getSharedRAbundFloatVectors();
	string lastLabel = lookupFloat[0]->getLabel();

	set<string> processedLabels;
	set<string> userLabels = labels;

	//if the user gave no labels, then use the first one read
	if (labels.size() == 0) {
		label = lastLabel;

		process(lookupFloat);
	}

	//as long as you are not at the end of the file or done wih the lines you want
	while ((lookupFloat[0] != NULL) && (userLabels.size() != 0)) {

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } delete input; for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; }  lookupFloat.clear(); return 0; }

		if (labels.count(lookupFloat[0]->getLabel()) == 1) {
			processedLabels.insert(lookupFloat[0]->getLabel());
			userLabels.erase(lookupFloat[0]->getLabel());

			process(lookupFloat);
		}

		if ((Utility::anyLabelsToProcess(lookupFloat[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = lookupFloat[0]->getLabel();

			for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; }  lookupFloat.clear();
			lookupFloat = input->getSharedRAbundFloatVectors(lastLabel);

			process(lookupFloat);

			processedLabels.insert(lookupFloat[0]->getLabel());
			userLabels.erase(lookupFloat[0]->getLabel());

			//restore real lastlabel to save below
			lookupFloat[0]->setLabel(saveLabel);
		}

		lastLabel = lookupFloat[0]->getLabel();

		//get next line to process
		//prevent memory leak
		for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; } lookupFloat.clear();
		lookupFloat = input->getSharedRAbundFloatVectors();
	}


	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } delete input; for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; } lookupFloat.clear(); return 0; }

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
		for (int i = 0; i < lookupFloat.size(); i++) { if (lookupFloat[i] != NULL) { delete lookupFloat[i]; } }  lookupFloat.clear();
		lookupFloat = input->getSharedRAbundFloatVectors(lastLabel);

		process(lookupFloat);

		for (int i = 0; i < lookupFloat.size(); i++) { if (lookupFloat[i] != NULL) { delete lookupFloat[i]; } } lookupFloat.clear();
	}

	for (int i = 0; i < lookupFloat.size(); i++) { if (lookupFloat[i] != NULL) { delete lookupFloat[i]; } } lookupFloat.clear();
	delete input;

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}

/**********************************************************************************************************************
vector< vector<double> > PCACommand::createMatrix(vector<SharedRAbundFloatVector*> lookupFloat){
		vector< vector<double> > matrix; matrix.resize(lookupFloat.size());

		//fill matrix with shared files relative abundances
		for (int i = 0; i < lookupFloat.size(); i++) {
			for (int j = 0; j < lookupFloat[i]->getNumBins(); j++) {
				matrix[i].push_back(lookupFloat[i]->getAbundance(j));
			}
		}

		vector< vector<double> > transposeMatrix; transposeMatrix.resize(matrix[0].size());
		for (int i = 0; i < transposeMatrix.size(); i++) {
			for (int j = 0; j < matrix.size(); j++) {
				transposeMatrix[i].push_back(matrix[j][i]);
			}
		}

		matrix = linearCalc.matrix_mult(matrix, transposeMatrix);

		return matrix;
}*/
//**********************************************************************************************************************

int PCACommand::process(vector<SharedRAbundFloatVector*>& lookupFloat) {
	LOG(INFO) << "\nProcessing " + lookupFloat[0]->getLabel() << '\n';

	int numOTUs = lookupFloat[0]->getNumBins();
	int numSamples = lookupFloat.size();

	vector< vector<double> > matrix(numSamples);
	vector<double> colMeans(numOTUs);

	//fill matrix with shared relative abundances, re-center
	for (int i = 0; i < lookupFloat.size(); i++) {
		matrix[i].resize(numOTUs, 0);

		for (int j = 0; j < numOTUs; j++) {
			matrix[i][j] = lookupFloat[i]->getAbundance(j);
			colMeans[j] += matrix[i][j];
		}
	}


	for (int j = 0;j < numOTUs;j++) {
		colMeans[j] = colMeans[j] / (double)numSamples;
	}

	vector<vector<double> > centered = matrix;
	for (int i = 0;i < numSamples;i++) {
		for (int j = 0;j < numOTUs;j++) {
			centered[i][j] = centered[i][j] - colMeans[j];
		}
	}


	vector< vector<double> > transpose(numOTUs);
	for (int i = 0; i < numOTUs; i++) {
		transpose[i].resize(numSamples, 0);

		for (int j = 0; j < numSamples; j++) {
			transpose[i][j] = centered[j][i];
		}
	}

	vector<vector<double> > crossProduct = linearCalc.matrix_mult(transpose, centered);

	vector<double> d;
	vector<double> e;

	linearCalc.tred2(crossProduct, d, e);		if (ctrlc_pressed) { return 0; }
	linearCalc.qtli(d, e, crossProduct);		if (ctrlc_pressed) { return 0; }

	vector<vector<double> > X = linearCalc.matrix_mult(centered, crossProduct);

	if (ctrlc_pressed) { return 0; }

	string fbase = outputDir + File::getRootName(File::getSimpleName(inputFile));
	//string outputFileName = fbase + lookupFloat[0]->getLabel();
	output(fbase, lookupFloat[0]->getLabel(), settings.getGroups(), X, d);

	if (metric) {

		vector<vector<double> > observedEuclideanDistance = linearCalc.getObservedEuclideanDistance(centered);

		for (int i = 1; i < 4; i++) {

			vector< vector<double> > PCAEuclidDists = linearCalc.calculateEuclidianDistance(X, i); //G is the pca file

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

			double corr = linearCalc.calcPearson(PCAEuclidDists, observedEuclideanDistance);

			LOG(INFO) << "Rsq " + toString(i) + " axis: " + toString(corr * corr) << '\n';

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }
		}
	}

	return 0;
}
/*********************************************************************************************************************************/

void PCACommand::output(string fbase, string label, vector<string> name_list, vector<vector<double> >& G, vector<double> d) {

	int numEigenValues = d.size();
	double dsum = 0.0000;
	for (int i = 0;i < numEigenValues;i++) {
		dsum += d[i];
	}

	ofstream pcaData;
	map<string, string> variables;
	variables["[filename]"] = fbase;
	variables["[distance]"] = label;
	string pcaFileName = getOutputFileName("pca", variables);
	File::openOutputFile(pcaFileName, pcaData);
	pcaData.setf(ios::fixed, ios::floatfield);
	pcaData.setf(ios::showpoint);
	outputNames.push_back(pcaFileName);
	outputTypes["pca"].push_back(pcaFileName);

	ofstream pcaLoadings;
	string loadingsFilename = getOutputFileName("loadings", variables);
	File::openOutputFile(loadingsFilename, pcaLoadings);
	pcaLoadings.setf(ios::fixed, ios::floatfield);
	pcaLoadings.setf(ios::showpoint);
	outputNames.push_back(loadingsFilename);
	outputTypes["loadings"].push_back(loadingsFilename);

	pcaLoadings << "axis\tloading\n";
	for (int i = 0;i < numEigenValues;i++) {
		pcaLoadings << i + 1 << '\t' << d[i] * 100.0 / dsum << endl;
	}

	pcaData << "group";
	for (int i = 0;i < numEigenValues;i++) {
		pcaData << '\t' << "axis" << i + 1;
	}
	pcaData << endl;

	for (int i = 0;i < name_list.size();i++) {
		pcaData << name_list[i];
		for (int j = 0;j < numEigenValues;j++) { pcaData << '\t' << G[i][j]; }
		pcaData << endl;
	}
}
/*********************************************************************************************************************************/


