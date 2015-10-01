/*
 *  trimflowscommand.cpp
 *  Mothur
 *
 *  Created by Pat Schloss on 12/22/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "trimflowscommand.h"
#include "needlemanoverlap.hpp"


 //**********************************************************************************************************************
vector<string> TrimFlowsCommand::setParameters() {
	try {
		CommandParameter pflow("flow", "InputTypes", "", "", "none", "none", "none", "flow-file", false, true, true); parameters.push_back(pflow);
		CommandParameter poligos("oligos", "InputTypes", "", "", "none", "none", "none", "", false, false, true); parameters.push_back(poligos);
		CommandParameter preorient("checkorient", "Boolean", "", "F", "", "", "", "", false, false, true); parameters.push_back(preorient);
		nkParameters.add(new NumberParameter("maxhomop", -INFINITY, INFINITY, 9, false, false));
		nkParameters.add(new NumberParameter("maxflows", -INFINITY, INFINITY, 450, false, false));
		nkParameters.add(new NumberParameter("minflows", -INFINITY, INFINITY, 450, false, false));
		CommandParameter ppdiffs("pdiffs", "Number", "", "0", "", "", "", "", false, false, true); parameters.push_back(ppdiffs);
		CommandParameter pbdiffs("bdiffs", "Number", "", "0", "", "", "", "", false, false, true); parameters.push_back(pbdiffs);
		nkParameters.add(new NumberParameter("ldiffs", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("sdiffs", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("tdiffs", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new NumberParameter("signal", -INFINITY, INFINITY, 0.50, false, false));
		nkParameters.add(new NumberParameter("noise", -INFINITY, INFINITY, 0.70, false, false));
		nkParameters.add(new BooleanParameter("allfiles", true, false, false));
		nkParameters.add(new MultipleParameter("order", vector<string>{"A", "B", "I"}, "A", false, false, true));
		nkParameters.add(new BooleanParameter("fasta", false, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in TrimFlowsCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string TrimFlowsCommand::getHelpString() {
	try {
		string helpString = "The trim.flows command reads a flowgram file and creates .....\n"
			"The oligos parameter allows you to provide an oligos file.\n"
			"The maxhomop parameter allows you to set a maximum homopolymer length. \n"
			"The tdiffs parameter is used to specify the total number of differences allowed in the sequence. The default is pdiffs + bdiffs + sdiffs + ldiffs.\n"
			"The checkorient parameter will check look for the reverse compliment of the barcode or primer in the sequence. The default is false.\n"
			"The bdiffs parameter is used to specify the number of differences allowed in the barcode. The default is 0.\n"
			"The pdiffs parameter is used to specify the number of differences allowed in the primer. The default is 0.\n"
			"The ldiffs parameter is used to specify the number of differences allowed in the linker. The default is 0.\n"
			"The sdiffs parameter is used to specify the number of differences allowed in the spacer. The default is 0.\n"
			"The order parameter options are A, B or I.  Default=A. A = TACG and B = TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC and I = TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC.\n"
			"Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n"
			"For more details please check out the wiki http://www.mothur.org/wiki/Trim.flows.\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in TrimFlowsCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string TrimFlowsCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "flow") { pattern = "[filename],[tag],flow"; }
	else if (type == "fasta") { pattern = "[filename],flow.fasta"; }
	else if (type == "file") { pattern = "[filename],flow.files"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************

TrimFlowsCommand::TrimFlowsCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["flow"] = tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["file"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in TrimFlowsCommand, TrimFlowsCommand";
		exit(1);
	}
}
//**********************************************************************************************************************

TrimFlowsCommand::TrimFlowsCommand(Settings& settings, string option) : Command(settings, option) {

	abort = false; calledHelp = false;
	comboStarts = 0;

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
		outputTypes["flow"] = tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["file"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("flow");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["flow"] = inputDir + it->second; }
			}

			it = parameters.find("oligos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["oligos"] = inputDir + it->second; }
			}

		}


		//check for required parameters
		flowFileName = validParameter.validFile(parameters, "flow", true);
		if (flowFileName == "not found") {
			flowFileName = settings.getCurrent("flow");
			if (flowFileName != "") { LOG(INFO) << "Using " + flowFileName + " as input file for the flow parameter." << '\n'; }
			else {
				LOG(INFO) << "No valid current flow file. You must provide a flow file." << '\n';
				abort = true;
			}
		}
		else if (flowFileName == "not open") { flowFileName = ""; abort = true; }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") {
			outputDir = "";
			outputDir += File::getPath(flowFileName); //if user entered a file with a path then preserve it	
		}


		//check for optional parameter and set defaults
		// ...at some point should added some additional type checking...

		string temp;
		temp = validParameter.validFile(parameters, "minflows", false);	if (temp == "not found") { temp = "450"; }
		Utility::mothurConvert(temp, minFlows);

		temp = validParameter.validFile(parameters, "maxflows", false);	if (temp == "not found") { temp = "450"; }
		Utility::mothurConvert(temp, maxFlows);


		temp = validParameter.validFile(parameters, "oligos", true);
		if (temp == "not found") { oligoFileName = ""; }
		else if (temp == "not open") { abort = true; }
		else { oligoFileName = temp;	settings.setCurrent("oligos", oligoFileName); }

		temp = validParameter.validFile(parameters, "fasta", false);		if (temp == "not found") { fasta = 0; }
		else if (m->isTrue(temp)) { fasta = 1; }

		temp = validParameter.validFile(parameters, "maxhomop", false);		if (temp == "not found") { temp = "9"; }
		Utility::mothurConvert(temp, maxHomoP);

		temp = validParameter.validFile(parameters, "signal", false);		if (temp == "not found") { temp = "0.50"; }
		Utility::mothurConvert(temp, signal);

		temp = validParameter.validFile(parameters, "noise", false);		if (temp == "not found") { temp = "0.70"; }
		Utility::mothurConvert(temp, noise);

		temp = validParameter.validFile(parameters, "bdiffs", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, bdiffs);

		temp = validParameter.validFile(parameters, "pdiffs", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, pdiffs);

		temp = validParameter.validFile(parameters, "ldiffs", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, ldiffs);

		temp = validParameter.validFile(parameters, "sdiffs", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, sdiffs);

		temp = validParameter.validFile(parameters, "tdiffs", false);		if (temp == "not found") { int tempTotal = pdiffs + bdiffs + ldiffs + sdiffs;  temp = toString(tempTotal); }
		Utility::mothurConvert(temp, tdiffs);

		if (tdiffs == 0) { tdiffs = bdiffs + pdiffs + ldiffs + sdiffs; }


		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "order", false);  if (temp == "not found") { temp = "A"; }
		if (temp.length() > 1) {
			LOG(LOGERROR) << "" + temp + " is not a valid option for order. order options are A, B, or I. A = TACG, B = TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC, and I = TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC.\n";  abort = true;
		}
		else {
			if (toupper(temp[0]) == 'A') { flowOrder = "TACG"; }
			else if (toupper(temp[0]) == 'B') {
				flowOrder = "TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC";
			}
			else if (toupper(temp[0]) == 'I') {
				flowOrder = "TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC";
			}
			else {
				LOG(LOGERROR) << "" + temp + " is not a valid option for order. order options are A, B, or I. A = TACG, B = TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC, and I = TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC.\n";  abort = true;
			}
		}

		if (oligoFileName == "") { allFiles = 0; }
		else { allFiles = 1; }

		temp = validParameter.validFile(parameters, "checkorient", false);		if (temp == "not found") { temp = "F"; }
		reorient = m->isTrue(temp);

		numBarcodes = 0;
		numFPrimers = 0;
		numRPrimers = 0;
		numLinkers = 0;
		numSpacers = 0;
	}
}

//***************************************************************************************************************

int TrimFlowsCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(flowFileName));
		string fastaFileName = getOutputFileName("fasta", variables);
		if (fasta) { outputNames.push_back(fastaFileName); outputTypes["fasta"].push_back(fastaFileName); }

		variables["[tag]"] = "trim";
		string trimFlowFileName = getOutputFileName("flow", variables);
		outputNames.push_back(trimFlowFileName); outputTypes["flow"].push_back(trimFlowFileName);

		variables["[tag]"] = "scrap";
		string scrapFlowFileName = getOutputFileName("flow", variables);
		outputNames.push_back(scrapFlowFileName); outputTypes["flow"].push_back(scrapFlowFileName);



		vector<unsigned long long> flowFilePos;
#if defined (UNIX)
		flowFilePos = getFlowFileBreaks();
		for (int i = 0; i < (flowFilePos.size() - 1); i++) {
			lines.push_back(new linePair(flowFilePos[i], flowFilePos[(i + 1)]));
		}
#else
		ifstream in; File::openInputFile(flowFileName, in); in >> numFlows; in.close();
		///////////////////////////////////////// until I fix multiple processors for windows //////////////////	
		processors = 1;
		///////////////////////////////////////// until I fix multiple processors for windows //////////////////		
		if (processors == 1) {
			lines.push_back(new linePair(0, 1000));
		}
		else {
			int numFlowLines;
			flowFilePos = m->setFilePosEachLine(flowFileName, numFlowLines);
			flowFilePos.erase(flowFilePos.begin() + 1); numFlowLines--;

			//figure out how many sequences you have to process
			int numSeqsPerProcessor = numFlowLines / processors;
			cout << numSeqsPerProcessor << '\t' << numFlowLines << endl;
			for (int i = 0; i < processors; i++) {
				int startIndex = i * numSeqsPerProcessor;
				if (i == (processors - 1)) { numSeqsPerProcessor = numFlowLines - i * numSeqsPerProcessor; }
				lines.push_back(new linePair(flowFilePos[startIndex], numSeqsPerProcessor));
				cout << flowFilePos[startIndex] << '\t' << numSeqsPerProcessor << endl;
			}
		}
#endif

		vector<vector<string> > barcodePrimerComboFileNames;
		if (oligoFileName != "") {
			getOligos(barcodePrimerComboFileNames);
		}

		if (processors == 1) {
			driverCreateTrim(flowFileName, trimFlowFileName, scrapFlowFileName, fastaFileName, barcodePrimerComboFileNames, lines[0]);
		}
		else {
			createProcessesCreateTrim(flowFileName, trimFlowFileName, scrapFlowFileName, fastaFileName, barcodePrimerComboFileNames);
		}

		if (ctrlc_pressed) { return 0; }

		string flowFilesFileName;
		ofstream output;

		if (allFiles) {
			set<string> namesAlreadyProcessed;
			set<string> namesToRemove;
			flowFilesFileName = getOutputFileName("file", variables);
			File::openOutputFile(flowFilesFileName, output);

			for (int i = 0;i < barcodePrimerComboFileNames.size();i++) {
				for (int j = 0;j < barcodePrimerComboFileNames[0].size();j++) {
					if (namesAlreadyProcessed.count(barcodePrimerComboFileNames[i][j]) == 0) {
						if (barcodePrimerComboFileNames[i][j] != "") {
							if (namesToRemove.count(barcodePrimerComboFileNames[i][j]) == 0) {
								FILE * pFile;
								unsigned long long size;

								//get num bytes in file
								pFile = fopen(barcodePrimerComboFileNames[i][j].c_str(), "rb");
								if (pFile == NULL) perror("Error opening file");
								else {
									fseek(pFile, 0, SEEK_END);
									size = ftell(pFile);
									fclose(pFile);
								}

								if (size < 10) {
									File::remove(barcodePrimerComboFileNames[i][j]);
									namesToRemove.insert(barcodePrimerComboFileNames[i][j]);
								}
								else {
									output << m->getFullPathName(barcodePrimerComboFileNames[i][j]) << endl;
								}
								namesAlreadyProcessed.insert(barcodePrimerComboFileNames[i][j]);
							}
						}
					}
				}
			}
			output.close();

			//remove names for outputFileNames, just cleans up the output
			vector<string> outputNames2;
			for (int i = 0; i < outputNames.size(); i++) { if (namesToRemove.count(outputNames[i]) == 0) { outputNames2.push_back(outputNames[i]); } }
			outputNames = outputNames2;
		}
		else {
			flowFilesFileName = getOutputFileName("file", variables);
			File::openOutputFile(flowFilesFileName, output);

			output << m->getFullPathName(trimFlowFileName) << endl;

			output.close();
		}
		outputTypes["file"].push_back(flowFilesFileName);
		outputNames.push_back(flowFilesFileName);
		settings.setCurrent("file", flowFilesFileName);

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";

		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in TrimSeqsCommand, execute";
		exit(1);
	}
}

//***************************************************************************************************************

int TrimFlowsCommand::driverCreateTrim(string flowFileName, string trimFlowFileName, string scrapFlowFileName, string fastaFileName, vector<vector<string> > thisBarcodePrimerComboFileNames, linePair* line) {

	try {
		ofstream trimFlowFile;
		File::openOutputFile(trimFlowFileName, trimFlowFile);
		trimFlowFile.setf(ios::fixed, ios::floatfield); trimFlowFile.setf(ios::showpoint);

		ofstream scrapFlowFile;
		File::openOutputFile(scrapFlowFileName, scrapFlowFile);
		scrapFlowFile.setf(ios::fixed, ios::floatfield); scrapFlowFile.setf(ios::showpoint);

		ofstream fastaFile;
		if (fasta) { File::openOutputFile(fastaFileName, fastaFile); }

		ifstream flowFile;
		File::openInputFile(flowFileName, flowFile);

		flowFile.seekg(line->start);

		if (line->start == 0) {
			flowFile >> numFlows; File::gobble(flowFile);
			scrapFlowFile << numFlows << endl;
			trimFlowFile << maxFlows << endl;
			if (allFiles) {
				for (int i = 0;i < thisBarcodePrimerComboFileNames.size();i++) {
					for (int j = 0;j < thisBarcodePrimerComboFileNames[0].size();j++) {
						if (thisBarcodePrimerComboFileNames[i][j] != "") {
							ofstream temp;
							File::openOutputFile(thisBarcodePrimerComboFileNames[i][j], temp);
							temp << maxFlows << endl;
							temp.close();
						}
					}
				}
			}
		}

		FlowData flowData(numFlows, signal, noise, maxHomoP, flowOrder);
		//cout << " driver flowdata address " <<  &flowData  << &flowFile << endl;	
		int count = 0;
		bool moreSeqs = 1;

		TrimOligos* trimOligos = NULL;
		if (pairedOligos) { trimOligos = new TrimOligos(pdiffs, bdiffs, 0, 0, oligos.getPairedPrimers(), oligos.getPairedBarcodes(), false); }
		else { trimOligos = new TrimOligos(pdiffs, bdiffs, ldiffs, sdiffs, oligos.getPrimers(), oligos.getBarcodes(), oligos.getReversePrimers(), oligos.getLinkers(), oligos.getSpacers()); }

		TrimOligos* rtrimOligos = NULL;
		if (reorient) {
			rtrimOligos = new TrimOligos(pdiffs, bdiffs, 0, 0, oligos.getReorientedPairedPrimers(), oligos.getReorientedPairedBarcodes(), false); numBarcodes = oligos.getReorientedPairedBarcodes().size();
		}


		while (moreSeqs) {

			if (ctrlc_pressed) { break; }

			int success = 1;
			int currentSeqDiffs = 0;
			string trashCode = "";
			string commentString = "";

			flowData.getNext(flowFile);
			flowData.capFlows(maxFlows);

			Sequence currSeq = flowData.getSequence();
			//for reorient
			Sequence savedSeq(currSeq.getName(), currSeq.getAligned());

			if (!flowData.hasMinFlows(minFlows)) {	//screen to see if sequence is of a minimum number of flows
				success = 0;
				trashCode += 'l';
			}
			if (!flowData.hasGoodHomoP()) {	//screen to see if sequence meets the maximum homopolymer limit
				success = 0;
				trashCode += 'h';
			}

			int primerIndex = 0;
			int barcodeIndex = 0;

			if (numLinkers != 0) {
				success = trimOligos->stripLinker(currSeq);
				if (success > ldiffs) { trashCode += 'k'; }
				else { currentSeqDiffs += success; }

			}

			if (app.isDebug) { LOG(DEBUG) << "" + currSeq.getName() + " " + currSeq.getUnaligned() + "\n"; }

			if (numBarcodes != 0) {
				vector<int> results = trimOligos->stripBarcode(currSeq, barcodeIndex);
				if (pairedOligos) {
					success = results[0] + results[2];
					commentString += "fbdiffs=" + toString(results[0]) + "(" + trimOligos->getCodeValue(results[1], bdiffs) + "), rbdiffs=" + toString(results[2]) + "(" + trimOligos->getCodeValue(results[3], bdiffs) + ") ";
				}
				else {
					success = results[0];
					commentString += "bdiffs=" + toString(results[0]) + "(" + trimOligos->getCodeValue(results[1], bdiffs) + ") ";
				}
				if (success > bdiffs) { trashCode += 'b'; }
				else { currentSeqDiffs += success; }
			}

			if (numSpacers != 0) {
				success = trimOligos->stripSpacer(currSeq);
				if (success > sdiffs) { trashCode += 's'; }
				else { currentSeqDiffs += success; }

			}

			if (numFPrimers != 0) {
				vector<int> results = trimOligos->stripForward(currSeq, primerIndex);
				if (pairedOligos) {
					success = results[0] + results[2];
					commentString += "fpdiffs=" + toString(results[0]) + "(" + trimOligos->getCodeValue(results[1], pdiffs) + "), rpdiffs=" + toString(results[2]) + "(" + trimOligos->getCodeValue(results[3], pdiffs) + ") ";
				}
				else {
					success = results[0];
					commentString += "fpdiffs=" + toString(results[0]) + "(" + trimOligos->getCodeValue(results[1], pdiffs) + ") ";
				}
				if (success > pdiffs) { trashCode += 'f'; }
				else { currentSeqDiffs += success; }
			}

			if (numRPrimers != 0) {
				vector<int> results = trimOligos->stripReverse(currSeq);
				success = results[0];
				commentString += "rpdiffs=" + toString(results[0]) + "(" + trimOligos->getCodeValue(results[1], pdiffs) + ") ";
				if (success > pdiffs) { trashCode += 'r'; }
				else { currentSeqDiffs += success; }
			}

			if (currentSeqDiffs > tdiffs) { trashCode += 't'; }

			if (reorient && (trashCode != "")) { //if you failed and want to check the reverse
				int thisSuccess = 0;
				string thisTrashCode = "";
				int thisCurrentSeqsDiffs = 0;
				string thiscommentString = "";

				int thisBarcodeIndex = 0;
				int thisPrimerIndex = 0;
				//cout << currSeq.getName() << '\t' << savedSeq.getUnaligned() << endl;
				if (numBarcodes != 0) {
					vector<int> results = rtrimOligos->stripBarcode(savedSeq, thisBarcodeIndex);
					if (pairedOligos) {
						thisSuccess = results[0] + results[2];
						thiscommentString += "fbdiffs=" + toString(results[0]) + "(" + rtrimOligos->getCodeValue(results[1], bdiffs) + "), rbdiffs=" + toString(results[2]) + "(" + rtrimOligos->getCodeValue(results[3], bdiffs) + ") ";
					}
					else {
						thisSuccess = results[0];
						thiscommentString += "bdiffs=" + toString(results[0]) + "(" + rtrimOligos->getCodeValue(results[1], bdiffs) + ") ";
					}
					if (thisSuccess > bdiffs) { thisTrashCode += "b"; }
					else { thisCurrentSeqsDiffs += thisSuccess; }
				}
				//cout << currSeq.getName() << '\t' << savedSeq.getUnaligned() << endl;
				if (numFPrimers != 0) {
					vector<int> results = rtrimOligos->stripForward(savedSeq, thisPrimerIndex);
					if (pairedOligos) {
						thisSuccess = results[0] + results[2];
						thiscommentString += "fpdiffs=" + toString(results[0]) + "(" + rtrimOligos->getCodeValue(results[1], pdiffs) + "), rpdiffs=" + toString(results[2]) + "(" + rtrimOligos->getCodeValue(results[3], pdiffs) + ") ";
					}
					else {
						thisSuccess = results[0];
						thiscommentString += "pdiffs=" + toString(results[0]) + "(" + rtrimOligos->getCodeValue(results[1], pdiffs) + ") ";
					}
					if (thisSuccess > pdiffs) { thisTrashCode += "f"; }
					else { thisCurrentSeqsDiffs += thisSuccess; }
				}

				if (thisCurrentSeqsDiffs > tdiffs) { thisTrashCode += 't'; }

				if (thisTrashCode == "") {
					trashCode = thisTrashCode;
					success = thisSuccess;
					currentSeqDiffs = thisCurrentSeqsDiffs;
					commentString = thiscommentString;
					barcodeIndex = thisBarcodeIndex;
					primerIndex = thisPrimerIndex;
					savedSeq.reverseComplement();
					currSeq.setAligned(savedSeq.getAligned());
				}
				else { trashCode += "(" + thisTrashCode + ")"; }
			}

			currSeq.setComment(commentString);

			if (trashCode.length() == 0) {
				string thisGroup = oligos.getGroupName(barcodeIndex, primerIndex);

				int pos = thisGroup.find("ignore");
				if (pos == string::npos) {
					flowData.printFlows(trimFlowFile);

					if (fasta) { currSeq.printSequence(fastaFile); }

					if (allFiles) {
						ofstream output;
						File::openOutputFileAppend(thisBarcodePrimerComboFileNames[barcodeIndex][primerIndex], output);
						output.setf(ios::fixed, ios::floatfield); trimFlowFile.setf(ios::showpoint);

						flowData.printFlows(output);
						output.close();
					}
				}
			}
			else {
				flowData.printFlows(scrapFlowFile, trashCode);
			}

			count++;
			//cout << "driver" << '\t' << currSeq.getName() << endl;			
			//report progress
			if ((count) % 10000 == 0) { LOG(INFO) << toString(count) << '\n'; }

#if defined (UNIX)
			unsigned long long pos = flowFile.tellg();

			if ((pos == -1) || (pos >= line->end)) { break; }
#else
			if (flowFile.eof()) { break; }
#endif

		}
		//report progress
		if ((count) % 10000 != 0) { LOG(INFO) << toString(count) << '\n'; }

		trimFlowFile.close();
		scrapFlowFile.close();
		flowFile.close();
		if (fasta) { fastaFile.close(); }
		delete trimOligos;
		if (reorient) { delete rtrimOligos; }

		return count;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in TrimSeqsCommand, driverCreateTrim";
		exit(1);
	}
}

//***************************************************************************************************************

int TrimFlowsCommand::getOligos(vector<vector<string> >& outFlowFileNames) {
	bool allBlank = false;
	oligos.read(oligoFileName);

	if (ctrlc_pressed) { return 0; } //error in reading oligos

	if (oligos.hasPairedBarcodes()) {
		pairedOligos = true;
		numFPrimers = oligos.getPairedPrimers().size();
		numBarcodes = oligos.getPairedBarcodes().size();
	}
	else {
		pairedOligos = false;
		numFPrimers = oligos.getPrimers().size();
		numBarcodes = oligos.getBarcodes().size();
	}

	numLinkers = oligos.getLinkers().size();
	numSpacers = oligos.getSpacers().size();
	numRPrimers = oligos.getReversePrimers().size();

	vector<string> groupNames = oligos.getGroupNames();
	if (groupNames.size() == 0) { allFiles = 0; allBlank = true; }


	outFlowFileNames.resize(oligos.getBarcodeNames().size());
	for (int i = 0;i < outFlowFileNames.size();i++) {
		for (int j = 0;j < oligos.getPrimerNames().size();j++) { outFlowFileNames[i].push_back(""); }
	}

	if (allFiles) {
		set<string> uniqueNames; //used to cleanup outputFileNames
		if (pairedOligos) {
			map<int, oligosPair> barcodes = oligos.getPairedBarcodes();
			map<int, oligosPair> primers = oligos.getPairedPrimers();
			for (map<int, oligosPair>::iterator itBar = barcodes.begin();itBar != barcodes.end();itBar++) {
				for (map<int, oligosPair>::iterator itPrimer = primers.begin();itPrimer != primers.end(); itPrimer++) {

					string primerName = oligos.getPrimerName(itPrimer->first);
					string barcodeName = oligos.getBarcodeName(itBar->first);

					if ((primerName == "ignore") || (barcodeName == "ignore")) {} //do nothing
					else if ((primerName == "") && (barcodeName == "")) {} //do nothing
					else {
						string comboGroupName = "";

						if (primerName == "") {
							comboGroupName = barcodeName;
						}
						else {
							if (barcodeName == "") {
								comboGroupName = primerName;
							}
							else {
								comboGroupName = barcodeName + "." + primerName;
							}
						}


						ofstream temp;
						map<string, string> variables;
						variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(flowFileName));
						variables["[tag]"] = comboGroupName;
						string fileName = getOutputFileName("flow", variables);
						if (uniqueNames.count(fileName) == 0) {
							outputNames.push_back(fileName);
							outputTypes["flow"].push_back(fileName);
							uniqueNames.insert(fileName);
						}

						outFlowFileNames[itBar->first][itPrimer->first] = fileName;
						File::openOutputFile(fileName, temp);		temp.close();
					}
				}
			}
		}
		else {
			map<string, int> barcodes = oligos.getBarcodes();
			map<string, int> primers = oligos.getPrimers();
			for (map<string, int>::iterator itBar = barcodes.begin();itBar != barcodes.end();itBar++) {
				for (map<string, int>::iterator itPrimer = primers.begin();itPrimer != primers.end(); itPrimer++) {

					string primerName = oligos.getPrimerName(itPrimer->second);
					string barcodeName = oligos.getBarcodeName(itBar->second);

					if ((primerName == "ignore") || (barcodeName == "ignore")) {} //do nothing
					else if ((primerName == "") && (barcodeName == "")) {} //do nothing
					else {
						string comboGroupName = "";

						if (primerName == "") {
							comboGroupName = barcodeName;
						}
						else {
							if (barcodeName == "") {
								comboGroupName = primerName;
							}
							else {
								comboGroupName = barcodeName + "." + primerName;
							}
						}

						ofstream temp;
						map<string, string> variables;
						variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(flowFileName));
						variables["[tag]"] = comboGroupName;
						string fileName = getOutputFileName("flow", variables);
						if (uniqueNames.count(fileName) == 0) {
							outputNames.push_back(fileName);
							outputTypes["flow"].push_back(fileName);
							uniqueNames.insert(fileName);
						}

						outFlowFileNames[itBar->second][itPrimer->second] = fileName;
						File::openOutputFile(fileName, temp);		temp.close();
					}
				}
			}
		}

	}
	return 0;
}
/**************************************************************************************************/
vector<unsigned long long> TrimFlowsCommand::getFlowFileBreaks() {

	try {

		vector<unsigned long long> filePos;
		filePos.push_back(0);

		FILE * pFile;
		unsigned long long size;

		//get num bytes in file
		pFile = fopen(flowFileName.c_str(), "rb");
		if (pFile == NULL) perror("Error opening file");
		else {
			fseek(pFile, 0, SEEK_END);
			size = ftell(pFile);
			fclose(pFile);
		}

		//estimate file breaks
		unsigned long long chunkSize = 0;
		chunkSize = size / processors;

		//file too small to divide by processors
		if (chunkSize == 0) { processors = 1;	filePos.push_back(size); return filePos; }

		//for each process seekg to closest file break and search for next '>' char. make that the filebreak
		for (int i = 0; i < processors; i++) {
			unsigned long long spot = (i + 1) * chunkSize;

			ifstream in;
			File::openInputFile(flowFileName, in);
			in.seekg(spot);

			string dummy = File::getline(in);

			//there was not another sequence before the end of the file
			unsigned long long sanityPos = in.tellg();

			//			if (sanityPos == -1) {	break;  }
			//			else {  filePos.push_back(newSpot);  }
			if (sanityPos == -1) { break; }
			else { filePos.push_back(sanityPos); }

			in.close();
		}

		//save end pos
		filePos.push_back(size);

		//sanity check filePos
		for (int i = 0; i < (filePos.size() - 1); i++) {
			if (filePos[(i + 1)] <= filePos[i]) { filePos.erase(filePos.begin() + (i + 1)); i--; }
		}

		ifstream in;
		File::openInputFile(flowFileName, in);
		in >> numFlows;
		File::gobble(in);
		//unsigned long long spot = in.tellg();
		//filePos[0] = spot;
		in.close();

		processors = (filePos.size() - 1);

		return filePos;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in TrimSeqsCommand, getFlowFileBreaks";
		exit(1);
	}
}

/**************************************************************************************************/

int TrimFlowsCommand::createProcessesCreateTrim(string flowFileName, string trimFlowFileName, string scrapFlowFileName, string fastaFileName, vector<vector<string> > barcodePrimerComboFileNames) {

	try {
		processIDS.clear();
		int exitCommand = 1;
		bool recalc = false;

#if defined (UNIX)
		int process = 1;

		//loop through and create all the processes you want
		while (process != processors) {
			pid_t pid = fork();

			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}
			else if (pid == 0) {

				vector<vector<string> > tempBarcodePrimerComboFileNames = barcodePrimerComboFileNames;
				if (allFiles) {
					for (int i = 0;i < tempBarcodePrimerComboFileNames.size();i++) {
						for (int j = 0;j < tempBarcodePrimerComboFileNames[0].size();j++) {
							if (tempBarcodePrimerComboFileNames[i][j] != "") {
								tempBarcodePrimerComboFileNames[i][j] += m->mothurGetpid(process) + ".temp";
								ofstream temp;
								File::openOutputFile(tempBarcodePrimerComboFileNames[i][j], temp);
								temp.close();
							}
						}
					}
				}
				driverCreateTrim(flowFileName,
					(trimFlowFileName + m->mothurGetpid(process) + ".temp"),
					(scrapFlowFileName + m->mothurGetpid(process) + ".temp"),
					(fastaFileName + m->mothurGetpid(process) + ".temp"),
					tempBarcodePrimerComboFileNames, lines[process]);

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
					File::remove(trimFlowFileName + (toString(processIDS[i]) + ".temp"));
					File::remove(scrapFlowFileName + (toString(processIDS[i]) + ".temp"));
					File::remove(fastaFileName + (toString(processIDS[i]) + ".temp"));
					if (allFiles) {
						for (int i = 0;i < barcodePrimerComboFileNames.size();i++) {
							for (int j = 0;j < barcodePrimerComboFileNames[0].size();j++) {
								if (barcodePrimerComboFileNames[i][j] != "") {
									string tempFile = barcodePrimerComboFileNames[i][j] + (toString(processIDS[i])) + ".temp";
									File::remove(tempFile);
								}
							}
						}
					}
				}
				recalc = true;
				break;
			}
		}

		if (recalc) {
			//test line, also set recalc to true.
			//for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } ctrlc_pressed = false;  for (int i=0;i<processIDS.size();i++) {File::remove(fastaFileName + (toString(processIDS[i]) + ".temp"));File::remove(trimFlowFileName + (toString(processIDS[i]) + ".temp"));File::remove(scrapFlowFileName + (toString(processIDS[i]) + ".temp"));}processors=3; LOG(LOGERROR) << "unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n";

			//redo file divide
			for (int i = 0; i < lines.size(); i++) { delete lines[i]; }  lines.clear();
			vector<unsigned long long> flowFilePos = getFlowFileBreaks();
			for (int i = 0; i < (flowFilePos.size() - 1); i++) {
				lines.push_back(new linePair(flowFilePos[i], flowFilePos[(i + 1)]));
			}

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

					vector<vector<string> > tempBarcodePrimerComboFileNames = barcodePrimerComboFileNames;
					if (allFiles) {
						for (int i = 0;i < tempBarcodePrimerComboFileNames.size();i++) {
							for (int j = 0;j < tempBarcodePrimerComboFileNames[0].size();j++) {
								if (tempBarcodePrimerComboFileNames[i][j] != "") {
									tempBarcodePrimerComboFileNames[i][j] += m->mothurGetpid(process) + ".temp";
									ofstream temp;
									File::openOutputFile(tempBarcodePrimerComboFileNames[i][j], temp);
									temp.close();
								}
							}
						}
					}
					driverCreateTrim(flowFileName,
						(trimFlowFileName + m->mothurGetpid(process) + ".temp"),
						(scrapFlowFileName + m->mothurGetpid(process) + ".temp"),
						(fastaFileName + m->mothurGetpid(process) + ".temp"),
						tempBarcodePrimerComboFileNames, lines[process]);

					exit(0);
				}
				else {
					LOG(LOGERROR) << "unable to spawn the necessary processes." << '\n';
					for (int i = 0; i < processIDS.size(); i++) { kill(processIDS[i], SIGINT); }
					exit(0);
				}
			}

		}

		//parent do my part
		ofstream temp;
		File::openOutputFile(trimFlowFileName, temp);
		temp.close();

		File::openOutputFile(scrapFlowFileName, temp);
		temp.close();

		if (fasta) {
			File::openOutputFile(fastaFileName, temp);
			temp.close();
		}

		driverCreateTrim(flowFileName, trimFlowFileName, scrapFlowFileName, fastaFileName, barcodePrimerComboFileNames, lines[0]);

		//force parent to wait until all the processes are done
		for (int i = 0;i < processIDS.size();i++) {
			int temp = processIDS[i];
			wait(&temp);
		}
#else
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//Windows version shared memory, so be careful when passing variables through the trimFlowData struct. 
		//Above fork() will clone, so memory is separate, but that's not the case with windows, 
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		/*
		vector<trimFlowData*> pDataArray;
		vector<DWORD> dwThreadIdArray(processors-1);
		vector<HANDLE> hThreadArray(processors-1);

		//Create processor worker threads.
		for( int i=0; i<processors-1; i++ ){
			// Allocate memory for thread data.
			string extension = "";
			if (i != 0) { extension = toString(i) + ".temp"; processIDS.push_back(i); }

			vector<vector<string> > tempBarcodePrimerComboFileNames = barcodePrimerComboFileNames;
			if(allFiles){
				for(int i=0;i<tempBarcodePrimerComboFileNames.size();i++){
					for(int j=0;j<tempBarcodePrimerComboFileNames[0].size();j++){
						if (tempBarcodePrimerComboFileNames[i][j] != "") {
							tempBarcodePrimerComboFileNames[i][j] += extension;
							ofstream temp;
							File::openOutputFile(tempBarcodePrimerComboFileNames[i][j], temp);
							temp.close();
						}
					}
				}
			}

			trimFlowData* tempflow = new trimFlowData(flowFileName, (trimFlowFileName + extension), (scrapFlowFileName + extension), fastaFileName, flowOrder, tempBarcodePrimerComboFileNames, barcodes, primers, revPrimer, fasta, allFiles, lines[i]->start, lines[i]->end, m, signal, noise, numFlows, maxFlows, minFlows, maxHomoP, tdiffs, bdiffs, pdiffs, i);
			pDataArray.push_back(tempflow);

			//MyTrimFlowThreadFunction is in header. It must be global or static to work with the threads.
			//default security attributes, thread function name, argument to thread function, use default creation flags, returns the thread identifier
			hThreadArray[i] = CreateThread(NULL, 0, MyTrimFlowThreadFunction, pDataArray[i], 0, &dwThreadIdArray[i]);
		}

		//using the main process as a worker saves time and memory
		ofstream temp;
		File::openOutputFile(trimFlowFileName, temp);
		temp.close();

		File::openOutputFile(scrapFlowFileName, temp);
		temp.close();

		if(fasta){
			File::openOutputFile(fastaFileName, temp);
			temp.close();
		}

		vector<vector<string> > tempBarcodePrimerComboFileNames = barcodePrimerComboFileNames;
		if(allFiles){
			for(int i=0;i<tempBarcodePrimerComboFileNames.size();i++){
				for(int j=0;j<tempBarcodePrimerComboFileNames[0].size();j++){
					if (tempBarcodePrimerComboFileNames[i][j] != "") {
						tempBarcodePrimerComboFileNames[i][j] += toString(processors-1) + ".temp";
						ofstream temp;
						File::openOutputFile(tempBarcodePrimerComboFileNames[i][j], temp);
						temp.close();
					}

				}
			}
		}

		//do my part - do last piece because windows is looking for eof
		int num = driverCreateTrim(flowFileName, (trimFlowFileName  + toString(processors-1) + ".temp"), (scrapFlowFileName  + toString(processors-1) + ".temp"), (fastaFileName + toString(processors-1) + ".temp"), tempBarcodePrimerComboFileNames, lines[processors-1]);
		processIDS.push_back((processors-1));

		//Wait until all threads have terminated.
		WaitForMultipleObjects(processors-1, &(hThreadArray[0]), TRUE, INFINITE);

		//Close all thread handles and free memory allocations.
		for(int i=0; i < pDataArray.size(); i++){
			num += pDataArray[i]->count;
			CloseHandle(hThreadArray[i]);
			delete pDataArray[i];
		}
		*/

#endif	
		//append files
		LOG(INFO) << "";
		for (int i = 0;i < processIDS.size();i++) {

			LOG(INFO) << "Appending files from process " + toString(processIDS[i]) << '\n';

			File::appendFiles((trimFlowFileName + toString(processIDS[i]) + ".temp"), trimFlowFileName);
			File::remove((trimFlowFileName + toString(processIDS[i]) + ".temp"));
			//			LOG(INFO) << "\tDone with trim.flow file" << '\n';

			File::appendFiles((scrapFlowFileName + toString(processIDS[i]) + ".temp"), scrapFlowFileName);
			File::remove((scrapFlowFileName + toString(processIDS[i]) + ".temp"));
			//			LOG(INFO) << "\tDone with scrap.flow file" << '\n';

			if (fasta) {
				File::appendFiles((fastaFileName + toString(processIDS[i]) + ".temp"), fastaFileName);
				File::remove((fastaFileName + toString(processIDS[i]) + ".temp"));
				//				LOG(INFO) << "\tDone with flow.fasta file" << '\n';
			}
			if (allFiles) {
				for (int j = 0; j < barcodePrimerComboFileNames.size(); j++) {
					for (int k = 0; k < barcodePrimerComboFileNames[0].size(); k++) {
						if (barcodePrimerComboFileNames[j][k] != "") {
							File::appendFiles((barcodePrimerComboFileNames[j][k] + toString(processIDS[i]) + ".temp"), barcodePrimerComboFileNames[j][k]);
							File::remove((barcodePrimerComboFileNames[j][k] + toString(processIDS[i]) + ".temp"));
						}
					}
				}
			}
		}

		return exitCommand;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in TrimFlowsCommand, createProcessesCreateTrim";
		exit(1);
	}
}

//***************************************************************************************************************
