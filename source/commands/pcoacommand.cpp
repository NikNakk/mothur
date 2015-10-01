
/*
 *  pcacommand.cpp
 *  Mothur
 *
 *  Created by westcott on 1/4/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "pcoacommand.h"
#include "readphylipvector.h"


 //**********************************************************************************************************************
vector<string> PCOACommand::setParameters() {
	try {
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "none", "none", "pcoa-loadings", false, true, true); parameters.push_back(pphylip);
		nkParameters.add(new BooleanParameter("metric", true, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PCOACommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string PCOACommand::getHelpString() {
	try {
		string helpString = "The pcoa command parameters are phylip and metric";
		helpString += "The phylip parameter allows you to enter your distance file."
			"The metric parameter allows indicate you if would like the pearson correlation coefficient calculated. Default=True";
		helpString += "Example pcoa(phylip=yourDistanceFile).\n"
			"Note: No spaces between parameter labels (i.e. phylip), '=' and parameters (i.e.yourDistanceFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PCOACommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string PCOACommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "pcoa") { pattern = "[filename],pcoa.axes"; }
	else if (type == "loadings") { pattern = "[filename],pcoa.loadings"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}


//**********************************************************************************************************************
PCOACommand::PCOACommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["pcoa"] = tempOutNames;
		outputTypes["loadings"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in PCOACommand, PCOACommand";
		exit(1);
	}
}
//**********************************************************************************************************************

PCOACommand::PCOACommand(Settings& settings, string option) : Command(settings, option) {
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
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["pcoa"] = tempOutNames;
		outputTypes["loadings"] = tempOutNames;

		//required parameters
		phylipfile = validParameter.validFile(parameters, "phylip", true);
		if (phylipfile == "not open") { abort = true; }
		else if (phylipfile == "not found") {
			//if there is a current phylip file, use it
			phylipfile = settings.getCurrent("phylip");
			if (phylipfile != "") { LOG(INFO) << "Using " + phylipfile + " as input file for the phylip parameter." << '\n'; }
			else { LOG(INFO) << "You have no current phylip file and the phylip parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("phylip", phylipfile); }

		filename = phylipfile;

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = "";
			outputDir += File::getPath(phylipfile); //if user entered a file with a path then preserve it	
		}

		string temp = validParameter.validFile(parameters, "metric", false);	if (temp == "not found") { temp = "T"; }
		metric = m->isTrue(temp);
	}

}
//**********************************************************************************************************************
int PCOACommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::showpoint);
	cerr.setf(ios::fixed, ios::floatfield);
	cerr.setf(ios::showpoint);

	vector<string> names;
	vector<vector<double> > D;

	fbase = outputDir + File::getRootName(File::getSimpleName(filename));

	ReadPhylipVector readFile(filename);
	names = readFile.read(D);

	if (ctrlc_pressed) { return 0; }

	double offset = 0.0000;
	vector<double> d;
	vector<double> e;
	vector<vector<double> > G = D;
	//vector<vector<double> > copy_G;

	LOG(INFO) << "\nProcessing...\n";

	for (int count = 0;count < 2;count++) {
		linearCalc.recenter(offset, D, G);		if (ctrlc_pressed) { return 0; }
		linearCalc.tred2(G, d, e);				if (ctrlc_pressed) { return 0; }
		linearCalc.qtli(d, e, G);				if (ctrlc_pressed) { return 0; }
		offset = d[d.size() - 1];
		if (offset > 0.0) break;
	}

	if (ctrlc_pressed) { return 0; }

	output(fbase, names, G, d);

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	if (metric) {

		for (int i = 1; i < 4; i++) {

			vector< vector<double> > EuclidDists = linearCalc.calculateEuclidianDistance(G, i); //G is the pcoa file

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

			double corr = linearCalc.calcPearson(EuclidDists, D); //G is the pcoa file, D is the users distance matrix

			LOG(INFO) << "Rsq " + toString(i) + " axis: " + toString(corr * corr) << '\n';

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }
		}
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
/*********************************************************************************************************************************/

void PCOACommand::get_comment(istream& f, char begin, char end) {
	char d = f.get();
	while (d != end) { d = f.get(); }
	d = f.peek();
}
/*********************************************************************************************************************************/

void PCOACommand::output(string fnameRoot, vector<string> name_list, vector<vector<double> >& G, vector<double> d) {
	int rank = name_list.size();
	double dsum = 0.0000;
	for (int i = 0;i < rank;i++) {
		dsum += d[i];
		for (int j = 0;j < rank;j++) {
			if (d[j] >= 0) { G[i][j] *= pow(d[j], 0.5); }
			else { G[i][j] = 0.00000; }
		}
	}

	ofstream pcaData;
	map<string, string> variables;
	variables["[filename]"] = fnameRoot;
	string pcoaDataFile = getOutputFileName("pcoa", variables);
	File::openOutputFile(pcoaDataFile, pcaData);
	pcaData.setf(ios::fixed, ios::floatfield);
	pcaData.setf(ios::showpoint);
	outputNames.push_back(pcoaDataFile);
	outputTypes["pcoa"].push_back(pcoaDataFile);

	ofstream pcaLoadings;
	string loadingsFile = getOutputFileName("loadings", variables);
	File::openOutputFile(loadingsFile, pcaLoadings);
	pcaLoadings.setf(ios::fixed, ios::floatfield);
	pcaLoadings.setf(ios::showpoint);
	outputNames.push_back(loadingsFile);
	outputTypes["loadings"].push_back(loadingsFile);

	pcaLoadings << "axis\tloading\n";
	for (int i = 0;i < rank;i++) {
		pcaLoadings << i + 1 << '\t' << d[i] * 100.0 / dsum << endl;
	}

	pcaData << "group";
	for (int i = 0;i < rank;i++) {
		pcaData << '\t' << "axis" << i + 1;
	}
	pcaData << endl;

	for (int i = 0;i < rank;i++) {
		pcaData << name_list[i];
		for (int j = 0;j < rank;j++) {
			pcaData << '\t' << G[i][j];
		}
		pcaData << endl;
	}
}

/*********************************************************************************************************************************/

