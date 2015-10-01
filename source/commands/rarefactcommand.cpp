/*
 *  rarefactcommand.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "rarefactcommand.h"
#include "ace.h"
#include "sobs.h"
#include "nseqs.h"
#include "chao1.h"
#include "bootstrap.h"
#include "simpson.h"
#include "simpsoneven.h"
#include "heip.h"
#include "smithwilson.h"
#include "invsimpson.h"
#include "npshannon.h"
#include "shannoneven.h"
#include "shannon.h"
#include "jackknife.h"
#include "coverage.h"
#include "shannonrange.h"


 //**********************************************************************************************************************
vector<string> RareFactCommand::setParameters() {
	try {
		CommandParameter plist("list", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false, true); parameters.push_back(plist);
		CommandParameter prabund("rabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false); parameters.push_back(prabund);
		CommandParameter psabund("sabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false); parameters.push_back(psabund);
		CommandParameter pshared("shared", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false, true); parameters.push_back(pshared);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("freq", -INFINITY, INFINITY, 100, false, false));
		nkParameters.add(new NumberParameter("iters", -INFINITY, INFINITY, 1000, false, false));
		nkParameters.add(new MultipleParameter("calc", vector<string>{"sobs", "chao", "nseqs", "coverage", "ace", "jack", "shannon", "shannoneven", "npshannon", "heip", "smithwilson-simpson-simpsoneven-invsimpson-bootstrap-shannonrange"}, "sobs", true, false, true));
		nkParameters.add(new NumberParameter("abund", -INFINITY, INFINITY, 10, false, false));
		nkParameters.add(new MultipleParameter("alpha", vector<string>{"0", "1", "2"}, "1", false, false, true));
		nkParameters.add(new ProcessorsParameter(settings));
		nkParameters.add(new BooleanParameter("groupmode", true, false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RareFactCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string RareFactCommand::getHelpString() {
	try {
		ValidCalculators validCalculator;
		string helpString = "The rarefaction.single command parameters are list, sabund, rabund, shared, label, iters, freq, calc, processors, groupmode and abund.  list, sabund, rabund or shared is required unless you have a valid current file. \n"
			"The freq parameter is used indicate when to output your data, by default it is set to 100. But you can set it to a percentage of the number of sequence. For example freq=0.10, means 10%. \n"
			"The processors parameter allows you to specify the number of processors to use. The default is 1.\n"
			"The rarefaction.single command should be in the following format: \n"
			"rarefaction.single(label=yourLabel, iters=yourIters, freq=yourFreq, calc=yourEstimators).\n"
			"Example rarefaction.single(label=unique-.01-.03, iters=10000, freq=10, calc=sobs-rchao-race-rjack-rbootstrap-rshannon-rnpshannon-rsimpson).\n"
			"The default values for iters is 1000, freq is 100, and calc is rarefaction which calculates the rarefaction curve for the observed richness.\n"
			"The alpha parameter is used to set the alpha value for the shannonrange calculator.\n";
		validCalculator.printCalc("rarefaction")
			"If you are running rarefaction.single with a shared file and would like your results collated in one file, set groupmode=t. (Default=true).\n"
			"The label parameter is used to analyze specific labels in your input.\n"
			"Note: No spaces between parameter labels (i.e. freq), '=' and parameters (i.e.yourFreq).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RareFactCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string RareFactCommand::getOutputPattern(string type) {
	string pattern = "";
	if (type == "rarefaction") { pattern = "[filename],rarefaction"; }
	else if (type == "r_chao") { pattern = "[filename],r_chao"; }
	else if (type == "r_ace") { pattern = "[filename],r_ace"; }
	else if (type == "r_jack") { pattern = "[filename],r_jack"; }
	else if (type == "r_shannon") { pattern = "[filename],r_shannon"; }
	else if (type == "r_shannoneven") { pattern = "[filename],r_shannoneven"; }
	else if (type == "r_smithwilson") { pattern = "[filename],r_smithwilson"; }
	else if (type == "r_npshannon") { pattern = "[filename],r_npshannon"; }
	else if (type == "r_shannonrange") { pattern = "[filename],r_shannonrange"; }
	else if (type == "r_simpson") { pattern = "[filename],r_simpson"; }
	else if (type == "r_simpsoneven") { pattern = "[filename],r_simpsoneven"; }
	else if (type == "r_invsimpson") { pattern = "[filename],r_invsimpson"; }
	else if (type == "r_bootstrap") { pattern = "[filename],r_bootstrap"; }
	else if (type == "r_coverage") { pattern = "[filename],r_coverage"; }
	else if (type == "r_nseqs") { pattern = "[filename],r_nseqs"; }
	else if (type == "r_heip") { pattern = "[filename],r_heip"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
RareFactCommand::RareFactCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["rarefaction"] = tempOutNames;
		outputTypes["r_chao"] = tempOutNames;
		outputTypes["r_ace"] = tempOutNames;
		outputTypes["r_jack"] = tempOutNames;
		outputTypes["r_shannon"] = tempOutNames;
		outputTypes["r_shannoneven"] = tempOutNames;
		outputTypes["r_shannonrange"] = tempOutNames;
		outputTypes["r_heip"] = tempOutNames;
		outputTypes["r_smithwilson"] = tempOutNames;
		outputTypes["r_npshannon"] = tempOutNames;
		outputTypes["r_simpson"] = tempOutNames;
		outputTypes["r_simpsoneven"] = tempOutNames;
		outputTypes["r_invsimpson"] = tempOutNames;
		outputTypes["r_bootstrap"] = tempOutNames;
		outputTypes["r_coverage"] = tempOutNames;
		outputTypes["r_nseqs"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in RareFactCommand, RareFactCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
RareFactCommand::RareFactCommand(Settings& settings, string option) : Command(settings, option) {
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
		outputTypes["rarefaction"] = tempOutNames;
		outputTypes["r_chao"] = tempOutNames;
		outputTypes["r_ace"] = tempOutNames;
		outputTypes["r_jack"] = tempOutNames;
		outputTypes["r_shannon"] = tempOutNames;
		outputTypes["r_shannoneven"] = tempOutNames;
		outputTypes["r_shannonrange"] = tempOutNames;
		outputTypes["r_heip"] = tempOutNames;
		outputTypes["r_smithwilson"] = tempOutNames;
		outputTypes["r_npshannon"] = tempOutNames;
		outputTypes["r_simpson"] = tempOutNames;
		outputTypes["r_simpsoneven"] = tempOutNames;
		outputTypes["r_invsimpson"] = tempOutNames;
		outputTypes["r_bootstrap"] = tempOutNames;
		outputTypes["r_coverage"] = tempOutNames;
		outputTypes["r_nseqs"] = tempOutNames;

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

			it = parameters.find("rabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["rabund"] = inputDir + it->second; }
			}

			it = parameters.find("sabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["sabund"] = inputDir + it->second; }
			}

			it = parameters.find("list");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["list"] = inputDir + it->second; }
			}
		}

		//check for required parameters
		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { listfile = ""; abort = true; }
		else if (listfile == "not found") { listfile = ""; }
		else { format = "list"; inputfile = listfile; settings.setCurrent("list", listfile); }

		sabundfile = validParameter.validFile(parameters, "sabund", true);
		if (sabundfile == "not open") { sabundfile = ""; abort = true; }
		else if (sabundfile == "not found") { sabundfile = ""; }
		else { format = "sabund"; inputfile = sabundfile; settings.setCurrent("sabund", sabundfile); }

		rabundfile = validParameter.validFile(parameters, "rabund", true);
		if (rabundfile == "not open") { rabundfile = ""; abort = true; }
		else if (rabundfile == "not found") { rabundfile = ""; }
		else { format = "rabund"; inputfile = rabundfile; settings.setCurrent("rabund", rabundfile); }

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { sharedfile = ""; abort = true; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		else { format = "sharedfile"; inputfile = sharedfile; settings.setCurrent("shared", sharedfile); }

		if ((sharedfile == "") && (listfile == "") && (rabundfile == "") && (sabundfile == "")) {
			//is there are current file available for any of these?
			//give priority to shared, then list, then rabund, then sabund
			//if there is a current shared file, use it
			sharedfile = settings.getCurrent("shared");
			if (sharedfile != "") { inputfile = sharedfile; format = "sharedfile"; LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
			else {
				listfile = settings.getCurrent("list");
				if (listfile != "") { inputfile = listfile; format = "list"; LOG(INFO) << "Using " + listfile + " as input file for the list parameter." << '\n'; }
				else {
					rabundfile = settings.getCurrent("rabund");
					if (rabundfile != "") { inputfile = rabundfile; format = "rabund"; LOG(INFO) << "Using " + rabundfile + " as input file for the rabund parameter." << '\n'; }
					else {
						sabundfile = settings.getCurrent("sabund");
						if (sabundfile != "") { inputfile = sabundfile; format = "sabund"; LOG(INFO) << "Using " + sabundfile + " as input file for the sabund parameter." << '\n'; }
						else {
							LOG(INFO) << "No valid current files. You must provide a list, sabund, rabund or shared file before you can use the collect.single command." << '\n';
							abort = true;
						}
					}
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

		calc = validParameter.validFile(parameters, "calc", false);
		if (calc == "not found") { calc = "sobs"; }
		else {
			if (calc == "default") { calc = "sobs"; }
		}
		Utility::split(calc, '-', Estimators);
		if (m->inUsersGroups("citation", Estimators)) {
			ValidCalculators validCalc; validCalc.printCitations(Estimators);
			//remove citation from list of calcs
			for (int i = 0; i < Estimators.size(); i++) { if (Estimators[i] == "citation") { Estimators.erase(Estimators.begin() + i); break; } }
		}

		string temp;
		temp = validParameter.validFile(parameters, "freq", false);			if (temp == "not found") { temp = "100"; }
		Utility::mothurConvert(temp, freq);

		temp = validParameter.validFile(parameters, "abund", false);			if (temp == "not found") { temp = "10"; }
		Utility::mothurConvert(temp, abund);

		temp = validParameter.validFile(parameters, "iters", false);			if (temp == "not found") { temp = "1000"; }
		Utility::mothurConvert(temp, nIters);

		temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);
		Utility::mothurConvert(temp, processors);

		temp = validParameter.validFile(parameters, "alpha", false);		if (temp == "not found") { temp = "1"; }
		Utility::mothurConvert(temp, alpha);

		if ((alpha != 0) && (alpha != 1) && (alpha != 2)) { LOG(LOGERROR) << "Not a valid alpha value. Valid values are 0, 1 and 2." << '\n'; abort = true; }

		temp = validParameter.validFile(parameters, "groupmode", false);		if (temp == "not found") { temp = "T"; }
		groupMode = m->isTrue(temp);
	}

}
//**********************************************************************************************************************

int RareFactCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	map<string, set<int> > labelToEnds;
	if ((format != "sharedfile")) { inputFileNames.push_back(inputfile); }
	else { inputFileNames = parseSharedFile(sharedfile, labelToEnds);  format = "rabund"; }

	if (ctrlc_pressed) { return 0; }

	map<int, string> file2Group; //index in outputNames[i] -> group
	for (int p = 0; p < inputFileNames.size(); p++) {

		string fileNameRoot = outputDir + File::getRootName(File::getSimpleName(inputFileNames[p]));

		if (ctrlc_pressed) { outputTypes.clear(); for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  m->clearGroups();  return 0; }

		if (inputFileNames.size() > 1) {
			LOG(INFO) << std::endl << "Processing group " + groups[p] << '\n' << '\n';
		}
		int i;
		ValidCalculators validCalculator;

		map<string, string> variables;
		variables["[filename]"] = fileNameRoot;

		for (i = 0; i < Estimators.size(); i++) {
			if (validCalculator.isValidCalculator("rarefaction", Estimators[i]) == true) {
				if (Estimators[i] == "sobs") {
					rDisplays.push_back(new RareDisplay(new Sobs(), new ThreeColumnFile(getOutputFileName("rarefaction", variables))));
					outputNames.push_back(getOutputFileName("rarefaction", variables)); outputTypes["rarefaction"].push_back(getOutputFileName("rarefaction", variables));
				}
				else if (Estimators[i] == "chao") {
					rDisplays.push_back(new RareDisplay(new Chao1(), new ThreeColumnFile(getOutputFileName("r_chao", variables))));
					outputNames.push_back(getOutputFileName("r_chao", variables)); outputTypes["r_chao"].push_back(getOutputFileName("r_chao", variables));
				}
				else if (Estimators[i] == "ace") {
					if (abund < 5)
						abund = 10;
					rDisplays.push_back(new RareDisplay(new Ace(abund), new ThreeColumnFile(getOutputFileName("r_ace", variables))));
					outputNames.push_back(getOutputFileName("r_ace", variables)); outputTypes["r_ace"].push_back(getOutputFileName("r_ace", variables));
				}
				else if (Estimators[i] == "jack") {
					rDisplays.push_back(new RareDisplay(new Jackknife(), new ThreeColumnFile(getOutputFileName("r_jack", variables))));
					outputNames.push_back(getOutputFileName("r_jack", variables)); outputTypes["r_jack"].push_back(getOutputFileName("r_jack", variables));
				}
				else if (Estimators[i] == "shannon") {
					rDisplays.push_back(new RareDisplay(new Shannon(), new ThreeColumnFile(getOutputFileName("r_shannon", variables))));
					outputNames.push_back(getOutputFileName("r_shannon", variables)); outputTypes["r_shannon"].push_back(getOutputFileName("r_shannon", variables));
				}
				else if (Estimators[i] == "shannoneven") {
					rDisplays.push_back(new RareDisplay(new ShannonEven(), new ThreeColumnFile(getOutputFileName("r_shannoneven", variables))));
					outputNames.push_back(getOutputFileName("r_shannoneven", variables)); outputTypes["r_shannoneven"].push_back(getOutputFileName("r_shannoneven", variables));
				}
				else if (Estimators[i] == "heip") {
					rDisplays.push_back(new RareDisplay(new Heip(), new ThreeColumnFile(getOutputFileName("r_heip", variables))));
					outputNames.push_back(getOutputFileName("r_heip", variables)); outputTypes["r_heip"].push_back(getOutputFileName("r_heip", variables));
				}
				else if (Estimators[i] == "r_shannonrange") {
					rDisplays.push_back(new RareDisplay(new RangeShannon(alpha), new ThreeColumnFile(getOutputFileName("r_shannonrange", variables))));
					outputNames.push_back(getOutputFileName("r_shannonrange", variables)); outputTypes["r_shannoneven"].push_back(getOutputFileName("r_shannonrange", variables));
				}
				else if (Estimators[i] == "smithwilson") {
					rDisplays.push_back(new RareDisplay(new SmithWilson(), new ThreeColumnFile(getOutputFileName("r_smithwilson", variables))));
					outputNames.push_back(getOutputFileName("r_smithwilson", variables)); outputTypes["r_smithwilson"].push_back(getOutputFileName("r_smithwilson", variables));
				}
				else if (Estimators[i] == "npshannon") {
					rDisplays.push_back(new RareDisplay(new NPShannon(), new ThreeColumnFile(getOutputFileName("r_npshannon", variables))));
					outputNames.push_back(getOutputFileName("r_npshannon", variables)); outputTypes["r_npshannon"].push_back(getOutputFileName("r_npshannon", variables));
				}
				else if (Estimators[i] == "simpson") {
					rDisplays.push_back(new RareDisplay(new Simpson(), new ThreeColumnFile(getOutputFileName("r_simpson", variables))));
					outputNames.push_back(getOutputFileName("r_simpson", variables)); outputTypes["r_simpson"].push_back(getOutputFileName("r_simpson", variables));
				}
				else if (Estimators[i] == "simpsoneven") {
					rDisplays.push_back(new RareDisplay(new SimpsonEven(), new ThreeColumnFile(getOutputFileName("r_simpsoneven", variables))));
					outputNames.push_back(getOutputFileName("r_simpsoneven", variables)); outputTypes["r_simpsoneven"].push_back(getOutputFileName("r_simpsoneven", variables));
				}
				else if (Estimators[i] == "invsimpson") {
					rDisplays.push_back(new RareDisplay(new InvSimpson(), new ThreeColumnFile(getOutputFileName("r_invsimpson", variables))));
					outputNames.push_back(getOutputFileName("r_invsimpson", variables)); outputTypes["r_invsimpson"].push_back(getOutputFileName("r_invsimpson", variables));
				}
				else if (Estimators[i] == "bootstrap") {
					rDisplays.push_back(new RareDisplay(new Bootstrap(), new ThreeColumnFile(getOutputFileName("r_bootstrap", variables))));
					outputNames.push_back(getOutputFileName("r_bootstrap", variables)); outputTypes["r_bootstrap"].push_back(getOutputFileName("r_bootstrap", variables));
				}
				else if (Estimators[i] == "coverage") {
					rDisplays.push_back(new RareDisplay(new Coverage(), new ThreeColumnFile(getOutputFileName("r_coverage", variables))));
					outputNames.push_back(getOutputFileName("r_coverage", variables)); outputTypes["r_coverage"].push_back(getOutputFileName("r_coverage", variables));
				}
				else if (Estimators[i] == "nseqs") {
					rDisplays.push_back(new RareDisplay(new NSeqs(), new ThreeColumnFile(getOutputFileName("r_nseqs", variables))));
					outputNames.push_back(getOutputFileName("r_nseqs", variables)); outputTypes["r_nseqs"].push_back(getOutputFileName("r_nseqs", variables));
				}
				if (inputFileNames.size() > 1) { file2Group[outputNames.size() - 1] = groups[p]; }
			}
		}


		//if the users entered no valid calculators don't execute command
		if (rDisplays.size() == 0) { for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }  return 0; }

		input = new InputData(inputFileNames[p], format);
		order = input->getOrderVector();
		string lastLabel = order->getLabel();

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		if (ctrlc_pressed) { for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }  delete input;  delete order;  for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

		//as long as you are not at the end of the file or done wih the lines you want
		while ((order != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) { for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }  delete input;  delete order;  for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }


			if (allLines == 1 || labels.count(order->getLabel()) == 1) {

				LOG(INFO) << order->getLabel() << '\n';
				map<string, set<int> >::iterator itEndings = labelToEnds.find(order->getLabel());
				set<int> ends;
				if (itEndings != labelToEnds.end()) { ends = itEndings->second; }
				rCurve = new Rarefact(order, rDisplays, processors, ends);
				rCurve->getCurve(freq, nIters);
				delete rCurve;

				processedLabels.insert(order->getLabel());
				userLabels.erase(order->getLabel());
			}

			if ((Utility::anyLabelsToProcess(order->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = order->getLabel();

				delete order;
				order = (input->getOrderVector(lastLabel));

				LOG(INFO) << order->getLabel() << '\n';
				map<string, set<int> >::iterator itEndings = labelToEnds.find(order->getLabel());
				set<int> ends;
				if (itEndings != labelToEnds.end()) { ends = itEndings->second; }
				rCurve = new Rarefact(order, rDisplays, processors, ends);

				rCurve->getCurve(freq, nIters);
				delete rCurve;

				processedLabels.insert(order->getLabel());
				userLabels.erase(order->getLabel());

				//restore real lastlabel to save below
				order->setLabel(saveLabel);
			}

			lastLabel = order->getLabel();

			delete order;
			order = (input->getOrderVector());
		}

		if (ctrlc_pressed) { for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }  delete input;   for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

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

		if (ctrlc_pressed) { for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }  delete input;   for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

		//run last label if you need to
		if (needToRun == true) {
			if (order != NULL) { delete order; }
			order = (input->getOrderVector(lastLabel));

			LOG(INFO) << order->getLabel() << '\n';
			map<string, set<int> >::iterator itEndings = labelToEnds.find(order->getLabel());
			set<int> ends;
			if (itEndings != labelToEnds.end()) { ends = itEndings->second; }
			rCurve = new Rarefact(order, rDisplays, processors, ends);

			rCurve->getCurve(freq, nIters);
			delete rCurve;

			delete order;
		}


		for (int i = 0;i < rDisplays.size();i++) { delete rDisplays[i]; }
		rDisplays.clear();
		delete input;
	}


	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	//create summary file containing all the groups data for each label - this function just combines the info from the files already created.
	if ((sharedfile != "") && (groupMode)) { outputNames = createGroupFile(outputNames, file2Group); }

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
//**********************************************************************************************************************
vector<string> RareFactCommand::createGroupFile(vector<string>& outputNames, map<int, string> file2Group) {

	vector<string> newFileNames;

	//find different types of files
	map<string, map<string, string> > typesFiles;
	map<string, vector< vector<string> > > fileLabels; //combofile name to labels. each label is a vector because it may be unique lci hci.
	vector<string> groupNames;
	for (int i = 0; i < outputNames.size(); i++) {

		string extension = m->getExtension(outputNames[i]);
		string combineFileName = outputDir + File::getRootName(File::getSimpleName(sharedfile)) + "groups" + extension;
		File::remove(combineFileName); //remove old file

		ifstream in;
		File::openInputFile(outputNames[i], in);

		string labels = File::getline(in);

		istringstream iss(labels, istringstream::in);
		string newLabel = ""; vector<string> theseLabels;
		while (!iss.eof()) { iss >> newLabel; File::gobble(iss); theseLabels.push_back(newLabel); }
		vector< vector<string> > allLabels;
		vector<string> thisSet; thisSet.push_back(theseLabels[0]); allLabels.push_back(thisSet); thisSet.clear(); //makes "numSampled" its own grouping
		for (int j = 1; j < theseLabels.size() - 1; j++) {
			if (theseLabels[j + 1] == "lci") {
				thisSet.push_back(theseLabels[j]);
				thisSet.push_back(theseLabels[j + 1]);
				thisSet.push_back(theseLabels[j + 2]);
				j++; j++;
			}
			else { //no lci or hci for this calc.
				thisSet.push_back(theseLabels[j]);
			}
			allLabels.push_back(thisSet);
			thisSet.clear();
		}
		fileLabels[combineFileName] = allLabels;

		map<string, map<string, string> >::iterator itfind = typesFiles.find(extension);
		if (itfind != typesFiles.end()) {
			(itfind->second)[outputNames[i]] = file2Group[i];
		}
		else {
			map<string, string> temp;
			temp[outputNames[i]] = file2Group[i];
			typesFiles[extension] = temp;
		}
		if (!(m->inUsersGroups(file2Group[i], groupNames))) { groupNames.push_back(file2Group[i]); }

	}

	//for each type create a combo file

	for (map<string, map<string, string> >::iterator it = typesFiles.begin(); it != typesFiles.end(); it++) {

		ofstream out;
		string combineFileName = outputDir + File::getRootName(File::getSimpleName(sharedfile)) + "groups" + it->first;
		File::openOutputFileAppend(combineFileName, out);
		newFileNames.push_back(combineFileName);
		map<string, string> thisTypesFiles = it->second; //it->second maps filename to group
		set<int> numSampledSet;

		//open each type summary file
		map<string, map<int, vector< vector<string> > > > files; //maps file name to lines in file
		int maxLines = 0;
		for (map<string, string>::iterator itFileNameGroup = thisTypesFiles.begin(); itFileNameGroup != thisTypesFiles.end(); itFileNameGroup++) {

			string thisfilename = itFileNameGroup->first;
			string group = itFileNameGroup->second;

			ifstream temp;
			File::openInputFile(thisfilename, temp);

			//read through first line - labels
			File::getline(temp);	File::gobble(temp);

			map<int, vector< vector<string> > > thisFilesLines;
			while (!temp.eof()) {
				int numSampled = 0;
				temp >> numSampled; File::gobble(temp);

				vector< vector<string> > theseReads;
				vector<string> thisSet; thisSet.push_back(toString(numSampled)); theseReads.push_back(thisSet); thisSet.clear();
				for (int k = 1; k < fileLabels[combineFileName].size(); k++) { //output thing like 0.03-A lci-A hci-A
					vector<string> reads;
					string next = "";
					for (int l = 0; l < fileLabels[combineFileName][k].size(); l++) { //output modified labels
						temp >> next; File::gobble(temp);
						reads.push_back(next);
					}
					theseReads.push_back(reads);
				}
				thisFilesLines[numSampled] = theseReads;
				File::gobble(temp);

				numSampledSet.insert(numSampled);
			}

			files[group] = thisFilesLines;

			//save longest file for below
			if (maxLines < thisFilesLines.size()) { maxLines = thisFilesLines.size(); }

			temp.close();
			File::remove(thisfilename);
		}

		//output new labels line
		out << fileLabels[combineFileName][0][0];
		for (int k = 1; k < fileLabels[combineFileName].size(); k++) { //output thing like 0.03-A lci-A hci-A
			for (int n = 0; n < groupNames.size(); n++) { // for each group
				for (int l = 0; l < fileLabels[combineFileName][k].size(); l++) { //output modified labels
					out << '\t' << fileLabels[combineFileName][k][l] << '-' << groupNames[n];
				}
			}
		}
		out << endl;

		//for each label
		for (set<int>::iterator itNumSampled = numSampledSet.begin(); itNumSampled != numSampledSet.end(); itNumSampled++) {

			out << (*itNumSampled);

			if (ctrlc_pressed) { break; }

			for (int k = 1; k < fileLabels[combineFileName].size(); k++) { //each chunk
				//grab data for each group
				for (int n = 0; n < groupNames.size(); n++) {
					string group = groupNames[n];

					map<int, vector< vector<string> > >::iterator itLine = files[group].find(*itNumSampled);
					if (itLine != files[group].end()) {
						for (int l = 0; l < (itLine->second)[k].size(); l++) {
							out << '\t' << (itLine->second)[k][l];

						}
					}
					else {
						for (int l = 0; l < fileLabels[combineFileName][k].size(); l++) {
							out << "\tNA";
						}
					}
				}
			}
			out << endl;
		}
		out.close();
	}

	//return combine file name
	return newFileNames;

}
//**********************************************************************************************************************
vector<string> RareFactCommand::parseSharedFile(string filename, map<string, set<int> >& label2Ends) {
	vector<string> filenames;

	map<string, ofstream*> filehandles;
	map<string, ofstream*>::iterator it3;

	input = new InputData(filename, "sharedfile");
	vector<SharedRAbundVector*> lookup = input->getSharedRAbundVectors();

	string sharedFileRoot = File::getRootName(filename);

	//clears file before we start to write to it below
	for (int i = 0; i < lookup.size(); i++) {
		File::remove((sharedFileRoot + lookup[i]->getGroup() + ".rabund"));
		filenames.push_back((sharedFileRoot + lookup[i]->getGroup() + ".rabund"));
	}

	ofstream* temp;
	for (int i = 0; i < lookup.size(); i++) {
		temp = new ofstream;
		filehandles[lookup[i]->getGroup()] = temp;
		groups.push_back(lookup[i]->getGroup());
	}

	while (lookup[0] != NULL) {

		for (int i = 0; i < lookup.size(); i++) {
			RAbundVector rav = lookup[i]->getRAbundVector();
			File::openOutputFileAppend(sharedFileRoot + lookup[i]->getGroup() + ".rabund", *(filehandles[lookup[i]->getGroup()]));
			rav.print(*(filehandles[lookup[i]->getGroup()]));
			(*(filehandles[lookup[i]->getGroup()])).close();
			label2Ends[lookup[i]->getLabel()].insert(rav.getNumSeqs());
		}

		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		lookup = input->getSharedRAbundVectors();
	}

	//free memory
	for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
		delete it3->second;
	}

	delete input;
	m->clearGroups();

	return filenames;
}
//**********************************************************************************************************************



