/*
 *  amovacommand.cpp
 *  mothur
 *
 *  Created by westcott on 2/7/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "amovacommand.h"
#include "readphylipvector.h"
#include "designmap.h"
#include "sharedutilities.h"


 //**********************************************************************************************************************
vector<string> AmovaCommand::setParameters() {
	try {
		CommandParameter pdesign("design", "InputTypes", "", "", "none", "none", "none", "amova", false, true, true); parameters.push_back(pdesign);
		nkParameters.add(new StringParameter("sets", "", false, false));
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "none", "none", "amova", false, true, true); parameters.push_back(pphylip);
		nkParameters.add(new NumberParameter("iters", -INFINITY, INFINITY, 1000, false, false));
		nkParameters.add(new NumberParameter("alpha", -INFINITY, INFINITY, 0.05, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in AmovaCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string AmovaCommand::getHelpString() {
	try {
		string helpString = "Referenced: Anderson MJ (2001). A new method for non-parametric multivariate analysis of variance. Austral Ecol 26: 32-46."
			"The amova command outputs a .amova file."
			"The amova command parameters are phylip, iters, sets and alpha.  The phylip and design parameters are required, unless you have valid current files."
			"The design parameter allows you to assign your samples to groups when you are running amova. It is required."
			"The design file looks like the group file.  It is a 2 column tab delimited file, where the first column is the sample name and the second column is the group the sample belongs to."
			"The sets parameter allows you to specify which of the sets in your designfile you would like to analyze. The set names are separated by dashes. THe default is all sets in the designfile.\n"
			"The iters parameter allows you to set number of randomization for the P value.  The default is 1000."
			"The amova command should be in the following format: amova(phylip=file.dist, design=file.design)."
			"Note: No spaces between parameter labels (i.e. iters), '=' and parameters (i.e. 1000).";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in AmovaCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string AmovaCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "amova") { pattern = "[filename],amova"; } //makes file like: amazon.align
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
AmovaCommand::AmovaCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["amova"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in AmovaCommand, AmovaCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
AmovaCommand::AmovaCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

	//allow user to run help
	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter;

		//check to make sure all parameters are valid for command
		map<string, string>::iterator it;
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["amova"] = tempOutNames;

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("design");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["design"] = inputDir + it->second; }
			}

			it = parameters.find("phylip");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["phylip"] = inputDir + it->second; }
			}
		}

		phylipFileName = validParameter.validFile(parameters, "phylip", true);
		if (phylipFileName == "not open") { phylipFileName = ""; abort = true; }
		else if (phylipFileName == "not found") {
			//if there is a current phylip file, use it
			phylipFileName = settings.getCurrent("phylip");
			if (phylipFileName != "") { LOG(INFO) << "Using " + phylipFileName + " as input file for the phylip parameter." << '\n'; }
			else { LOG(INFO) << "You have no current phylip file and the phylip parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("phylip", phylipFileName); }

		//check for required parameters
		designFileName = validParameter.validFile(parameters, "design", true);
		if (designFileName == "not open") { designFileName = ""; abort = true; }
		else if (designFileName == "not found") {
			//if there is a current design file, use it
			designFileName = settings.getCurrent("design");
			if (designFileName != "") { LOG(INFO) << "Using " + designFileName + " as input file for the design parameter." << '\n'; }
			else { LOG(INFO) << "You have no current design file and the design parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("design", designFileName); }

		string temp = validParameter.validFile(parameters, "iters", false);
		if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, iters);

		temp = validParameter.validFile(parameters, "alpha", false);
		if (temp == "not found") { temp = "0.05"; }
		Utility::mothurConvert(temp, experimentwiseAlpha);

		string sets = validParameter.validFile(parameters, "sets", false);
		if (sets == "not found") { sets = ""; }
		else {
			Utility::split(sets, '-', Sets);
		}
	}
}
catch (exception& e) {
	LOG(FATAL) << e.what() << " in AmovaCommand, AmovaCommand";
	exit(1);
}
}
//**********************************************************************************************************************

int AmovaCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	//read design file
	designMap = new DesignMap(designFileName);

	if (outputDir == "") { outputDir = File::getPath(phylipFileName); }

	//read in distance matrix and square it
	ReadPhylipVector readMatrix(phylipFileName);
	vector<string> sampleNames = readMatrix.read(distanceMatrix);

	if (Sets.size() != 0) { //user selected sets, so we want to remove the samples not in those sets
		SharedUtil util;
		vector<string> dGroups = designMap->getCategory();
		util.setGroups(Sets, dGroups);

		for (int i = 0;i < distanceMatrix.size();i++) {

			if (ctrlc_pressed) { delete designMap; return 0; }

			string group = designMap->get(sampleNames[i]);

			if (group == "not found") {
				LOG(LOGERROR) << "" + sampleNames[i] + " is not in your design file, please correct." << '\n'; ctrlc_pressed = true;
			}
			else if (!m->inUsersGroups(group, Sets)) {  //not in set we want remove it
			   //remove from all other rows
				for (int j = 0;j < distanceMatrix.size();j++) {
					distanceMatrix[j].erase(distanceMatrix[j].begin() + i);
				}
				distanceMatrix.erase(distanceMatrix.begin() + i);
				sampleNames.erase(sampleNames.begin() + i);
				i--;
			}
		}
	}

	for (int i = 0;i < distanceMatrix.size();i++) {
		for (int j = 0;j < i;j++) {
			distanceMatrix[i][j] *= distanceMatrix[i][j];
		}
	}

	//link designMap to rows/columns in distance matrix
	map<string, vector<int> > origGroupSampleMap;
	for (int i = 0;i < sampleNames.size();i++) {
		string group = designMap->get(sampleNames[i]);

		if (group == "not found") {
			LOG(LOGERROR) << "" + sampleNames[i] + " is not in your design file, please correct." << '\n'; ctrlc_pressed = true;
		}
		else { origGroupSampleMap[group].push_back(i); }

	}
	int numGroups = origGroupSampleMap.size();

	if (ctrlc_pressed) { delete designMap; return 0; }

	//create a new filename
	ofstream AMOVAFile;
	map<string, string> variables; variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(phylipFileName));
	string AMOVAFileName = getOutputFileName("amova", variables);

	File::openOutputFile(AMOVAFileName, AMOVAFile);
	outputNames.push_back(AMOVAFileName); outputTypes["amova"].push_back(AMOVAFileName);

	double fullANOVAPValue = runAMOVA(AMOVAFile, origGroupSampleMap, experimentwiseAlpha);
	if (fullANOVAPValue <= experimentwiseAlpha && numGroups > 2) {

		int numCombos = numGroups * (numGroups - 1) / 2;
		double pairwiseAlpha = experimentwiseAlpha / (double)numCombos;

		map<string, vector<int> >::iterator itA;
		map<string, vector<int> >::iterator itB;

		for (itA = origGroupSampleMap.begin();itA != origGroupSampleMap.end();itA++) {
			itB = itA;itB++;
			for (itB;itB != origGroupSampleMap.end();itB++) {

				map<string, vector<int> > pairwiseGroupSampleMap;
				pairwiseGroupSampleMap[itA->first] = itA->second;
				pairwiseGroupSampleMap[itB->first] = itB->second;

				runAMOVA(AMOVAFile, pairwiseGroupSampleMap, pairwiseAlpha);
			}
		}
		LOG(INFO) << "Experiment-wise error rate: " + toString(experimentwiseAlpha) + '\n';
		LOG(INFO) << "Pair-wise error rate (Bonferroni): " + toString(pairwiseAlpha) + '\n';
	}
	else {
		LOG(INFO) << "Experiment-wise error rate: " + toString(experimentwiseAlpha) + '\n';
	}
	LOG(INFO) << "If you have borderline P-values, you should try increasing the number of iterations\n";
	AMOVAFile.close();

	delete designMap;

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}

//**********************************************************************************************************************

double AmovaCommand::runAMOVA(ofstream& AMOVAFile, map<string, vector<int> > groupSampleMap, double alpha) {
	map<string, vector<int> >::iterator it;

	int numGroups = groupSampleMap.size();
	int totalNumSamples = 0;

	for (it = groupSampleMap.begin();it != groupSampleMap.end();it++) {
		totalNumSamples += it->second.size();
	}

	double ssTotalOrig = calcSSTotal(groupSampleMap);
	double ssWithinOrig = calcSSWithin(groupSampleMap);
	double ssAmongOrig = ssTotalOrig - ssWithinOrig;

	double counter = 0;
	for (int i = 0;i < iters;i++) {
		map<string, vector<int> > randomizedGroup = getRandomizedGroups(groupSampleMap);
		double ssWithinRand = calcSSWithin(randomizedGroup);
		if (ssWithinRand <= ssWithinOrig) { counter++; }
	}

	double pValue = (double)counter / (double)iters;
	string pString = "";
	if (pValue < 1 / (double)iters) { pString = '<' + toString(1 / (double)iters); }
	else { pString = toString(pValue); }


	//print anova table
	it = groupSampleMap.begin();
	AMOVAFile << it->first;
	LOG(INFO) << it->first;
	it++;
	for (it;it != groupSampleMap.end();it++) {
		AMOVAFile << '-' << it->first;
		LOG(INFO) << '-' + it->first;
	}

	AMOVAFile << "\tAmong\tWithin\tTotal" << endl;
	LOG(INFO) << "\tAmong\tWithin\tTotal\n";

	AMOVAFile << "SS\t" << ssAmongOrig << '\t' << ssWithinOrig << '\t' << ssTotalOrig << endl;
	LOG(INFO) << "SS\t" + toString(ssAmongOrig) + '\t' + toString(ssWithinOrig) + '\t' + toString(ssTotalOrig) + '\n';

	int dfAmong = numGroups - 1;				double MSAmong = ssAmongOrig / (double)dfAmong;
	int dfWithin = totalNumSamples - numGroups;	double MSWithin = ssWithinOrig / (double)dfWithin;
	int dfTotal = totalNumSamples - 1;			double Fs = MSAmong / MSWithin;

	AMOVAFile << "df\t" << dfAmong << '\t' << dfWithin << '\t' << dfTotal << endl;
	LOG(INFO) << "df\t" + toString(dfAmong) + '\t' + toString(dfWithin) + '\t' + toString(dfTotal) + '\n';

	AMOVAFile << "MS\t" << MSAmong << '\t' << MSWithin << endl << endl;
	LOG(INFO) << "MS\t" + toString(MSAmong) + '\t' + toString(MSWithin) + "\n\n";

	AMOVAFile << "Fs:\t" << Fs << endl;
	LOG(INFO) << "Fs:\t" + toString(Fs) + '\n';

	AMOVAFile << "p-value: " << pString;
	LOG(INFO) << "p-value: " + pString;

	if (pValue < alpha) {
		AMOVAFile << "*";
		LOG(INFO) << "*";
	}
	AMOVAFile << endl << endl;
	LOG(INFO) << '\n';

	return pValue;
}

//**********************************************************************************************************************

map<string, vector<int> > AmovaCommand::getRandomizedGroups(map<string, vector<int> > origMapping) {
	try {
		vector<int> sampleIndices;
		vector<int> samplesPerGroup;

		map<string, vector<int> >::iterator it;
		for (it = origMapping.begin();it != origMapping.end();it++) {
			vector<int> indices = it->second;
			samplesPerGroup.push_back(indices.size());
			sampleIndices.insert(sampleIndices.end(), indices.begin(), indices.end());
		}

		random_shuffle(sampleIndices.begin(), sampleIndices.end());

		int index = 0;
		map<string, vector<int> > randomizedGroups = origMapping;
		for (it = randomizedGroups.begin();it != randomizedGroups.end();it++) {
			for (int i = 0;i < it->second.size();i++) {
				it->second[i] = sampleIndices[index++];
			}
		}

		return randomizedGroups;
	}

	//**********************************************************************************************************************

	double AmovaCommand::calcSSTotal(map<string, vector<int> >& groupSampleMap) {

		vector<int> indices;
		map<string, vector<int> >::iterator it;
		for (it = groupSampleMap.begin();it != groupSampleMap.end();it++) {
			indices.insert(indices.end(), it->second.begin(), it->second.end());
		}
		sort(indices.begin(), indices.end());

		int numIndices = indices.size();
		double ssTotal = 0.0;

		for (int i = 1;i < numIndices;i++) {
			int row = indices[i];

			for (int j = 0;j < i;j++) {
				ssTotal += distanceMatrix[row][indices[j]];
			}
		}
		ssTotal /= numIndices;

		return ssTotal;
	}

	//**********************************************************************************************************************

	double AmovaCommand::calcSSWithin(map<string, vector<int> >& groupSampleMap) {

		double ssWithin = 0.0;

		map<string, vector<int> >::iterator it;
		for (it = groupSampleMap.begin();it != groupSampleMap.end();it++) {

			double withinGroup = 0;

			vector<int> samples = it->second;

			for (int i = 0;i < samples.size();i++) {
				int row = samples[i];

				for (int j = 0;j < samples.size();j++) {
					int col = samples[j];

					if (col < row) {
						withinGroup += distanceMatrix[row][col];
					}

				}
			}

			ssWithin += withinGroup / samples.size();
		}

		return ssWithin;
	}

	//**********************************************************************************************************************
