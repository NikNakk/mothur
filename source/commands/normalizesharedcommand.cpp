/*
 *  normalizesharedcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 9/15/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "normalizesharedcommand.h"

 //**********************************************************************************************************************
vector<string> NormalizeSharedCommand::setParameters() {
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "LRSS", "LRSS", "none", "shared", false, false, true); parameters.push_back(pshared);
		CommandParameter prelabund("relabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "shared", false, false, true); parameters.push_back(prelabund);
		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new MultipleParameter("method", vector<string>{"totalgroup", "zscore"}, "totalgroup", false, false, true));
		nkParameters.add(new NumberParameter("norm", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new BooleanParameter("makerelabund", false, false, false));
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in NormalizeSharedCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string NormalizeSharedCommand::getHelpString() {
	try {
		string helpString = "The normalize.shared command parameters are shared, relabund, groups, method, norm, makerelabund and label.  shared or relabund is required, unless you have a valid current file.\n"
			"The groups parameter allows you to specify which of the groups in your groupfile you would like included. The group names are separated by dashes.\n"
			"The label parameter allows you to select what distance levels you would like, and are also separated by dashes.\n"
			"The method parameter allows you to select what method you would like to use to normalize. The options are totalgroup and zscore. We hope to add more ways to normalize in the future, suggestions are welcome!\n"
			"The makerelabund parameter allows you to convert a shared file to a relabund file before you normalize. default=f.\n"
			"The norm parameter allows you to number you would like to normalize to. By default this is set to the number of sequences in your smallest group.\n"
			"The normalize.shared command should be in the following format: normalize.shared(groups=yourGroups, label=yourLabels).\n"
			"Example normalize.shared(groups=A-B-C, scale=totalgroup).\n"
			"The default value for groups is all the groups in your groupfile, and all labels in your inputfile will be used.\n"
			"The normalize.shared command outputs a .norm.shared file.\n"
			"Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in NormalizeSharedCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string NormalizeSharedCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "shared") { pattern = "[filename],[distance],norm.shared"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
NormalizeSharedCommand::NormalizeSharedCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["shared"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in NormalizeSharedCommand, NormalizeSharedCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

NormalizeSharedCommand::NormalizeSharedCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;
	allLines = 1;

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
		for (it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["shared"] = tempOutNames;

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

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = ""; abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { format = "sharedfile"; inputfile = sharedfile; settings.setCurrent("shared", sharedfile); }

		relabundfile = validParameter.validFile(parameters, "relabund", true);
		if (relabundfile == "not open") { relabundfile = ""; abort = true; }
		else if (relabundfile == "not found") { relabundfile = ""; }
		else { format = "relabund"; inputfile = relabundfile; settings.setCurrent("relabund", relabundfile); }


		if ((sharedfile == "") && (relabundfile == "")) {
			//is there are current file available for any of these?
			//give priority to shared, then list, then rabund, then sabund
			//if there is a current shared file, use it
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { inputfile = sharedfile; format = "sharedfile"; LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else {
				relabundfile = settings.getCurrent("relabund");
				if (relabundfile != "") { inputfile = relabundfile; format = "relabund"; LOG(INFO) << "Using " + relabundfile + " as input file for the relabund parameter." << '\n'; }
				else {
					LOG(INFO) << "No valid current files. You must provide a list, sabund, rabund, relabund or shared file." << '\n';
					abort = true;
				}
			}
		}


		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = File::getPath(inputfile); }



		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...
		label = validParameter.validFile(parameters, "label", false);
		if (label == "not found") { label = ""; }
		else {
			if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
			else { allLines = 1; }
		}

		groups = validParameter.validFile(parameters, "groups", false);
		if (groups == "not found") { groups = ""; pickedGroups = false; }
		else {
			pickedGroups = true;
			Utility::split(groups, '-', Groups);
			m->setGroups(Groups);
		}

		method = validParameter.validFile(parameters, "method", false);				if (method == "not found") { method = "totalgroup"; }
		if ((method != "totalgroup") && (method != "zscore")) { LOG(INFO) << method + " is not a valid scaling option for the normalize.shared command. The options are totalgroup and zscore. We hope to add more ways to normalize in the future, suggestions are welcome!" << '\n'; abort = true; }

		string temp = validParameter.validFile(parameters, "norm", false);
		if (temp == "not found") {
			norm = 0;  //once you have read, set norm to smallest group number
		}
		else {
			Utility::mothurConvert(temp, norm);
			if (norm < 0) { LOG(INFO) << "norm must be positive." << '\n'; abort = true; }
		}

		temp = validParameter.validFile(parameters, "makerelabund", false);	if (temp == "") { temp = "f"; }
		makeRelabund = m->isTrue(temp);
	}

}
//**********************************************************************************************************************

int NormalizeSharedCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	input = new InputData(inputfile, format);

	//you are reading a sharedfile and you do not want to make relabund
	if ((format == "sharedfile") && (!makeRelabund)) {
		lookup = input->getSharedRAbundVectors();
		string lastLabel = lookup[0]->getLabel();

		//look for groups whose numseqs is below norm and remove them, warning the user
		if (norm != 0) {
			m->clearGroups();
			vector<string> mGroups;
			vector<SharedRAbundVector*> temp;
			for (int i = 0; i < lookup.size(); i++) {
				if (lookup[i]->getNumSeqs() < norm) {
					LOG(INFO) << lookup[i]->getGroup() + " contains " + toString(lookup[i]->getNumSeqs()) + ". Eliminating." << '\n';
					delete lookup[i];
				}
				else {
					mGroups.push_back(lookup[i]->getGroup());
					temp.push_back(lookup[i]);
				}
			}
			lookup = temp;
			m->setGroups(mGroups);
		}

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		if (method == "totalgroup") {
			//set norm to smallest group number
			if (norm == 0) {
				norm = lookup[0]->getNumSeqs();
				for (int i = 1; i < lookup.size(); i++) {
					if (lookup[i]->getNumSeqs() < norm) { norm = lookup[i]->getNumSeqs(); }
				}
			}

			LOG(INFO) << "Normalizing to " + toString(norm) + "." << '\n';
		}


		//as long as you are not at the end of the file or done wih the lines you want
		while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();  for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } m->clearGroups();   return 0; }

			if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

				LOG(INFO) << lookup[0]->getLabel() << '\n';
				normalize(lookup);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());
			}

			if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = lookup[0]->getLabel();

				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
				lookup = input->getSharedRAbundVectors(lastLabel);
				LOG(INFO) << lookup[0]->getLabel() << '\n';

				normalize(lookup);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());

				//restore real lastlabel to save below
				lookup[0]->setLabel(saveLabel);
			}

			lastLabel = lookup[0]->getLabel();
			//prevent memory leak
			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; lookup[i] = NULL; }

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear(); m->clearGroups();  return 0; }

			//get next line to process
			lookup = input->getSharedRAbundVectors();
		}

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear(); m->clearGroups();   return 0; }

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

			normalize(lookup);

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		}

	}
	else { //relabund values
		lookupFloat = input->getSharedRAbundFloatVectors();
		string lastLabel = lookupFloat[0]->getLabel();

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		//look for groups whose numseqs is below norm and remove them, warning the user
		if (norm != 0) {
			m->clearGroups();
			vector<string> mGroups;
			vector<SharedRAbundFloatVector*> temp;
			for (int i = 0; i < lookupFloat.size(); i++) {
				if (lookupFloat[i]->getNumSeqs() < norm) {
					LOG(INFO) << lookupFloat[i]->getGroup() + " contains " + toString(lookupFloat[i]->getNumSeqs()) + ". Eliminating." << '\n';
					delete lookupFloat[i];
				}
				else {
					mGroups.push_back(lookupFloat[i]->getGroup());
					temp.push_back(lookupFloat[i]);
				}
			}
			lookupFloat = temp;
			m->setGroups(mGroups);
		}

		//set norm to smallest group number
		if (method == "totalgroup") {
			if (norm == 0) {
				norm = lookupFloat[0]->getNumSeqs();
				for (int i = 1; i < lookupFloat.size(); i++) {
					if (lookupFloat[i]->getNumSeqs() < norm) { norm = lookupFloat[i]->getNumSeqs(); }
				}
			}

			LOG(INFO) << "Normalizing to " + toString(norm) + "." << '\n';
		}

		//as long as you are not at the end of the file or done wih the lines you want
		while ((lookupFloat[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();  for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; } m->clearGroups();  return 0; }

			if (allLines == 1 || labels.count(lookupFloat[0]->getLabel()) == 1) {

				LOG(INFO) << lookupFloat[0]->getLabel() << '\n';

				normalize(lookupFloat);

				processedLabels.insert(lookupFloat[0]->getLabel());
				userLabels.erase(lookupFloat[0]->getLabel());
			}

			if ((Utility::anyLabelsToProcess(lookupFloat[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = lookupFloat[0]->getLabel();

				for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; }
				lookupFloat = input->getSharedRAbundFloatVectors(lastLabel);

				LOG(INFO) << lookupFloat[0]->getLabel() << '\n';

				normalize(lookupFloat);

				processedLabels.insert(lookupFloat[0]->getLabel());
				userLabels.erase(lookupFloat[0]->getLabel());

				//restore real lastlabel to save below
				lookupFloat[0]->setLabel(saveLabel);
			}

			lastLabel = lookupFloat[0]->getLabel();
			//prevent memory leak
			for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; lookupFloat[i] = NULL; }

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear(); m->clearGroups();   return 0; }

			//get next line to process
			lookupFloat = input->getSharedRAbundFloatVectors();
		}

		if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear(); m->clearGroups();   return 0; }

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
			for (int i = 0; i < lookupFloat.size(); i++) { if (lookupFloat[i] != NULL) { delete lookupFloat[i]; } }
			lookupFloat = input->getSharedRAbundFloatVectors(lastLabel);

			LOG(INFO) << lookupFloat[0]->getLabel() << '\n';

			normalize(lookupFloat);

			for (int i = 0; i < lookupFloat.size(); i++) { delete lookupFloat[i]; }
		}

	}
	//reset groups parameter
	m->clearGroups();
	delete input;

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear(); return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	//LOG(INFO) << outputFileName << '\n'; outputNames.push_back(outputFileName); outputTypes["shared"].push_back(outputFileName);
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	//set shared file as new current sharedfile
	string current = "";
	itTypes = outputTypes.find("shared");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("shared", current); }
	}

	return 0;
}
//**********************************************************************************************************************

int NormalizeSharedCommand::normalize(vector<SharedRAbundVector*>& thisLookUp) {
	//save mothurOut's binLabels to restore for next label
	vector<string> saveBinLabels = settings.currentSharedBinLabels;

	if (pickedGroups) { eliminateZeroOTUS(thisLookUp); }

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
	variables["[distance]"] = thisLookUp[0]->getLabel();
	string outputFileName = getOutputFileName("shared", variables);

	ofstream out;
	File::openOutputFile(outputFileName, out);
	outputNames.push_back(outputFileName); outputTypes["shared"].push_back(outputFileName);

	if (method == "totalgroup") {

		//save numSeqs since they will change as the data is normalized
		vector<int> sizes;
		for (int i = 0; i < thisLookUp.size(); i++) { sizes.push_back(thisLookUp[i]->getNumSeqs()); }

		for (int j = 0; j < thisLookUp[0]->getNumBins(); j++) {

			for (int i = 0; i < thisLookUp.size(); i++) {

				if (ctrlc_pressed) { out.close(); return 0; }

				int abund = thisLookUp[i]->getAbundance(j);

				float relabund = abund / (float)sizes[i];
				float newNorm = relabund * norm;

				//round to nearest int
				int finalNorm = (int)floor((newNorm + 0.5));

				thisLookUp[i]->set(j, finalNorm, thisLookUp[i]->getGroup());
			}
		}

	}
	else if (method == "zscore") {

		for (int j = 0; j < thisLookUp[0]->getNumBins(); j++) {

			if (ctrlc_pressed) { out.close(); return 0; }

			//calc mean
			float mean = 0.0;
			for (int i = 0; i < thisLookUp.size(); i++) { mean += thisLookUp[i]->getAbundance(j); }
			mean /= (float)thisLookUp.size();

			//calc standard deviation
			float sumSquared = 0.0;
			for (int i = 0; i < thisLookUp.size(); i++) { sumSquared += (((float)thisLookUp[i]->getAbundance(j) - mean) * ((float)thisLookUp[i]->getAbundance(j) - mean)); }
			sumSquared /= (float)thisLookUp.size();

			float standardDev = sqrt(sumSquared);

			for (int i = 0; i < thisLookUp.size(); i++) {
				int finalNorm = 0;
				if (standardDev != 0) { // stop divide by zero
					float newNorm = ((float)thisLookUp[i]->getAbundance(j) - mean) / standardDev;
					//round to nearest int
					finalNorm = (int)floor((newNorm + 0.5));
				}

				thisLookUp[i]->set(j, finalNorm, thisLookUp[i]->getGroup());
			}
		}

	}
	else { LOG(INFO) << method + " is not a valid scaling option." << '\n'; ctrlc_pressed = true; return 0; }



	eliminateZeroOTUS(thisLookUp);

	thisLookUp[0]->printHeaders(out);

	for (int i = 0; i < thisLookUp.size(); i++) {
		out << thisLookUp[i]->getLabel() << '\t' << thisLookUp[i]->getGroup() << '\t';
		thisLookUp[i]->print(out);
	}

	out.close();

	settings.currentSharedBinLabels = saveBinLabels;

	return 0;
}
//**********************************************************************************************************************

int NormalizeSharedCommand::normalize(vector<SharedRAbundFloatVector*>& thisLookUp) {

	//save mothurOut's binLabels to restore for next label
	vector<string> saveBinLabels = settings.currentSharedBinLabels;

	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
	variables["[distance]"] = thisLookUp[0]->getLabel();
	string outputFileName = getOutputFileName("shared", variables);
	ofstream out;
	File::openOutputFile(outputFileName, out);
	outputNames.push_back(outputFileName); outputTypes["shared"].push_back(outputFileName);


	if (pickedGroups) { eliminateZeroOTUS(thisLookUp); }

	if (method == "totalgroup") {

		//save numSeqs since they will change as the data is normalized
		vector<float> sizes;
		for (int i = 0; i < thisLookUp.size(); i++) { sizes.push_back(thisLookUp[i]->getNumSeqs()); }

		for (int j = 0; j < thisLookUp[0]->getNumBins(); j++) {

			for (int i = 0; i < thisLookUp.size(); i++) {

				if (ctrlc_pressed) { out.close(); return 0; }

				float abund = thisLookUp[i]->getAbundance(j);

				float relabund = abund / (float)sizes[i];
				float newNorm = relabund * norm;

				thisLookUp[i]->set(j, newNorm, thisLookUp[i]->getGroup());
			}
		}

	}
	else if (method == "zscore") {
		for (int j = 0; j < thisLookUp[0]->getNumBins(); j++) {

			if (ctrlc_pressed) { out.close(); return 0; }

			//calc mean
			float mean = 0.0;
			for (int i = 0; i < thisLookUp.size(); i++) { mean += thisLookUp[i]->getAbundance(j); }
			mean /= (float)thisLookUp.size();

			//calc standard deviation
			float sumSquared = 0.0;
			for (int i = 0; i < thisLookUp.size(); i++) { sumSquared += ((thisLookUp[i]->getAbundance(j) - mean) * (thisLookUp[i]->getAbundance(j) - mean)); }
			sumSquared /= (float)thisLookUp.size();

			float standardDev = sqrt(sumSquared);

			for (int i = 0; i < thisLookUp.size(); i++) {
				float newNorm = 0.0;
				if (standardDev != 0) { // stop divide by zero
					newNorm = (thisLookUp[i]->getAbundance(j) - mean) / standardDev;
				}
				thisLookUp[i]->set(j, newNorm, thisLookUp[i]->getGroup());
			}
		}

	}
	else { LOG(INFO) << method + " is not a valid scaling option." << '\n'; ctrlc_pressed = true; return 0; }


	eliminateZeroOTUS(thisLookUp);

	thisLookUp[0]->printHeaders(out);

	for (int i = 0; i < thisLookUp.size(); i++) {
		out << thisLookUp[i]->getLabel() << '\t' << thisLookUp[i]->getGroup() << '\t';
		thisLookUp[i]->print(out);
	}

	out.close();

	settings.currentSharedBinLabels = saveBinLabels;

	return 0;
}
//**********************************************************************************************************************
int NormalizeSharedCommand::eliminateZeroOTUS(vector<SharedRAbundVector*>& thislookup) {

	vector<SharedRAbundVector*> newLookup;
	for (int i = 0; i < thislookup.size(); i++) {
		SharedRAbundVector* temp = new SharedRAbundVector();
		temp->setLabel(thislookup[i]->getLabel());
		temp->setGroup(thislookup[i]->getGroup());
		newLookup.push_back(temp);
	}

	//for each bin
	vector<string> newBinLabels;
	string snumBins = toString(thislookup[0]->getNumBins());
	for (int i = 0; i < thislookup[0]->getNumBins(); i++) {
		if (ctrlc_pressed) { for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; } return 0; }

		//look at each sharedRabund and make sure they are not all zero
		bool allZero = true;
		for (int j = 0; j < thislookup.size(); j++) {
			if (thislookup[j]->getAbundance(i) != 0) { allZero = false;  break; }
		}

		//if they are not all zero add this bin
		if (!allZero) {
			for (int j = 0; j < thislookup.size(); j++) {
				newLookup[j]->push_back(thislookup[j]->getAbundance(i), thislookup[j]->getGroup());
			}
			//if there is a bin label use it otherwise make one
			string binLabel = "Otu";
			string sbinNumber = toString(i + 1);
			if (sbinNumber.length() < snumBins.length()) {
				int diff = snumBins.length() - sbinNumber.length();
				for (int h = 0; h < diff; h++) { binLabel += "0"; }
			}
			binLabel += sbinNumber;
			if (i < settings.currentSharedBinLabels.size()) { binLabel = settings.currentSharedBinLabels[i]; }

			newBinLabels.push_back(binLabel);
		}
	}

	for (int j = 0; j < thislookup.size(); j++) { delete thislookup[j]; }

	thislookup = newLookup;
	settings.currentSharedBinLabels = newBinLabels;

	return 0;

}
//**********************************************************************************************************************
int NormalizeSharedCommand::eliminateZeroOTUS(vector<SharedRAbundFloatVector*>& thislookup) {

	vector<SharedRAbundFloatVector*> newLookup;
	for (int i = 0; i < thislookup.size(); i++) {
		SharedRAbundFloatVector* temp = new SharedRAbundFloatVector();
		temp->setLabel(thislookup[i]->getLabel());
		temp->setGroup(thislookup[i]->getGroup());
		newLookup.push_back(temp);
	}

	//for each bin
	vector<string> newBinLabels;
	string snumBins = toString(thislookup[0]->getNumBins());
	for (int i = 0; i < thislookup[0]->getNumBins(); i++) {
		if (ctrlc_pressed) { for (int j = 0; j < newLookup.size(); j++) { delete newLookup[j]; } return 0; }

		//look at each sharedRabund and make sure they are not all zero
		bool allZero = true;
		for (int j = 0; j < thislookup.size(); j++) {
			if (thislookup[j]->getAbundance(i) != 0) { allZero = false;  break; }
		}

		//if they are not all zero add this bin
		if (!allZero) {
			for (int j = 0; j < thislookup.size(); j++) {
				newLookup[j]->push_back(thislookup[j]->getAbundance(i), thislookup[j]->getGroup());
			}
			//if there is a bin label use it otherwise make one
			string binLabel = "Otu";
			string sbinNumber = toString(i + 1);
			if (sbinNumber.length() < snumBins.length()) {
				int diff = snumBins.length() - sbinNumber.length();
				for (int h = 0; h < diff; h++) { binLabel += "0"; }
			}
			binLabel += sbinNumber;
			if (i < settings.currentSharedBinLabels.size()) { binLabel = settings.currentSharedBinLabels[i]; }

			newBinLabels.push_back(binLabel);
		}
	}

	for (int j = 0; j < thislookup.size(); j++) { delete thislookup[j]; }

	thislookup = newLookup;
	settings.currentSharedBinLabels = newBinLabels;

	return 0;

}

//**********************************************************************************************************************
