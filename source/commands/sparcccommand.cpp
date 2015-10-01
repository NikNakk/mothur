//
//  sparcccommand.cpp
//  Mothur
//
//  Created by SarahsWork on 5/10/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#include "sparcccommand.h"


//**********************************************************************************************************************
vector<string> SparccCommand::setParameters() {
	CommandParameter pshared("shared", "InputTypes", "", "", "none", "none", "none", "outputType", false, true); parameters.push_back(pshared);
	nkParameters.add(new StringParameter("groups", "", false, false));
	nkParameters.add(new StringParameter("label", "", false, false));
	CommandParameter psamplings("samplings", "Number", "", "20", "", "", "", "", false, false, false); parameters.push_back(psamplings);
	CommandParameter piterations("iterations", "Number", "", "10", "", "", "", "", false, false, false); parameters.push_back(piterations);
	CommandParameter ppermutations("permutations", "Number", "", "1000", "", "", "", "", false, false, false); parameters.push_back(ppermutations);
	CommandParameter pmethod("method", "Multiple", "relabund-dirichlet", "dirichlet", "", "", "", "", false, false); parameters.push_back(pmethod);
	nkParameters.add(new ProcessorsParameter(settings));
	//every command must have inputdir and outputdir.  This allows mothur users to redirect input and output files.
	nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
	nkParameters.add(new StringParameter("inputdir", "", false, false));
	nkParameters.add(new StringParameter("outputdir", "", false, false));

	vector<string> myArray;
	for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
	return myArray;
}
//**********************************************************************************************************************
string SparccCommand::getHelpString() {
	string helpString = "The sparcc command allows you to ....\n"
		"The sparcc command parameters are: shared, groups, label, samplings, iterations, permutations, processors and method.\n"
		"The samplings parameter is used to .... Default=20.\n"
		"The iterations parameter is used to ....Default=10.\n"
		"The permutations parameter is used to ....Default=1000.\n"
		"The method parameter is used to ....Options are relabund and dirichlet. Default=dirichlet.\n"
		"The default value for groups is all the groups in your sharedfile.\n"
		"The label parameter is used to analyze specific labels in your shared file.\n"
		"The sparcc command should be in the following format: sparcc(shared=yourSharedFile)\n"
		"sparcc(shared=final.an.shared)\n";
	return helpString;
}
//**********************************************************************************************************************
string SparccCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "corr") { pattern = "[filename],[distance],sparcc_correlation"; }
	else if (type == "pvalue") { pattern = "[filename],[distance],sparcc_pvalue"; }
	else if (type == "sparccrelabund") { pattern = "[filename],[distance],sparcc_relabund"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
SparccCommand::SparccCommand(Settings& settings) : Command(settings) {
	abort = true; calledHelp = true;
	setParameters();
	vector<string> tempOutNames;
	outputTypes["corr"] = tempOutNames;
	outputTypes["pvalue"] = tempOutNames;
	outputTypes["sparccrelabund"] = tempOutNames;
}
//**********************************************************************************************************************
SparccCommand::SparccCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	allLines = 1;

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

		vector<string> tempOutNames;
		outputTypes["corr"] = tempOutNames; //filetypes should be things like: shared, fasta, accnos...
		outputTypes["pvalue"] = tempOutNames;
		outputTypes["sparccrelabund"] = tempOutNames;

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
		}

		//check for parameters
		//get shared file, it is required
		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = ""; abort = true; }
		else if (sharedfile == "not found") {
			//if there is a current shared file, use it
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else { LOG(INFO) << "You have no current sharedfile and the shared parameter is required." << '\n'; abort = true; }
		}
		else { settings.setCurrent("shared", sharedfile); }

		//if the user changes the output directory command factory will send this info to us in the output parameter
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = File::getPath(sharedfile); //if user entered a file with a path then preserve it
		}

		normalizeMethod = validParameter.validFile(parameters, "method", false);
		if (normalizeMethod == "not found") { normalizeMethod = "dirichlet"; }
		if ((normalizeMethod == "dirichlet") || (normalizeMethod == "relabund")) {}
		else { LOG(INFO) << normalizeMethod + " is not a valid method.  Valid methods are dirichlet and relabund." << '\n'; abort = true; }


		string temp = validParameter.validFile(parameters, "samplings", false);	if (temp == "not found") { temp = "20"; }
		Utility::mothurConvert(temp, numSamplings);

		if (normalizeMethod == "relabund") { numSamplings = 1; }

		temp = validParameter.validFile(parameters, "iterations", false);	if (temp == "not found") { temp = "10"; }
		Utility::mothurConvert(temp, maxIterations);

		temp = validParameter.validFile(parameters, "permutations", false);	if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, numPermutations);

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		string groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; }
		else { Utility::split(groups, '-', Groups); }
		m->setGroups(Groups);

		string label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}
	}

}
//**********************************************************************************************************************
int SparccCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	int start = time(NULL);

	InputData input(sharedfile, "sharedfile");
	vector<SharedRAbundVector*> lookup = input.getSharedRAbundVectors();
	string lastLabel = lookup[0]->getLabel();

	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	set<string> processedLabels;
	set<string> userLabels = labels;

	//as long as you are not at the end of the file or done wih the lines you want
	while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

		if (ctrlc_pressed) { for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }  for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }return 0; }

		if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

			LOG(INFO) << lookup[0]->getLabel() << '\n';

			process(lookup);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());
		}

		if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
			string saveLabel = lookup[0]->getLabel();

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
			lookup = input.getSharedRAbundVectors(lastLabel);
			LOG(INFO) << lookup[0]->getLabel() << '\n';

			process(lookup);

			processedLabels.insert(lookup[0]->getLabel());
			userLabels.erase(lookup[0]->getLabel());

			//restore real lastlabel to save below
			lookup[0]->setLabel(saveLabel);
		}

		lastLabel = lookup[0]->getLabel();
		//prevent memory leak
		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }

		if (ctrlc_pressed) { return 0; }

		//get next line to process
		lookup = input.getSharedRAbundVectors();
	}

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

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

		LOG(INFO) << lookup[0]->getLabel() << '\n';

		process(lookup);

		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
	}

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << "It took " + toString(time(NULL) - start) + " seconds to process.";
	LOG(INFO) << '\n' << "";

	//output files created by command
	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";
	return 0;

}
/**************************************************************************************************/
vector<vector<float> > SparccCommand::shuffleSharedVector(vector<vector<float> >& sharedVector) {
	int numGroups = (int)sharedVector.size();
	int numOTUs = (int)sharedVector[0].size();

	vector<vector<float> > shuffledVector = sharedVector;

	for (int i = 0;i < numGroups;i++) {
		for (int j = 0;j < numOTUs;j++) {
			shuffledVector[i][j] = sharedVector[rand() % numGroups][j];
		}
	}

	return shuffledVector;
}
//**********************************************************************************************************************
int SparccCommand::process(vector<SharedRAbundVector*>& lookup) {
	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::showpoint);

	vector<vector<float> > sharedVector;
	vector<string> otuNames = settings.currentSharedBinLabels;

	//fill sharedVector to pass to CalcSparcc
	for (int i = 0; i < lookup.size(); i++) {
		vector<int> abunds = lookup[i]->getAbundances();
		vector<float> temp;
		for (int j = 0; j < abunds.size(); j++) { temp.push_back((float)abunds[j]); }
		sharedVector.push_back(temp);
	}
	int numOTUs = (int)sharedVector[0].size();
	int numGroups = lookup.size();

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(sharedfile));
	variables["[distance]"] = lookup[0]->getLabel();


	string relAbundFileName = getOutputFileName("sparccrelabund", variables);
	ofstream relAbundFile;
	File::openOutputFile(relAbundFileName, relAbundFile);
	outputNames.push_back(relAbundFileName);  outputTypes["sparccrelabund"].push_back(relAbundFileName);

	relAbundFile << "OTU\taveRelAbund\n";
	for (int i = 0;i < numOTUs;i++) {
		if (ctrlc_pressed) { relAbundFile.close(); return 0; }

		double relAbund = 0.0000;
		for (int j = 0;j < numGroups;j++) {
			relAbund += sharedVector[j][i] / (double)lookup[j]->getNumSeqs();
		}
		relAbundFile << otuNames[i] << '\t' << relAbund / (double)numGroups << endl;
	}
	relAbundFile.close();

	CalcSparcc originalData(sharedVector, maxIterations, numSamplings, normalizeMethod);
	vector<vector<float> > origCorrMatrix = originalData.getRho();

	string correlationFileName = getOutputFileName("corr", variables);
	ofstream correlationFile;
	File::openOutputFile(correlationFileName, correlationFile);
	outputNames.push_back(correlationFileName);  outputTypes["corr"].push_back(correlationFileName);
	correlationFile.setf(ios::fixed, ios::floatfield);
	correlationFile.setf(ios::showpoint);

	for (int i = 0;i < numOTUs;i++) { correlationFile << '\t' << otuNames[i]; }   correlationFile << endl;
	for (int i = 0;i < numOTUs;i++) {
		correlationFile << otuNames[i];
		for (int j = 0;j < numOTUs;j++) {
			correlationFile << '\t' << origCorrMatrix[i][j];
		}
		correlationFile << endl;
	}


	if (numPermutations != 0) {
		vector<vector<float> > pValues = createProcesses(sharedVector, origCorrMatrix);

		if (ctrlc_pressed) { return 0; }

		string pValueFileName = getOutputFileName("pvalue", variables);
		ofstream pValueFile;
		File::openOutputFile(pValueFileName, pValueFile);
		outputNames.push_back(pValueFileName);  outputTypes["pvalue"].push_back(pValueFileName);
		pValueFile.setf(ios::fixed, ios::floatfield);
		pValueFile.setf(ios::showpoint);

		for (int i = 0;i < numOTUs;i++) { pValueFile << '\t' << otuNames[i]; }   pValueFile << endl;
		for (int i = 0;i < numOTUs;i++) {
			pValueFile << otuNames[i];
			for (int j = 0;j < numOTUs;j++) {
				pValueFile << '\t' << pValues[i][j];
			}
			pValueFile << endl;
		}
	}


	return 0;
}
//**********************************************************************************************************************
vector<vector<float> > SparccCommand::createProcesses(vector<vector<float> >& sharedVector, vector<vector<float> >& origCorrMatrix) {
	int numOTUs = sharedVector[0].size();
	vector<vector<float> > pValues;
	bool recalc = false;

	if (processors == 1) {
		pValues = driver(sharedVector, origCorrMatrix, numPermutations);
	}
	else {
		//divide iters between processors
		vector<int> procIters;
		int numItersPerProcessor = numPermutations / processors;

		//divide iters between processes
		for (int h = 0; h < processors; h++) {
			if (h == processors - 1) { numItersPerProcessor = numPermutations - h * numItersPerProcessor; }
			procIters.push_back(numItersPerProcessor);
		}

		vector<int> processIDS;
		int process = 1;

#if defined (UNIX)

		//loop through and create all the processes you want
		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {
				pValues = driver(sharedVector, origCorrMatrix, procIters[process]);

				//pass pvalues to parent
				ofstream out;
				string tempFile = m->mothurGetpid(process) + ".pvalues.temp";
				File::openOutputFile(tempFile, out);

				//pass values
				for (int i = 0; i < pValues.size(); i++) {
					for (int j = 0; j < pValues[i].size(); j++) {
						out << pValues[i][j] << '\t';
					}
					out << endl;
				}
				out << endl;

				out.close();

				exit(0);
			}
			else {
				LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(process) + "\n"; processors = process;
				for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
				//wait to die
				for (int i = 0;i < processIDS.size();i++) {
					int temp = processIDS[i];
					wait(&temp);
				}
				ctrlc_pressed = false;
				for (int i = 0;i < processIDS.size();i++) {
					File::remove((toString(processIDS[i]) + ".pvalues.temp"));
				}
				recalc = true;
				break;
			}
		}

		if (recalc) {
			//test line, also set recalc to true.
			//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove((toString(processIDS[i]) + ".pvalues.temp"));}processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

			procIters.clear();
			int numItersPerProcessor = numPermutations / processors;

			//divide iters between processes
			for (int h = 0; h < processors; h++) {
				if (h == processors - 1) { numItersPerProcessor = numPermutations - h * numItersPerProcessor; }
				procIters.push_back(numItersPerProcessor);
			}

			pValues.clear();
			processIDS.resize(0);
			process = 1;

			//loop through and create all the processes you want
			while (process != processors) {
				pid_t pid = fork();

				if (pid > 0) {
					processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
					process++;
				}
				else if (pid == 0) {
					pValues = driver(sharedVector, origCorrMatrix, procIters[process]);

					//pass pvalues to parent
					ofstream out;
					string tempFile = m->mothurGetpid(process) + ".pvalues.temp";
					File::openOutputFile(tempFile, out);

					//pass values
					for (int i = 0; i < pValues.size(); i++) {
						for (int j = 0; j < pValues[i].size(); j++) {
							out << pValues[i][j] << '\t';
						}
						out << endl;
					}
					out << endl;

					out.close();

					exit(0);
				}
				else {
					LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
					for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
					exit(0);
				}
			}
		}


		//do my part
		pValues = driver(sharedVector, origCorrMatrix, procIters[0]);

		//force parent to wait until all the processes are done
		for (int i = 0;i < processIDS.size();i++) {
			int temp = processIDS[i];
			wait(&temp);
		}

		//combine results
		for (int i = 0; i < processIDS.size(); i++) {
			ifstream in;
			string tempFile = toString(processIDS[i]) + ".pvalues.temp";
			File::openInputFile(tempFile, in);

			////// to do ///////////
			int numTemp; numTemp = 0;

			for (int j = 0; j < pValues.size(); j++) {
				for (int k = 0; k < pValues.size(); k++) {
					in >> numTemp; File::gobble(in);
					pValues[j][k] += numTemp;
				}
				File::gobble(in);
			}
			in.close(); File::remove(tempFile);
		}
#else

		//fill in functions
		vector<sparccData*> pDataArray;
		vector<DWORD> dwThreadIdArray(processors - 1);
		vector<HANDLE> hThreadArray(processors - 1);

		//Create processor worker threads.
		for (int i = 1; i < processors; i++) {

			//make copy so we don't get access violations
			vector< vector<float> > copySharedVector = sharedVector;
			vector< vector<float> > copyOrig = origCorrMatrix;

			sparccData* temp = new sparccData(m, procIters[i], copySharedVector, copyOrig, numSamplings, maxIterations, numPermutations, normalizeMethod);
			pDataArray.push_back(temp);
			processIDS.push_back(i);

			hThreadArray[i - 1] = CreateThread(NULL, 0, MySparccThreadFunction, pDataArray[i - 1], 0, &dwThreadIdArray[i - 1]);
		}

		//do my part
		pValues = driver(sharedVector, origCorrMatrix, procIters[0]);

		//Wait until all threads have terminated.
		WaitForMultipleObjects(processors - 1, &(hThreadArray[0]), TRUE, INFINITE);

		//Close all thread handles and free memory allocations.
		for (int i = 0; i < pDataArray.size(); i++) {
			for (int j = 0; j < pDataArray[i]->pValues.size(); j++) {
				for (int k = 0; k < pDataArray[i]->pValues[j].size(); k++) {
					pValues[j][k] += pDataArray[i]->pValues[j][k];
				}
			}

			CloseHandle(hThreadArray[i]);
			delete pDataArray[i];
		}
#endif
	}

	for (int i = 0;i < numOTUs;i++) {
		pValues[i][i] = 1;
		for (int j = 0;j < i;j++) {
			pValues[i][j] /= (double)numPermutations;
			pValues[j][i] = pValues[i][j];
		}
	}

	return pValues;
}

//**********************************************************************************************************************
vector<vector<float> > SparccCommand::driver(vector<vector<float> >& sharedVector, vector<vector<float> >& origCorrMatrix, int numPerms) {
	int numOTUs = sharedVector[0].size();
	vector<vector<float> > sharedShuffled = sharedVector;
	vector<vector<float> > pValues(numOTUs);
	for (int i = 0;i < numOTUs;i++) { pValues[i].assign(numOTUs, 0); }

	for (int i = 0;i < numPerms;i++) {
		if (ctrlc_pressed) { return pValues; }
		sharedShuffled = shuffleSharedVector(sharedVector);
		CalcSparcc permutedData(sharedShuffled, maxIterations, numSamplings, normalizeMethod);
		vector<vector<float> > permuteCorrMatrix = permutedData.getRho();

		for (int j = 0;j < numOTUs;j++) {
			for (int k = 0;k < j;k++) {
				//cout << k << endl;
				double randValue = permuteCorrMatrix[j][k];
				double observedValue = origCorrMatrix[j][k];
				if (observedValue >= 0 && randValue > observedValue) { pValues[j][k]++; }//this method seems to deflate the
				else if (observedValue < 0 && randValue < observedValue) { pValues[j][k]++; }//pvalues of small rho values
			}
		}
		if ((i + 1) % (int)(numPermutations * 0.05) == 0) { cout << i + 1 << endl; }
	}

	return pValues;
}
//**********************************************************************************************************************

