//
//  classifysvmsharedcommand.h
//  Mothur
//
//  Created by Joshua Lynch on 6/28/2013.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//
//  This class is based on ClassifySharedCommand
//

#ifndef __Mothur__classifysvmsharedcommand__
#define __Mothur__classifysvmsharedcommand__

#include "command.hpp"
#include "inputdata.h"
#include "svm.hpp"
#include "designmap.h"

class ClassifySvmSharedCommand : public Command {
public:
	ClassifySvmSharedCommand(Settings& settings);
	ClassifySvmSharedCommand(Settings& settings, string option);
	//~ClassifySvmSharedCommand() throw() {};
	~ClassifySvmSharedCommand() {};

	vector<string> setParameters();
	string getCommandName() { return "classifysvm.shared"; }
	string getCommandCategory() { return "OTU-Based Approaches"; }
	string getHelpString();
	string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/ClassifySvm.shared\n"; }
	string getDescription() { return "implements the support vector machine machine learning algorithm to identify OTUs that can be used to differentiate between various groups of samples"; }
	int execute();

	void help() { LOG(INFO) << getHelpString(); }

	void readSharedAndDesignFiles(const string&, const string&, LabeledObservationVector&, FeatureVector&);
	void readSharedRAbundVectors(vector<SharedRAbundVector*>&, DesignMap&, LabeledObservationVector&, FeatureVector&);

	//bool interruptTraining() { return ctrlc_pressed; }

	vector<double>& getSmocList() { return smocList; }
	const KernelParameterRangeMap& getKernelParameterRangeMap() { return kernelParameterRangeMap; }

	//bool interruptTraining() { return ctrlc_pressed; }

	//std::vector<double>& getSmocList() { return smocList; }
	//const KernelParameterRangeMap& getKernelParameterRangeMap() { return kernelParameterRangeMap; }


private:
	bool abort;
	string outputDir;
	vector<string> outputNames, Groups;

	string sharedfile, designfile;
	set<string> labels;
	bool allLines;

	int processors;
	bool useTiming;

	DesignMap designMap;

	//void readSharedAndDesignFiles(const std::string&, const std::string&, LabeledObservationVector&, FeatureVector&);
	//void readSharedRAbundVectors(vector<SharedRAbundVector*>&, GroupMap&, LabeledObservationVector&, FeatureVector&);

	// mode is either "rfe" or "classify"
	string mode;

	int evaluationFoldCount;
	int trainingFoldCount;
	vector<double> smocList;
	KernelParameterRangeMap kernelParameterRangeMap;

	string transformName;

	int verbosity;

	double stdthreshold;

	//int numDecisionTrees;
	//string treeSplitCriterion, optimumFeatureSubsetSelectionCriteria;
	//bool doPruning, discardHighErrorTrees;
	//double pruneAggressiveness, highErrorTreeDiscardThreshold, featureStandardDeviationThreshold;

	void processSharedAndDesignData(vector<SharedRAbundVector*> lookup);
	void trainSharedAndDesignData(vector<SharedRAbundVector*> lookup);

	void getParameterValue(int& target, string pstring, int defaultvalue) {
		if (pstring == "not found" || pstring == "") {
			target = defaultvalue;
		}
		else {
			Utility::mothurConvert(pstring, target);
		}
	}


};

#endif /* defined(__Mothur__classifysvmsharedcommand__) */
