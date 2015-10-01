/*
 *  catchallcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/11/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "catchallcommand.h"

 //**********************************************************************************************************************
vector<string> CatchAllCommand::setParameters() {
	try {
		nkParameters.add(new StringParameter("label", "", false, false));
		//can choose shared or sabund not both, so put them in the same chooseOnlyOneGroup
		CommandParameter pshared("shared", "InputTypes", "", "", "catchallInputs", "catchallInputs", "none", "analysis-bestanalysis-models-bubble-summary", false, false, true); parameters.push_back(pshared);
		CommandParameter psabund("sabund", "InputTypes", "", "", "catchallInputs", "catchallInputs", "none", "analysis-bestanalysis-models-bubble-summary", false, false, true); parameters.push_back(psabund);
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CatchAllCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string CatchAllCommand::getHelpString() {
	try {
		string helpString = "The catchall command interfaces mothur with the catchall program written by Linda Woodard, Sean Connolly and John Bunge.\n"
			"For more information about catchall refer to http://www.northeastern.edu/catchall/index.html \n"
			"The catchall executable must be in the same folder as your mothur executable. \n"
			"If you are a MAC or Linux user you must also have installed mono, a link to mono is on the webpage. \n"
			"The catchall command parameters are shared, sabund and label.  shared or sabund is required. \n"
			"The label parameter is used to analyze specific labels in your input.\n"
			"The catchall command should be in the following format: \n"
			"catchall(sabund=yourSabundFile) \n"
			"Example: catchall(sabund=abrecovery.fn.sabund) \n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CatchAllCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string CatchAllCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "analysis") { pattern = "[filename],_Analysis.csv"; }
	else if (type == "bestanalysis") { pattern = "[filename],_BestModelsAnalysis.csv"; }
	else if (type == "models") { pattern = "[filename],_BestModelsAnalysis.csv"; }
	else if (type == "bubble") { pattern = "[filename],_BubblePlot.csv"; }
	else if (type == "summary") { pattern = "[filename],catchall.summary"; }
	else if (type == "sabund") { pattern = "[filename],[distance],csv"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
CatchAllCommand::CatchAllCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["analysis"] = tempOutNames;
		outputTypes["bestanalysis"] = tempOutNames;
		outputTypes["models"] = tempOutNames;
		outputTypes["bubble"] = tempOutNames;
		outputTypes["summary"] = tempOutNames;
		outputTypes["sabund"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CatchAllCommand, CatchAllCommand";
		exit(1);
	}
}
/**************************************************************************************/
CatchAllCommand::CatchAllCommand(Settings& settings, string option) : Command(settings, option) {
	try {

		abort = false; calledHelp = false;
		allLines = 1;

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
			outputTypes["analysis"] = tempOutNames;
			outputTypes["bestanalysis"] = tempOutNames;
			outputTypes["models"] = tempOutNames;
			outputTypes["bubble"] = tempOutNames;
			outputTypes["summary"] = tempOutNames;
			outputTypes["sabund"] = tempOutNames;


			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);
			if (inputDir == "not found") { inputDir = ""; }
			else {
				string path;
				it = parameters.find("sabund");
				//user has given a template file
				if (it != parameters.end()) {
					path = File::getPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") { parameters["sabund"] = inputDir + it->second; }
				}

				it = parameters.find("shared");
				//user has given a template file
				if (it != parameters.end()) {
					path = File::getPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") { parameters["shared"] = inputDir + it->second; }
				}
			}

			//check for required parameters
			sabundfile = validParameter.validFile(parameters, "sabund", true);
			if (sabundfile == "not open") { sabundfile = ""; abort = true; }
			else if (sabundfile == "not found") { sabundfile = ""; }
			else { settings.setCurrent("sabund", sabundfile); }

			sharedfile = validParameter.validFile(parameters, "shared", true);
			if (sharedfile == "not open") { sharedfile = ""; abort = true; }
			else if (sharedfile == "not found") { sharedfile = ""; }
			else { settings.setCurrent("shared", sharedfile); }

			string label = validParameter.validFile(parameters, "label", false);
			if (label == "not found") { label = ""; }
			else {
				if (label != "all") { Utility::split(label, '-', labels);  allLines = 0; }
				else { allLines = 1; }
			}

			if ((sharedfile == "") && (sabundfile == "")) {
				//is there are current file available for either of these?
				//give priority to shared, then sabund
				//if there is a current shared file, use it
				sharedfile = settings.getCurrent("shared");
				if (sharedfile != "") { LOG(INFO) << "Using " + sharedfile + " as input file for the shared parameter." << '\n'; }
				else {
					sabundfile = settings.getCurrent("sabund");
					if (sabundfile != "") { LOG(INFO) << "Using " + sabundfile + " as input file for the sabund parameter." << '\n'; }
					else {
						LOG(INFO) << "No valid current files. You must provide a sabund or shared file before you can use the catchall command." << '\n';
						abort = true;
					}
				}
			}

			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);
			if (outputDir == "not found") {
				if (sabundfile != "") { outputDir = File::getPath(sabundfile); }
				else { outputDir = File::getPath(sharedfile); }
			}
		}

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CatchAllCommand, CatchAllCommand";
		exit(1);
	}
}
/**************************************************************************************/
int CatchAllCommand::execute() {
	try {

		//get location of catchall
		path = m->argv;
		path = path.substr(0, (path.find_last_of("othur") - 5));
		path = m->getFullPathName(path);

		LOG(DEBUG) << "mothur's path = " + path + "\n";

		savedOutputDir = outputDir;
		string catchAllCommandExe = "";
		string catchAllTest = "";
		string programName = "";
#if defined (UNIX)
		if (outputDir == "") { outputDir = "./"; } //force full pathname to be created for catchall, this is necessary because if catchall is in the path it will look for input file whereever the exe is and not the cwd.
		catchAllTest = path + "CatchAllcmdL.exe";
#else
		if (outputDir == "") { outputDir = ".\\"; } //force full pathname to be created for catchall, this is necessary because if catchall is in the path it will look for input file whereever the exe is and not the cwd.
		catchAllTest = path + "CatchAllcmdW.exe";
#endif

		//test to make sure formatdb exists
		ifstream in;
		catchAllTest = m->getFullPathName(catchAllTest);
		int ableToOpen = File::openInputFile(catchAllTest, in, "no error"); in.close();
		if (ableToOpen == 1) {
			LOG(INFO) << catchAllTest + " file does not exist. Checking path... \n";

			programName = "CatchAllcmdW.exe";
#if defined (UNIX)
			programName = "CatchAllcmdL.exe";
#endif
			string cLocation = m->findProgramPath(programName);

			ifstream in2;
			ableToOpen = File::openInputFile(cLocation, in2, "no error"); in2.close();

			if (ableToOpen == 1) {
				programName = "catchall";

				string cLocation = m->findProgramPath(programName);

				ifstream in3;
				ableToOpen = File::openInputFile(cLocation, in3, "no error"); in3.close();

				if (ableToOpen == 1) { LOG(LOGERROR) << "" + cLocation + " file does not exist. mothur requires the catchall executable." << '\n';  return 0; }
				else { LOG(INFO) << "Found catchall in your path, using " + cLocation + "\n"; catchAllTest = cLocation; }
			}
			else { LOG(INFO) << "Found catchall in your path, using " + cLocation + "\n"; catchAllTest = cLocation; }
		}
		catchAllTest = m->getFullPathName(catchAllTest);

#if defined (UNIX)
		if (programName == "catchall") { catchAllCommandExe += "catchall "; }
		else { catchAllCommandExe += "mono \"" + catchAllTest + "\" "; }
#else
		catchAllCommandExe += "\"" + catchAllTest + "\" ";
#endif

		//prepare full output directory
		outputDir = m->getFullPathName(outputDir);

		if (app.isDebug) { LOG(DEBUG) << "catchall location = " + catchAllCommandExe + "\n[DEBUG]: outputDir = " + outputDir + "\n"; }

		vector<string> inputFileNames;
		if (sharedfile != "") { inputFileNames = parseSharedFile(sharedfile); }
		else { inputFileNames.push_back(sabundfile); }

		for (int p = 0; p < inputFileNames.size(); p++) {
			if (inputFileNames.size() > 1) {
				LOG(INFO) << std::endl << "Processing group " + groups[p] << '\n' << '\n';
			}

			InputData input(inputFileNames[p], "sabund");
			SAbundVector* sabund = input.getSAbundVector();
			string lastLabel = sabund->getLabel();

			set<string> processedLabels;
			set<string> userLabels = labels;

			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputFileNames[p]));
			string summaryfilename = getOutputFileName("summary", variables);
			summaryfilename = m->getFullPathName(summaryfilename);

			if (app.isDebug) { LOG(DEBUG) << "Input File = " + inputFileNames[p] + ".\n[DEBUG]: inputdata address = " + toString(&input) + ".\n[DEBUG]: sabund address = " + toString(&sabund) + ".\n"; }

			ofstream out;
			File::openOutputFile(summaryfilename, out);

			out << "label\tmodel\testimate\tlci\tuci" << endl;

			if (app.isDebug) { string open = "no"; if (out.is_open()) { open = "yes"; } LOG(DEBUG) << "output stream is open = " + open + ".\n"; }

			//for each label the user selected
			while ((sabund != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {


				if (allLines == 1 || labels.count(sabund->getLabel()) == 1) {
					LOG(INFO) << sabund->getLabel() << '\n';

					//create catchall input file from mothur's inputfile
					string filename = process(sabund, inputFileNames[p]);
					string outputPath = m->getPathName(filename);

					//create system command
					string catchAllCommand = "";
#if defined (UNIX)
					catchAllCommand += catchAllCommandExe + "\"" + filename + "\" \"" + outputPath + +"\" 1";
#else
					//removes extra '\\' catchall doesnt like that
					vector<string> tempNames;
					string tempFilename = filename;
					Utility::split(tempFilename, '-', tempNames);
					tempFilename = tempNames[0];
					tempNames.clear();
					string tempOutputPath = outputPath;
					Utility::split(tempOutputPath, '-', tempNames);
					tempOutputPath = tempNames[0];
					if (tempOutputPath.length() > 0) { tempOutputPath = tempOutputPath.substr(0, tempOutputPath.length() - 1); }
					catchAllCommand += catchAllCommandExe + "\"" + tempFilename + "\" \"" + tempOutputPath + "\" 1";
					catchAllCommand = "\"" + catchAllCommand + "\"";
#endif

					if (app.isDebug) { LOG(DEBUG) << "catchall command = " + catchAllCommand + ". About to call system.\n"; }

					//run catchall
					system(catchAllCommand.c_str());

					if (app.isDebug) { LOG(DEBUG) << "back from system call. Keeping file: " + filename + ".\n"; }

					if (!app.isDebug) { File::remove(filename); }


					filename = File::getRootName(filename); filename = filename.substr(0, filename.length() - 1); //rip off extra .
					if (savedOutputDir == "") { filename = File::getSimpleName(filename); }

					variables["[filename]"] = filename;
					outputNames.push_back(getOutputFileName("analysis", variables)); outputTypes["analysis"].push_back(getOutputFileName("analysis", variables));
					outputNames.push_back(getOutputFileName("bestanalysis", variables)); outputTypes["bestanalysis"].push_back(getOutputFileName("bestanalysis", variables));
					outputNames.push_back(getOutputFileName("models", variables)); outputTypes["models"].push_back(getOutputFileName("models", variables));
					outputNames.push_back(getOutputFileName("bubble", variables)); outputTypes["bubble"].push_back(getOutputFileName("bubble", variables));

					if (app.isDebug) { LOG(DEBUG) << "About to create summary file for: " + filename + ".\n[DEBUG]: sabund label = " + sabund->getLabel() + ".\n"; }

					createSummaryFile(filename + "_BestModelsAnalysis.csv", sabund->getLabel(), out);

					if (app.isDebug) { LOG(DEBUG) << "Done creating summary file.\n"; }

					if (ctrlc_pressed) { out.close(); for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  delete sabund;  return 0; }

					processedLabels.insert(sabund->getLabel());
					userLabels.erase(sabund->getLabel());
				}

				if ((Utility::anyLabelsToProcess(sabund->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
					string saveLabel = sabund->getLabel();

					delete sabund;
					sabund = (input.getSAbundVector(lastLabel));

					LOG(INFO) << sabund->getLabel() << '\n';


					//create catchall input file from mothur's inputfile
					string filename = process(sabund, inputFileNames[p]);
					string outputPath = m->getPathName(filename);

					//create system command
					string catchAllCommand = "";
#if defined (UNIX)
					catchAllCommand += catchAllCommandExe + "\"" + filename + "\" \"" + outputPath + +"\" 1";
#else
					//removes extra '\\' catchall doesnt like that
					vector<string> tempNames;
					string tempFilename = filename;
					Utility::split(tempFilename, '-', tempNames);
					tempFilename = tempNames[0];
					tempNames.clear();
					string tempOutputPath = outputPath;
					Utility::split(tempOutputPath, '-', tempNames);
					tempOutputPath = tempNames[0];
					if (tempOutputPath.length() > 0) { tempOutputPath = tempOutputPath.substr(0, tempOutputPath.length() - 1); }
					catchAllCommand += catchAllCommandExe + "\"" + tempFilename + "\" \"" + tempOutputPath + "\" 1";
					catchAllCommand = "\"" + catchAllCommand + "\"";
#endif

					if (app.isDebug) { LOG(DEBUG) << "catchall command = " + catchAllCommand + ". About to call system.\n"; }

					//run catchall
					system(catchAllCommand.c_str());

					if (app.isDebug) { LOG(DEBUG) << "back from system call. Keeping file: " + filename + ".\n"; }

					if (!app.isDebug) { File::remove(filename); }


					filename = File::getRootName(filename); filename = filename.substr(0, filename.length() - 1); //rip off extra .
					if (savedOutputDir == "") { filename = File::getSimpleName(filename); }

					variables["[filename]"] = filename;
					outputNames.push_back(getOutputFileName("analysis", variables)); outputTypes["analysis"].push_back(getOutputFileName("analysis", variables));
					outputNames.push_back(getOutputFileName("bestanalysis", variables)); outputTypes["bestanalysis"].push_back(getOutputFileName("bestanalysis", variables));
					outputNames.push_back(getOutputFileName("models", variables)); outputTypes["models"].push_back(getOutputFileName("models", variables));
					outputNames.push_back(getOutputFileName("bubble", variables)); outputTypes["bubble"].push_back(getOutputFileName("bubble", variables));


					if (app.isDebug) { LOG(DEBUG) << "About to create summary file for: " + filename + ".\n[DEBUG]: sabund label = " + sabund->getLabel() + ".\n"; }

					createSummaryFile(filename + "_BestModelsAnalysis.csv", sabund->getLabel(), out);

					if (app.isDebug) { LOG(DEBUG) << "Done creating summary file.\n"; }

					if (ctrlc_pressed) { out.close(); for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }   delete sabund;  return 0; }

					processedLabels.insert(sabund->getLabel());
					userLabels.erase(sabund->getLabel());

					//restore real lastlabel to save below
					sabund->setLabel(saveLabel);
				}


				lastLabel = sabund->getLabel();

				delete sabund;
				sabund = (input.getSAbundVector());
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
				if (sabund != NULL) { delete sabund; }
				sabund = (input.getSAbundVector(lastLabel));

				LOG(INFO) << sabund->getLabel() << '\n';

				//create catchall input file from mothur's inputfile
				string filename = process(sabund, inputFileNames[p]);
				string outputPath = m->getPathName(filename);

				//create system command
				string catchAllCommand = "";
#if defined (UNIX)
				catchAllCommand += catchAllCommandExe + "\"" + filename + "\" \"" + outputPath + +"\" 1";
#else
				//removes extra '\\' catchall doesnt like that
				vector<string> tempNames;
				string tempFilename = filename;
				Utility::split(tempFilename, '-', tempNames);
				tempFilename = tempNames[0];
				tempNames.clear();
				string tempOutputPath = outputPath;
				Utility::split(tempOutputPath, '-', tempNames);
				tempOutputPath = tempNames[0];
				if (tempOutputPath.length() > 0) { tempOutputPath = tempOutputPath.substr(0, tempOutputPath.length() - 1); }
				catchAllCommand += catchAllCommandExe + "\"" + tempFilename + "\" \"" + tempOutputPath + "\" 1";
				catchAllCommand = "\"" + catchAllCommand + "\"";
#endif

				if (app.isDebug) { LOG(DEBUG) << "catchall command = " + catchAllCommand + ". About to call system.\n"; }

				//run catchall
				system(catchAllCommand.c_str());

				if (app.isDebug) { LOG(DEBUG) << "back from system call. Keeping file: " + filename + ".\n"; }

				if (!app.isDebug) { File::remove(filename); }

				filename = File::getRootName(filename); filename = filename.substr(0, filename.length() - 1); //rip off extra .
				if (savedOutputDir == "") { filename = File::getSimpleName(filename); }

				variables["[filename]"] = filename;
				outputNames.push_back(getOutputFileName("analysis", variables)); outputTypes["analysis"].push_back(getOutputFileName("analysis", variables));
				outputNames.push_back(getOutputFileName("bestanalysis", variables)); outputTypes["bestanalysis"].push_back(getOutputFileName("bestanalysis", variables));
				outputNames.push_back(getOutputFileName("models", variables)); outputTypes["models"].push_back(getOutputFileName("models", variables));
				outputNames.push_back(getOutputFileName("bubble", variables)); outputTypes["bubble"].push_back(getOutputFileName("bubble", variables));
				if (app.isDebug) { LOG(DEBUG) << "About to create summary file for: " + filename + ".\n[DEBUG]: sabund label = " + sabund->getLabel() + ".\n"; }

				createSummaryFile(filename + "_BestModelsAnalysis.csv", sabund->getLabel(), out);

				if (app.isDebug) { LOG(DEBUG) << "Done creating summary file.\n"; }

				delete sabund;
			}

			out.close();

			if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

		}

		if (sharedfile == "") {
			string summaryfilename = savedOutputDir + File::getRootName(File::getSimpleName(inputFileNames[0])) + "catchall.summary";
			summaryfilename = m->getFullPathName(summaryfilename);
			outputNames.push_back(summaryfilename); outputTypes["summary"].push_back(summaryfilename);
		}
		else { //combine summaries
			vector<string> sumNames;
			for (int i = 0; i < inputFileNames.size(); i++) {
				sumNames.push_back(m->getFullPathName(outputDir + File::getRootName(File::getSimpleName(inputFileNames[i])) + "catchall.summary"));
			}
			string summaryfilename = combineSummmary(sumNames);
			outputNames.push_back(summaryfilename); outputTypes["summary"].push_back(summaryfilename);
		}

		LOG(INFO) << '\n' << "Output File Names: " << '\n';
		for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
		LOG(INFO) << "";


		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in CatchAllCommand, execute";
		exit(1);
	}
}
//**********************************************************************************************************************
string CatchAllCommand::process(SAbundVector* sabund, string file1) {
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(file1));
	variables["[distance]"] = sabund->getLabel();
	string filename = getOutputFileName("sabund", variables);
	filename = m->getFullPathName(filename);

	ofstream out;
	File::openOutputFile(filename, out);

	if (app.isDebug) { LOG(DEBUG) << "Creating " + filename + " file for catchall, shown below.\n\n"; }

	for (int i = 1; i <= sabund->getMaxRank(); i++) {
		int temp = sabund->get(i);

		if (temp != 0) {
			out << i << "," << temp << endl;
			if (app.isDebug) { LOG(INFO) << toString(i) + "," + toString(temp) + "\n"; }
		}
	}
	out.close();

	if (app.isDebug) { LOG(DEBUG) << "Done creating " + filename + " file for catchall, shown above.\n\n"; }

	return filename;

}
//*********************************************************************************************************************
string CatchAllCommand::combineSummmary(vector<string>& outputNames) {

	ofstream out;
	map<string, string> variables;
	variables["[filename]"] = savedOutputDir + File::getRootName(File::getSimpleName(sharedfile));
	string combineFileName = getOutputFileName("summary", variables);

	//open combined file
	File::openOutputFile(combineFileName, out);

	out << "label\tgroup\tmodel\testimate\tlci\tuci" << endl;

	//open each groups summary file
	string newLabel = "";
	int numLines = 0;
	map<string, vector<string> > files;
	for (int i = 0; i < outputNames.size(); i++) {
		vector<string> thisFilesLines;

		ifstream temp;
		File::openInputFile(outputNames[i], temp);

		//read through first line - labels
		File::getline(temp);
		File::gobble(temp);

		//for each label
		while (!temp.eof()) {

			string thisLine = "";
			string tempLabel;

			for (int j = 0; j < 5; j++) {
				temp >> tempLabel;

				//save for later
				if (j == 1) { thisLine += groups[i] + "\t" + tempLabel + "\t"; }
				else { thisLine += tempLabel + "\t"; }
			}

			thisLine += "\n";

			thisFilesLines.push_back(thisLine);

			File::gobble(temp);
		}

		files[outputNames[i]] = thisFilesLines;

		numLines = thisFilesLines.size();

		temp.close();
		File::remove(outputNames[i]);
	}

	//for each label
	for (int k = 0; k < numLines; k++) {

		//grab summary data for each group
		for (int i = 0; i < outputNames.size(); i++) {
			out << files[outputNames[i]][k];
		}
	}


	out.close();

	//return combine file name
	return combineFileName;

}
//**********************************************************************************************************************
int CatchAllCommand::createSummaryFile(string file1, string label, ofstream& out) {

	ifstream in;
	int able = File::openInputFile(file1, in, "noerror");

	if (able == 1) { LOG(LOGERROR) << "the catchall program did not run properly. Please check to make sure it is located in the same folder as your mothur executable." << '\n';  ctrlc_pressed = true; return 0; }

	if (!in.eof()) {

		string header = File::getline(in); File::gobble(in);

		int pos = header.find("Total Number of Observed Species =");
		string numString = "";


		if (pos == string::npos) { LOG(INFO) << "[ERROR]: cannot parse " + file1 << '\n'; }
		else {
			//pos will be the position of the T in total, so we want to count to the position of =
			pos += 34;
			char c = header[pos];
			while (c != ',') {
				if (c != ' ') {
					numString += c;
				}
				pos++;
				c = header[pos];

				//sanity check
				if (pos > header.length()) { LOG(INFO) << "Cannot find number of OTUs in " + file1 << '\n'; in.close(); return 0; }
			}
		}

		string firstline = File::getline(in); File::gobble(in);
		vector<string> values;
		m->splitAtComma(firstline, values);

		values.pop_back(); //last value is always a blank string since the last character in the line is always a ','

		if (values.size() == 1) { //grab next line if firstline didn't have what you wanted
			string secondline = File::getline(in); File::gobble(in);
			values.clear();
			m->splitAtComma(secondline, values);

			values.pop_back(); //last value is always a blank string since the last character in the line is always a ','
		}

		if (values.size() == 1) { //still not what we wanted fill values with numOTUs
			values.resize(8, "");
			values[1] = "Sobs";
			values[4] = numString;
			values[6] = numString;
			values[7] = numString;
		}

		if (values.size() < 8) { values.resize(8, ""); }

		out << label << '\t' << values[1] << '\t' << values[4] << '\t' << values[6] << '\t' << values[7] << endl;
	}

	in.close();

	return 0;

}
//**********************************************************************************************************************
vector<string> CatchAllCommand::parseSharedFile(string filename) {
	vector<string> filenames;

	//read first line
	InputData input(filename, "sharedfile");
	vector<SharedRAbundVector*> lookup = input.getSharedRAbundVectors();

	string sharedFileRoot = outputDir + File::getRootName(File::getSimpleName(filename));

	//clears file before we start to write to it below
	for (int i = 0; i < lookup.size(); i++) {
		File::remove((sharedFileRoot + lookup[i]->getGroup() + ".sabund"));
		filenames.push_back((sharedFileRoot + lookup[i]->getGroup() + ".sabund"));
		groups.push_back(lookup[i]->getGroup());
	}

	while (lookup[0] != NULL) {

		for (int i = 0; i < lookup.size(); i++) {
			SAbundVector sav = lookup[i]->getSAbundVector();
			ofstream out;
			File::openOutputFileAppend(sharedFileRoot + lookup[i]->getGroup() + ".sabund", out);
			sav.print(out);
			out.close();
		}

		for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
		lookup = input.getSharedRAbundVectors();
	}

	return filenames;
}
/**************************************************************************************/
