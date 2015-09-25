/*
 *  aligncommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 5/15/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 *	This version of nast does everything I think that the greengenes nast server does and then some.  I have added the 
 *	feature of allowing users to define their database, kmer size for searching, alignment penalty values and alignment 
 *	method.  This latter feature is perhaps most significant.  nastPlus enables a user to use either a Needleman-Wunsch 
 *	(non-affine gap penalty) or Gotoh (affine gap penalty) pairwise alignment algorithm.  This is significant because it
 *	allows for a global alignment and not the local alignment provided by bLAst.  Furthermore, it has the potential to
 *	provide a better alignment because of the banding method employed by blast (I'm not sure about this).
 *
 */

#include "aligncommand.h"
#include "referencedb.h"
#include <thread>

//**********************************************************************************************************************
vector<string> AlignCommand::setParameters(){	
	try {
		CommandParameter ptemplate("reference", "InputTypes", "", "", "none", "none", "none","",false,true,true); parameters.push_back(ptemplate);
		CommandParameter pcandidate("fasta", "InputTypes", "", "", "none", "none", "none","fasta-alignreport-accnos",false,true,true); parameters.push_back(pcandidate);
		CommandParameter psearch("search", "Multiple", "kmer-blast-suffix", "kmer", "", "", "","",false,false,true); parameters.push_back(psearch);
		CommandParameter pksize("ksize", "Number", "", "8", "", "", "","",false,false); parameters.push_back(pksize);
		CommandParameter pmatch("match", "Number", "", "1.0", "", "", "","",false,false); parameters.push_back(pmatch);
		CommandParameter palign("align", "Multiple", "needleman-gotoh-blast-noalign", "needleman", "", "", "","",false,false,true); parameters.push_back(palign);
		CommandParameter pmismatch("mismatch", "Number", "", "-1.0", "", "", "","",false,false); parameters.push_back(pmismatch);
		CommandParameter pgapopen("gapopen", "Number", "", "-5.0", "", "", "","",false,false); parameters.push_back(pgapopen);
		CommandParameter pgapextend("gapextend", "Number", "", "-2.0", "", "", "","",false,false); parameters.push_back(pgapextend);
		CommandParameter pprocessors("processors", "Number", "", "1", "", "", "","",false,false,true); parameters.push_back(pprocessors);
		CommandParameter pflip("flip", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pflip);
		CommandParameter psave("save", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(psave);
		CommandParameter pthreshold("threshold", "Number", "", "0.50", "", "", "","",false,false); parameters.push_back(pthreshold);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "AlignCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string AlignCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The align.seqs command reads a file containing sequences and creates an alignment file and a report file.";
		helpString += "The align.seqs command parameters are reference, fasta, search, ksize, align, match, mismatch, gapopen, gapextend and processors.";
		helpString += "The reference and fasta parameters are required. You may leave fasta blank if you have a valid fasta file. You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amzon.fasta.";
		helpString += "The search parameter allows you to specify the method to find most similar template.  Your options are: suffix, kmer and blast. The default is kmer.";
		helpString += "The align parameter allows you to specify the alignment method to use.  Your options are: gotoh, needleman, blast and noalign. The default is needleman.";
		helpString += "The ksize parameter allows you to specify the kmer size for finding most similar template to candidate.  The default is 8.";
		helpString += "The match parameter allows you to specify the bonus for having the same base. The default is 1.0.";
		helpString += "The mistmatch parameter allows you to specify the penalty for having different bases.  The default is -1.0.";
		helpString += "The gapopen parameter allows you to specify the penalty for opening a gap in an alignment. The default is -5.0.";
		helpString += "The gapextend parameter allows you to specify the penalty for extending a gap in an alignment.  The default is -2.0.";
		helpString += "The flip parameter is used to specify whether or not you want mothur to try the reverse complement if a sequence falls below the threshold.  The default is false.";
		helpString += "The threshold is used to specify a cutoff at which an alignment is deemed 'bad' and the reverse complement may be tried. The default threshold is 0.50, meaning 50% of the bases are removed in the alignment.";
		helpString += "If the flip parameter is set to true the reverse complement of the sequence is aligned and the better alignment is reported.";
		helpString += "If the save parameter is set to true the reference sequences will be saved in memory, to clear them later you can use the clear.memory command. Default=f.";
		helpString += "The default for the threshold parameter is 0.50, meaning at least 50% of the bases must remain or the sequence is reported as potentially reversed.";
		helpString += "The align.seqs command should be in the following format:";
		helpString += "align.seqs(reference=yourTemplateFile, fasta=yourCandidateFile, align=yourAlignmentMethod, search=yourSearchmethod, ksize=yourKmerSize, match=yourMatchBonus, mismatch=yourMismatchpenalty, gapopen=yourGapopenPenalty, gapextend=yourGapExtendPenalty)";
		helpString += "Example align.seqs(candidate=candidate.fasta, template=core.filtered, align=kmer, search=gotoh, ksize=8, match=2.0, mismatch=3.0, gapopen=-2.0, gapextend=-1.0)";
		helpString += "Note: No spaces between parameter labels (i.e. candidate), '=' and parameters (i.e.yourFastaFile).";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "AlignCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string AlignCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "fasta") {  pattern = "[filename],align"; } //makes file like: amazon.align
        else if (type == "alignreport") {  pattern = "[filename],align.report"; }
        else if (type == "accnos") {  pattern = "[filename],flip.accnos"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->control_pressed = true;  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "AlignCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
AlignCommand::AlignCommand(){	
	try {
		abort = true; calledHelp = true; 
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["alignreport"] = tempOutNames;
		outputTypes["accnos"] = tempOutNames;
	}
	catch(exception& e) {
		m->errorOut(e, "AlignCommand", "AlignCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
AlignCommand::AlignCommand(string option)  {
	try {
		abort = false; calledHelp = false;
		ReferenceDB* rdb = ReferenceDB::getInstance();
		
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true;}
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
		
		else {
			vector<string> myArray = setParameters();
			
			OptionParser parser(option);
			map<string, string> parameters = parser.getParameters(); 
			
			ValidParameters validParameter("align.seqs");
			map<string, string>::iterator it;
			
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["fasta"] = tempOutNames;
			outputTypes["alignreport"] = tempOutNames;
			outputTypes["accnos"] = tempOutNames;
			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	outputDir = "";		}
			

			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;

				it = parameters.find("reference");

				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["reference"] = inputDir + it->second;		}
				}
			}

			candidateFileName = validParameter.validFile(parameters, "fasta", false);
			if (candidateFileName == "not found") { 
				//if there is a current fasta file, use it
				string filename = m->getFastaFile(); 
				if (filename != "") { candidateFileNames.push_back(filename); m->mothurOut("Using " + filename + " as input file for the fasta parameter."); m->mothurOutEndLine(); }
				else { 	m->mothurOut("You have no current fastafile and the candidate parameter is required."); m->mothurOutEndLine(); abort = true; }
			}else { 
				m->splitAtDash(candidateFileName, candidateFileNames);
				
				//go through files and make sure they are good, if not, then disregard them
				for (int i = 0; i < candidateFileNames.size(); i++) {
					//candidateFileNames[i] = m->getFullPathName(candidateFileNames[i]);
					
					bool ignore = false;
					if (candidateFileNames[i] == "current") { 
						candidateFileNames[i] = m->getFastaFile(); 
						if (candidateFileNames[i] != "") {  m->mothurOut("Using " + candidateFileNames[i] + " as input file for the fasta parameter where you had given current."); m->mothurOutEndLine(); }
						else { 	
							m->mothurOut("You have no current fastafile, ignoring current."); m->mothurOutEndLine(); ignore=true; 
							//erase from file list
							candidateFileNames.erase(candidateFileNames.begin()+i);
							i--;
						}
					}
					
					if (!ignore) {
					
						if (inputDir != "") {
							string path = m->hasPath(candidateFileNames[i]);
							//if the user has not given a path then, add inputdir. else leave path alone.
							if (path == "") {	candidateFileNames[i] = inputDir + candidateFileNames[i];		}
						}
		
						int ableToOpen;
						ifstream in;
						ableToOpen = m->openInputFile(candidateFileNames[i], in, "noerror");
						in.close();	
						
						//if you can't open it, try default location
						if (ableToOpen == 1) {
							if (m->getDefaultPath() != "") { //default path is set
								string tryPath = m->getDefaultPath() + m->getSimpleName(candidateFileNames[i]);
								m->mothurOut("Unable to open " + candidateFileNames[i] + ". Trying default " + tryPath); m->mothurOutEndLine();
								ifstream in2;
								ableToOpen = m->openInputFile(tryPath, in2, "noerror");
								in2.close();
								candidateFileNames[i] = tryPath;
							}
						}
						
						//if you can't open it, try output location
						if (ableToOpen == 1) {
							if (m->getOutputDir() != "") { //default path is set
								string tryPath = m->getOutputDir() + m->getSimpleName(candidateFileNames[i]);
								m->mothurOut("Unable to open " + candidateFileNames[i] + ". Trying output directory " + tryPath); m->mothurOutEndLine();
								ifstream in2;
								ableToOpen = m->openInputFile(tryPath, in2, "noerror");
								in2.close();
								candidateFileNames[i] = tryPath;
							}
						}
						
										

						if (ableToOpen == 1) { 
							m->mothurOut("Unable to open " + candidateFileNames[i] + ". It will be disregarded."); m->mothurOutEndLine(); 
							//erase from file list
							candidateFileNames.erase(candidateFileNames.begin()+i);
							i--;
						}else {
							m->setFastaFile(candidateFileNames[i]);
						}
					}
				}
				
				//make sure there is at least one valid file left
				if (candidateFileNames.size() == 0) { m->mothurOut("no valid files."); m->mothurOutEndLine(); abort = true; }
			}
		
			//check for optional parameter and set defaults
			// ...at some point should added some additional type checking...
			string temp;
			temp = validParameter.validFile(parameters, "ksize", false);		if (temp == "not found"){	temp = "8";				}
			m->mothurConvert(temp, kmerSize); 
			
			temp = validParameter.validFile(parameters, "match", false);		if (temp == "not found"){	temp = "1.0";			}
			m->mothurConvert(temp, match);  
			
			temp = validParameter.validFile(parameters, "mismatch", false);		if (temp == "not found"){	temp = "-1.0";			}
			m->mothurConvert(temp, misMatch);  
			
			temp = validParameter.validFile(parameters, "gapopen", false);		if (temp == "not found"){	temp = "-5.0";			}
			m->mothurConvert(temp, gapOpen);  
			
			temp = validParameter.validFile(parameters, "gapextend", false);	if (temp == "not found"){	temp = "-2.0";			}
			m->mothurConvert(temp, gapExtend); 
			
			temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found"){	temp = m->getProcessors();	}
			m->setProcessors(temp);
			m->mothurConvert(temp, processors); 
			
			temp = validParameter.validFile(parameters, "flip", false);			if (temp == "not found"){	temp = "f";				}
			flip = m->isTrue(temp); 
			
			temp = validParameter.validFile(parameters, "save", false);			if (temp == "not found"){	temp = "f";				}
			save = m->isTrue(temp); 
			rdb->save = save; 
			if (save) { //clear out old references
				rdb->clearMemory();
			}
			
			//this has to go after save so that if the user sets save=t and provides no reference we abort
			templateFileName = validParameter.validFile(parameters, "reference", true);
			if (templateFileName == "not found") { 
				//check for saved reference sequences
				if (rdb->referenceSeqs.size() != 0) {
					templateFileName = "saved";
				}else {
					m->mothurOut("[ERROR]: You don't have any saved reference sequences and the reference parameter is a required for the align.seqs command."); 
					m->mothurOutEndLine();
					abort = true; 
				}
			}else if (templateFileName == "not open") { abort = true; }	
			else {	if (save) {	rdb->setSavedReference(templateFileName);	}	}
			
			temp = validParameter.validFile(parameters, "threshold", false);	if (temp == "not found"){	temp = "0.50";			}
			m->mothurConvert(temp, threshold); 
			
			search = validParameter.validFile(parameters, "search", false);		if (search == "not found"){	search = "kmer";		}
			if ((search != "suffix") && (search != "kmer") && (search != "blast")) { m->mothurOut("invalid search option: choices are kmer, suffix or blast."); m->mothurOutEndLine(); abort=true; }
			
			align = validParameter.validFile(parameters, "align", false);		if (align == "not found"){	align = "needleman";	}
			if ((align != "needleman") && (align != "gotoh") && (align != "blast") && (align != "noalign")) { m->mothurOut("invalid align option: choices are needleman, gotoh, blast or noalign."); m->mothurOutEndLine(); abort=true; }

		}
		
	}
	catch(exception& e) {
		m->errorOut(e, "AlignCommand", "AlignCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
AlignCommand::~AlignCommand(){}
//**********************************************************************************************************************

int AlignCommand::execute(){
	try {
		if (abort == true) { if (calledHelp) { return 0; }  return 2;	}

		templateDB = make_shared<AlignmentDB>(templateFileName, search, kmerSize, gapOpen, gapExtend, match, misMatch, rand());
		
		for (int s = 0; s < candidateFileNames.size(); s++) {
			if (m->control_pressed) { outputTypes.clear(); return 0; }
			
			m->mothurOut("Aligning sequences from " + candidateFileNames[s] + " ..." ); m->mothurOutEndLine();
			
			if (outputDir == "") {  outputDir += m->hasPath(candidateFileNames[s]); }
            map<string, string> variables; variables["[filename]"] = outputDir + m->getRootName(m->getSimpleName(candidateFileNames[s]));
			string alignFileName = getOutputFileName("fasta", variables);  
			string reportFileName = getOutputFileName("alignreport", variables);
			string accnosFileName = getOutputFileName("accnos", variables);
            
			bool hasAccnos = true;
			
			int numFastaSeqs = 0;
			lines.clear();
			int start = time(NULL);
		

			vector<unsigned long long> positions; 
			positions = m->divideFile(candidateFileNames[s], processors);
			for (int i = 0; i < (positions.size()-1); i++) {	lines.push_back(linePair(positions[i], positions[(i+1)]));	}
			
			if(processors == 1){
				numFastaSeqs = driver(lines[0], alignFileName, reportFileName, accnosFileName, candidateFileNames[s]);
			}else{
				numFastaSeqs = createProcesses(alignFileName, reportFileName, accnosFileName, candidateFileNames[s]); 
			}
				
			if (m->control_pressed) { m->mothurRemove(accnosFileName); m->mothurRemove(alignFileName); m->mothurRemove(reportFileName); outputTypes.clear();  return 0; }
			
			//delete accnos file if its blank else report to user
			if (m->isBlank(accnosFileName)) {  m->mothurRemove(accnosFileName);  hasAccnos = false; }
			else { 
				m->mothurOut("Some of your sequences generated alignments that eliminated too many bases, a list is provided in " + accnosFileName + ".");
				if (!flip) {
					m->mothurOut(" If you set the flip parameter to true mothur will try aligning the reverse compliment as well."); 
				}else{  m->mothurOut(" If the reverse compliment proved to be better it was reported.");  }
				m->mothurOutEndLine();
			}

			outputNames.push_back(alignFileName); outputTypes["fasta"].push_back(alignFileName);
			outputNames.push_back(reportFileName); outputTypes["alignreport"].push_back(reportFileName);
			if (hasAccnos)	{	outputNames.push_back(accnosFileName);	outputTypes["accnos"].push_back(accnosFileName);  }

			m->mothurOut("It took " + toString(time(NULL) - start) + " secs to align " + toString(numFastaSeqs) + " sequences.");
			m->mothurOutEndLine();
			m->mothurOutEndLine();
		}
		
		//set align file as new current fastafile
		string currentFasta = "";
		itTypes = outputTypes.find("fasta");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { currentFasta = (itTypes->second)[0]; m->setFastaFile(currentFasta); }
		}
		
		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
		m->mothurOutEndLine();

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "AlignCommand", "execute");
		exit(1);
	}
}

//**********************************************************************************************************************
int AlignCommand::driver(linePair filePos, string alignFName, string reportFName, string accnosFName, string filename){
	try {
		ofstream alignmentFile;
		m->openOutputFile(alignFName, alignmentFile);
		
		ofstream accnosFile;
		m->openOutputFile(accnosFName, accnosFile);
		
		NastReport report(reportFName);
		
		ifstream inFASTA;
		m->openInputFile(filename, inFASTA);

		inFASTA.seekg(filePos.start);

		bool done = false;
		int count = 0;
		
		//moved this into driver to avoid deep copies in windows paralellized version
		shared_ptr<Alignment> alignment;
		int longestBase = templateDB->getLongestBase();
        if (m->debug) { m->mothurOut("[DEBUG]: template longest base = "  + toString(templateDB->getLongestBase()) + " \n"); }
		if(align == "gotoh")			{	alignment = (shared_ptr<Alignment>)make_shared<GotohOverlap>(gapOpen, gapExtend, match, misMatch, longestBase);			}
		else if(align == "needleman")	{	alignment = (shared_ptr<Alignment>)make_shared<NeedlemanOverlap>(gapOpen, match, misMatch, longestBase);				}
		else if(align == "blast")		{	alignment = (shared_ptr<Alignment>)make_shared<BlastAlignment>(gapOpen, gapExtend, match, misMatch);		}
		else if(align == "noalign")		{	alignment = (shared_ptr<Alignment>)make_shared<NoAlign>();													}
		else {
			m->mothurOut(align + " is not a valid alignment option. I will run the command using needleman.");
			m->mothurOutEndLine();
			alignment = (shared_ptr<Alignment>)make_shared<NeedlemanOverlap>(gapOpen, match, misMatch, longestBase);
		}
	
		while (!done) {
			
			if (m->control_pressed) {  break; }
			
			shared_ptr<Sequence> candidateSeq = make_shared<Sequence>(inFASTA);  m->gobble(inFASTA);
			report.setCandidate(candidateSeq.get());

			int origNumBases = candidateSeq->getNumBases();
			string originalUnaligned = candidateSeq->getUnaligned();
			int numBasesNeeded = origNumBases * threshold;
	
			if (candidateSeq->getName() != "") { //incase there is a commented sequence at the end of a file
				if (candidateSeq->getUnaligned().length()+1 > alignment->getnRows()) {
                    if (m->debug) { m->mothurOut("[DEBUG]: " + candidateSeq->getName() + " " + toString(candidateSeq->getUnaligned().length()) + " " + toString(alignment->getnRows()) + " \n"); }
					alignment->resize(candidateSeq->getUnaligned().length()+2);
				}
				Sequence temp = templateDB->findClosestSequence(candidateSeq.get());
				shared_ptr<Sequence> templateSeq = make_shared<Sequence>(temp.getName(), temp.getAligned());
				
				float searchScore = templateDB->getSearchScore();
								
				Nast nast = Nast(alignment, candidateSeq, templateSeq);
		
				//if there is a possibility that this sequence should be reversed
				if (candidateSeq->getNumBases() < numBasesNeeded) {
					
					string wasBetter =  "";
					//if the user wants you to try the reverse
					if (flip) {
				
						//get reverse compliment
						shared_ptr<Sequence> copy = make_shared<Sequence>(candidateSeq->getName(), originalUnaligned);
						copy->reverseComplement();
                        
                        if (m->debug) { m->mothurOut("[DEBUG]: flipping "  + candidateSeq->getName() + " \n"); }
						
						//rerun alignment
						Sequence temp2 = templateDB->findClosestSequence(copy.get());
						shared_ptr<Sequence> templateSeq2 = make_shared<Sequence>(temp2.getName(), temp2.getAligned());
                        
                        if (m->debug) { m->mothurOut("[DEBUG]: closest template "  + temp2.getName() + " \n"); }
						
						searchScore = templateDB->getSearchScore();
						
						Nast nast2 = Nast(alignment, copy, templateSeq2);
                        
                        if (m->debug) { m->mothurOut("[DEBUG]: completed Nast2 "  + candidateSeq->getName() + " flipped numBases = " + toString(copy->getNumBases()) + " old numbases = " + toString(candidateSeq->getNumBases()) +" \n"); }
			
						//check if any better
						if (copy->getNumBases() > candidateSeq->getNumBases()) {
							candidateSeq->setAligned(copy->getAligned());  //use reverse compliments alignment since its better
							templateSeq = templateSeq2;
							nast = nast2;
							wasBetter = "\treverse complement produced a better alignment, so mothur used the reverse complement.";
						}else{  
							wasBetter = "\treverse complement did NOT produce a better alignment so it was not used, please check sequence.";
						}
                        if (m->debug) { m->mothurOut("[DEBUG]: done.\n"); }
					}
					
					//create accnos file with names
					accnosFile << candidateSeq->getName() << wasBetter << endl;
				}
				
				report.setTemplate(templateSeq.get());
				report.setSearchParameters(search, searchScore);
				report.setAlignmentParameters(align, alignment.get());
				report.setNastParameters(nast);
	
				alignmentFile << '>' << candidateSeq->getName() << '\n' << candidateSeq->getAligned() << endl;
				
				report.print();
				
				count++;
			}
			
			unsigned long long pos = inFASTA.tellg();
			if ((pos == -1) || (pos >= filePos.end)) { break; }
			
			//report progress
			if((count) % 100 == 0){	m->mothurOutJustToScreen(toString(count) + "\n"); 		}
			
		}
		//report progress
		if((count) % 100 != 0){	m->mothurOutJustToScreen(toString(count) + "\n"); 		}
		
		alignmentFile.close();
		inFASTA.close();
		accnosFile.close();
		
		return count;
	}
	catch(exception& e) {
		m->errorOut(e, "AlignCommand", "driver");
		exit(1);
	}
}
/**************************************************************************************************/

void AlignCommand::driverWithCount(linePair filePos, string alignFName, string reportFName, string accnosFName, string filename, int & count) {
	count = driver(filePos, alignFName, reportFName, accnosFName, filename);
}

int AlignCommand::createProcesses(string alignFileName, string reportFileName, string accnosFName, string filename) {
	try {
		int num = 0;
		vector<thread> thrds(lines.size() - 1);
		vector<int> nums(lines.size() - 1);
        		
		//loop through and create all the processes you want
		for (int i = 1; i < lines.size(); i++) {
			thrds[i - 1] = thread(&AlignCommand::driverWithCount, this, lines[i], alignFileName + toString(i) + ".temp", reportFileName + toString(i) + ".temp", accnosFName + toString(i) + ".temp", filename, ref(nums[i - 1]));
		}
				
		//do my part
		num = driver(lines[0], alignFileName, reportFileName, accnosFName, filename);		        
		
		//force parent to wait until all the processes are done
		for (int i=0; i<thrds.size(); i++) {
			thrds[i].join();
			num += nums[i];
		}
		
		vector<string> nonBlankAccnosFiles;
		if (!(m->isBlank(accnosFName))) { nonBlankAccnosFiles.push_back(accnosFName); }
		else { m->mothurRemove(accnosFName); } //remove so other files can be renamed to it
			
		for (int i = 1; i < lines.size(); i++) {
			
			m->appendFiles(alignFileName + toString(i) + ".temp", alignFileName);
			m->mothurRemove(alignFileName + toString(i) + ".temp");
			
			appendReportFiles(reportFileName + toString(i) + ".temp", reportFileName);
			m->mothurRemove(reportFileName + toString(i) + ".temp");
			
			if (!(m->isBlank(accnosFName + toString(i) + ".temp"))) {
				nonBlankAccnosFiles.push_back(accnosFName + toString(i) + ".temp");
			}else { m->mothurRemove(accnosFName + toString(i) + ".temp");  }
			
		}
		
		//append accnos files
		if (nonBlankAccnosFiles.size() != 0) { 
			rename(nonBlankAccnosFiles[0].c_str(), accnosFName.c_str());
			
			for (int h=1; h < nonBlankAccnosFiles.size(); h++) {
				m->appendFiles(nonBlankAccnosFiles[h], accnosFName);
				m->mothurRemove(nonBlankAccnosFiles[h]);
			}
		}else { //recreate the accnosfile if needed
			ofstream out;
			m->openOutputFile(accnosFName, out);
			out.close();
		}
		
		return num;
	}
	catch(exception& e) {
		m->errorOut(e, "AlignCommand", "createProcesses");
		exit(1);
	}
}
//**********************************************************************************************************************

void AlignCommand::appendReportFiles(string temp, string filename) {
	try{
		
		ofstream output;
		ifstream input;
		m->openOutputFileAppend(filename, output);
		m->openInputFile(temp, input);

		while (!input.eof())	{	char c = input.get(); if (c == 10 || c == 13){	break;	}	} // get header line
				
        char buffer[4096];        
        while (!input.eof()) {
            input.read(buffer, 4096);
            output.write(buffer, input.gcount());
        }
		
		input.close();
		output.close();
	}
	catch(exception& e) {
		m->errorOut(e, "AlignCommand", "appendReportFiles");
		exit(1);
	}
}
//**********************************************************************************************************************
