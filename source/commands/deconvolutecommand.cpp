/*
 *  deconvolute.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 1/21/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "deconvolutecommand.h"
#include "sequence.h"
#include "commandparameters/fastafileparameter.h"
#include "commandparameters/namefileparameter.h"
#include "commandparameters/countfileparameter.h"
#include "commandparameters/multipleexclusiveparameter.h"
#include "datastructures/counttable.h"
#include "filehandling/fastafilewrite.h"
#include <unordered_set>

 //**********************************************************************************************************************
void DeconvoluteCommand::setParameters() {
	parameters.add(new FastaFileParameter(fastafile, settings, "fasta", true, true));
	parameters.add(new NameFileParameter(namefile, settings, "name", false, true, "namecount"));
	parameters.add(new CountFileParameter(countfile, settings, "count", false, true, "namecount"));
	parameters.add(new MultipleExclusiveParameter(format, "format", vector<string> {"count", "name"}, "name"));
	parameters.addStandardParameters(inputDir, outputDir);
}

void DeconvoluteCommand::setOutputTypes() {
	outputTypes.add("fasta", "[filename],unique,[extension]");
	outputTypes.add("name", "[filename],names-[filename],[tag],names");
	outputTypes.add("count", "[filename],count_table-[filename],[tag],count_table");
}
//**********************************************************************************************************************
string DeconvoluteCommand::getHelpString() const {
	string helpString = "The unique.seqs command reads a fastafile and creates a name or count file.\n"
		"The unique.seqs command parameters are fasta, name, count and format.  fasta is required, unless there is a valid current fasta file.\n"
		"The name parameter is used to provide an existing name file associated with the fasta file. \n"
		"The count parameter is used to provide an existing count file associated with the fasta file. \n"
		"The format parameter is used to indicate what type of file you want outputted.  Choices are name and count, default=name unless count file used then default=count.\n"
		"The unique.seqs command should be in the following format: \n"
		"unique.seqs(fasta=yourFastaFile) \n";
	return helpString;
}
/**************************************************************************************/
DeconvoluteCommand::DeconvoluteCommand(Settings& settings, ParameterListToProcess& ptp) : Command(settings, ptp) {}
/**************************************************************************************/
int DeconvoluteCommand::execute() {
	//prepare filenames and open files
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getSimpleRootName(fastafile);
	map<string, string> mvariables = variables;
	mvariables["[tag]"] = "unique";
	std::string outNameFileName(getOutputFileName("name", variables, namefile, mvariables));
	std::string outCountFileName(getOutputFileName("count", variables, countfile, mvariables));
	variables["[extension]"] = File::getExtension(fastafile);
	FastaFileRead fasta(fastafile);
	FastaFileWrite outFasta(getOutputFileName("fasta", variables));

	NameMapUniqueToDups nameMap;
	NameMapUniqueToDups newNameMap;
	CountTable ct;
	CountTable newCt;

	if (namefile != "") {
		nameMap = NameMapUniqueToDups(namefile);
	}
	if (countfile != "") {
		ct = CountTable(countfile);
	}

	if (ctrlc_pressed) { return 0; }

	std::map<std::string, std::string> sequenceStrings; //sequenceString -> first name of that sequence.
	std::unordered_set<std::string> nameInFastaFile; //for sanity checking
	std::vector<std::string> nameFileOrder;
	int count = 0;
	while (!(fasta.eof()) && !ctrlc_pressed) {

		Sequence seq = fasta.readSequence();
		if (seq.getName() != "") {
			//sanity checks
			auto itname = nameInFastaFile.find(seq.getName());
			if (itname == nameInFastaFile.end()) {
				nameInFastaFile.insert(seq.getName());
			}
			else {
				LOG(LOGERROR) << "You already have a sequence named " + seq.getName() + " in your fasta file, sequence names must be unique, please correct." << '\n';
			}

			auto itStrings = sequenceStrings.find(seq.getSequence());

			if (itStrings == sequenceStrings.end()) { //this is a new unique sequence
				//output to unique fasta file
				outFasta.writeSequence(seq);

				if (namefile != "") {
					auto itNames = nameMap.find(seq.getName());

					if (itNames == nameMap.end()) { //namefile and fastafile do not match
						LOG(LOGERROR) << seq.getName() + " is in your fasta file, and not in your namefile, please correct.";
						ctrlc_pressed = true;
					}
					else {
						if (format == "name") {
							std::swap(newNameMap[seq.getName()], itNames->second);
						}
						else {
							newCt.setTotalCount(seq.getName(), itNames->second.size());
						}
					}
				}
				else if (countfile != "") {
					if (format == "name") {
						long long numSeqs = ct.getNumSeqs(seq.getName());
						for (long long i = 0; i < numSeqs; i++) {
							newNameMap[seq.getName()].push_back(seq.getName() + "_" + std::to_string(i));
						}
					}
					else {
						newCt.setTotalCount(seq.getName(), ct.getNumSeqs(seq.getName())); //checks to make sure seq is in table
					}
				}
				else {
					if (format == "name") {
						newNameMap[seq.getName()].push_back(seq.getName());
					}
					else {
						newCt.setTotalCount(seq.getName(), 1);
					}
				}
				sequenceStrings[seq.getSequence()] = seq.getName();
				nameFileOrder.push_back(seq.getName());
			}
			else { //this is a dup
				if (namefile != "") {
					auto itNames = nameMap.find(seq.getName());

					if (itNames == nameMap.end()) { //namefile and fastafile do not match
						LOG(LOGERROR) << seq.getName() + " is in your fasta file, and not in your namefile, please correct." << '\n';
					}
					else {
						if (format == "name") {
							newNameMap[itStrings->second].insert(newNameMap[itStrings->second].end(), itNames->second.begin(), itNames->second.end());
						}
						else {
							long long currentReps = newCt.getNumSeqs(itStrings->second);
							newCt.setTotalCount(itStrings->second, currentReps + itNames->second.size());
						}
					}
				}
				else if (countfile != "") {
					if (format == "name") {
						long long numSeqs = ct.getNumSeqs(seq.getName());
						for (long long i = 0; i < numSeqs; i++) {
							newNameMap[itStrings->second].push_back(seq.getName() + "_" + std::to_string(i));
						}
					}
					else {
						long long currentReps = newCt.getNumSeqs(itStrings->second);
						newCt.setTotalCount(itStrings->second, currentReps + ct.getNumSeqs(seq.getName())); //merges counts and saves in uniques name
					}
				}
				else {
					if (format == "name") {
						newNameMap[itStrings->second].push_back(seq.getName());
					}
					else {
						long long currentReps = newCt.getNumSeqs(itStrings->second);
						newCt.setTotalCount(itStrings->second, currentReps + 1);
					}
				}
			}
			count++;
		}

		if (count % 1000 == 0) { LOG(SCREENONLY) << toString(count) + "\t" + toString(sequenceStrings.size()); }
	}

	if (ctrlc_pressed) { return 0; }

	LOG(INFO) << toString(count) + "\t" + toString(sequenceStrings.size());

	bool success = false;

	if (format == "name") {
		success = newNameMap.writeNameFile(outNameFileName, nameFileOrder);
		if (success) {
			outputTypes["name"].files.push_back(outNameFileName);
			settings.setCurrent("name", outFasta.getFileName());
		}
	}
	else {
		success = newCt.writeCountTable(outCountFileName, nameFileOrder);
		if (success) {
			outputTypes["name"].files.push_back(outCountFileName);
			settings.setCurrent("count", outFasta.getFileName());
		}
	}

	if (!success) {
		if (!ctrlc_pressed) {
			LOG(LOGERROR) << "Error writing output file";
		}
		return 0;
	}

	LOG(INFO) << '\n' << "Output File Names: ";
	outputTypes["fasta"].files.push_back(outFasta.getFileName());
	for (auto ot : outputTypes) {
		if (ot.second.files.size() > 0) {
			LOG(INFO) << Utility::join(ot.second.files, "\n");
		}
	}

	//set fasta file as new current fastafile
	outFasta.commit();
	settings.setCurrent("fasta", outFasta.getFileName());
	return 0;
}
/**************************************************************************************/
