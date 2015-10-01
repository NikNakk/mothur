/*
 *  homovacommand.cpp
 *  mothur
 *
 *  Created by westcott on 2/8/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "homovacommand.h"
#include "groupmap.h"
#include "readphylipvector.h"
#include "sharedutilities.h"
#include "designmap.h"

 //**********************************************************************************************************************
vector<string> HomovaCommand::setParameters() {
	try {
		CommandParameter pdesign("design", "InputTypes", "", "", "none", "none", "none", "homova", false, true, true); parameters.push_back(pdesign);
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "none", "none", "homova", false, true, true); parameters.push_back(pphylip);
		nkParameters.add(new StringParameter("sets", "", false, false));
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
		LOG(FATAL) << e.what() << " in HomovaCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string HomovaCommand::getHelpString() {
	try {
		string helpString = "Referenced: Stewart CN, Excoffier L (1996). Assessing population genetic structure and variability with RAPD data: Application to Vaccinium macrocarpon (American Cranberry). J Evol Biol 9: 153-71.\n"
			"The homova command outputs a .homova file. \n"
			"The homova command parameters are phylip, iters, sets and alpha.  The phylip and design parameters are required, unless valid current files exist.\n"
			"The design parameter allows you to assign your samples to groups when you are running homova. It is required. \n"
			"The design file looks like the group file.  It is a 2 column tab delimited file, where the first column is the sample name and the second column is the group the sample belongs to.\n"
			"The sets parameter allows you to specify which of the sets in your designfile you would like to analyze. The set names are separated by dashes. THe default is all sets in the designfile.\n"
			"The iters parameter allows you to set number of randomization for the P value.  The default is 1000. \n"
			"The homova command should be in the following format: homova(phylip=file.dist, design=file.design).\n"
			"Note: No spaces between parameter labels (i.e. iters), '=' and parameters (i.e. 1000).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in HomovaCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string HomovaCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "homova") { pattern = "[filename],homova"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
HomovaCommand::HomovaCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["homova"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in HomovaCommand, HomovaCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

HomovaCommand::HomovaCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["homova"] = tempOutNames;

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
		if (designFileName == "not open") { abort = true; }
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
	LOG(FATAL) << e.what() << " in HomovaCommand, HomovaCommand";
	exit(1);
}
}
//**********************************************************************************************************************

int HomovaCommand::execute() {

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
	ofstream HOMOVAFile;
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(phylipFileName));
	string HOMOVAFileName = getOutputFileName("homova", variables);
	File::openOutputFile(HOMOVAFileName, HOMOVAFile);
	outputNames.push_back(HOMOVAFileName); outputTypes["homova"].push_back(HOMOVAFileName);

	HOMOVAFile << "HOMOVA\tBValue\tP-value\tSSwithin/(Ni-1)_values" << endl;
	LOG(INFO) << "HOMOVA\tBValue\tP-value\tSSwithin/(Ni-1)_values\n";

	double fullHOMOVAPValue = runHOMOVA(HOMOVAFile, origGroupSampleMap, experimentwiseAlpha);

	if (fullHOMOVAPValue <= experimentwiseAlpha && numGroups > 2) {

		int numCombos = numGroups * (numGroups - 1) / 2;
		double pairwiseAlpha = experimentwiseAlpha / (double)numCombos;

		map<string, vector<int> >::iterator itA;
		map<string, vector<int> >::iterator itB;

		for (itA = origGroupSampleMap.begin();itA != origGroupSampleMap.end();itA++) {
			itB = itA;itB++;
			for (;itB != origGroupSampleMap.end();itB++) {
				map<string, vector<int> > pairwiseGroupSampleMap;
				pairwiseGroupSampleMap[itA->first] = itA->second;
				pairwiseGroupSampleMap[itB->first] = itB->second;

				runHOMOVA(HOMOVAFile, pairwiseGroupSampleMap, pairwiseAlpha);
			}
		}
		HOMOVAFile << endl;
		LOG(INFO) << '\n' << "Experiment-wise error rate: " + toString(experimentwiseAlpha) + '\n';
		LOG(INFO) << "Pair-wise error rate (Bonferroni): " + toString(pairwiseAlpha) + '\n';
	}
	else {
		LOG(INFO) << "Experiment-wise error rate: " + toString(experimentwiseAlpha) + '\n';
	}

	LOG(INFO) << "If you have borderline P-values, you should try increasing the number of iterations\n";

	delete designMap;

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}

//**********************************************************************************************************************

double HomovaCommand::runHOMOVA(ofstream& HOMOVAFile, map<string, vector<int> > groupSampleMap, double alpha) {
	map<string, vector<int> >::iterator it;
	int numGroups = groupSampleMap.size();

	vector<double> ssWithinOrigVector;
	double bValueOrig = calcBValue(groupSampleMap, ssWithinOrigVector);

	double counter = 0;
	for (int i = 0;i < iters;i++) {
		vector<double> ssWithinRandVector;
		map<string, vector<int> > randomizedGroup = getRandomizedGroups(groupSampleMap);
		double bValueRand = calcBValue(randomizedGroup, ssWithinRandVector);
		if (bValueRand >= bValueOrig) { counter++; }
	}

	double pValue = (double)counter / (double)iters;
	string pString = "";
	if (pValue < 1 / (double)iters) { pString = '<' + toString(1 / (double)iters); }
	else { pString = toString(pValue); }


	//print homova table
	it = groupSampleMap.begin();
	HOMOVAFile << it->first;
	LOG(INFO) << it->first;
	it++;
	for (;it != groupSampleMap.end();it++) {
		HOMOVAFile << '-' << it->first;
		LOG(INFO) << '-' + it->first;
	}

	HOMOVAFile << '\t' << bValueOrig << '\t' << pString;
	LOG(INFO) << '\t' + toString(bValueOrig) + '\t' + pString;

	if (pValue < alpha) {
		HOMOVAFile << "*";
		LOG(INFO) << "*";
	}

	for (int i = 0;i < numGroups;i++) {
		HOMOVAFile << '\t' << ssWithinOrigVector[i];
		LOG(INFO) << '\t' + toString(ssWithinOrigVector[i]);
	}
	HOMOVAFile << endl;
	LOG(INFO) << "";

	return pValue;
}

//**********************************************************************************************************************

double HomovaCommand::calcSigleSSWithin(vector<int> sampleIndices) {
	double ssWithin = 0.0;
	int numSamplesInGroup = sampleIndices.size();

	for (int i = 0;i < numSamplesInGroup;i++) {
		int row = sampleIndices[i];

		for (int j = 0;j < numSamplesInGroup;j++) {
			int col = sampleIndices[j];

			if (col < row) {
				ssWithin += distanceMatrix[row][col];
			}

		}
	}

	ssWithin /= numSamplesInGroup;
	return ssWithin;
}

//**********************************************************************************************************************

double HomovaCommand::calcBValue(map<string, vector<int> > groupSampleMap, vector<double>& ssWithinVector) {

	map<string, vector<int> >::iterator it;

	double numGroups = (double)groupSampleMap.size();
	ssWithinVector.resize(numGroups, 0);

	double totalNumSamples = 0;
	double ssWithinFull;
	double secondTermSum = 0;
	double inverseOneMinusSum = 0;
	int index = 0;

	ssWithinVector.resize(numGroups, 0);
	for (it = groupSampleMap.begin();it != groupSampleMap.end();it++) {
		int numSamplesInGroup = it->second.size();
		totalNumSamples += numSamplesInGroup;

		ssWithinVector[index] = calcSigleSSWithin(it->second);
		ssWithinFull += ssWithinVector[index];

		secondTermSum += (numSamplesInGroup - 1) * log(ssWithinVector[index] / (double)(numSamplesInGroup - 1));
		inverseOneMinusSum += 1.0 / (double)(numSamplesInGroup - 1);

		ssWithinVector[index] /= (double)(numSamplesInGroup - 1); //this line is only for output purposes to scale SSw by the number of samples in the group
		index++;
	}

	double B = (totalNumSamples - numGroups) * log(ssWithinFull / (totalNumSamples - numGroups)) - secondTermSum;
	double denomintor = 1 + 1.0 / (3.0 * (numGroups - 1.0)) * (inverseOneMinusSum - 1.0 / (double)(totalNumSamples - numGroups));
	B /= denomintor;

	return B;

}

//**********************************************************************************************************************

map<string, vector<int> > HomovaCommand::getRandomizedGroups(map<string, vector<int> > origMapping) {
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


