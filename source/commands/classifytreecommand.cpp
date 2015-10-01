//
//  classifytreecommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 2/20/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "classifytreecommand.h"
#include "phylotree.h"
#include "treereader.h"

//**********************************************************************************************************************
vector<string> ClassifyTreeCommand::setParameters() {
	try {
		CommandParameter ptree("tree", "InputTypes", "", "", "", "", "none", "tree-summary", false, true, true); parameters.push_back(ptree);
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "", "", "none", "", false, true, true); parameters.push_back(ptaxonomy);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "none", "", false, false, true); parameters.push_back(pname);
		CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "none", "none", "", false, false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none", "", false, false, true); parameters.push_back(pgroup);
		CommandParameter pmethod("output", "Multiple", "node-taxon", "node", "", "", "", "", false, false); parameters.push_back(pmethod);
		nkParameters.add(new NumberParameter("cutoff", -INFINITY, INFINITY, 51, false, true));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifyTreeCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClassifyTreeCommand::getHelpString() {
	try {
		string helpString = "The classify.tree command reads a tree and taxonomy file and output the consensus taxonomy for each node on the tree. \n"
			"If you provide a group file, the concensus for each group will also be provided. \n"
			"The new tree contains labels at each internal node.  The label is the node number so you can relate the tree to the summary file.\n"
			"The count parameter allows you add a count file so you can have the summary totals broken up by group.\n"
			"The summary file lists the concensus taxonomy for the descendants of each node.\n"
			"The classify.tree command parameters are tree, group, name, count and taxonomy. The tree and taxonomy files are required.\n"
			"The cutoff parameter allows you to specify a consensus confidence threshold for your taxonomy.  The default is 51, meaning 51%. Cutoff cannot be below 51.\n"
			"The output parameter allows you to specify whether you want the tree node number displayed on the tree, or the taxonomy displayed. Default=node. Options are node or taxon.\n"
			"The classify.tree command should be used in the following format: classify.tree(tree=test.tre, group=test.group, taxonomy=test.taxonomy)\n"
			"Note: No spaces between parameter labels (i.e. tree), '=' and parameters (i.e.yourTreefile).\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifyTreeCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ClassifyTreeCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "summary") { pattern = "[filename],taxonomy.summary"; } //makes file like: amazon.0.03.fasta
	else if (type == "tree") { pattern = "[filename],taxonomy.tre"; }
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ClassifyTreeCommand::ClassifyTreeCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["tree"] = tempOutNames;
		outputTypes["summary"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifyTreeCommand, ClassifyTreeCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
ClassifyTreeCommand::ClassifyTreeCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false;

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

		vector<string> tempOutNames;
		outputTypes["tree"] = tempOutNames;
		outputTypes["summary"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		string inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("tree");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["tree"] = inputDir + it->second; }
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

			it = parameters.find("taxonomy");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["taxonomy"] = inputDir + it->second; }
			}

			it = parameters.find("count");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["count"] = inputDir + it->second; }
			}
		}

		outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found") { outputDir = ""; }

		//check for required parameters
		treefile = validParameter.validFile(parameters, "tree", true);
		if (treefile == "not open") { treefile = ""; abort = true; }
		else if (treefile == "not found") {
			treefile = "";
			treefile = settings.getCurrent("tree");
			if (treefile != "") { LOG(INFO) << "Using " + treefile + " as input file for the tree parameter." << '\n'; }
			else { LOG(INFO) << "No valid current files. You must provide a tree file." << '\n'; abort = true; }
		}
		else { settings.setCurrent("tree", treefile); }

		taxonomyfile = validParameter.validFile(parameters, "taxonomy", true);
		if (taxonomyfile == "not open") { taxonomyfile = ""; abort = true; }
		else if (taxonomyfile == "not found") {
			taxonomyfile = "";
			taxonomyfile = settings.getCurrent("taxonomy");
			if (taxonomyfile != "") { LOG(INFO) << "Using " + taxonomyfile + " as input file for the taxonomy parameter." << '\n'; }
			else { LOG(INFO) << "No valid current files. You must provide a taxonomy file." << '\n'; abort = true; }
		}
		else { settings.setCurrent("taxonomy", taxonomyfile); }

		namefile = validParameter.validFile(parameters, "name", true);
		if (namefile == "not open") { namefile = ""; abort = true; }
		else if (namefile == "not found") { namefile = ""; }
		else { settings.setCurrent("name", namefile); }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not open") { groupfile = ""; abort = true; }
		else if (groupfile == "not found") { groupfile = ""; }
		else { settings.setCurrent("group", groupfile); }

		countfile = validParameter.validFile(parameters, "count", true);
		if (countfile == "not open") { countfile = ""; abort = true; }
		else if (countfile == "not found") { countfile = ""; }
		else { settings.setCurrent("counttable", countfile); }

		if ((namefile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: name or count." << '\n'; abort = true;
		}

		if ((groupfile != "") && (countfile != "")) {
			LOG(LOGERROR) << "you may only use one of the following: group or count." << '\n'; abort = true;
		}

		string temp = validParameter.validFile(parameters, "cutoff", false);			if (temp == "not found") { temp = "51"; }
		Utility::mothurConvert(temp, cutoff);

		if ((cutoff < 51) || (cutoff > 100)) { LOG(INFO) << "cutoff must be above 50, and no greater than 100." << '\n'; abort = true; }

		output = validParameter.validFile(parameters, "output", false);
		if (output == "not found") { output = "node"; }

		if ((output == "node") || (output == "taxon")) {
		}
		else { LOG(LOGERROR) << "" + output + "is not a valid output option.  Valid output options are node or taxon.\n";  abort = true; }

		if (countfile == "") {
			if (namefile == "") {
				vector<string> files; files.push_back(treefile);
				OptionParser::getNameFile(files);
			}
		}
	}
}
//**********************************************************************************************************************

int ClassifyTreeCommand::execute() {

	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	cout.setf(ios::fixed, ios::floatfield); cout.setf(ios::showpoint);

	int start = time(NULL);

	/***************************************************/
	//    reading tree info							   //
	/***************************************************/
	settings.setCurrent("tree", treefile);

	TreeReader* reader = new TreeReader(treefile, groupfile, namefile);
	vector<Tree*> T = reader->getTrees();
	CountTable* tmap = T[0]->getCountTable();
	Tree* outputTree = T[0];
	delete reader;

	if (namefile != "") { m->readNames(namefile, nameMap, nameCount); }

	if (ctrlc_pressed) { delete tmap;  delete outputTree;  return 0; }

	m->readTax(taxonomyfile, taxMap, true);

	/***************************************************/
	//		get concensus taxonomies                    //
	/***************************************************/
	getClassifications(outputTree);
	delete outputTree; delete tmap;

	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); } return 0; }

	//set tree file as new current treefile
	if (treefile != "") {
		string current = "";
		itTypes = outputTypes.find("tree");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("tree", current); }
		}
	}

	LOG(INFO) << std::endl << '\n' << "It took " + toString(time(NULL) - start) + " secs to find the concensus taxonomies." << '\n';
	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
//**********************************************************************************************************************
//traverse tree finding concensus taxonomy at each node
//label node with a number to relate to output summary file
//report all concensus taxonomies to file 
int ClassifyTreeCommand::getClassifications(Tree*& T) {

	string thisOutputDir = outputDir;
	if (outputDir == "") { thisOutputDir += File::getPath(treefile); }
	map<string, string> variables;
	variables["[filename]"] = thisOutputDir + File::getRootName(File::getSimpleName(treefile));
	string outputFileName = getOutputFileName("summary", variables);
	outputNames.push_back(outputFileName); outputTypes["summary"].push_back(outputFileName);

	ofstream out;
	File::openOutputFile(outputFileName, out);
	out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);

	//print headings
	out << "TreeNode\t";
	if (groupfile != "") { out << "Group\t"; }
	out << "NumRep\tTaxonomy" << endl;

	string treeOutputDir = outputDir;
	if (outputDir == "") { treeOutputDir += File::getPath(treefile); }
	variables["[filename]"] = treeOutputDir + File::getRootName(File::getSimpleName(treefile));
	string outputTreeFileName = getOutputFileName("tree", variables);

	//create a map from tree node index to names of descendants, save time later
	map<int, map<string, set<string> > > nodeToDescendants; //node# -> (groupName -> groupMembers)
	for (int i = 0; i < T->getNumNodes(); i++) {
		if (ctrlc_pressed) { return 0; }

		nodeToDescendants[i] = getDescendantList(T, i, nodeToDescendants);
	}

	//for each node
	for (int i = T->getNumLeaves(); i < T->getNumNodes(); i++) {

		if (ctrlc_pressed) { out.close(); return 0; }

		string tax = "not classifed";
		int size;
		if (groupfile != "") {
			for (map<string, set<string> >::iterator itGroups = nodeToDescendants[i].begin(); itGroups != nodeToDescendants[i].end(); itGroups++) {
				if (itGroups->first != "AllGroups") {
					tax = getTaxonomy(itGroups->second, size);
					out << (i + 1) << '\t' << itGroups->first << '\t' << size << '\t' << tax << endl;
				}
			}
		}
		else {
			string group = "AllGroups";
			tax = getTaxonomy(nodeToDescendants[i][group], size);
			out << (i + 1) << '\t' << size << '\t' << tax << endl;
		}

		if (output == "node") { T->tree[i].setLabel(toString(i + 1)); }
		else {
			string cleanedTax = tax;
			m->removeConfidences(cleanedTax);
			for (int j = 0; j < cleanedTax.length(); j++) {
				//special chars to trees -  , ) ( ; [ ] :
				if ((cleanedTax[j] == ',') || (cleanedTax[j] == '(') || (cleanedTax[j] == ')') || (cleanedTax[j] == ';') || (cleanedTax[j] == ':') || (cleanedTax[j] == ']') || (cleanedTax[j] == '[')) {
					cleanedTax[j] = '_'; //change any special chars to _ so the tree can be read by tree readers
				}
			}
			cout << tax << '\t' << cleanedTax << endl;
			T->tree[i].setLabel(cleanedTax);
		}

	}
	out.close();

	ofstream outTree;
	File::openOutputFile(outputTreeFileName, outTree);
	outputNames.push_back(outputTreeFileName); outputTypes["tree"].push_back(outputTreeFileName);
	T->print(outTree, "both");
	outTree.close();

	return 0;
}
//**********************************************************************************************************************
string ClassifyTreeCommand::getTaxonomy(set<string> names, int& size) {
	try {
		string conTax = "";
		size = 0;

		//create a tree containing sequences from this bin
		PhyloTree* phylo = new PhyloTree();

		for (set<string>::iterator it = names.begin(); it != names.end(); it++) {

			//if namesfile include the names
			if (namefile != "") {

				//is this sequence in the name file - namemap maps seqName -> repSeqName
				map<string, string>::iterator it2 = nameMap.find(*it);

				if (it2 == nameMap.end()) { //this name is not in name file, skip it
					LOG(INFO) << (*it) + " is not in your name file.  I will not include it in the consensus." << '\n';
				}
				else {

					//is this sequence in the taxonomy file - look for repSeqName since we are assuming the taxonomy file is unique
					map<string, string>::iterator itTax = taxMap.find((it2->second));

					if (itTax == taxMap.end()) { //this name is not in taxonomy file, skip it

						if ((*it) != (it2->second)) { LOG(INFO) << (*it) + " is represented by " + it2->second + " and is not in your taxonomy file.  I will not include it in the consensus." << '\n'; }
						else { LOG(INFO) << (*it) + " is not in your taxonomy file.  I will not include it in the consensus." << '\n'; }
					}
					else {
						//add seq to tree
						int num = nameCount[(*it)]; // we know its there since we found it in nameMap
						for (int i = 0; i < num; i++) { phylo->addSeqToTree((*it) + toString(i), itTax->second); }
						size += num;
					}
				}

			}
			else {
				//is this sequence in the taxonomy file - look for repSeqName since we are assuming the taxonomy file is unique
				map<string, string>::iterator itTax = taxMap.find((*it));

				if (itTax == taxMap.end()) { //this name is not in taxonomy file, skip it
					LOG(INFO) << (*it) + " is not in your taxonomy file.  I will not include it in the consensus." << '\n';
				}
				else {
					if (countfile != "") {
						int numDups = ct->getNumSeqs((*it));
						for (int j = 0; j < numDups; j++) { phylo->addSeqToTree((*it), itTax->second); }
						size += numDups;
					}
					else {
						//add seq to tree
						phylo->addSeqToTree((*it), itTax->second);
						size++;
					}
				}
			}

			if (ctrlc_pressed) { delete phylo; return conTax; }

		}

		//build tree
		phylo->assignHeirarchyIDs(0);

		TaxNode currentNode = phylo->get(0);
		int myLevel = 0;
		//at each level
		while (currentNode.children.size() != 0) { //you still have more to explore

			TaxNode bestChild;
			int bestChildSize = 0;

			//go through children
			for (map<string, int>::iterator itChild = currentNode.children.begin(); itChild != currentNode.children.end(); itChild++) {

				TaxNode temp = phylo->get(itChild->second);

				//select child with largest accesions - most seqs assigned to it
				if (temp.accessions.size() > bestChildSize) {
					bestChild = phylo->get(itChild->second);
					bestChildSize = temp.accessions.size();
				}

			}

			//is this taxonomy above cutoff
			int consensusConfidence = ceil((bestChildSize / (float)size) * 100);

			if (consensusConfidence >= cutoff) { //if yes, add it
				conTax += bestChild.name + "(" + toString(consensusConfidence) + ");";
				myLevel++;
			}
			else { //if no, quit
				break;
			}

			//move down a level
			currentNode = bestChild;
		}

		if (myLevel != phylo->getMaxLevel()) {
			while (myLevel != phylo->getMaxLevel()) {
				conTax += "unclassified;";
				myLevel++;
			}
		}
		if (conTax == "") { conTax = "no_consensus;"; }

		delete phylo;

		return conTax;

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ClassifyTreeCommand, getTaxonomy";
		exit(1);
	}
}

//**********************************************************************************************************************
map<string, set<string> > ClassifyTreeCommand::getDescendantList(Tree*& T, int i, map<int, map<string, set<string> > > descendants) {
	map<string, set<string> > names;

	map<string, set<string> >::iterator it;
	map<string, set<string> >::iterator it2;

	int lc = T->tree[i].getLChild();
	int rc = T->tree[i].getRChild();
	// TreeMap* tmap = T->getTreeMap();

	if (lc == -1) { //you are a leaf your only descendant is yourself
		vector<string> groups = T->tree[i].getGroup();
		set<string> mynames; mynames.insert(T->tree[i].getName());
		for (int j = 0; j < groups.size(); j++) { names[groups[j]] = mynames; } //mygroup -> me
		names["AllGroups"] = mynames;
	}
	else { //your descedants are the combination of your childrens descendants
		names = descendants[lc];
		for (it = descendants[rc].begin(); it != descendants[rc].end(); it++) {
			it2 = names.find(it->first); //do we already have this group
			if (it2 == names.end()) { //nope, so add it
				names[it->first] = it->second;
			}
			else {
				for (set<string>::iterator it3 = (it->second).begin(); it3 != (it->second).end(); it3++) {
					names[it->first].insert(*it3);
				}
			}

		}
	}

	return names;
}
/*****************************************************************/


