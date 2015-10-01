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
#include "filehandling/file.h"

 /**************************************************************************************************/
void Classify::generateDatabaseAndNames(string tfile, string tempFile, string method, int kmerSize, float gapOpen, float gapExtend, float match, float misMatch) {
	try {
		ReferenceDB& rdb = ReferenceDB::getInstance();

		if (tfile == "saved") { tfile = rdb.getSavedTaxonomy(); }

		taxFile = tfile;

		int numSeqs = 0;

		if (tempFile == "saved") {
			int start = time(NULL);
			LOG(INFO) << '\n' << "Using sequences from " + rdb.getSavedReference() + " that are saved in memory." << '\n';

			numSeqs = rdb.referenceSeqs.size();
			templateFile = rdb.getSavedReference();
			tempFile = rdb.getSavedReference();

			bool needToGenerate = true;
			string kmerDBName;
			if (method == "kmer") {
				database = new KmerDB(tempFile, kmerSize);

				kmerDBName = tempFile.substr(0, tempFile.find_last_of(".") + 1) + char('0' + kmerSize) + "mer";
				ifstream kmerFileTest(kmerDBName.c_str());
				if (kmerFileTest) {
					bool GoodFile = m->checkReleaseVersion(kmerFileTest, m->getVersion());
					if (GoodFile) { needToGenerate = false; }
				}
			}
			else if (method == "suffix") { database = new SuffixDB(numSeqs); }
			else if (method == "blast") { database = new BlastDB(tempFile.substr(0, tempFile.find_last_of(".") + 1), gapOpen, gapExtend, match, misMatch, "", threadID); }
			else if (method == "distance") { database = new DistanceDB(); }
			else {
				LOG(INFO) << method + " is not a valid search option. I will run the command using kmer, ksize=8.";
				LOG(INFO) << "";
				database = new KmerDB(tempFile, 8);
			}

			if (needToGenerate) {
				for (int k = 0; k < rdb.referenceSeqs.size(); k++) {
					Sequence temp(rdb.referenceSeqs[k].getName(), rdb.referenceSeqs[k].getAligned());
					names.push_back(temp.getName());
					database->addSequence(temp);
				}
				if ((method == "kmer") && (!shortcuts)) { ; } //don't print
				else { database->generateDB(); }
			}
			else if ((method == "kmer") && (!needToGenerate)) {
				ifstream kmerFileTest(kmerDBName.c_str());
				database->readKmerDB(kmerFileTest);

				for (int k = 0; k < rdb.referenceSeqs.size(); k++) {
					names.push_back(rdb.referenceSeqs[k].getName());
				}
			}

			database->setNumSeqs(numSeqs);

			LOG(INFO) << "It took " + toString(time(NULL) - start) + " to load " + toString(rdb.referenceSeqs.size()) + " sequences and generate the search databases." << '\n';

		}
		else {

			templateFile = tempFile;

			int start = time(NULL);

			LOG(INFO) << "Generating search database...    "; cout.flush();

			//need to know number of template seqs for suffixdb
			if (method == "suffix") {
				ifstream inFASTA;
				File::openInputFile(tempFile, inFASTA);
				m->getNumSeqs(inFASTA, numSeqs);
				inFASTA.close();
			}

			bool needToGenerate = true;
			string kmerDBName;
			if (method == "kmer") {
				database = new KmerDB(tempFile, kmerSize);

				kmerDBName = tempFile.substr(0, tempFile.find_last_of(".") + 1) + char('0' + kmerSize) + "mer";
				ifstream kmerFileTest(kmerDBName.c_str());
				if (kmerFileTest) {
					bool GoodFile = m->checkReleaseVersion(kmerFileTest, m->getVersion());
					if (GoodFile) { needToGenerate = false; }
				}
			}
			else if (method == "suffix") { database = new SuffixDB(numSeqs); }
			else if (method == "blast") { database = new BlastDB(tempFile.substr(0, tempFile.find_last_of(".") + 1), gapOpen, gapExtend, match, misMatch, "", threadID); }
			else if (method == "distance") { database = new DistanceDB(); }
			else {
				LOG(INFO) << method + " is not a valid search option. I will run the command using kmer, ksize=8.";
				LOG(INFO) << "";
				database = new KmerDB(tempFile, 8);
			}

			if (needToGenerate) {
				ifstream fastaFile;
				File::openInputFile(tempFile, fastaFile);

				while (!fastaFile.eof()) {
					Sequence temp(fastaFile);
					File::gobble(fastaFile);

					if (rdb.save) { rdb.referenceSeqs.push_back(temp); }

					names.push_back(temp.getName());

					database->addSequence(temp);
				}
				fastaFile.close();

				if ((method == "kmer") && (!shortcuts)) { ; } //don't print
				else { database->generateDB(); }

			}
			else if ((method == "kmer") && (!needToGenerate)) {
				ifstream kmerFileTest(kmerDBName.c_str());
				database->readKmerDB(kmerFileTest);

				ifstream fastaFile;
				File::openInputFile(tempFile, fastaFile);

				while (!fastaFile.eof()) {
					Sequence temp(fastaFile);
					File::gobble(fastaFile);

					if (rdb.save) { rdb.referenceSeqs.push_back(temp); }
					names.push_back(temp.getName());
				}
				fastaFile.close();
			}

			database->setNumSeqs(names.size());

			LOG(INFO) << "DONE." << '\n';
			LOG(INFO) << "It took " + toString(time(NULL) - start) + " seconds generate search database. " << '\n';
		}

		readTaxonomy(taxFile);

		//sanity check
		bool okay = phyloTree->ErrorCheck(names);

		if (!okay) { ctrlc_pressed = true; }
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in Classify, generateDatabaseAndNames";
		exit(1);
	}
}
/**************************************************************************************************/
Classify::Classify() { m = MothurOut::getInstance();	database = NULL;	phyloTree = NULL; flipped = false; }
/**************************************************************************************************/

int Classify::readTaxonomy(string file) {

	phyloTree = new PhyloTree();
	string name, taxInfo;

	LOG(INFO) << '\n' << "Reading in the " + file + " taxonomy...\t";	cout.flush();
	if (app.isDebug) { LOG(DEBUG) << "Taxonomies read in...\n"; }


	taxonomy.clear();
	m->readTax(file, taxonomy, true);

	//commented out to save time with large templates. 6/12/13
	//map<string, string> tempTaxonomy;
	for (map<string, string>::iterator itTax = taxonomy.begin(); itTax != taxonomy.end(); itTax++) {
		//if (m->inUsersGroups(itTax->first, names)) {
		phyloTree->addSeqToTree(itTax->first, itTax->second);
		if (ctrlc_pressed) { break; }
		//tempTaxonomy[itTax->first] = itTax->second;
   // }else {
	//    LOG(WARNING) << "" + itTax->first + " is in your taxonomy file and not in your reference file, ignoring.\n";
	//}
	}
	//taxonomy = tempTaxonomy;

	phyloTree->assignHeirarchyIDs(0);

	phyloTree->setUp(file);

	LOG(INFO) << "DONE.";
	LOG(INFO) << "";	cout.flush();

	return phyloTree->getNumSeqs();

}
/**************************************************************************************************/

vector<string> Classify::parseTax(string tax) {
	vector<string> taxons;
	m->splitAtChar(tax, taxons, ';');
	return taxons;
}
/**************************************************************************************************/

double Classify::getLogExpSum(vector<double> probabilities, int& maxIndex) {
	//	http://jblevins.org/notes/log-sum-exp

	double maxProb = probabilities[0];
	maxIndex = 0;

	int numProbs = (int)probabilities.size();

	for (int i = 1;i < numProbs;i++) {
		if (probabilities[i] >= maxProb) {
			maxProb = probabilities[i];
			maxIndex = i;
		}
	}

	double probSum = 0.0000;

	for (int i = 0;i < numProbs;i++) {
		probSum += exp(probabilities[i] - maxProb);
	}

	probSum = log(probSum) + maxProb;

	return probSum;
}

/**************************************************************************************************/

