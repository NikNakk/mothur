/*
 *  clearcutcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/11/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "clearcutcommand.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "clearcut.h"
#ifdef __cplusplus
}
#endif

#include <memory>
#include "utility.h"

//**********************************************************************************************************************
vector<string> ClearcutCommand::setParameters(){	
	try {
		CommandParameter pphylip("phylip", "InputTypes", "", "", "FastaPhylip", "FastaPhylip", "none","tree",false,false,true); parameters.push_back(pphylip);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "FastaPhylip", "FastaPhylip", "none","tree",false,false,true); parameters.push_back(pfasta);
		CommandParameter pverbose("verbose", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pverbose);
		CommandParameter pquiet("quiet", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pquiet);
		CommandParameter pversion("version", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pversion);
		CommandParameter prseed("rseed", "String", "", "", "*", "", "","",false,false); parameters.push_back(prseed);
		CommandParameter pnorandom("norandom", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pnorandom);
		CommandParameter pshuffle("shuffle", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pshuffle);
		CommandParameter pneighbor("neighbor", "Boolean", "", "T", "", "", "","",false,false); parameters.push_back(pneighbor);
		CommandParameter pexpblen("expblen", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pexpblen);
		CommandParameter pexpdist("expdist", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pexpdist);
		CommandParameter pDNA("DNA", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pDNA);
		CommandParameter pprotein("protein", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pprotein);
		CommandParameter pjukes("jukes", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pjukes);
		CommandParameter pkimura("kimura", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pkimura);
		CommandParameter pstdout("stdout", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pstdout);
		CommandParameter pntrees("ntrees", "Number", "", "1", "", "", "","",false,false); parameters.push_back(pntrees);
		CommandParameter pmatrixout("matrixout", "String", "", "", "", "", "","",false,false); parameters.push_back(pmatrixout);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ClearcutCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string ClearcutCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The clearcut command interfaces mothur with the clearcut program written by Initiative for Bioinformatics and Evolutionary Studies (IBEST) at the University of Idaho.\n";
		helpString += "For more information about clearcut refer to http://bioinformatics.hungry.com/clearcut/ \n";
		helpString += "The clearcut command parameters are phylip, fasta, version, verbose, quiet, seed, norandom, shuffle, neighbor, expblen, expdist, ntrees, matrixout, stdout, kimura, jukes, protein, DNA. \n";
		helpString += "The phylip parameter allows you to enter your phylip formatted distance matrix. \n";
		helpString += "The fasta parameter allows you to enter your aligned fasta file, if you enter a fastafile you specify if the sequences are DNA or protein using the DNA or protein parameters. \n";
		
		helpString += "The version parameter prints out the version of clearcut you are using, default=F. \n";
		helpString += "The verbose parameter prints out more output from clearcut, default=F. \n";
		helpString += "The quiet parameter turns on silent operation mode, default=F. \n";
		helpString += "The rseed parameter allows you to explicitly set the PRNG seed to a specific value. \n";
		helpString += "The norandom parameter allows you to attempt joins deterministically, default=F. \n";
		helpString += "The shuffle parameter allows you to randomly shuffle the distance matrix, default=F. \n";
		helpString += "The neighbor parameter allows you to use traditional Neighbor-Joining algorithm, default=T. \n";
		
		helpString += "The DNA parameter allows you to indicate your fasta file contains DNA sequences, default=F. \n";
		helpString += "The protein parameter allows you to indicate your fasta file contains protein sequences, default=F. \n";
		
		helpString += "The stdout parameter outputs your tree to STDOUT, default=F. \n";
		helpString += "The matrixout parameter allows you to specify a filename to output a distance matrix to. \n";
		helpString += "The ntrees parameter allows you to specify the number of output trees, default=1. \n";
		helpString += "The expblen parameter allows you to use exponential notation for branch lengths, default=F. \n";
		helpString += "The expdist parameter allows you to use exponential notation for distance outputs, default=F. \n";
		
		helpString += "The clearcut command should be in the following format: \n";
		helpString += "clearcut(phylip=yourDistanceFile) \n";
		helpString += "Example: clearcut(phylip=abrecovery.phylip.dist) \n";	
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "ClearcutCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string ClearcutCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "matrixout") {  pattern = "[filename],"; } 
        else if (type == "tree") {  pattern = "[filename],tre"; } 
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->control_pressed = true;  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "ClearcutCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
ClearcutCommand::ClearcutCommand(){	
	try {
		abort = true; calledHelp = true; 
		setParameters();
		vector<string> tempOutNames;
		outputTypes["tree"] = tempOutNames;
		outputTypes["matrixout"] = tempOutNames;
	}
	catch(exception& e) {
		m->errorOut(e, "ClearcutCommand", "ClearcutCommand");
		exit(1);
	}
}
/**************************************************************************************/
ClearcutCommand::ClearcutCommand(string option)  {	
	try {
		abort = false; calledHelp = false;   
		
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
		
		else {
			vector<string> myArray = setParameters();
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			map<string, string>::iterator it;
		
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["tree"] = tempOutNames;
			outputTypes["matrixout"] = tempOutNames;

			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;
				it = parameters.find("fasta");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["fasta"] = inputDir + it->second;		}
				}
				
				it = parameters.find("phylip");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["phylip"] = inputDir + it->second;		}
				}
			}

			//check for required parameters
			fastafile = validParameter.validFile(parameters, "fasta", true);
			if (fastafile == "not open") { fastafile = ""; abort = true; }
			else if (fastafile == "not found") { fastafile = ""; }	
			else { inputFile = fastafile;  m->setFastaFile(fastafile); }
			
			phylipfile = validParameter.validFile(parameters, "phylip", true);
			if (phylipfile == "not open") { phylipfile = ""; abort = true; }
			else if (phylipfile == "not found") { phylipfile = ""; }
			else { inputFile = phylipfile;  m->setPhylipFile(phylipfile); }
				
			if ((phylipfile == "") && (fastafile == "")) {  
				//is there are current file available for either of these?
				//give priority to phylip, then fasta
				phylipfile = m->getPhylipFile(); 
				if (phylipfile != "") {  inputFile = phylipfile; m->mothurOut("Using " + phylipfile + " as input file for the phylip parameter."); m->mothurOutEndLine(); }
				else { 
					fastafile = m->getFastaFile(); 
					if (fastafile != "") { inputFile = fastafile;  m->mothurOut("Using " + fastafile + " as input file for the fasta parameter."); m->mothurOutEndLine(); }
					else { 
						m->mothurOut("No valid current files. You must provide a phylip or fasta file before you can use the clearcut command."); m->mothurOutEndLine(); 
						abort = true;
					}
				}
			}
			if ((phylipfile != "") && (fastafile != "")) {  m->mothurOut("You must provide either a phylip formatted distance matrix or an aligned fasta file, not BOTH."); m->mothurOutEndLine(); abort=true; }

			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	outputDir = m->hasPath(inputFile);	}
			
			string temp;
			temp = validParameter.validFile(parameters, "version", false);		if (temp == "not found"){	temp = "F";			}
			version = m->isTrue(temp);
			
			temp = validParameter.validFile(parameters, "verbose", false);		if (temp == "not found"){	temp = "F";			}
			verbose = m->isTrue(temp); 
			
			temp = validParameter.validFile(parameters, "quiet", false);		if (temp == "not found"){	temp = "F";			}
			quiet = m->isTrue(temp); 
			
			seed = validParameter.validFile(parameters, "rseed", false);			if (seed == "not found"){	seed = "*";			}
			
			temp = validParameter.validFile(parameters, "norandom", false);		if (temp == "not found"){	temp = "F";			}
			norandom = m->isTrue(temp); 
			
			temp = validParameter.validFile(parameters, "shuffle", false);		if (temp == "not found"){	temp = "F";			}
			shuffle = m->isTrue(temp); 
			
			temp = validParameter.validFile(parameters, "neighbor", false);		if (temp == "not found"){	temp = "T";			}
			neighbor = m->isTrue(temp); 
			
			temp = validParameter.validFile(parameters, "DNA", false);			if (temp == "not found"){	temp = "F";			}
			DNA = m->isTrue(temp);
			
			temp = validParameter.validFile(parameters, "protein", false);		if (temp == "not found"){	temp = "F";			}
			protein = m->isTrue(temp);
			
			temp = validParameter.validFile(parameters, "jukes", false);		if (temp == "not found"){	temp = "F";			}
			jukes = m->isTrue(temp);
			
			temp = validParameter.validFile(parameters, "kimura", false);		if (temp == "not found"){	temp = "F";			}
			kimura = m->isTrue(temp);
			
			temp = validParameter.validFile(parameters, "stdout", false);		if (temp == "not found"){	temp = "F";			}
			stdoutWanted = m->isTrue(temp); 
			
			matrixout = validParameter.validFile(parameters, "matrixout", false);	if (matrixout == "not found"){	matrixout = "";		}
			
			ntrees = validParameter.validFile(parameters, "ntrees", false);		if (ntrees == "not found"){	ntrees = "1";		}
			
			temp = validParameter.validFile(parameters, "expblen", false);		if (temp == "not found"){	temp = "F";			}
			expblen = m->isTrue(temp);
			
			temp = validParameter.validFile(parameters, "expdist", false);		if (temp == "not found"){	temp = "F";			}
			expdist = m->isTrue(temp);
			
			if ((fastafile != "") && ((!DNA) && (!protein))) { m->mothurOut("You must specify the type of sequences you are using: DNA or protein"); m->mothurOutEndLine(); abort=true; }
		}

	}
	catch(exception& e) {
		m->errorOut(e, "ClearcutCommand", "ClearcutCommand");
		exit(1);
	}
}
/**************************************************************************************/
int ClearcutCommand::execute() {	
	try {
		
		if (abort == true) { if (calledHelp) { return 0; }  return 2;	}
		
		//prepare filename
        map<string, string> variables; 
        variables["[filename]"] = outputDir + m->getRootName(m->getSimpleName(inputFile));
		string outputName = getOutputFileName("tree", variables);
		outputNames.push_back(outputName); outputTypes["tree"].push_back(outputName);
		
        int numArgs = 4; //clearcut, in, out and fastafile or phylipfile
        if (version) { numArgs++; } if (verbose) { numArgs++; } if (quiet) { numArgs++; } if (seed != "*")	{ numArgs++; } if (norandom) { numArgs++; }
        if (shuffle) { numArgs++; } if (neighbor) { numArgs++; } if (stdoutWanted) { numArgs++; } if (DNA)	{ numArgs++; } if (protein) { numArgs++; }
        if (jukes) { numArgs++; } if (kimura) { numArgs++; } if (matrixout != "") { numArgs++; } if (ntrees != "1")	{ numArgs++; } if (expblen) { numArgs++; } if (expdist) { numArgs++; }
        
		vector<string> cPara;
		cPara.push_back("clearcut");
				
		//you gave us a distance matrix
		if (phylipfile != "") { cPara.push_back("--distance"); }
		
		//you gave us a fastafile
		if (fastafile != "") { cPara.push_back("--alignment"); }
		
        int parameterCount = 2;
		if (version) { cPara.push_back("--version"); }
		if (verbose) { cPara.push_back("--verbose"); }
		if (quiet) { cPara.push_back("--quiet"); }
		if (seed != "*") { cPara.push_back("--seed=" + seed); }
		if (norandom) { cPara.push_back("--norandom"); }
		if (shuffle) { cPara.push_back("--shuffle"); }
		if (neighbor) { cPara.push_back("--neighbor"); }
		
		cPara.push_back("--in=" + inputFile);  
		
		if (stdoutWanted)		{ cPara.push_back("--stdout"); }
		else{  
			cPara.push_back("--out=" + outputName);
		}
			
		if (DNA) { cPara.push_back("--DNA"); }
		if (protein) { cPara.push_back("--protein"); }
		if (jukes) { cPara.push_back("--jukes"); }
		if (kimura) { cPara.push_back("--kimura"); }
		if (matrixout != "")	{  
			cPara.push_back("--matrixout=" + matrixout);
			outputNames.push_back((outputDir + matrixout));
			outputTypes["matrixout"].push_back((outputDir + matrixout));
		}

		if (ntrees != "1")		{  
			cPara.push_back("--ntrees=" + ntrees);
		}

		if (expblen) { cPara.push_back("--expblen"); }
		if (expdist) { cPara.push_back("--expdist"); }
        
        errno = 0;
		vector<char*> cParaCStr;
		Utility::to_c_strs(cPara, cParaCStr);

		clearcut_main(cPara.size(), &cParaCStr[0]); 
		
		if (!stdoutWanted) {	
			
			//set first tree file as new current treefile
			string currentTree = "";
			itTypes = outputTypes.find("tree");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { currentTree = (itTypes->second)[0]; m->setTreeFile(currentTree); }
			}
			
			m->mothurOutEndLine();
			m->mothurOut("Output File Names: "); m->mothurOutEndLine();
			for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
			m->mothurOutEndLine();
		}

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ClearcutCommand", "execute");
		exit(1);
	}
}
/**************************************************************************************/




