/*
 *  unifracweightedcommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 2/9/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "unifracweightedcommand.h"

//**********************************************************************************************************************
vector<string> UnifracWeightedCommand::setParameters(){	
	try {
		CommandParameter ptree("tree", "InputTypes", "", "", "none", "none", "none",false,true); parameters.push_back(ptree);
		CommandParameter pgroup("group", "InputTypes", "", "", "none", "none", "none",false,false); parameters.push_back(pgroup);
		CommandParameter pname("name", "InputTypes", "", "", "none", "none", "none",false,false); parameters.push_back(pname);
		CommandParameter pgroups("groups", "String", "", "", "", "", "",false,false); parameters.push_back(pgroups);
		CommandParameter piters("iters", "Number", "", "1000", "", "", "",false,false); parameters.push_back(piters);
		CommandParameter pprocessors("processors", "Number", "", "1", "", "", "",false,false); parameters.push_back(pprocessors);
		CommandParameter prandom("random", "Boolean", "", "F", "", "", "",false,false); parameters.push_back(prandom);
		CommandParameter pdistance("distance", "Multiple", "column-lt-square", "column", "", "", "",false,false); parameters.push_back(pdistance);
		CommandParameter proot("root", "Boolean", "F", "", "", "", "",false,false); parameters.push_back(proot);
		CommandParameter pinputdir("inputdir", "String", "", "", "", "", "",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "",false,false); parameters.push_back(poutputdir);
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string UnifracWeightedCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The unifrac.weighted command parameters are tree, group, name, groups, iters, distance, processors, root and random.  tree parameter is required unless you have valid current tree file.\n";
		helpString += "The groups parameter allows you to specify which of the groups in your groupfile you would like analyzed.  You must enter at least 2 valid groups.\n";
		helpString += "The group names are separated by dashes.  The iters parameter allows you to specify how many random trees you would like compared to your tree.\n";
		helpString += "The distance parameter allows you to create a distance file from the results. The default is false.\n";
		helpString += "The random parameter allows you to shut off the comparison to random trees. The default is false, meaning don't compare your trees with randomly generated trees.\n";
		helpString += "The root parameter allows you to include the entire root in your calculations. The default is false, meaning stop at the root for this comparision instead of the root of the entire tree.\n";
		helpString += "The processors parameter allows you to specify the number of processors to use. The default is 1.\n";
		helpString += "The unifrac.weighted command should be in the following format: unifrac.weighted(groups=yourGroups, iters=yourIters).\n";
		helpString += "Example unifrac.weighted(groups=A-B-C, iters=500).\n";
		helpString += "The default value for groups is all the groups in your groupfile, and iters is 1000.\n";
		helpString += "The unifrac.weighted command output two files: .weighted and .wsummary their descriptions are in the manual.\n";
		helpString += "Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
UnifracWeightedCommand::UnifracWeightedCommand(){	
	try {
		abort = true; calledHelp = true; 
		setParameters();
		vector<string> tempOutNames;
		outputTypes["weighted"] = tempOutNames;
		outputTypes["wsummary"] = tempOutNames;
		outputTypes["phylip"] = tempOutNames;
		outputTypes["column"] = tempOutNames;
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "UnifracWeightedCommand");
		exit(1);
	}
}

/***********************************************************/
UnifracWeightedCommand::UnifracWeightedCommand(string option) {
	try {
		abort = false; calledHelp = false;   
			
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
		
		else {
			vector<string> myArray = setParameters();
			
			OptionParser parser(option);
			map<string,string> parameters=parser.getParameters();
			map<string,string>::iterator it;
			
			ValidParameters validParameter;
		
			//check to make sure all parameters are valid for command
			for (map<string,string>::iterator it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["weighted"] = tempOutNames;
			outputTypes["wsummary"] = tempOutNames;
			outputTypes["phylip"] = tempOutNames;
			outputTypes["column"] = tempOutNames;
			
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;
				it = parameters.find("tree");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["tree"] = inputDir + it->second;		}
				}
				
				it = parameters.find("group");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["group"] = inputDir + it->second;		}
				}
				
				it = parameters.find("name");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["name"] = inputDir + it->second;		}
				}
			}
			
			m->runParse = true;
			m->clearGroups();
			m->clearAllGroups();
			m->Treenames.clear();
			m->names.clear();
			
			//check for required parameters
			treefile = validParameter.validFile(parameters, "tree", true);
			if (treefile == "not open") { treefile = ""; abort = true; }
			else if (treefile == "not found") { 				//if there is a current design file, use it
				treefile = m->getTreeFile(); 
				if (treefile != "") { m->mothurOut("Using " + treefile + " as input file for the tree parameter."); m->mothurOutEndLine(); }
				else { 	m->mothurOut("You have no current tree file and the tree parameter is required."); m->mothurOutEndLine(); abort = true; }								
			}else { m->setTreeFile(treefile); }	
			
			//check for required parameters
			groupfile = validParameter.validFile(parameters, "group", true);
			if (groupfile == "not open") { abort = true; }
			else if (groupfile == "not found") { groupfile = ""; }
			else { m->setGroupFile(groupfile); }
			
			namefile = validParameter.validFile(parameters, "name", true);
			if (namefile == "not open") { namefile = ""; abort = true; }
			else if (namefile == "not found") { namefile = ""; }
			else { m->setNameFile(namefile); }
			
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	outputDir = "";	}
			
																	
			//check for optional parameter and set defaults
			// ...at some point should added some additional type checking...
			groups = validParameter.validFile(parameters, "groups", false);			
			if (groups == "not found") { groups = ""; }
			else { 
				m->splitAtDash(groups, Groups);
				m->setGroups(Groups);
			}
				
			itersString = validParameter.validFile(parameters, "iters", false);			if (itersString == "not found") { itersString = "1000"; }
			m->mothurConvert(itersString, iters); 
			
			string temp = validParameter.validFile(parameters, "distance", false);			
			if (temp == "not found") { phylip = false; outputForm = ""; }
			else{
				if ((temp == "lt") || (temp == "column") || (temp == "square")) {  phylip = true;  outputForm = temp; }
				else { m->mothurOut("Options for distance are: lt, square, or column. Using lt."); m->mothurOutEndLine(); phylip = true; outputForm = "lt"; }
			}
			
			temp = validParameter.validFile(parameters, "random", false);				if (temp == "not found") { temp = "F"; }
			random = m->isTrue(temp);
			
			temp = validParameter.validFile(parameters, "root", false);					if (temp == "not found") { temp = "F"; }
			includeRoot = m->isTrue(temp);
			
			temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found"){	temp = m->getProcessors();	}
			m->setProcessors(temp);
			m->mothurConvert(temp, processors);
			
			if (!random) {  iters = 0;  } //turn off random calcs
			
			if (namefile == "") {
				vector<string> files; files.push_back(treefile);
				parser.getNameFile(files);
			}
		}
		
		
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "UnifracWeightedCommand");
		exit(1);
	}
}
/***********************************************************/
int UnifracWeightedCommand::execute() {
	try {
	
		if (abort == true) { if (calledHelp) { return 0; }  return 2;	}
		
		m->setTreeFile(treefile);
		
		if (groupfile != "") {
			//read in group map info.
			tmap = new TreeMap(groupfile);
			tmap->readMap();
		}else{ //fake out by putting everyone in one group
			Tree* tree = new Tree(treefile); delete tree;  //extracts names from tree to make faked out groupmap
			tmap = new TreeMap();
			
			for (int i = 0; i < m->Treenames.size(); i++) { tmap->addSeq(m->Treenames[i], "Group1"); }
		}
		
		if (namefile != "") { readNamesFile(); }
		
		read = new ReadNewickTree(treefile);
		int readOk = read->read(tmap); 
		
		if (readOk != 0) { m->mothurOut("Read Terminated."); m->mothurOutEndLine(); delete tmap; delete read; return 0; }
		
		read->AssembleTrees();
		T = read->getTrees();
		delete read;
		
		//make sure all files match
		//if you provide a namefile we will use the numNames in the namefile as long as the number of unique match the tree names size.
		int numNamesInTree;
		if (namefile != "")  {  
			if (numUniquesInName == m->Treenames.size()) {  numNamesInTree = nameMap.size();  }
			else {   numNamesInTree = m->Treenames.size();  }
		}else {  numNamesInTree = m->Treenames.size();  }
		
		
		//output any names that are in group file but not in tree
		if (numNamesInTree < tmap->getNumSeqs()) {
			for (int i = 0; i < tmap->namesOfSeqs.size(); i++) {
				//is that name in the tree?
				int count = 0;
				for (int j = 0; j < m->Treenames.size(); j++) {
					if (tmap->namesOfSeqs[i] == m->Treenames[j]) { break; } //found it
					count++;
				}
				
				if (m->control_pressed) { 
					delete tmap; for (int i = 0; i < T.size(); i++) { delete T[i]; }
					for (int i = 0; i < outputNames.size(); i++) {	m->mothurRemove(outputNames[i]); } outputTypes.clear();
					m->clearGroups();
					return 0;
				}
				
				//then you did not find it so report it 
				if (count == m->Treenames.size()) { 
					//if it is in your namefile then don't remove
					map<string, string>::iterator it = nameMap.find(tmap->namesOfSeqs[i]);
					
					if (it == nameMap.end()) {
						m->mothurOut(tmap->namesOfSeqs[i] + " is in your groupfile and not in your tree. It will be disregarded."); m->mothurOutEndLine();
						tmap->removeSeq(tmap->namesOfSeqs[i]);
						i--; //need this because removeSeq removes name from namesOfSeqs
					}
				}
			}
		}
		
		sumFile = outputDir + m->getSimpleName(treefile) + ".wsummary";
		m->openOutputFile(sumFile, outSum);
		outputNames.push_back(sumFile);  outputTypes["wsummary"].push_back(sumFile);
			
		util = new SharedUtil();
		string s; //to make work with setgroups
		Groups = m->getGroups();
		vector<string> nameGroups = tmap->getNamesOfGroups();
		util->setGroups(Groups, nameGroups, s, numGroups, "weighted");	//sets the groups the user wants to analyze
		util->getCombos(groupComb, Groups, numComp);
		m->setGroups(Groups);
		delete util;
		
		weighted = new Weighted(tmap, includeRoot);
			
		int start = time(NULL);
		
		//get weighted for users tree
		userData.resize(numComp,0);  //data[0] = weightedscore AB, data[1] = weightedscore AC...
		randomData.resize(numComp,0); //data[0] = weightedscore AB, data[1] = weightedscore AC...
		
		if (numComp < processors) { processors = numComp; }
				
		//get weighted scores for users trees
		for (int i = 0; i < T.size(); i++) {
			
			if (m->control_pressed) { delete tmap; delete weighted;
				for (int i = 0; i < T.size(); i++) { delete T[i]; } outSum.close(); for (int i = 0; i < outputNames.size(); i++) {	m->mothurRemove(outputNames[i]);  } return 0; }

			counter = 0;
			rScores.resize(numComp);  //data[0] = weightedscore AB, data[1] = weightedscore AC...
			uScores.resize(numComp);  //data[0] = weightedscore AB, data[1] = weightedscore AC...
			
			if (random) {  
				output = new ColumnFile(outputDir + m->getSimpleName(treefile)  + toString(i+1) + ".weighted", itersString);  
				outputNames.push_back(outputDir + m->getSimpleName(treefile)  + toString(i+1) + ".weighted");
				outputTypes["weighted"].push_back(outputDir + m->getSimpleName(treefile)  + toString(i+1) + ".weighted");
			} 

			userData = weighted->getValues(T[i], processors, outputDir);  //userData[0] = weightedscore
			
			if (m->control_pressed) { delete tmap; delete weighted;
				for (int i = 0; i < T.size(); i++) { delete T[i]; } if (random) { delete output; } outSum.close(); for (int i = 0; i < outputNames.size(); i++) {	m->mothurRemove(outputNames[i]);  } return 0; }
			
			//save users score
			for (int s=0; s<numComp; s++) {
				//add users score to vector of user scores
				uScores[s].push_back(userData[s]);
				
				//save users tree score for summary file
				utreeScores.push_back(userData[s]);
			}
			
			if (random) { 
			
				//calculate number of comparisons i.e. with groups A,B,C = AB, AC, BC = 3;
				vector< vector<string> > namesOfGroupCombos;
				for (int a=0; a<numGroups; a++) { 
					for (int l = 0; l < a; l++) {	
						vector<string> groups; groups.push_back((m->getGroups())[a]); groups.push_back((m->getGroups())[l]);
						namesOfGroupCombos.push_back(groups);
					}
				}
				
				lines.clear();
				
				#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux)
					if(processors != 1){
						int numPairs = namesOfGroupCombos.size();
						int numPairsPerProcessor = numPairs / processors;
					
						for (int i = 0; i < processors; i++) {
							int startPos = i * numPairsPerProcessor;
							if(i == processors - 1){
								numPairsPerProcessor = numPairs - i * numPairsPerProcessor;
							}
							lines.push_back(linePair(startPos, numPairsPerProcessor));
						}
					}
				#endif

				
				//get scores for random trees
				for (int j = 0; j < iters; j++) {
				
					#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux)
						if(processors == 1){
							driver(T[i],  namesOfGroupCombos, 0, namesOfGroupCombos.size(),  rScores);
						}else{
							createProcesses(T[i],  namesOfGroupCombos, rScores);
						}
					#else
						driver(T[i], namesOfGroupCombos, 0, namesOfGroupCombos.size(), rScores);
					#endif
					
					if (m->control_pressed) { delete tmap; delete weighted;
						for (int i = 0; i < T.size(); i++) { delete T[i]; } delete output; outSum.close(); for (int i = 0; i < outputNames.size(); i++) {	m->mothurRemove(outputNames[i]);  } return 0; }
					
					//report progress
//					m->mothurOut("Iter: " + toString(j+1)); m->mothurOutEndLine();		
				}
				lines.clear();
			
				//find the signifigance of the score for summary file
				for (int f = 0; f < numComp; f++) {
					//sort random scores
					sort(rScores[f].begin(), rScores[f].end());
					
					//the index of the score higher than yours is returned 
					//so if you have 1000 random trees the index returned is 100 
					//then there are 900 trees with a score greater then you. 
					//giving you a signifigance of 0.900
					int index = findIndex(userData[f], f);    if (index == -1) { m->mothurOut("error in UnifracWeightedCommand"); m->mothurOutEndLine(); exit(1); } //error code
					
					//the signifigance is the number of trees with the users score or higher 
					WScoreSig.push_back((iters-index)/(float)iters);
				}
				
				//out << "Tree# " << i << endl;
				calculateFreqsCumuls();
				printWeightedFile();
				
				delete output;
			
			}
			
			//clear data
			rScores.clear();
			uScores.clear();
			validScores.clear();
		}
		
		
		if (m->control_pressed) { delete tmap; delete weighted;
			for (int i = 0; i < T.size(); i++) { delete T[i]; } outSum.close(); for (int i = 0; i < outputNames.size(); i++) {	m->mothurRemove(outputNames[i]);  } return 0;  }
		
		printWSummaryFile();
		
		if (phylip) {	createPhylipFile();		}

		//clear out users groups
		m->clearGroups();
		delete tmap; delete weighted;
		for (int i = 0; i < T.size(); i++) { delete T[i]; }
		
		
		if (m->control_pressed) { 
			for (int i = 0; i < outputNames.size(); i++) {	m->mothurRemove(outputNames[i]);  }
			return 0; 
		}
		
		m->mothurOut("It took " + toString(time(NULL) - start) + " secs to run unifrac.weighted."); m->mothurOutEndLine();
		
		//set phylip file as new current phylipfile
		string current = "";
		itTypes = outputTypes.find("phylip");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; m->setPhylipFile(current); }
		}
		
		//set column file as new current columnfile
		itTypes = outputTypes.find("column");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; m->setColumnFile(current); }
		}
		
		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
		m->mothurOutEndLine();
		
		return 0;
		
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "execute");
		exit(1);
	}
}
/**************************************************************************************************/

int UnifracWeightedCommand::createProcesses(Tree* t, vector< vector<string> > namesOfGroupCombos, vector< vector<double> >& scores) {
	try {
#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux)
		int process = 1;
		vector<int> processIDS;
		
		EstOutput results;
		
		//loop through and create all the processes you want
		while (process != processors) {
			int pid = fork();
			
			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}else if (pid == 0){
				driver(t, namesOfGroupCombos, lines[process].start, lines[process].num, scores);
			
				//pass numSeqs to parent
				ofstream out;
				string tempFile = outputDir + toString(getpid()) + ".weightedcommand.results.temp";
				m->openOutputFile(tempFile, out);
				for (int i = lines[process].start; i < (lines[process].start + lines[process].num); i++) { out << scores[i][(scores[i].size()-1)] << '\t';  } out << endl;
				out.close();
				
				exit(0);
			}else { 
				m->mothurOut("[ERROR]: unable to spawn the necessary processes."); m->mothurOutEndLine(); 
				for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); }
				exit(0);
			}
		}
		
		driver(t, namesOfGroupCombos, lines[0].start, lines[0].num, scores);
		
		//force parent to wait until all the processes are done
		for (int i=0;i<(processors-1);i++) { 
			int temp = processIDS[i];
			wait(&temp);
		}
		
		//get data created by processes
		for (int i=0;i<(processors-1);i++) { 
	
			ifstream in;
			string s = outputDir + toString(processIDS[i]) + ".weightedcommand.results.temp";
			m->openInputFile(s, in);
			
			double tempScore;
			for (int j = lines[(i+1)].start; j < (lines[(i+1)].start + lines[(i+1)].num); j++) { in >> tempScore; scores[j].push_back(tempScore); }
			in.close();
			m->mothurRemove(s);
		}
		
		return 0;
#endif		
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "createProcesses");
		exit(1);
	}
}

/**************************************************************************************************/
int UnifracWeightedCommand::driver(Tree* t, vector< vector<string> > namesOfGroupCombos, int start, int num, vector< vector<double> >& scores) { 
 try {
		Tree* randT = new Tree(tmap);

		for (int h = start; h < (start+num); h++) {
	
			if (m->control_pressed) { return 0; }
		
			//initialize weighted score
			string groupA = namesOfGroupCombos[h][0]; 
			string groupB = namesOfGroupCombos[h][1];
			
			//copy T[i]'s info.
			randT->getCopy(t);
			 
			//create a random tree with same topology as T[i], but different labels
			randT->assembleRandomUnifracTree(groupA, groupB);
			
			if (m->control_pressed) { delete randT;  return 0;  }

			//get wscore of random tree
			EstOutput randomData = weighted->getValues(randT, groupA, groupB);
		
			if (m->control_pressed) { delete randT;  return 0;  }
										
			//save scores
			scores[h].push_back(randomData[0]);
		}
	
		delete randT;
	
		return 0;

	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "driver");
		exit(1);
	}
}
/***********************************************************/
void UnifracWeightedCommand::printWeightedFile() {
	try {
		vector<double> data;
		vector<string> tags;
		tags.push_back("Score"); tags.push_back("RandFreq"); tags.push_back("RandCumul");
		
		for(int a = 0; a < numComp; a++) {
			output->initFile(groupComb[a], tags);
			//print each line
			for (map<float,float>::iterator it = validScores.begin(); it != validScores.end(); it++) { 
				data.push_back(it->first);  data.push_back(rScoreFreq[a][it->first]); data.push_back(rCumul[a][it->first]); 
				output->output(data);
				data.clear();
			} 
			output->resetFile();
		}
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "printWeightedFile");
		exit(1);
	}
}


/***********************************************************/
void UnifracWeightedCommand::printWSummaryFile() {
	try {
		//column headers
		outSum << "Tree#" << '\t' << "Groups" << '\t' << "WScore" << '\t';
		m->mothurOut("Tree#\tGroups\tWScore\t");
		if (random) { outSum << "WSig"; m->mothurOut("WSig"); }
		outSum << endl; m->mothurOutEndLine();
		
		//format output
		outSum.setf(ios::fixed, ios::floatfield); outSum.setf(ios::showpoint);
		
		//print each line
		int count = 0;
		for (int i = 0; i < T.size(); i++) { 
			for (int j = 0; j < numComp; j++) {
				if (random) {
					if (WScoreSig[count] > (1/(float)iters)) {
						outSum << setprecision(6) << i+1 << '\t' << groupComb[j] << '\t' << utreeScores[count] << '\t' << setprecision(itersString.length()) << WScoreSig[count] << endl; 
						cout << setprecision(6) << i+1 << '\t' << groupComb[j] << '\t' << utreeScores[count] << '\t' << setprecision(itersString.length()) << WScoreSig[count] << endl; 
						m->mothurOutJustToLog(toString(i+1) +"\t" + groupComb[j] +"\t" + toString(utreeScores[count]) +"\t" +  toString(WScoreSig[count]) + "\n");   
					}else{
						outSum << setprecision(6) << i+1 << '\t' << groupComb[j] << '\t' << utreeScores[count] << '\t' << setprecision(itersString.length()) << "<" << (1/float(iters)) << endl; 
						cout << setprecision(6) << i+1 << '\t' << groupComb[j] << '\t' << utreeScores[count] << '\t' << setprecision(itersString.length()) << "<" << (1/float(iters)) << endl; 
						m->mothurOutJustToLog(toString(i+1) +"\t" + groupComb[j] +"\t" + toString(utreeScores[count]) +"\t<" +  toString((1/float(iters))) + "\n");  
					}
				}else{
					outSum << setprecision(6) << i+1 << '\t' << groupComb[j] << '\t' << utreeScores[count] << endl; 
					cout << setprecision(6) << i+1 << '\t' << groupComb[j] << '\t' << utreeScores[count]  << endl; 
					m->mothurOutJustToLog(toString(i+1) +"\t" + groupComb[j] +"\t" + toString(utreeScores[count]) +"\n"); 
				}
				count++;
			}
		}
		outSum.close();
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "printWSummaryFile");
		exit(1);
	}
}
/***********************************************************/
void UnifracWeightedCommand::createPhylipFile() {
	try {
		int count = 0;
		//for each tree
		for (int i = 0; i < T.size(); i++) { 
		
			string phylipFileName;
			if ((outputForm == "lt") || (outputForm == "square")) {
				phylipFileName = outputDir + m->getSimpleName(treefile)  + toString(i+1) + ".weighted.phylip.dist";
				outputNames.push_back(phylipFileName); outputTypes["phylip"].push_back(phylipFileName); 
			}else { //column
				phylipFileName = outputDir + m->getSimpleName(treefile)  + toString(i+1) + ".weighted.column.dist";
				outputNames.push_back(phylipFileName); outputTypes["column"].push_back(phylipFileName); 
			}
			
			ofstream out;
			m->openOutputFile(phylipFileName, out);
			
			if ((outputForm == "lt") || (outputForm == "square")) {
				//output numSeqs
				out << m->getNumGroups() << endl;
			}

			//make matrix with scores in it
			vector< vector<float> > dists;	dists.resize(m->getNumGroups());
			for (int i = 0; i < m->getNumGroups(); i++) {
				dists[i].resize(m->getNumGroups(), 0.0);
			}
			
			//flip it so you can print it
			for (int r=0; r<m->getNumGroups(); r++) { 
				for (int l = 0; l < r; l++) {
					dists[r][l] = utreeScores[count];
					dists[l][r] = utreeScores[count];
					count++;
				}
			}

			//output to file
			for (int r=0; r<m->getNumGroups(); r++) { 
				//output name
				string name = (m->getGroups())[r];
				if (name.length() < 10) { //pad with spaces to make compatible
					while (name.length() < 10) {  name += " ";  }
				}
				
				if (outputForm == "lt") {
					out << name << '\t';
					
					//output distances
					for (int l = 0; l < r; l++) {	out  << dists[r][l] << '\t';  }
					out << endl;
				}else if (outputForm == "square") {
					out << name << '\t';
					
					//output distances
					for (int l = 0; l < m->getNumGroups(); l++) {	out  << dists[r][l] << '\t';  }
					out << endl;
				}else{
					//output distances
					for (int l = 0; l < r; l++) {	
						string otherName = (m->getGroups())[l];
						if (otherName.length() < 10) { //pad with spaces to make compatible
							while (otherName.length() < 10) {  otherName += " ";  }
						}
						
						out  << name << '\t' << otherName << dists[r][l] << endl;  
					}
				}
			}
			out.close();
		}
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "createPhylipFile");
		exit(1);
	}
}
/***********************************************************/
int UnifracWeightedCommand::findIndex(float score, int index) {
	try{
		for (int i = 0; i < rScores[index].size(); i++) {
			if (rScores[index][i] >= score)	{	return i;	}
		}
		return rScores[index].size();
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "findIndex");
		exit(1);
	}
}

/***********************************************************/

void UnifracWeightedCommand::calculateFreqsCumuls() {
	try {
		//clear out old tree values
		rScoreFreq.clear();
		rScoreFreq.resize(numComp);
		rCumul.clear();
		rCumul.resize(numComp);
		validScores.clear();
	
		//calculate frequency
		for (int f = 0; f < numComp; f++) {
			for (int i = 0; i < rScores[f].size(); i++) { //looks like 0,0,1,1,1,2,4,7...  you want to make a map that say rScoreFreq[0] = 2, rScoreFreq[1] = 3...
				validScores[rScores[f][i]] = rScores[f][i];
				map<float,float>::iterator it = rScoreFreq[f].find(rScores[f][i]);
				if (it != rScoreFreq[f].end()) {
					rScoreFreq[f][rScores[f][i]]++;
				}else{
					rScoreFreq[f][rScores[f][i]] = 1;
				}
			}
		}
		
		//calculate rcumul
		for(int a = 0; a < numComp; a++) {
			float rcumul = 1.0000;
			//this loop fills the cumulative maps and put 0.0000 in the score freq map to make it easier to print.
			for (map<float,float>::iterator it = validScores.begin(); it != validScores.end(); it++) {
				//make rscoreFreq map and rCumul
				map<float,float>::iterator it2 = rScoreFreq[a].find(it->first);
				rCumul[a][it->first] = rcumul;
				//get percentage of random trees with that info
				if (it2 != rScoreFreq[a].end()) {  rScoreFreq[a][it->first] /= iters; rcumul-= it2->second;  }
				else { rScoreFreq[a][it->first] = 0.0000; } //no random trees with that score
			}
		}

	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "calculateFreqsCums");
		exit(1);
	}
}
/*****************************************************************/
int UnifracWeightedCommand::readNamesFile() {
	try {
		m->names.clear();
		numUniquesInName = 0;
		
		ifstream in;
		m->openInputFile(namefile, in);
		
		string first, second;
		map<string, string>::iterator itNames;
		
		while(!in.eof()) {
			in >> first >> second; m->gobble(in);
			
			numUniquesInName++;
			
			itNames = m->names.find(first);
			if (itNames == m->names.end()) {  
				m->names[first] = second; 
				
				//we need a list of names in your namefile to use above when removing extra seqs above so we don't remove them
				vector<string> dupNames;
				m->splitAtComma(second, dupNames);
				
				for (int i = 0; i < dupNames.size(); i++) {	
					nameMap[dupNames[i]] = dupNames[i]; 
					if ((groupfile == "") && (i != 0)) { tmap->addSeq(dupNames[i], "Group1"); } 
				}
			}else {  m->mothurOut(first + " has already been seen in namefile, disregarding names file."); m->mothurOutEndLine(); in.close(); m->names.clear(); namefile = ""; return 1; }			
		}
		in.close();
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "UnifracWeightedCommand", "readNamesFile");
		exit(1);
	}
}
/***********************************************************/





