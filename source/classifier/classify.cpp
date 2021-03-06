/*
 *  classify.cpp
 *  Mothur
 *
 *  Created by westcott on 11/3/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "classify.h"
#include "sequence.hpp"
#include "kmerdb.hpp"
#include "suffixdb.hpp"
#include "blastdb.hpp"
#include "distancedb.hpp"
#include "referencedb.h"

/**************************************************************************************************/
void Classify::generateDatabaseAndNames(string tfile, string tempFile, string method, int kmerSize, float gapOpen, float gapExtend, float match, float misMatch)  {		
	try {	
		ReferenceDB* rdb = ReferenceDB::getInstance();
		
		if (tfile == "saved") { tfile = rdb->getSavedTaxonomy(); }
		
		taxFile = tfile;
		
		int numSeqs = 0;
		
		if (tempFile == "saved") {
			int start = time(NULL);
			m->mothurOutEndLine();  m->mothurOut("Using sequences from " + rdb->getSavedReference() + " that are saved in memory.");	m->mothurOutEndLine();
			
			numSeqs = rdb->referenceSeqs.size();
			templateFile = rdb->getSavedReference();
			tempFile = rdb->getSavedReference();
			
			bool needToGenerate = true;
			string kmerDBName;
			if(method == "kmer")			{	
				database = new KmerDB(tempFile, kmerSize);			
				
				kmerDBName = tempFile.substr(0,tempFile.find_last_of(".")+1) + char('0'+ kmerSize) + "mer";
				ifstream kmerFileTest(kmerDBName.c_str());
				if(kmerFileTest){	
					bool GoodFile = m->checkReleaseVersion(kmerFileTest, m->getVersion());
					if (GoodFile) {  needToGenerate = false;	}
				}
			}
			else if(method == "suffix")		{	database = new SuffixDB(numSeqs);								}
			else if(method == "blast")		{	database = new BlastDB(tempFile.substr(0,tempFile.find_last_of(".")+1), gapOpen, gapExtend, match, misMatch, "", threadID);	}
			else if(method == "distance")	{	database = new DistanceDB();	}
			else {
				m->mothurOut(method + " is not a valid search option. I will run the command using kmer, ksize=8.");
				m->mothurOutEndLine();
				database = new KmerDB(tempFile, 8);
			}
			
			if (needToGenerate) {
				for (int k = 0; k < rdb->referenceSeqs.size(); k++) {
					Sequence temp(rdb->referenceSeqs[k].getName(), rdb->referenceSeqs[k].getAligned());
					names.push_back(temp.getName());
					database->addSequence(temp);	
				}
				if ((method == "kmer") && (!shortcuts)) {;} //don't print
                else {database->generateDB(); }
			}else if ((method == "kmer") && (!needToGenerate)) {	
				ifstream kmerFileTest(kmerDBName.c_str());
				database->readKmerDB(kmerFileTest);	
				
				for (int k = 0; k < rdb->referenceSeqs.size(); k++) {
					names.push_back(rdb->referenceSeqs[k].getName());
				}			
			}	
			
			database->setNumSeqs(numSeqs);
			
			m->mothurOut("It took " + toString(time(NULL) - start) + " to load " + toString(rdb->referenceSeqs.size()) + " sequences and generate the search databases.");m->mothurOutEndLine();  
			
		}else {
			
			templateFile = tempFile;	
			
			int start = time(NULL);
			
			m->mothurOut("Generating search database...    "); cout.flush();
	#ifdef USE_MPI	
				int pid, processors;
				vector<unsigned long long> positions;
				int tag = 2001;
			
				MPI_Status status; 
				MPI_File inMPI;
				MPI_Comm_rank(MPI_COMM_WORLD, &pid); //find out who we are
				MPI_Comm_size(MPI_COMM_WORLD, &processors);

				//char* inFileName = new char[tempFile.length()];
				//memcpy(inFileName, tempFile.c_str(), tempFile.length());
				
				char inFileName[1024];
				strcpy(inFileName, tempFile.c_str());

				MPI_File_open(MPI_COMM_WORLD, inFileName, MPI_MODE_RDONLY, MPI_INFO_NULL, &inMPI);  //comm, filename, mode, info, filepointer
				//delete inFileName;

				if (pid == 0) { //only one process needs to scan file
					positions = m->setFilePosFasta(tempFile, numSeqs); //fills MPIPos, returns numSeqs

					//send file positions to all processes
					for(int i = 1; i < processors; i++) { 
						MPI_Send(&numSeqs, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
						MPI_Send(&positions[0], (numSeqs+1), MPI_LONG, i, tag, MPI_COMM_WORLD);
					}
				}else{
					MPI_Recv(&numSeqs, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
					positions.resize(numSeqs+1);
					MPI_Recv(&positions[0], (numSeqs+1), MPI_LONG, 0, tag, MPI_COMM_WORLD, &status);
				}
				
				//create database
				if(method == "kmer")			{	database = new KmerDB(tempFile, kmerSize);			}
				else if(method == "suffix")		{	database = new SuffixDB(numSeqs);								}
				else if(method == "blast")		{	database = new BlastDB(tempFile.substr(0,tempFile.find_last_of(".")+1), gapOpen, gapExtend, match, misMatch, "", pid);	}
				else if(method == "distance")	{	database = new DistanceDB();	}
				else {
					m->mothurOut(method + " is not a valid search option. I will run the command using kmer, ksize=8."); m->mothurOutEndLine();
					database = new KmerDB(tempFile, 8);
				}

				//read file 
				for(int i=0;i<numSeqs;i++){
					//read next sequence
					int length = positions[i+1] - positions[i];
					char* buf4 = new char[length];
					MPI_File_read_at(inMPI, positions[i], buf4, length, MPI_CHAR, &status);
					
					string tempBuf = buf4;
					if (tempBuf.length() > length) { tempBuf = tempBuf.substr(0, length); }
					delete buf4;
					istringstream iss (tempBuf,istringstream::in);
					
					Sequence temp(iss);  
					if (temp.getName() != "") {
						if (rdb->save) { rdb->referenceSeqs.push_back(temp); }
						names.push_back(temp.getName());
						database->addSequence(temp);	
					}
				}
				
				database->generateDB();
				MPI_File_close(&inMPI);
				MPI_Barrier(MPI_COMM_WORLD); //make everyone wait - just in case
		#else
			
			//need to know number of template seqs for suffixdb
			if (method == "suffix") {
				ifstream inFASTA;
				m->openInputFile(tempFile, inFASTA);
				m->getNumSeqs(inFASTA, numSeqs);
				inFASTA.close();
			}

			bool needToGenerate = true;
			string kmerDBName;
			if(method == "kmer")			{	
				database = new KmerDB(tempFile, kmerSize);			
				
				kmerDBName = tempFile.substr(0,tempFile.find_last_of(".")+1) + char('0'+ kmerSize) + "mer";
				ifstream kmerFileTest(kmerDBName.c_str());
				if(kmerFileTest){	
					bool GoodFile = m->checkReleaseVersion(kmerFileTest, m->getVersion());
					if (GoodFile) {  needToGenerate = false;	}
				}
			}
			else if(method == "suffix")		{	database = new SuffixDB(numSeqs);								}
			else if(method == "blast")		{	database = new BlastDB(tempFile.substr(0,tempFile.find_last_of(".")+1), gapOpen, gapExtend, match, misMatch, "", threadID);	}
			else if(method == "distance")	{	database = new DistanceDB();	}
			else {
				m->mothurOut(method + " is not a valid search option. I will run the command using kmer, ksize=8.");
				m->mothurOutEndLine();
				database = new KmerDB(tempFile, 8);
			}
			
			if (needToGenerate) {
				ifstream fastaFile;
				m->openInputFile(tempFile, fastaFile);
				
				while (!fastaFile.eof()) {
					Sequence temp(fastaFile);
					m->gobble(fastaFile);
					
					if (rdb->save) { rdb->referenceSeqs.push_back(temp); }
					
					names.push_back(temp.getName());
								
					database->addSequence(temp);	
				}
				fastaFile.close();

                if ((method == "kmer") && (!shortcuts)) {;} //don't print
                else {database->generateDB(); } 
				
			}else if ((method == "kmer") && (!needToGenerate)) {	
				ifstream kmerFileTest(kmerDBName.c_str());
				database->readKmerDB(kmerFileTest);	
			
				ifstream fastaFile;
				m->openInputFile(tempFile, fastaFile);
				
				while (!fastaFile.eof()) {
					Sequence temp(fastaFile);
					m->gobble(fastaFile);
					
					if (rdb->save) { rdb->referenceSeqs.push_back(temp); }
					names.push_back(temp.getName());
				}
				fastaFile.close();
			}
	#endif	
            	
			database->setNumSeqs(names.size());
			
			m->mothurOut("DONE."); m->mothurOutEndLine();
			m->mothurOut("It took " + toString(time(NULL) - start) + " seconds generate search database. "); m->mothurOutEndLine();
		}
        
        readTaxonomy(taxFile);
        
        //sanity check
        bool okay = phyloTree->ErrorCheck(names);
        
        if (!okay) { m->control_pressed = true; }
	}
	catch(exception& e) {
		m->errorOut(e, "Classify", "generateDatabaseAndNames");
		exit(1);
	}
}
/**************************************************************************************************/
Classify::Classify() {		m = MothurOut::getInstance();	database = NULL;	phyloTree=NULL; flipped=false; }
/**************************************************************************************************/

int Classify::readTaxonomy(string file) {
	try {
		
		phyloTree = new PhyloTree();
		string name, taxInfo;
		
		m->mothurOutEndLine();
		m->mothurOut("Reading in the " + file + " taxonomy...\t");	cout.flush();
        if (m->debug) { m->mothurOut("[DEBUG]: Taxonomies read in...\n"); }
        
#ifdef USE_MPI	
		int pid, num, processors;
		vector<unsigned long long> positions;
		int tag = 2001;
		
		MPI_Status status; 
		MPI_File inMPI;
		MPI_Comm_rank(MPI_COMM_WORLD, &pid); //find out who we are
		MPI_Comm_size(MPI_COMM_WORLD, &processors);
		
		char inFileName[1024];
		strcpy(inFileName, file.c_str());

		MPI_File_open(MPI_COMM_WORLD, inFileName, MPI_MODE_RDONLY, MPI_INFO_NULL, &inMPI);  //comm, filename, mode, info, filepointer
		//delete inFileName;

		if (pid == 0) {
			positions = m->setFilePosEachLine(file, num);
			
			//send file positions to all processes
			for(int i = 1; i < processors; i++) { 
				MPI_Send(&num, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
				MPI_Send(&positions[0], (num+1), MPI_LONG, i, tag, MPI_COMM_WORLD);
			}
		}else{
			MPI_Recv(&num, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			positions.resize(num+1);
			MPI_Recv(&positions[0], (num+1), MPI_LONG, 0, tag, MPI_COMM_WORLD, &status);
		}
	
		//read file 
		for(int i=0;i<num;i++){
			//read next sequence
			int length = positions[i+1] - positions[i];
			char* buf4 = new char[length];

			MPI_File_read_at(inMPI, positions[i], buf4, length, MPI_CHAR, &status);

			string tempBuf = buf4;
			if (tempBuf.length() > length) { tempBuf = tempBuf.substr(0, length); }
			delete buf4;

			istringstream iss (tempBuf,istringstream::in);
			iss >> name; m->gobble(iss);
            iss >> taxInfo;
            if (m->debug) { m->mothurOut("[DEBUG]: name = " + name + " tax = " + taxInfo + "\n"); }
            //commented out to save time with large templates. 10/7/13
			//if (m->inUsersGroups(name, names)) {
                taxonomy[name] = taxInfo;
                phyloTree->addSeqToTree(name, taxInfo);
            //}else {
            //    m->mothurOut("[WARNING]: " + name + " is in your taxonomy file and not in your reference file, ignoring.\n");
            //}
        }
		
		MPI_File_close(&inMPI);
		MPI_Barrier(MPI_COMM_WORLD); //make everyone wait - just in case
#else	
        
        taxonomy.clear(); 
        m->readTax(file, taxonomy, true);
        
        //commented out to save time with large templates. 6/12/13
        //map<string, string> tempTaxonomy;
        for (map<string, string>::iterator itTax = taxonomy.begin(); itTax != taxonomy.end(); itTax++) {  
            //if (m->inUsersGroups(itTax->first, names)) {
                phyloTree->addSeqToTree(itTax->first, itTax->second);
            if (m->control_pressed) { break; }
                //tempTaxonomy[itTax->first] = itTax->second;
           // }else {
            //    m->mothurOut("[WARNING]: " + itTax->first + " is in your taxonomy file and not in your reference file, ignoring.\n");
            //}
        }
        //taxonomy = tempTaxonomy;
#endif	
		phyloTree->assignHeirarchyIDs(0);
		
		phyloTree->setUp(file);
	
		m->mothurOut("DONE.");
		m->mothurOutEndLine();	cout.flush();
		
		return phyloTree->getNumSeqs();
	
	}
	catch(exception& e) {
		m->errorOut(e, "Classify", "readTaxonomy");
		exit(1);
	}
}
/**************************************************************************************************/

vector<string> Classify::parseTax(string tax) {
	try {
		vector<string> taxons;
		m->splitAtChar(tax, taxons, ';');
        return taxons;
	}
	catch(exception& e) {
		m->errorOut(e, "Classify", "parseTax");
		exit(1);
	}
}
/**************************************************************************************************/

double Classify::getLogExpSum(vector<double> probabilities, int& maxIndex){
	try {
        //	http://jblevins.org/notes/log-sum-exp
        
        double maxProb = probabilities[0];
        maxIndex = 0;
        
        int numProbs = (int)probabilities.size();
        
        for(int i=1;i<numProbs;i++){
            if(probabilities[i] >= maxProb){
                maxProb = probabilities[i];
                maxIndex = i;
            }
        }
        
        double probSum = 0.0000;
        
        for(int i=0;i<numProbs;i++){
            probSum += exp(probabilities[i] - maxProb);		
        }
        
        probSum = log(probSum) + maxProb;
        
        return probSum;
	}
	catch(exception& e) {
		m->errorOut(e, "Classify", "getLogExpSum");
		exit(1);
	}
}

/**************************************************************************************************/

