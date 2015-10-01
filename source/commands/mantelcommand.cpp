/*
 *  mantelcommand.cpp
 *  mothur
 *
 *  Created by westcott on 2/9/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "mantelcommand.h"
#include "readphylipvector.h"


 //**********************************************************************************************************************
vector<string> MantelCommand::setParameters() {
	try {
		CommandParameter pphylip1("phylip1", "InputTypes", "", "", "none", "none", "none", "mantel", false, true, true); parameters.push_back(pphylip1);
		CommandParameter pphylip2("phylip2", "InputTypes", "", "", "none", "none", "none", "mantel", false, true, true); parameters.push_back(pphylip2);
		nkParameters.add(new NumberParameter("iters", -INFINITY, INFINITY, 1000, false, false));
		CommandParameter pmethod("method", "Multiple", "pearson-spearman-kendall", "pearson", "", "", "", "", false, false); parameters.push_back(pmethod);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MantelCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string MantelCommand::getHelpString() {
	try {
		string helpString = "Sokal, R. R., & Rohlf, F. J. (1995). Biometry, 3rd edn. New York: Freeman.\n"
			"The mantel command reads two distance matrices and calculates the mantel correlation coefficient.\n"
			"The mantel command parameters are phylip1, phylip2, iters and method.  The phylip1 and phylip2 parameters are required.  Matrices must be the same size and contain the same names.\n"
			"The method parameter allows you to select what method you would like to use. Options are pearson, spearman and kendall. Default=pearson.\n"
			"The iters parameter allows you to set number of randomization for the P value.  The default is 1000. \n"
			"The mantel command should be in the following format: mantel(phylip1=veg.dist, phylip2=env.dist).\n"
			"The mantel command outputs a .mantel file.\n"
			"Note: No spaces between parameter labels (i.e. phylip1), '=' and parameters (i.e. veg.dist).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MantelCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string MantelCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "mantel") { pattern = "[filename],mantel"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
MantelCommand::MantelCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["mantel"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in MantelCommand, MantelCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
MantelCommand::MantelCommand(Settings& settings, string option) : Command(settings, option) {
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

		vector<string> tempOutNames;
		outputTypes["mantel"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("phylip1");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["phylip1"] = inputDir + it->second; }
			}

			it = parameters.find("phylip2");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["phylip2"] = inputDir + it->second; }
			}
		}


		//check for required parameters
		phylipfile1 = validParameter.validFile(parameters, "phylip1", true);
		if (phylipfile1 == "not open") { phylipfile1 = ""; abort = true; }
		else if (phylipfile1 == "not found") { phylipfile1 = ""; LOG(INFO) << "phylip1 is a required parameter for the mantel command." << '\n'; abort = true; }

		phylipfile2 = validParameter.validFile(parameters, "phylip2", true);
		if (phylipfile2 == "not open") { phylipfile2 = ""; abort = true; }
		else if (phylipfile2 == "not found") { phylipfile2 = ""; LOG(INFO) << "phylip2 is a required parameter for the mantel command." << '\n'; abort = true; }

		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(phylipfile1); }

		method = validParameter.validFile(parameters, "method", false);		if (method == "not found") { method = "pearson"; }

		string temp = validParameter.validFile(parameters, "iters", false);			if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, iters);

		if ((method != "pearson") && (method != "spearman") && (method != "kendall")) { LOG(INFO) << method + " is not a valid method. Valid methods are pearson, spearman, and kendall." << '\n'; abort = true; }
	}
}
//**********************************************************************************************************************

int MantelCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	/***************************************************/
	//    reading distance files					   //
	/***************************************************/

	//read phylip1
	ReadPhylipVector readMatrix(phylipfile1);
	vector< vector<double> > matrix1;
	vector<string> names1 = readMatrix.read(matrix1);

	if (ctrlc_pressed) { return 0; }

	//read phylip2
	ReadPhylipVector readMatrix2(phylipfile2);
	vector< vector<double> > matrix2;
	vector<string> names2 = readMatrix2.read(matrix2);

	if (ctrlc_pressed) { return 0; }

	//make sure matrix2 and matrix1 are in the same order
	if (names1 == names2) { //then everything is in same order and same size
	}
	else if (names1.size() != names2.size()) { //wrong size no need to order, abort
		LOG(LOGERROR) << "distance matrices are not the same size, aborting." << '\n';
		ctrlc_pressed = true;
	}
	else { //sizes are the same, but either the names are different or they are in different order
		LOG(WARNING) << "Names do not match between distance files. Comparing based on order in files." << '\n';
	}

	if (ctrlc_pressed) { return 0; }

	/***************************************************/
	//    calculating mantel and signifigance		   //
	/***************************************************/

	//calc mantel coefficient
	LinearAlgebra linear;
	double mantel = 0.0;
	if (method == "pearson") { mantel = linear.calcPearson(matrix1, matrix2); }
	else if (method == "spearman") { mantel = linear.calcSpearman(matrix1, matrix2); }
	else if (method == "kendall") { mantel = linear.calcKendall(matrix1, matrix2); }


	//calc signifigance
	int count = 0;
	for (int i = 0; i < iters; i++) {

		if (ctrlc_pressed) { return 0; }

		//randomize matrix2
		vector< vector<double> > matrix2Copy = matrix2;
		random_shuffle(matrix2Copy.begin(), matrix2Copy.end());

		//calc random mantel
		double randomMantel = 0.0;
		if (method == "pearson") { randomMantel = linear.calcPearson(matrix1, matrix2Copy); }
		else if (method == "spearman") { randomMantel = linear.calcSpearman(matrix1, matrix2Copy); }
		else if (method == "kendall") { randomMantel = linear.calcKendall(matrix1, matrix2Copy); }

		if (randomMantel >= mantel) { count++; }
	}

	double pValue = count / (float)iters;

	if (ctrlc_pressed) { return 0; }

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(phylipfile1));
	string outputFile = getOutputFileName("mantel", variables);
	outputNames.push_back(outputFile); outputTypes["mantel"].push_back(outputFile);
	ofstream out;

	File::openOutputFile(outputFile, out);

	out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);
	cout.setf(ios::fixed, ios::floatfield); cout.setf(ios::showpoint);

	out << "Mantel\tpValue" << endl;
	out << mantel << '\t' << pValue << endl;

	out.close();

	cout << "\nmantel = " << mantel << "\tpValue = " << pValue << endl;
	LOG(FILEONLY) << "\nmantel = " + toString(mantel) + "\tpValue = " + toString(pValue) + "\n";

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}

//**********************************************************************************************************************


