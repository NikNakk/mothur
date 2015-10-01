/*
 *  setcurrentcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 3/16/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "setcurrentcommand.h"

 //**********************************************************************************************************************
vector<string> SetCurrentCommand::setParameters() {
	try {

		nkParameters.add(new ProcessorsParameter(settings));
		CommandParameter pflow("flow", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pflow);
		CommandParameter pfile("file", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pfile);
		CommandParameter pbiom("biom", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pbiom);
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pphylip);
		CommandParameter pcolumn("column", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pcolumn);
		CommandParameter psummary("summary", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(psummary);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pfasta);
		CommandParameter pname("name", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pname);
		CommandParameter pgroup("group", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pgroup);
		CommandParameter plist("list", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(plist);
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(ptaxonomy);
		CommandParameter pqfile("qfile", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pqfile);
		CommandParameter paccnos("accnos", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(paccnos);
		CommandParameter prabund("rabund", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(prabund);
		CommandParameter psabund("sabund", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(psabund);
		CommandParameter pdesign("design", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pdesign);
		CommandParameter porder("order", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(porder);
		CommandParameter ptree("tree", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(ptree);
		CommandParameter pshared("shared", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pshared);
		CommandParameter pordergroup("ordergroup", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pordergroup);
		CommandParameter pcount("count", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(pcount);
		CommandParameter prelabund("relabund", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(prelabund);
		CommandParameter psff("sff", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(psff);
		CommandParameter poligos("oligos", "InputTypes", "", "", "none", "none", "none", "", false, false); parameters.push_back(poligos);
		nkParameters.add(new StringParameter("clear", "", false, false));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SetCurrentCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string SetCurrentCommand::getHelpString() {
	try {
		string helpString = "The set.current command allows you to set the current files saved by mothur.\n"
			"The set.current command parameters are: clear, phylip, column, list, rabund, sabund, name, group, design, order, tree, shared, ordergroup, relabund, fasta, qfile, sff, oligos, accnos, biom, count, summary, file and taxonomy.\n"
			"The clear parameter is used to indicate which file types you would like to clear values for, multiple types can be separated by dashes.\n"
			"The set.current command should be in the following format: \n"
			"set.current(fasta=yourFastaFile) or set.current(fasta=amazon.fasta, clear=name-accnos)\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SetCurrentCommand, getHelpString";
		exit(1);
	}
}


//**********************************************************************************************************************
SetCurrentCommand::SetCurrentCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SetCurrentCommand, SetCurrentCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
SetCurrentCommand::SetCurrentCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

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


		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("phylip");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["phylip"] = inputDir + it->second; }
			}

			it = parameters.find("column");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["column"] = inputDir + it->second; }
			}

			it = parameters.find("fasta");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fasta"] = inputDir + it->second; }
			}

			it = parameters.find("list");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["list"] = inputDir + it->second; }
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

			it = parameters.find("name");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["name"] = inputDir + it->second; }
			}

			it = parameters.find("group");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["group"] = inputDir + it->second; }
			}

			it = parameters.find("design");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["design"] = inputDir + it->second; }
			}

			it = parameters.find("order");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["order"] = inputDir + it->second; }
			}

			it = parameters.find("tree");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["tree"] = inputDir + it->second; }
			}

			it = parameters.find("shared");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["shared"] = inputDir + it->second; }
			}

			it = parameters.find("ordergroup");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["ordergroup"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}

			it = parameters.find("relabund");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["relabund"] = inputDir + it->second; }
			}

			it = parameters.find("fasta");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fasta"] = inputDir + it->second; }
			}

			it = parameters.find("qfile");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["qfile"] = inputDir + it->second; }
			}

			it = parameters.find("sff");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["sff"] = inputDir + it->second; }
			}

			it = parameters.find("oligos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["oligos"] = inputDir + it->second; }
			}

			it = parameters.find("accnos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["accnos"] = inputDir + it->second; }
			}

			it = parameters.find("taxonomy");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["taxonomy"] = inputDir + it->second; }
			}

			it = parameters.find("flow");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["flow"] = inputDir + it->second; }
			}

			it = parameters.find("biom");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["biom"] = inputDir + it->second; }
			}

			it = parameters.find("summary");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["summary"] = inputDir + it->second; }
			}

			it = parameters.find("file");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["file"] = inputDir + it->second; }
			}
		}

		//check for parameters
		phylipfile = validParameter.validFile(parameters, "phylip", true);
		if (phylipfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["phylip"] << '\n'; phylipfile = ""; }
		else if (phylipfile == "not found") { phylipfile = ""; }
		if (phylipfile != "") { settings.setCurrent("phylip", phylipfile); }

		columnfile = validParameter.validFile(parameters, "column", true);
		if (columnfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["column"] << '\n'; columnfile = ""; }
		else if (columnfile == "not found") { columnfile = ""; }
		if (columnfile != "") { settings.setCurrent("column", columnfile); }

		listfile = validParameter.validFile(parameters, "list", true);
		if (listfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["list"] << '\n'; listfile = ""; }
		else if (listfile == "not found") { listfile = ""; }
		if (listfile != "") { settings.setCurrent("list", listfile); }

		rabundfile = validParameter.validFile(parameters, "rabund", true);
		if (rabundfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["rabund"] << '\n'; rabundfile = ""; }
		else if (rabundfile == "not found") { rabundfile = ""; }
		if (rabundfile != "") { settings.setCurrent("rabund", rabundfile); }

		sabundfile = validParameter.validFile(parameters, "sabund", true);
		if (sabundfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["sabund"] << '\n'; sabundfile = ""; }
		else if (sabundfile == "not found") { sabundfile = ""; }
		if (sabundfile != "") { settings.setCurrent("sabund", sabundfile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { LOG(INFO) << "Ignoring: " + parameters["name"] << '\n'; namefile = ""; }
		else if (namefile == "not found") { namefile = ""; }
		if (namefile != "") { settings.setCurrent("name", namefile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["group"] << '\n'; groupfile = ""; }
		else if (groupfile == "not found") { groupfile = ""; }
		if (groupfile != "") { settings.setCurrent("group", groupfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["count"] << '\n'; countfile = ""; }
		else if (countfile == "not found") { countfile = ""; }
		if (countfile != "") { settings.setCurrent("counttable", countfile); }

		designfile = validParameter.validFile(parameters, "design", true);
		if (designfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["design"] << '\n'; designfile = ""; }
		else if (designfile == "not found") { designfile = ""; }
		if (designfile != "") { settings.setCurrent("design", designfile); }

		orderfile = validParameter.validFile(parameters, "order", true);
		if (orderfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["order"] << '\n'; orderfile = ""; }
		else if (orderfile == "not found") { orderfile = ""; }
		if (orderfile != "") { settings.setCurrent("order", orderfile); }

		treefile = validParameter.validFile(parameters, "tree", true);
		if (treefile == "not open") { LOG(INFO) << "Ignoring: " + parameters["tree"] << '\n'; treefile = ""; }
		else if (treefile == "not found") { treefile = ""; }
		if (treefile != "") { settings.setCurrent("tree", treefile); }

		sharedfile = validParameter.validFile(parameters, "shared", true);
		if (sharedfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["shared"] << '\n'; sharedfile = ""; }
		else if (sharedfile == "not found") { sharedfile = ""; }
		if (sharedfile != "") { settings.setCurrent("shared", sharedfile); }

		ordergroupfile = validParameter.validFile(parameters, "ordergroup", true);
		if (ordergroupfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["ordergroup"] << '\n'; ordergroupfile = ""; }
		else if (ordergroupfile == "not found") { ordergroupfile = ""; }
		if (ordergroupfile != "") { settings.setCurrent("ordergroup", ordergroupfile); }

		relabundfile = validParameter.validFile(parameters, "relabund", true);
		if (relabundfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["relabund"] << '\n'; relabundfile = ""; }
		else if (relabundfile == "not found") { relabundfile = ""; }
		if (relabundfile != "") { settings.setCurrent("relabund", relabundfile); }

		fastafile = validParameter.validFile(parameters, "fasta", true);
		if (fastafile == "not open") { LOG(INFO) << "Ignoring: " + parameters["fasta"] << '\n'; fastafile = ""; }
		else if (fastafile == "not found") { fastafile = ""; }
		if (fastafile != "") { settings.setCurrent("fasta", fastafile); }

		qualfile = validParameter.validFile(parameters, "qfile", true);
		if (qualfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["qfile"] << '\n'; qualfile = ""; }
		else if (qualfile == "not found") { qualfile = ""; }
		if (qualfile != "") { settings.setCurrent("qual", qualfile); }

		sfffile = validParameter.validFile(parameters, "sff", true);
		if (sfffile == "not open") { LOG(INFO) << "Ignoring: " + parameters["sff"] << '\n'; sfffile = ""; }
		else if (sfffile == "not found") { sfffile = ""; }
		if (sfffile != "") { settings.setCurrent("sff", sfffile); }

		oligosfile = validParameter.validFile(parameters, "oligos", true);
		if (oligosfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["oligos"] << '\n'; oligosfile = ""; }
		else if (oligosfile == "not found") { oligosfile = ""; }
		if (oligosfile != "") { settings.setCurrent("oligos", oligosfile); }

		accnosfile = validParameter.validFile(parameters, "accnos", true);
		if (accnosfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["accnos"] << '\n'; accnosfile = ""; }
		else if (accnosfile == "not found") { accnosfile = ""; }
		if (accnosfile != "") { settings.setCurrent("accnos", accnosfile); }

		taxonomyfile = validParameter.validFile(parameters, "taxonomy", true);
		if (taxonomyfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["taxonomy"] << '\n'; taxonomyfile = ""; }
		else if (taxonomyfile == "not found") { taxonomyfile = ""; }
		if (taxonomyfile != "") { settings.setCurrent("taxonomy", taxonomyfile); }

		flowfile = validParameter.validFile(parameters, "flow", true);
		if (flowfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["flow"] << '\n'; flowfile = ""; }
		else if (flowfile == "not found") { flowfile = ""; }
		if (flowfile != "") { settings.setCurrent("flow", flowfile); }

		biomfile = validParameter.validFile(parameters, "biom", true);
		if (biomfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["biom"] << '\n'; biomfile = ""; }
		else if (biomfile == "not found") { biomfile = ""; }
		if (biomfile != "") { settings.setCurrent("biom", biomfile); }

		summaryfile = validParameter.validFile(parameters, "summary", true);
		if (summaryfile == "not open") { LOG(INFO) << "Ignoring: " + parameters["summary"] << '\n'; summaryfile = ""; }
		else if (summaryfile == "not found") { summaryfile = ""; }
		if (summaryfile != "") { settings.setCurrent("summary", summaryfile); }

		filefile = validParameter.validFile(parameters, "file", true);
		if (filefile == "not open") { LOG(INFO) << "Ignoring: " + parameters["file"] << '\n'; filefile = ""; }
		else if (filefile == "not found") { filefile = ""; }
		if (filefile != "") { settings.setCurrent("file", filefile); }

		string temp = validParameter.validFile(parameters, "processors", false);
		if (temp == "not found") { temp = settings.getProcessors(); }
		settings.setProcessors(temp);

		clearTypes = validParameter.validFile(parameters, "clear", false);
		if (clearTypes == "not found") { clearTypes = ""; }
		else { Utility::split(clearTypes, '-', types); }
	}

}
//**********************************************************************************************************************

int SetCurrentCommand::execute() {
	try {

		if (abort == true) { if (calledHelp) { return 0; }  return 2; }

		//user wants to clear a type
		if (types.size() != 0) {
			for (int i = 0; i < types.size(); i++) {

				if (ctrlc_pressed) { break; }

				//look for file types
				if (types[i] == "fasta") {
					settings.setCurrent("fasta", "");
				}
				else if (types[i] == "qfile") {
					settings.setCurrent("qual", "");
				}
				else if (types[i] == "phylip") {
					settings.setCurrent("phylip", "");
				}
				else if (types[i] == "column") {
					settings.setCurrent("column", "");
				}
				else if (types[i] == "list") {
					settings.setCurrent("list", "");
				}
				else if (types[i] == "rabund") {
					settings.setCurrent("rabund", "");
				}
				else if (types[i] == "sabund") {
					settings.setCurrent("sabund", "");
				}
				else if (types[i] == "name") {
					settings.setCurrent("name", "");
				}
				else if (types[i] == "group") {
					settings.setCurrent("group", "");
				}
				else if (types[i] == "order") {
					settings.setCurrent("order", "");
				}
				else if (types[i] == "ordergroup") {
					settings.setCurrent("ordergroup", "");
				}
				else if (types[i] == "tree") {
					settings.setCurrent("tree", "");
				}
				else if (types[i] == "shared") {
					settings.setCurrent("shared", "");
				}
				else if (types[i] == "relabund") {
					settings.setCurrent("relabund", "");
				}
				else if (types[i] == "design") {
					settings.setCurrent("design", "");
				}
				else if (types[i] == "sff") {
					settings.setCurrent("sff", "");
				}
				else if (types[i] == "oligos") {
					settings.setCurrent("oligos", "");
				}
				else if (types[i] == "accnos") {
					settings.setCurrent("accnos", "");
				}
				else if (types[i] == "taxonomy") {
					settings.setCurrent("taxonomy", "");
				}
				else if (types[i] == "flow") {
					settings.setCurrent("flow", "");
				}
				else if (types[i] == "biom") {
					settings.setCurrent("biom", "");
				}
				else if (types[i] == "count") {
					settings.setCurrent("counttable", "");
				}
				else if (types[i] == "summary") {
					settings.setCurrent("summary", "");
				}
				else if (types[i] == "file") {
					settings.setCurrent("file", "");
				}
				else if (types[i] == "processors") {
					settings.setProcessors("1");
				}
				else if (types[i] == "all") {
					m->clearCurrentFiles();
				}
				else {
					LOG(INFO) << "[ERROR]: mothur does not save a current file for " + types[i] << '\n';
				}
			}
		}

		LOG(INFO) << '\n' << "Current files saved by mothur:" << '\n';
		m->printCurrentFiles();

		return 0;
	}

	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SetCurrentCommand, execute";
		exit(1);
	}
}

//**********************************************************************************************************************



