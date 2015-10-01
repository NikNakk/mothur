/*
 *  nmdscommand.cpp
 *  mothur
 *
 *  Created by westcott on 1/11/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "nmdscommand.h"
#include "readphylipvector.h"

 //**********************************************************************************************************************
vector<string> NMDSCommand::setParameters() {
	try {
		CommandParameter paxes("axes", "InputTypes", "", "", "none", "none", "none", "", false, false, true); parameters.push_back(paxes);
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "none", "none", "nmds-stress", false, true, true); parameters.push_back(pphylip);
		nkParameters.add(new NumberParameter("maxdim", -INFINITY, INFINITY, 2, false, false));
		nkParameters.add(new NumberParameter("mindim", -INFINITY, INFINITY, 2, false, false));
		nkParameters.add(new NumberParameter("iters", -INFINITY, INFINITY, 10, false, false));
		nkParameters.add(new NumberParameter("maxiters", -INFINITY, INFINITY, 500, false, false));
		nkParameters.add(new NumberParameter("epsilon", -INFINITY, INFINITY, 0.000000000001, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in NMDSCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string NMDSCommand::getHelpString() {
	try {
		string helpString = "The nmds command is modelled after the nmds code written in R by Sarah Goslee, using Non-metric multidimensional scaling function using the majorization algorithm from Borg & Groenen 1997, Modern Multidimensional Scaling.\n"
			"The nmds command parameters are phylip, axes, mindim, maxdim, maxiters, iters and epsilon.\n";
		helpString += "The phylip parameter allows you to enter your distance file.\n";
		helpString += "The axes parameter allows you to enter a file containing a starting configuration.\n"
			"The maxdim parameter allows you to select the maximum dimensions to use. Default=2\n";
		helpString += "The mindim parameter allows you to select the minimum dimensions to use. Default=2\n"
			"The maxiters parameter allows you to select the maximum number of iters to try with each random configuration. Default=500\n";
		helpString += "The iters parameter allows you to select the number of random configuration to try. Default=10\n";
		helpString += "The epsilon parameter allows you to select set an acceptable stopping point. Default=1e-12.\n";
		helpString += "Example nmds(phylip=yourDistanceFile).\n"
			"Note: No spaces between parameter labels (i.e. phylip), '=' and parameters (i.e.yourDistanceFile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in NMDSCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string NMDSCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "nmds") { pattern = "[filename],nmds.axes"; }
	else if (type == "stress") { pattern = "[filename],nmds.stress"; }
	else if (type == "iters") { pattern = "[filename],nmds.iters"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}

//**********************************************************************************************************************
NMDSCommand::NMDSCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["nmds"] = tempOutNames;
		outputTypes["stress"] = tempOutNames;
		outputTypes["iters"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in NMDSCommand, NMDSCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

NMDSCommand::NMDSCommand(Settings& settings, string option) : Command(settings, option) {
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

			it = parameters.find("axes");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["axes"] = inputDir + it->second; }
			}
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["nmds"] = tempOutNames;
		outputTypes["iters"] = tempOutNames;
		outputTypes["stress"] = tempOutNames;

		//required parameters
		phylipfile = validParameter.validFile(parameters, "phylip", true);
		if (phylipfile == "not open") { phylipfile = ""; abort = true; }
		else if (phylipfile == "not found") {
			//if there is a current phylip file, use it
			phylipfile = settings.getCurrent("phylip");
			if (phylipfile != "") { LOG(INFO) << "Using " + phylipfile + " as input file for the phylip parameter." << '\n'; }
			else { LOG(INFO) << "You have no current phylip file and the phylip parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("phylip", phylipfile); }

		axesfile = validParameter.validFile(parameters, "axes", true);
		if (axesfile == "not open") { axesfile = ""; abort = true; }
		else if (axesfile == "not found") { axesfile = ""; }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = "";
			outputDir += File::getPath(phylipfile); //if user entered a file with a path then preserve it	
		}

		string temp = validParameter.validFile(parameters, "mindim", false);	if (temp == "not found") { temp = "2"; }
		Utility::mothurConvert(temp, mindim);

		temp = validParameter.validFile(parameters, "maxiters", false);	if (temp == "not found") { temp = "500"; }
		Utility::mothurConvert(temp, maxIters);

		temp = validParameter.validFile(parameters, "iters", false);	if (temp == "not found") { temp = "10"; }
		Utility::mothurConvert(temp, iters);

		temp = validParameter.validFile(parameters, "maxdim", false);	if (temp == "not found") { temp = "2"; }
		Utility::mothurConvert(temp, maxdim);

		temp = validParameter.validFile(parameters, "epsilon", false);	if (temp == "not found") { temp = "0.000000000001"; }
		Utility::mothurConvert(temp, epsilon);

		if (mindim < 1) { LOG(INFO) << "mindim must be at least 1." << '\n'; abort = true; }
		if (maxdim < mindim) { maxdim = mindim; }
	}

}
//**********************************************************************************************************************
int NMDSCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::showpoint);

	vector<string> names;
	vector< vector< double> > matrix;

	//read in phylip file
	ReadPhylipVector readFile(phylipfile);
	names = readFile.read(matrix);
	if (ctrlc_pressed) { return 0; }

	//read axes
	vector< vector<double> > axes;
	if (axesfile != "") { axes = readAxes(names); }

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(phylipfile));
	string outputFileName = getOutputFileName("iters", variables);
	string stressFileName = getOutputFileName("stress", variables);
	outputNames.push_back(outputFileName); outputTypes["iters"].push_back(outputFileName);
	outputNames.push_back(stressFileName); outputTypes["stress"].push_back(stressFileName);

	ofstream out, out2;
	File::openOutputFile(outputFileName, out);
	File::openOutputFile(stressFileName, out2);

	out2.setf(ios::fixed, ios::floatfield);
	out2.setf(ios::showpoint);
	out.setf(ios::fixed, ios::floatfield);
	out.setf(ios::showpoint);

	out2 << "Dimension\tIter\tStress\tRsq" << endl;

	double bestStress = 10000000;
	double bestR2 = 10000000;
	vector< vector<double> > bestConfig;
	int bestDim = 0;

	for (int i = mindim; i <= maxdim; i++) {
		LOG(INFO) << "Processing Dimension: " + toString(i) << '\n';

		for (int j = 0; j < iters; j++) {
			LOG(INFO) << toString(j + 1) << '\n';

			//get configuration - either randomly generate or resize to this dimension
			vector< vector<double> > thisConfig;
			if (axesfile == "") { thisConfig = generateStartingConfiguration(names.size(), i); }
			else { thisConfig = getConfiguration(axes, i); }
			if (ctrlc_pressed) { out.close(); out2.close(); for (int k = 0; k < outputNames.size(); k++) { File::remove(outputNames[k]); } return 0; }

			//calc nmds for this dimension
			double stress;
			vector< vector<double> > endConfig = nmdsCalc(matrix, thisConfig, stress);
			if (ctrlc_pressed) { out.close(); out2.close(); for (int k = 0; k < outputNames.size(); k++) { File::remove(outputNames[k]); } return 0; }

			//calc euclid distances for new config
			vector< vector<double> > newEuclid = linearCalc.calculateEuclidianDistance(endConfig);
			if (ctrlc_pressed) { out.close(); out2.close(); for (int k = 0; k < outputNames.size(); k++) { File::remove(outputNames[k]); } return 0; }

			//calc correlation between original distances and euclidean distances from this config
			double rsquared = linearCalc.calcPearson(newEuclid, matrix);
			rsquared *= rsquared;
			if (ctrlc_pressed) { out.close(); out2.close(); for (int k = 0; k < outputNames.size(); k++) { File::remove(outputNames[k]); } return 0; }

			//output results
			out << "Config" << (j + 1);
			for (int k = 0; k < i; k++) { out << '\t' << "axis" << (k + 1); }
			out << endl;
			out2 << i << '\t' << (j + 1) << '\t' << stress << '\t' << rsquared << endl;

			output(endConfig, names, out);

			//save best
			if (stress < bestStress) {
				bestDim = i;
				bestStress = stress;
				bestR2 = rsquared;
				bestConfig = endConfig;
			}

			if (ctrlc_pressed) { out.close(); out2.close(); for (int k = 0; k < outputNames.size(); k++) { File::remove(outputNames[k]); } return 0; }
		}
	}

	out.close(); out2.close();

	//output best config
	string BestFileName = getOutputFileName("nmds", variables);
	outputNames.push_back(BestFileName); outputTypes["nmds"].push_back(BestFileName);

	LOG(INFO) << "\nNumber of dimensions:\t" + toString(bestDim) + "\n";
	LOG(INFO) << "Lowest stress :\t" + toString(bestStress) + "\n";
	LOG(INFO) << "R-squared for configuration:\t" + toString(bestR2) + "\n";

	ofstream outBest;
	File::openOutputFile(BestFileName, outBest);
	outBest.setf(ios::fixed, ios::floatfield);
	outBest.setf(ios::showpoint);

	outBest << "group";
	for (int k = 0; k < bestConfig.size(); k++) { outBest << '\t' << "axis" << (k + 1); }
	outBest << endl;

	output(bestConfig, names, outBest);

	outBest.close();

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
//**********************************************************************************************************************
vector< vector<double> > NMDSCommand::nmdsCalc(vector< vector<double> >& matrix, vector< vector<double> >& config, double& stress1) {

	vector< vector<double> > newConfig = config;

	//calc euclid distances
	vector< vector<double> > euclid = linearCalc.calculateEuclidianDistance(newConfig);
	if (ctrlc_pressed) { return newConfig; }

	double stress2 = calculateStress(matrix, euclid);
	stress1 = stress2 + 1.0 + epsilon;

	int count = 0;
	while ((count < maxIters) && (abs(stress1 - stress2) > epsilon)) {
		count++;

		stress1 = stress2;

		if (ctrlc_pressed) { return newConfig; }

		vector< vector<double> > b; b.resize(euclid.size());
		for (int i = 0; i < b.size(); i++) { b[i].resize(euclid[i].size(), 0.0); }

		vector<double> columnSums; columnSums.resize(euclid.size(), 0.0);
		for (int i = 0; i < euclid.size(); i++) {
			for (int j = 0; j < euclid[i].size(); j++) {
				//eliminate divide by zero error
				if (euclid[i][j] != 0) {
					b[i][j] = matrix[i][j] / euclid[i][j];
					columnSums[j] += b[i][j];
					b[i][j] *= -1.0;
				}
			}
		}

		//put in diagonal sums
		for (int i = 0; i < euclid.size(); i++) { b[i][i] = columnSums[i]; }

		int numInLowerTriangle = matrix.size() * (matrix.size() - 1) / 2.0;
		double n = (1.0 + sqrt(1.0 + 8.0 * numInLowerTriangle)) / 2.0;

		//matrix mult
		newConfig = linearCalc.matrix_mult(newConfig, b);
		for (int i = 0; i < newConfig.size(); i++) {
			for (int j = 0; j < newConfig[i].size(); j++) {
				newConfig[i][j] *= (1.0 / n);
			}
		}

		euclid = linearCalc.calculateEuclidianDistance(newConfig);

		stress2 = calculateStress(matrix, euclid);
	}

	return newConfig;
}

//**********************************************************************************************************************
//generate random config
vector< vector<double> > NMDSCommand::generateStartingConfiguration(int numNames, int dimension) {
	vector< vector<double> > axes;  axes.resize(dimension);
	for (int i = 0; i < axes.size(); i++) { axes[i].resize(numNames); }

	//generate random number between -1 and 1, precision 6
	for (int i = 0; i < axes.size(); i++) {
		for (int j = 0; j < axes[i].size(); j++) {

			if (ctrlc_pressed) { return axes; }

			//generate random int between 0 and 99999
			int myrand = (int)((float)(rand()) / ((RAND_MAX / 99998) + 1));

			//generate random sign
			int mysign = (int)((float)(rand()) / ((RAND_MAX / 99998) + 1));

			//if mysign is even then sign = positive, else sign = negative
			if ((mysign % 2) == 0) { mysign = 1.0; }
			else { mysign = -1.0; }

			axes[i][j] = mysign * myrand / (float)100000;
		}
	}

	return axes;
}
//**********************************************************************************************************************
//normalize configuration
int NMDSCommand::normalizeConfiguration(vector< vector<double> >& axes, int numNames, int dimension) {
	vector<double> averageAxes; averageAxes.resize(dimension, 0.0);

	//find average
	for (int i = 0; i < axes.size(); i++) {
		for (int j = 0; j < axes[i].size(); j++) { averageAxes[i] += axes[i][j]; }

		averageAxes[i] /= (float)numNames;
	}

	//normalize axes
	double sumDenom = 0.0;
	for (int i = 0; i < axes.size(); i++) {
		for (int j = 0; j < axes[i].size(); j++) {
			sumDenom += ((axes[i][j] - averageAxes[i]) * (axes[i][j] - averageAxes[i]));
		}
	}

	double denom = sqrt((sumDenom / (float)(axes.size() * numNames)));

	for (int i = 0; i < axes.size(); i++) {
		for (int j = 0; j < axes[i].size(); j++) {
			axes[i][j] = (axes[i][j] - averageAxes[i]) / denom;
		}
	}

	return 0;
}
//**********************************************************************************************************************
//get configuration
vector< vector<double> > NMDSCommand::getConfiguration(vector< vector<double> >& axes, int dimension) {
	vector< vector<double> > newAxes; newAxes.resize(dimension);

	for (int i = 0; i < dimension; i++) {
		newAxes[i] = axes[i];
	}

	return newAxes;
}
//**********************************************************************************************************************
//find raw stress, and normalize using
double NMDSCommand::calculateStress(vector< vector<double> >& matrix, vector< vector<double> >& config) {
	double normStress = 0.0;
	double denom = 0.0;
	double rawStress = 0.0;

	//find raw stress
	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix[i].size(); j++) {
			if (ctrlc_pressed) { return normStress; }

			rawStress += ((matrix[i][j] - config[i][j]) * (matrix[i][j] - config[i][j]));
			denom += (config[i][j] * config[i][j]);
		}
	}

	//normalize stress
	if ((rawStress != 0.0) && (denom != 0.0)) {
		normStress = sqrt((rawStress / denom));
	}

	return normStress;
}

//**********************************************************************************************************************
int NMDSCommand::output(vector< vector<double> >& config, vector<string>& names, ofstream& out) {

	for (int i = 0; i < names.size(); i++) {

		out << names[i];

		for (int j = 0; j < config.size(); j++) {
			out << '\t' << config[j][i];
		}

		out << endl;
	}

	out << endl << endl;

	return 0;
}
/*****************************************************************/
vector< vector<double> > NMDSCommand::readAxes(vector<string> names) {
	ifstream in;
	File::openInputFile(axesfile, in);

	string headerLine = File::getline(in); File::gobble(in);

	//count the number of axis you are reading
	bool done = false;
	int count = 0;
	while (!done) {
		int pos = headerLine.find("axis");
		if (pos != string::npos) {
			count++;
			headerLine = headerLine.substr(pos + 4);
		}
		else { done = true; }
	}

	if (maxdim > count) {
		LOG(INFO) << "You requested maxdim = " + toString(maxdim) + ", but your file only includes " + toString(count) + ". Using " + toString(count) + "." << '\n';
		maxdim = count;
		if (maxdim < mindim) { LOG(INFO) << "Also adjusting mindim to " + toString(maxdim - 1) + "." << '\n'; }
	}

	vector< vector<double> > axes;  axes.resize(maxdim);
	for (int i = 0; i < axes.size(); i++) { axes[i].resize(names.size(), 0.0); }

	map <string, vector<double> > orderedAxes;
	map	<string, vector<double> >::iterator it;

	while (!in.eof()) {

		if (ctrlc_pressed) { in.close(); return axes; }

		string group = "";
		in >> group; File::gobble(in);

		bool ignore = false;
		if (!m->inUsersGroups(group, names)) { ignore = true; LOG(INFO) << group + " is in your axes file and not in your distance file, ignoring." << '\n'; }

		vector<double> thisGroupsAxes;
		for (int i = 0; i < count; i++) {
			float temp = 0.0;
			in >> temp;

			//only save the axis we want
			if (i < maxdim) { thisGroupsAxes.push_back(temp); }
		}

		if (!ignore) { orderedAxes[group] = thisGroupsAxes; }

		File::gobble(in);
	}
	in.close();

	//sanity check
	if (names.size() != orderedAxes.size()) { LOG(LOGERROR) << "your axes file does not match your distance file, aborting." << '\n'; ctrlc_pressed = true; return axes; }

	//put axes info in same order as distance file, just in case
	for (int i = 0; i < names.size(); i++) {
		it = orderedAxes.find(names[i]);

		if (it != orderedAxes.end()) {
			vector<double> thisGroupsAxes = it->second;

			for (int j = 0; j < thisGroupsAxes.size(); j++) {
				axes[j][i] = thisGroupsAxes[j];
			}

		}
		else { LOG(LOGERROR) << "your axes file does not match your distance file, aborting." << '\n'; ctrlc_pressed = true; return axes; }
	}

	return axes;
}
/**********************************************************************************************************************/



