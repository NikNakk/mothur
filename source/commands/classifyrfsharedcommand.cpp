//
//  classifysharedcommand.cpp
//  Mothur
//
//  Created by Abu Zaher Md. Faridee on 8/13/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "classifyrfsharedcommand.h"
#include "randomforest.hpp"
#include "decisiontree.hpp"
#include "rftreenode.hpp"

//**********************************************************************************************************************
vector<string> ClassifyRFSharedCommand::setParameters() {
	try {
		//CommandParameter pprocessors("processors", "Number", "", "1", "", "", "",false,false); parameters.push_back(pprocessors);        
		CommandParameter pshared("shared", "InputTypes", "", "", "none", "none", "none", "summary", false, true, true); parameters.push_back(pshared);
		CommandParameter pdesign("design", "InputTypes", "", "", "none", "none", "none", "", false, true, true); parameters.push_back(pdesign);
		CommandParameter potupersplit("otupersplit", "Multiple", "log2-squareroot", "log2", "", "", "", "", false, false); parameters.push_back(potupersplit);
		CommandParameter psplitcriteria("splitcriteria", "Multiple", "gainratio-infogain", "gainratio", "", "", "", "", false, false); parameters.push_back(psplitcriteria);
		nkParameters.add(new NumberParameter("numtrees", -INFINITY, INFINITY, 100, false, false));

		// parameters related to pruning
		CommandParameter pdopruning("prune", "Boolean", "", "T", "", "", "", "", false, false); parameters.push_back(pdopruning);
		CommandParameter ppruneaggrns("pruneaggressiveness", "Number", "", "0.9", "", "", "", "", false, false); parameters.push_back(ppruneaggrns);
		CommandParameter pdiscardhetrees("discarderrortrees", "Boolean", "", "T", "", "", "", "", false, false); parameters.push_back(pdiscardhetrees);
		CommandParameter phetdiscardthreshold("errorthreshold", "Number", "", "0.4", "", "", "", "", false, false); parameters.push_back(phetdiscardthreshold);
		CommandParameter psdthreshold("stdthreshold", "Number", "", "0.0", "", "", "", "", false, false); parameters.push_back(psdthreshold);
		// pruning params end

		nkParameters.add(new StringParameter("groups", "", false, false));
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.addStandardParameters();

		return nkParameters.getNames();
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifySharedCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClassifyRFSharedCommand::getHelpString() {
	try {
		string helpString = "The classify.rf command allows you to ....\n"
			"The classify.rf command parameters are: shared, design, label, groups, otupersplit.\n"
			"The label parameter is used to analyze specific labels in your input.\n"
			"The groups parameter allows you to specify which of the groups in your designfile you would like analyzed.\n"
			"The classify.rf should be in the following format: \n"
			"classify.rf(shared=yourSharedFile, design=yourDesignFile)\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifySharedCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClassifyRFSharedCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "summary") { pattern = "[filename],[distance],summary"; } //makes file like: amazon.0.03.fasta
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true;; }

	return pattern;
}
//**********************************************************************************************************************

ClassifyRFSharedCommand::ClassifyRFSharedCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["summary"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifySharedCommand, ClassifySharedCommand";
		exit(1);
	}
}

//**********************************************************************************************************************
ClassifyRFSharedCommand::ClassifyRFSharedCommand(Settings& settings, string option) : Command(settings, option) {
	//try {
	//    
	//      //Commonly used to process list, rabund, sabund, shared and relabund files.  Look at "smart distancing" examples below in the execute function.
	//    string label = validParameter.validFile(parameters, "label", false);
	//    if (label == "not found") { label = ""; }
	//    else {
	//      if(label != "all") {  Utility::split(label, '-', labels);  allLines = 0;  }
	//      else { allLines = 1;  }
	//    }
	//  }
	//  
	//}
	//catch(exception& e) {
	//  LOG(FATAL) << e.what() << " in ClassifySharedCommand, ClassifySharedCommand";
	//  exit(1);
	//}
}
//**********************************************************************************************************************
int ClassifyRFSharedCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		InputData input(sharedfile, "sharedfile");
		vector<SharedRAbundVector*> lookup = input.getSharedRAbundVectors();

		//read design file
		designMap.read(designfile);

		string lastLabel = lookup[0]->getLabel();
		set<string> processedLabels;
		set<string> userLabels = labels;

		//as long as you are not at the end of the file or done wih the lines you want
		while ((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }  return 0; }

			if (allLines == 1 || labels.count(lookup[0]->getLabel()) == 1) {

				LOG(INFO) << lookup[0]->getLabel() << '\n';

				processSharedAndDesignData(lookup);

				processedLabels.insert(lookup[0]->getLabel());
				userLabels.erase(lookup[0]->getLabel());
			}

			if ((Utility::anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = lookup[0]->getLabel();

				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
				lookup = input.getSharedRAbundVectors(lastLabel);
				LOG(INFO) << lookup[0]->getLabel() << '\n';
				processSharedAndDesignData(lookup);

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

			LOG(INFO) << lookup[0]->getLabel() << '\n';

			processSharedAndDesignData(lookup);

			for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }

		}

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		return 0;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifySharedCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************

void ClassifyRFSharedCommand::processSharedAndDesignData(vector<SharedRAbundVector*> lookup) {
	try {
		//    for (int i = 0; i < designMap->getNamesOfGroups().size(); i++) {
		//      string groupName = designMap->getNamesOfGroups()[i];
		//      cout << groupName << endl;
		//    }

		//    for (int i = 0; i < designMap->getNumSeqs(); i++) {
		//      string sharedGroupName = designMap->getNamesSeqs()[i];
		//      string treatmentName = designMap->getGroup(sharedGroupName);
		//      cout << sharedGroupName << " : " << treatmentName <<  endl;
		//    }

		map<string, int> treatmentToIntMap;
		map<int, string> intToTreatmentMap;
		vector<string> groups = designMap.getCategory();
		for (int i = 0; i < groups.size(); i++) {
			string treatmentName = groups[i];
			treatmentToIntMap[treatmentName] = i;
			intToTreatmentMap[i] = treatmentName;
		}

		int numSamples = lookup.size();
		int numFeatures = lookup[0]->getNumBins();

		int numRows = numSamples;
		int numColumns = numFeatures + 1;           // extra one space needed for the treatment/outcome

		vector< vector<int> > dataSet(numRows, vector<int>(numColumns, 0));

		vector<string> names;

		for (int i = 0; i < lookup.size(); i++) {
			string sharedGroupName = lookup[i]->getGroup();
			names.push_back(sharedGroupName);
			string treatmentName = designMap.get(sharedGroupName);

			int j = 0;
			for (; j < lookup[i]->getNumBins(); j++) {
				int otuCount = lookup[i]->getAbundance(j);
				dataSet[i][j] = otuCount;
			}
			dataSet[i][j] = treatmentToIntMap[treatmentName];
		}

		RandomForest randomForest(dataSet, numDecisionTrees, treeSplitCriterion, doPruning, pruneAggressiveness, discardHighErrorTrees, highErrorTreeDiscardThreshold, optimumFeatureSubsetSelectionCriteria, featureStandardDeviationThreshold);

		randomForest.populateDecisionTrees();
		randomForest.calcForrestErrorRate();
		randomForest.printConfusionMatrix(intToTreatmentMap);

		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(sharedfile)) + "RF.";
		variables["[distance]"] = lookup[0]->getLabel();
		string filename = getOutputFileName("summary", variables);
		outputNames.push_back(filename); outputTypes["summary"].push_back(filename);
		randomForest.calcForrestVariableImportance(filename);

		//
		map<string, string> variable;
		variable["[filename]"] = outputDir + File::getRootName(File::getSimpleName(sharedfile)) + "misclassifications.";
		variable["[distance]"] = lookup[0]->getLabel();
		string mc_filename = getOutputFileName("summary", variable);
		outputNames.push_back(mc_filename); outputTypes["summary"].push_back(mc_filename);
		randomForest.getMissclassifications(mc_filename, intToTreatmentMap, names);
		//

		LOG(INFO) << "";
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifySharedCommand, processSharedAndDesignData";
		exit(1);
	}
}
//**********************************************************************************************************************

