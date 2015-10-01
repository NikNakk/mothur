/*
 *  collectcommand.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "collectcommand.h"
#include "ace.h"
#include "sobs.h"
#include "nseqs.h"
#include "chao1.h"
#include "bootstrap.h"
#include "simpson.h"
#include "simpsoneven.h"
#include "invsimpson.h"
#include "npshannon.h"
#include "shannon.h"
#include "smithwilson.h"
#include "heip.h"
#include "shannoneven.h"
#include "jackknife.h"
#include "geom.h"
#include "qstat.h"
#include "logsd.h"
#include "bergerparker.h"
#include "bstick.h"
#include "goodscoverage.h"
#include "efron.h"
#include "boneh.h"
#include "solow.h"
#include "shen.h"
#include "coverage.h"
#include "shannonrange.h"


 //**********************************************************************************************************************
vector<string> CollectCommand::setParameters() {
	try {
		CommandParameter plist("list", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false, true); parameters.push_back(plist);
		CommandParameter prabund("rabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false, true); parameters.push_back(prabund);
		CommandParameter psabund("sabund", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false, true); parameters.push_back(psabund);
		CommandParameter pshared("shared", "InputTypes", "", "", "LRSS", "LRSS", "none", "", false, false, true); parameters.push_back(pshared);
		nkParameters.add(new StringParameter("label", "", false, false));
		nkParameters.add(new NumberParameter("freq", -INFINITY, INFINITY, 100, false, false));
		nkParameters.add(new CalcParameter("calc", "sobs-chao-ace-jack-shannon-npshannon-simpson-shannonrange", true, false, true));
		nkParameters.add(new NumberParameter("abund", -INFINITY, INFINITY, 10, false, false));
		nkParameters.add(new MultipleParameter("alpha", vector<string>{"0", "1", "2"}, "1", false, false, true));
		nkParameters.add(new NumberParameter("size", -INFINITY, INFINITY, 0, false, false));
		nkParameters.addStandardParameters();

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CollectCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string CollectCommand::getHelpString() {
	try {
		ValidCalculators validCalculator;
		string helpString = "The collect.single command parameters are list, sabund, rabund, shared, label, freq, calc, alpha and abund.  list, sabund, rabund or shared is required unless you have a valid current file. \n"
			"The collect.single command should be in the following format: \n"
			"The freq parameter is used indicate when to output your data, by default it is set to 100. But you can set it to a percentage of the number of sequence. For example freq=0.10, means 10%. \n"
			"collect.single(label=yourLabel, freq=yourFreq, calc=yourEstimators).\n"
			"Example collect(label=unique-.01-.03, freq=10, calc=sobs-chao-ace-jack).\n"
			"The default values for freq is 100, and calc are sobs-chao-ace-jack-shannon-npshannon-simpson.\n"
			"The alpha parameter is used to set the alpha value for the shannonrange calculator.\n";
		helpString += validCalculator.printCalc("single");
		helpString += "The label parameter is used to analyze specific labels in your input.\n"
			"Note: No spaces between parameter labels (i.e. freq), '=' and parameters (i.e.yourFreq).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CollectCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string CollectCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "sobs") { pattern = "[filename],sobs"; }
	else if (type == "chao") { pattern = "[filename],chao"; }
	else if (type == "nseqs") { pattern = "[filename],nseqs"; }
	else if (type == "coverage") { pattern = "[filename],coverage"; }
	else if (type == "ace") { pattern = "[filename],ace"; }
	else if (type == "jack") { pattern = "[filename],jack"; }
	else if (type == "shannon") { pattern = "[filename],shannon"; }
	else if (type == "shannoneven") { pattern = "[filename],shannoneven"; }
	else if (type == "shannonrange") { pattern = "[filename],shannonrange"; }
	else if (type == "npshannon") { pattern = "[filename],npshannon"; }
	else if (type == "heip") { pattern = "[filename],heip"; }
	else if (type == "smithwilson") { pattern = "[filename],smithwilson"; }
	else if (type == "simpson") { pattern = "[filename],simpson"; }
	else if (type == "simpsoneven") { pattern = "[filename],simpsoneven"; }
	else if (type == "invsimpson") { pattern = "[filename],invsimpson"; }
	else if (type == "bootstrap") { pattern = "[filename],bootstrap"; }
	else if (type == "geometric") { pattern = "[filename],geometric"; }
	else if (type == "qstat") { pattern = "[filename],qstat"; }
	else if (type == "logseries") { pattern = "[filename],logseries"; }
	else if (type == "bergerparker") { pattern = "[filename],bergerparker"; }
	else if (type == "bstick") { pattern = "[filename],bstick"; }
	else if (type == "goodscoverage") { pattern = "[filename],goodscoverage"; }
	else if (type == "efron") { pattern = "[filename],efron"; }
	else if (type == "boneh") { pattern = "[filename],boneh"; }
	else if (type == "solow") { pattern = "[filename],solow"; }
	else if (type == "shen") { pattern = "[filename],shen"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
CollectCommand::CollectCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["sobs"] = tempOutNames;
		outputTypes["chao"] = tempOutNames;
		outputTypes["nseqs"] = tempOutNames;
		outputTypes["coverage"] = tempOutNames;
		outputTypes["ace"] = tempOutNames;
		outputTypes["jack"] = tempOutNames;
		outputTypes["shannon"] = tempOutNames;
		outputTypes["shannoneven"] = tempOutNames;
		outputTypes["shannonrange"] = tempOutNames;
		outputTypes["npshannon"] = tempOutNames;
		outputTypes["heip"] = tempOutNames;
		outputTypes["smithwilson"] = tempOutNames;
		outputTypes["simpson"] = tempOutNames;
		outputTypes["simpsoneven"] = tempOutNames;
		outputTypes["invsimpson"] = tempOutNames;
		outputTypes["bootstrap"] = tempOutNames;
		outputTypes["geometric"] = tempOutNames;
		outputTypes["qstat"] = tempOutNames;
		outputTypes["logseries"] = tempOutNames;
		outputTypes["bergerparker"] = tempOutNames;
		outputTypes["bstick"] = tempOutNames;
		outputTypes["goodscoverage"] = tempOutNames;
		outputTypes["efron"] = tempOutNames;
		outputTypes["boneh"] = tempOutNames;
		outputTypes["solow"] = tempOutNames;
		outputTypes["shen"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CollectCommand, CollectCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
CollectCommand::CollectCommand(Settings& settings, string option) : Command(settings, option) {
	//	allLines = 1;
	//		if (label == "not found") { label = ""; }
	//		else { 
	//			if(label != "all") {  Utility::split(label, '-', labels);  allLines = 0;  }
	//			else { allLines = 1;  }
	//		}
	//		
	//		Utility::split(calc, '-', Estimators);
	//		if (m->inUsersGroups("citation", Estimators)) { 
	//			ValidCalculators validCalc; validCalc.printCitations(Estimators); 
	//			//remove citation from list of calcs
	//			for (int i = 0; i < Estimators.size(); i++) { if (Estimators[i] == "citation") {  Estimators.erase(Estimators.begin()+i); break; } }
	//		}
	//}
	//catch(exception& e) {
	//	LOG(FATAL) << e.what() << " in CollectCommand, CollectCommand";
	//	exit(1);
	//}			
}
//**********************************************************************************************************************

int CollectCommand::execute() {

	if ((format != "sharedfile")) { inputFileNames.push_back(inputfile); }
	else { inputFileNames = parseSharedFile(sharedfile);  format = "rabund"; }

	for (int p = 0; p < inputFileNames.size(); p++) {

		if (ctrlc_pressed) { outputTypes.clear(); for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  m->clearGroups();  return 0; }

		if (outputDir == "") { outputDir += File::getPath(inputFileNames[p]); }
		string fileNameRoot = outputDir + File::getRootName(File::getSimpleName(inputFileNames[p]));
		map<string, string> variables;
		variables["[filename]"] = fileNameRoot;
		//globaldata->inputFileName = inputFileNames[p];

		if (inputFileNames.size() > 1) {
			LOG(INFO) << std::endl << "Processing group " + groups[p] << '\n' << '\n';
		}

		ValidCalculators validCalculator;

		for (int i = 0; i < Estimators.size(); i++) {
			if (validCalculator.isValidCalculator("single", Estimators[i]) == true) {
				if (Estimators[i] == "sobs") {
					cDisplays.push_back(new CollectDisplay(new Sobs(), new OneColumnFile(getOutputFileName("sobs", variables))));
					outputNames.push_back(getOutputFileName("sobs", variables)); outputTypes["sobs"].push_back(getOutputFileName("sobs", variables));
				}
				else if (Estimators[i] == "chao") {
					cDisplays.push_back(new CollectDisplay(new Chao1(), new ThreeColumnFile(getOutputFileName("chao", variables))));
					outputNames.push_back(getOutputFileName("chao", variables)); outputTypes["chao"].push_back(getOutputFileName("chao", variables));
				}
				else if (Estimators[i] == "nseqs") {
					cDisplays.push_back(new CollectDisplay(new NSeqs(), new OneColumnFile(getOutputFileName("nseqs", variables))));
					outputNames.push_back(getOutputFileName("nseqs", variables)); outputTypes["nseqs"].push_back(getOutputFileName("nseqs", variables));
				}
				else if (Estimators[i] == "coverage") {
					cDisplays.push_back(new CollectDisplay(new Coverage(), new OneColumnFile(getOutputFileName("coverage", variables))));
					outputNames.push_back(getOutputFileName("coverage", variables)); outputTypes["coverage"].push_back(getOutputFileName("coverage", variables));
				}
				else if (Estimators[i] == "ace") {
					cDisplays.push_back(new CollectDisplay(new Ace(abund), new ThreeColumnFile(getOutputFileName("ace", variables))));
					outputNames.push_back(getOutputFileName("ace", variables)); outputTypes["ace"].push_back(getOutputFileName("ace", variables));
				}
				else if (Estimators[i] == "jack") {
					cDisplays.push_back(new CollectDisplay(new Jackknife(), new ThreeColumnFile(getOutputFileName("jack", variables))));
					outputNames.push_back(getOutputFileName("jack", variables)); outputTypes["jack"].push_back(getOutputFileName("jack", variables));
				}
				else if (Estimators[i] == "shannon") {
					cDisplays.push_back(new CollectDisplay(new Shannon(), new ThreeColumnFile(getOutputFileName("shannon", variables))));
					outputNames.push_back(getOutputFileName("shannon", variables)); outputTypes["shannon"].push_back(getOutputFileName("shannon", variables));
				}
				else if (Estimators[i] == "shannoneven") {
					cDisplays.push_back(new CollectDisplay(new ShannonEven(), new OneColumnFile(getOutputFileName("shannoneven", variables))));
					outputNames.push_back(getOutputFileName("shannoneven", variables)); outputTypes["shannoneven"].push_back(getOutputFileName("shannoneven", variables));
				}
				else if (Estimators[i] == "shannonrange") {
					cDisplays.push_back(new CollectDisplay(new RangeShannon(alpha), new ThreeColumnFile(getOutputFileName("shannonrange", variables))));
					outputNames.push_back(getOutputFileName("shannonrange", variables)); outputTypes["shannoneven"].push_back(getOutputFileName("shannonrange", variables));
				}
				else if (Estimators[i] == "npshannon") {
					cDisplays.push_back(new CollectDisplay(new NPShannon(), new OneColumnFile(getOutputFileName("npshannon", variables))));
					outputNames.push_back(getOutputFileName("npshannon", variables)); outputTypes["npshannon"].push_back(getOutputFileName("npshannon", variables));
				}
				else if (Estimators[i] == "heip") {
					cDisplays.push_back(new CollectDisplay(new Heip(), new OneColumnFile(getOutputFileName("heip", variables))));
					outputNames.push_back(getOutputFileName("heip", variables)); outputTypes["heip"].push_back(getOutputFileName("heip", variables));
				}
				else if (Estimators[i] == "smithwilson") {
					cDisplays.push_back(new CollectDisplay(new SmithWilson(), new OneColumnFile(getOutputFileName("smithwilson", variables))));
					outputNames.push_back(getOutputFileName("smithwilson", variables)); outputTypes["smithwilson"].push_back(getOutputFileName("smithwilson", variables));
				}
				else if (Estimators[i] == "simpson") {
					cDisplays.push_back(new CollectDisplay(new Simpson(), new ThreeColumnFile(getOutputFileName("simpson", variables))));
					outputNames.push_back(getOutputFileName("simpson", variables)); outputTypes["simpson"].push_back(getOutputFileName("simpson", variables));
				}
				else if (Estimators[i] == "simpsoneven") {
					cDisplays.push_back(new CollectDisplay(new SimpsonEven(), new OneColumnFile(getOutputFileName("simpsoneven", variables))));
					outputNames.push_back(getOutputFileName("simpsoneven", variables)); outputTypes["simpsoneven"].push_back(getOutputFileName("simpsoneven", variables));
				}
				else if (Estimators[i] == "invsimpson") {
					cDisplays.push_back(new CollectDisplay(new InvSimpson(), new ThreeColumnFile(getOutputFileName("invsimpson", variables))));
					outputNames.push_back(getOutputFileName("invsimpson", variables)); outputTypes["invsimpson"].push_back(getOutputFileName("invsimpson", variables));
				}
				else if (Estimators[i] == "bootstrap") {
					cDisplays.push_back(new CollectDisplay(new Bootstrap(), new OneColumnFile(getOutputFileName("bootstrap", variables))));
					outputNames.push_back(getOutputFileName("bootstrap", variables)); outputTypes["bootstrap"].push_back(getOutputFileName("bootstrap", variables));
				}
				else if (Estimators[i] == "geometric") {
					cDisplays.push_back(new CollectDisplay(new Geom(), new OneColumnFile(getOutputFileName("geometric", variables))));
					outputNames.push_back(getOutputFileName("geometric", variables)); outputTypes["geometric"].push_back(getOutputFileName("geometric", variables));
				}
				else if (Estimators[i] == "qstat") {
					cDisplays.push_back(new CollectDisplay(new QStat(), new OneColumnFile(getOutputFileName("qstat", variables))));
					outputNames.push_back(getOutputFileName("qstat", variables)); outputTypes["qstat"].push_back(getOutputFileName("qstat", variables));
				}
				else if (Estimators[i] == "logseries") {
					cDisplays.push_back(new CollectDisplay(new LogSD(), new OneColumnFile(getOutputFileName("logseries", variables))));
					outputNames.push_back(getOutputFileName("logseries", variables)); outputTypes["logseries"].push_back(getOutputFileName("logseries", variables));
				}
				else if (Estimators[i] == "bergerparker") {
					cDisplays.push_back(new CollectDisplay(new BergerParker(), new OneColumnFile(getOutputFileName("bergerparker", variables))));
					outputNames.push_back(getOutputFileName("bergerparker", variables)); outputTypes["bergerparker"].push_back(getOutputFileName("bergerparker", variables));
				}
				else if (Estimators[i] == "bstick") {
					cDisplays.push_back(new CollectDisplay(new BStick(), new ThreeColumnFile(getOutputFileName("bstick", variables))));
					outputNames.push_back(getOutputFileName("bstick", variables)); outputTypes["bstick"].push_back(getOutputFileName("bstick", variables));
				}
				else if (Estimators[i] == "goodscoverage") {
					cDisplays.push_back(new CollectDisplay(new GoodsCoverage(), new OneColumnFile(getOutputFileName("goodscoverage", variables))));
					outputNames.push_back(getOutputFileName("goodscoverage", variables)); outputTypes["goodscoverage"].push_back(getOutputFileName("goodscoverage", variables));
				}
				else if (Estimators[i] == "efron") {
					cDisplays.push_back(new CollectDisplay(new Efron(size), new OneColumnFile(getOutputFileName("efron", variables))));
					outputNames.push_back(getOutputFileName("efron", variables)); outputTypes["efron"].push_back(getOutputFileName("efron", variables));
				}
				else if (Estimators[i] == "boneh") {
					cDisplays.push_back(new CollectDisplay(new Boneh(size), new OneColumnFile(getOutputFileName("boneh", variables))));
					outputNames.push_back(getOutputFileName("boneh", variables)); outputTypes["boneh"].push_back(getOutputFileName("boneh", variables));
				}
				else if (Estimators[i] == "solow") {
					cDisplays.push_back(new CollectDisplay(new Solow(size), new OneColumnFile(getOutputFileName("solow", variables))));
					outputNames.push_back(getOutputFileName("solow", variables)); outputTypes["solow"].push_back(getOutputFileName("solow", variables));
				}
				else if (Estimators[i] == "shen") {
					cDisplays.push_back(new CollectDisplay(new Shen(size, abund), new OneColumnFile(getOutputFileName("shen", variables))));
					outputNames.push_back(getOutputFileName("shen", variables)); outputTypes["shen"].push_back(getOutputFileName("shen", variables));
				}
			}
		}

		//if the users entered no valid calculators don't execute command
		if (cDisplays.size() == 0) { return 0; }

		input = new InputData(inputFileNames[p], format);
		order = input->getOrderVector();
		string lastLabel = order->getLabel();

		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		if (ctrlc_pressed) {
			for (int i = 0;i < cDisplays.size();i++) { delete cDisplays[i]; }
			for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
			delete input;
			delete order;
			m->clearGroups();
			return 0;
		}


		while ((order != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

			if (ctrlc_pressed) {
				for (int i = 0;i < cDisplays.size();i++) { delete cDisplays[i]; }
				for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
				delete input;
				delete order;
				m->clearGroups();
				return 0;
			}


			if (allLines == 1 || labels.count(order->getLabel()) == 1) {

				LOG(INFO) << order->getLabel() << '\n';
				cCurve = new Collect(order, cDisplays);
				cCurve->getCurve(freq);
				delete cCurve;

				processedLabels.insert(order->getLabel());
				userLabels.erase(order->getLabel());


			}
			//you have a label the user want that is smaller than this label and the last label has not already been processed 
			if ((Utility::anyLabelsToProcess(order->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = order->getLabel();

				delete order;
				order = (input->getOrderVector(lastLabel));

				LOG(INFO) << order->getLabel() << '\n';
				cCurve = new Collect(order, cDisplays);
				cCurve->getCurve(freq);
				delete cCurve;


				processedLabels.insert(order->getLabel());
				userLabels.erase(order->getLabel());

				//restore real lastlabel to save below
				order->setLabel(saveLabel);
			}

			lastLabel = order->getLabel();

			delete order;
			order = (input->getOrderVector());
		}


		if (ctrlc_pressed) {
			for (int i = 0;i < cDisplays.size();i++) { delete cDisplays[i]; }
			for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
			delete input;
			m->clearGroups();
			return 0;
		}

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
			if (order != NULL) { delete order; }
			order = (input->getOrderVector(lastLabel));

			LOG(INFO) << order->getLabel() << '\n';

			cCurve = new Collect(order, cDisplays);
			cCurve->getCurve(freq);
			delete cCurve;

			if (ctrlc_pressed) {
				for (int i = 0;i < cDisplays.size();i++) { delete cDisplays[i]; }
				for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } outputTypes.clear();
				delete input;
				delete order;
				m->clearGroups();
				return 0;
			}
			delete order;
		}

		for (int i = 0;i < cDisplays.size();i++) { delete cDisplays[i]; }
		cDisplays.clear();
		delete input;
	}

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";


	return 0;
}

//**********************************************************************************************************************
vector<string> CollectCommand::parseSharedFile(string filename) {
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
		}

		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		lookup = input->getSharedRAbundVectors();
	}

	//free memory
	for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
		delete it3->second;
	}

	delete input;

	return filenames;
}
//**********************************************************************************************************************

