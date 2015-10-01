/*
 *  parsefastaqcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 9/30/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "parsefastaqcommand.h"
#include "sequence.hpp"

 //**********************************************************************************************************************
vector<string> ParseFastaQCommand::setParameters() {
	try {
		CommandParameter pfile("file", "InputTypes", "", "", "fastqFile", "fastqFile", "none", "", false, false, true); parameters.push_back(pfile);
		CommandParameter pfastq("fastq", "InputTypes", "", "", "fastqFile", "fastqFile", "none", "", false, false, true); parameters.push_back(pfastq);
		CommandParameter poligos("oligos", "InputTypes", "", "", "oligosGroup", "none", "none", "", false, false); parameters.push_back(poligos);
		CommandParameter pgroup("group", "InputTypes", "", "", "oligosGroup", "none", "none", "", false, false); parameters.push_back(pgroup);
		CommandParameter preorient("checkorient", "Boolean", "", "F", "", "", "", "", false, false, true); parameters.push_back(preorient);
		nkParameters.add(new NumberParameter("pdiffs", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("bdiffs", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("ldiffs", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("sdiffs", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new NumberParameter("tdiffs", -INFINITY, INFINITY, 0, false, false));
		CommandParameter pfasta("fasta", "Boolean", "", "T", "", "", "", "fasta", false, false); parameters.push_back(pfasta);
		CommandParameter pqual("qfile", "Boolean", "", "T", "", "", "", "qfile", false, false); parameters.push_back(pqual);
		nkParameters.add(new BooleanParameter("pacbio", false, false, false));
		nkParameters.add(new MultipleParameter("format", vector<string>{"sanger", "illumina", "solexa", "illumina1.8+"}, "sanger", false, false, true));
		nkParameters.add(new NumberParameter("seed", -INFINITY, INFINITY, 0, false, false));
		nkParameters.add(new StringParameter("inputdir", "", false, false));
		nkParameters.add(new StringParameter("outputdir", "", false, false));

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) { myArray.push_back(parameters[i].name); }
		return myArray;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ParseFastaQCommand, setParameters";
		exit(1);
	}
}
//**********************************************************************************************************************
string ParseFastaQCommand::getHelpString() {
	try {
		string helpString = "The fastq.info command reads a fastq file and creates a fasta and quality file or can be used to parse fastq files by sample.\n"
			"The fastq.info command parameters are file, fastq, fasta, qfile, oligos, group and format; file or fastq is required.\n"
			"The fastq.info command should be in the following format: fastq.info(fastaq=yourFastaQFile).\n"
			"The oligos parameter allows you to provide an oligos file to split your fastq file into separate fastq files by barcode and primers. \n"
			"The group parameter allows you to provide a group file to split your fastq file into separate fastq files by group. \n"
			"The tdiffs parameter is used to specify the total number of differences allowed in the reads. The default is pdiffs + bdiffs + sdiffs + ldiffs.\n"
			"The bdiffs parameter is used to specify the number of differences allowed in the barcode. The default is 0.\n"
			"The pdiffs parameter is used to specify the number of differences allowed in the primer. The default is 0.\n"
			"The ldiffs parameter is used to specify the number of differences allowed in the linker. The default is 0.\n"
			"The sdiffs parameter is used to specify the number of differences allowed in the spacer. The default is 0.\n"
			"The checkorient parameter will check look for the reverse compliment of the barcode or primer in the sequence. If found the sequence is flipped. The default is false.\n"
			"The format parameter is used to indicate whether your sequences are sanger, solexa, illumina1.8+ or illumina, default=sanger.\n"
			"The fasta parameter allows you to indicate whether you want a fasta file generated. Default=T.\n"
			"The qfile parameter allows you to indicate whether you want a quality file generated. Default=T.\n"
			"The pacbio parameter allows you to indicate .... When set to true, quality scores of 0 will results in a corresponding base of N. Default=F.\n"
			"Example fastq.info(fastaq=test.fastaq).\n"
			"Note: No spaces between parameter labels (i.e. fastq), '=' and yourFastQFile.\n";
		return helpString;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ParseFastaQCommand, getHelpString";
		exit(1);
	}
}
//**********************************************************************************************************************
string ParseFastaQCommand::getOutputPattern(string type) {
	string pattern = "";

	if (type == "fasta") { pattern = "[filename],fasta-[filename],[group],[tag],fasta-[filename],[group],fasta"; }
	else if (type == "qfile") { pattern = "[filename],qual-[filename],[group],[tag],qual-[filename],[group],qual"; }
	else if (type == "fastq") { pattern = "[filename],[group],fastq-[filename],[group],[tag],fastq"; } //make.sra assumes the [filename],[group],[tag],fastq format for the 4 column file option. If this changes, may have to modify fixMap function. 
	else { LOG(LOGERROR) << "No definition for type " + type + " output pattern.\n"; ctrlc_pressed = true; }

	return pattern;
}
//**********************************************************************************************************************
ParseFastaQCommand::ParseFastaQCommand(Settings& settings) : Command(settings) {
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;
		outputTypes["fastq"] = tempOutNames;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in ParseFastaQCommand, ParseFastaQCommand";
		exit(1);
	}
}
//**********************************************************************************************************************
ParseFastaQCommand::ParseFastaQCommand(Settings& settings, string option) : Command(settings, option) {
	abort = false; calledHelp = false; fileOption = 0; createFileGroup = false; hasIndex = false;
	split = 1;

	if (option == "help") { help(); abort = true; calledHelp = true; }
	else if (option == "citation") { citation(); abort = true; calledHelp = true; }

	else {
		vector<string> myArray = setParameters();

		OptionParser parser(option);
		map<string, string> parameters = parser.getParameters();

		ValidParameters validParameter;
		map<string, string>::iterator it;

		//check to make sure all parameters are valid for command
		for (map<string, string>::iterator it = parameters.begin(); it != parameters.end(); it++) {
			if (validParameter.isValidParameter(it->first, myArray, it->second) != true) { abort = true; }
		}

		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;
		outputTypes["fastq"] = tempOutNames;

		//if the user changes the input directory command factory will send this info to us in the output parameter 
		inputDir = validParameter.validFile(parameters, "inputdir", false);
		if (inputDir == "not found") { inputDir = ""; }
		else {
			string path;
			it = parameters.find("fastq");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["fastq"] = inputDir + it->second; }
			}

			it = parameters.find("file");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["file"] = inputDir + it->second; }
			}

			it = parameters.find("oligos");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["oligos"] = inputDir + it->second; }
			}

			it = parameters.find("group");
			//user has given a template file
			if (it != parameters.end()) {
				path = File::getPath(it->second);
				//if the user has not given a path then, add inputdir. else leave path alone.
				if (path == "") { parameters["group"] = inputDir + it->second; }
			}
		}

		//check for required parameters
		fastaQFile = validParameter.validFile(parameters, "fastq", true);
		if (fastaQFile == "not found") { fastaQFile = ""; }
		else if (fastaQFile == "not open") { fastaQFile = ""; abort = true; }
		else { inputfile = fastaQFile; }

		file = validParameter.validFile(parameters, "file", true);
		if (file == "not found") { file = ""; }
		else if (file == "not open") { file = ""; abort = true; }
		else { inputfile = file; fileOption = true; }

		if ((file == "") && (fastaQFile == "")) { LOG(INFO) << "You must provide a file or fastq option." << '\n'; abort = true; }


		oligosfile = validParameter.validFile(parameters, "oligos", true);
		if (oligosfile == "not found") { oligosfile = ""; }
		else if (oligosfile == "not open") { oligosfile = ""; abort = true; }
		else { settings.setCurrent("oligos", oligosfile); split = 2; }

		groupfile = validParameter.validFile(parameters, "group", true);
		if (groupfile == "not found") { groupfile = ""; }
		else if (groupfile == "not open") { groupfile = ""; abort = true; }
		else { settings.setCurrent("group", groupfile); split = 2; }

		if ((groupfile != "") && (oligosfile != "")) { LOG(INFO) << "You must enter ONLY ONE of the following: oligos or group." << '\n'; abort = true; }

		//if the user changes the output directory command factory will send this info to us in the output parameter 
		outputDir = validParameter.validFile(parameters, "outputdir", false);	if (outputDir == "not found") { outputDir = File::getPath(inputfile); }

		string temp;
		temp = validParameter.validFile(parameters, "fasta", false);	if (temp == "not found") { temp = "T"; }
		fasta = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "qfile", false);	if (temp == "not found") { temp = "T"; }
		qual = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "pacbio", false);	if (temp == "not found") { temp = "F"; }
		pacbio = m->isTrue(temp);

		temp = validParameter.validFile(parameters, "bdiffs", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, bdiffs);

		temp = validParameter.validFile(parameters, "pdiffs", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, pdiffs);

		temp = validParameter.validFile(parameters, "ldiffs", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, ldiffs);

		temp = validParameter.validFile(parameters, "sdiffs", false);		if (temp == "not found") { temp = "0"; }
		Utility::mothurConvert(temp, sdiffs);

		temp = validParameter.validFile(parameters, "tdiffs", false);		if (temp == "not found") { int tempTotal = pdiffs + bdiffs + ldiffs + sdiffs;  temp = toString(tempTotal); }
		Utility::mothurConvert(temp, tdiffs);

		if (tdiffs == 0) { tdiffs = bdiffs + pdiffs + ldiffs + sdiffs; }


		format = validParameter.validFile(parameters, "format", false);		if (format == "not found") { format = "sanger"; }

		if ((format != "sanger") && (format != "illumina") && (format != "illumina1.8+") && (format != "solexa")) {
			LOG(INFO) << format + " is not a valid format. Your format choices are sanger, solexa, illumina1.8+ and illumina, aborting." << '\n';
			abort = true;
		}

		if ((!fasta) && (!qual) && (file == "") && (fastaQFile == "") && (oligosfile == "")) { LOG(LOGERROR) << "no outputs selected. Aborting." << '\n'; abort = true; }
		temp = validParameter.validFile(parameters, "checkorient", false);		if (temp == "not found") { temp = "F"; }
		reorient = m->isTrue(temp);

	}
}
//**********************************************************************************************************************

int ParseFastaQCommand::execute() {
	if (abort == true) { if (calledHelp) { return 0; }  return 2; }

	vector< vector<string> > files;
	if (file != "") {
		//read file
		files = readFile();
	}

	if (ctrlc_pressed) { return 0; }

	TrimOligos* trimOligos = NULL; TrimOligos* rtrimOligos = NULL;
	pairedOligos = false; numBarcodes = 0; numPrimers = 0; numLinkers = 0; numSpacers = 0; numRPrimers = 0;
	if (oligosfile != "") {
		readOligos(oligosfile);
		//find group read belongs to
		if (pairedOligos) { trimOligos = new TrimOligos(pdiffs, bdiffs, 0, 0, oligos.getPairedPrimers(), oligos.getPairedBarcodes(), hasIndex); numBarcodes = oligos.getPairedBarcodes().size(); numPrimers = oligos.getPairedPrimers().size(); }
		else { trimOligos = new TrimOligos(pdiffs, bdiffs, ldiffs, sdiffs, oligos.getPrimers(), oligos.getBarcodes(), oligos.getReversePrimers(), oligos.getLinkers(), oligos.getSpacers());  numPrimers = oligos.getPrimers().size(); numBarcodes = oligos.getBarcodes().size(); }

		if (reorient) {
			rtrimOligos = new TrimOligos(pdiffs, bdiffs, 0, 0, oligos.getReorientedPairedPrimers(), oligos.getReorientedPairedBarcodes(), hasIndex); numBarcodes = oligos.getReorientedPairedBarcodes().size();
		}

	}
	else if (groupfile != "") { readGroup(groupfile); }

	if (file != "") {
		if (ctrlc_pressed) { return 0; }

		for (int i = 0; i < files.size(); i++) { //process each pair

			if (ctrlc_pressed) { break; }

			if ((fileOption == 2) || (fileOption == 4)) { processFile(files[i], trimOligos, rtrimOligos); }
			else if (fileOption == 3) {
				if (m->mothurCalling) {
					//add group names to fastq files and make copies
					ofstream temp, temp2;
					map<string, string> variables;
					variables["[filename]"] = File::getRootName(files[i][0]);
					variables["[group]"] = file2Group[i];
					variables["[tag]"] = "forward";
					string newffqFile = getOutputFileName("fastq", variables);
					File::openOutputFile(newffqFile, temp);		temp.close();
					File::appendFiles(files[i][0], newffqFile);
					outputNames.push_back(newffqFile); outputTypes["fastq"].push_back(newffqFile);

					variables["[filename]"] = File::getRootName(files[i][1]);
					variables["[group]"] = file2Group[i];
					variables["[tag]"] = "reverse";
					string newfrqFile = getOutputFileName("fastq", variables);
					File::openOutputFile(newfrqFile, temp2);		temp2.close();
					File::appendFiles(files[i][1], newfrqFile);
					outputNames.push_back(newfrqFile); outputTypes["fastq"].push_back(newfrqFile);
				}

				//if requested, make fasta and qual
				if (fasta || qual) { processFile(files[i], trimOligos, rtrimOligos); }  //split = 1, so no parseing by group will be done.
			}
		}
	}
	else {
		processFile(fastaQFile, trimOligos, rtrimOligos);
	}

	if (split > 1) {
		map<string, string>::iterator it;
		set<string> namesToRemove;
		for (int i = 0;i < fastqFileNames.size();i++) {
			for (int j = 0;j < fastqFileNames[0].size();j++) {
				if (fastqFileNames[i][j] != "") {
					if (namesToRemove.count(fastqFileNames[i][j]) == 0) {
						if (File::isBlank(fastqFileNames[i][j])) {
							File::remove(fastqFileNames[i][j]);
							namesToRemove.insert(fastqFileNames[i][j]);

							if (pairedOligos) { if (fileOption) { File::remove(rfastqFileNames[i][j]); namesToRemove.insert(rfastqFileNames[i][j]); } }

							if (fasta) {
								File::remove(fastaFileNames[i][j]);
								namesToRemove.insert(fastaFileNames[i][j]);

								if (pairedOligos) { if (fileOption) { File::remove(rfastaFileNames[i][j]); namesToRemove.insert(rfastaFileNames[i][j]); } }
							}

							if (qual) {
								File::remove(qualFileNames[i][j]);
								namesToRemove.insert(qualFileNames[i][j]);

								if (pairedOligos) { if (fileOption) { File::remove(rqualFileNames[i][j]); namesToRemove.insert(rqualFileNames[i][j]); } }
							}
						}
					}
				}
			}
		}

		//remove names for outputFileNames, just cleans up the output
		for (int i = 0; i < outputNames.size(); i++) {
			if (namesToRemove.count(outputNames[i]) != 0) {
				outputNames.erase(outputNames.begin() + i);
				i--;
			}
			else {
				string ending = outputNames[i].substr(outputNames[i].length() - 5);
				if (ending == "fastq") { outputTypes["fastq"].push_back(outputNames[i]); }
				else if (ending == "fasta") { outputTypes["fasta"].push_back(outputNames[i]); }
				else if (ending == ".qfile") { outputTypes["qfile"].push_back(outputNames[i]); }
			}
		}

		//ffqnoMatchFile, rfqnoMatchFile, ffnoMatchFile, rfnoMatchFile, fqnoMatchFile, rqnoMatchFile
		if (File::isBlank(ffqnoMatchFile)) { File::remove(ffqnoMatchFile); }
		else { outputNames.push_back(ffqnoMatchFile); outputTypes["fastq"].push_back(ffqnoMatchFile); }

		if (fasta) {
			if (File::isBlank(ffnoMatchFile)) { File::remove(ffnoMatchFile); }
			else { outputNames.push_back(ffnoMatchFile); outputTypes["fasta"].push_back(ffnoMatchFile); }
		}

		if (qual) {
			if (File::isBlank(fqnoMatchFile)) { File::remove(fqnoMatchFile); }
			else { outputNames.push_back(fqnoMatchFile); outputTypes["qfile"].push_back(fqnoMatchFile); }
		}

		if (pairedOligos) {
			if (fileOption) {
				if (File::isBlank(rfqnoMatchFile)) { File::remove(rfqnoMatchFile); }
				else { outputNames.push_back(rfqnoMatchFile); outputTypes["fastq"].push_back(rfqnoMatchFile); }

				if (fasta) {
					if (File::isBlank(rfnoMatchFile)) { File::remove(rfnoMatchFile); }
					else { outputNames.push_back(rfnoMatchFile); outputTypes["fasta"].push_back(rfnoMatchFile); }
				}

				if (qual) {
					if (File::isBlank(rqnoMatchFile)) { File::remove(rqnoMatchFile); }
					else { outputNames.push_back(rqnoMatchFile); outputTypes["qfile"].push_back(rqnoMatchFile); }
				}
			}
		}
	}


	if (groupfile != "") { delete groupMap; }
	else if (oligosfile != "") { delete trimOligos; if (reorient) { delete rtrimOligos; } }


	if (ctrlc_pressed) { for (int i = 0; i < outputNames.size(); i++) { File::remove(outputNames[i]); }  outputTypes.clear(); outputNames.clear();  return 0; }

	//set fasta file as new current fastafile
	string current = "";
	itTypes = outputTypes.find("fasta");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("fasta", current); }
	}

	itTypes = outputTypes.find("qfile");
	if (itTypes != outputTypes.end()) {
		if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; settings.setCurrent("qual", current); }
	}

	LOG(INFO) << '\n' << "Output File Names: " << '\n';
	for (int i = 0; i < outputNames.size(); i++) { LOG(INFO) << outputNames[i] << '\n'; }
	LOG(INFO) << "";

	return 0;
}
//**********************************************************************************************************************
//assumes file option was used
int ParseFastaQCommand::processFile(vector<string> files, TrimOligos*& trimOligos, TrimOligos*& rtrimOligos) {
	string inputfile = files[0]; string inputReverse = files[1];

	//open Output Files
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
	string ffastaFile = getOutputFileName("fasta", variables);
	string fqualFile = getOutputFileName("qfile", variables);
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputReverse));
	string rfastaFile = getOutputFileName("fasta", variables);
	string rqualFile = getOutputFileName("qfile", variables);
	ofstream outfFasta, outfQual, outrFasta, outrQual;

	if (fasta) { File::openOutputFile(ffastaFile, outfFasta);  outputNames.push_back(ffastaFile); outputTypes["fasta"].push_back(ffastaFile);	File::openOutputFile(rfastaFile, outrFasta);  outputNames.push_back(rfastaFile); outputTypes["fasta"].push_back(rfastaFile); }
	if (qual) { File::openOutputFile(fqualFile, outfQual);	outputNames.push_back(fqualFile);  outputTypes["qfile"].push_back(fqualFile);	File::openOutputFile(rqualFile, outrQual);	outputNames.push_back(rqualFile);  outputTypes["qfile"].push_back(rqualFile); }

	ifstream inf;
	File::openInputFile(inputfile, inf);

	ifstream inr;
	File::openInputFile(inputReverse, inr);

	ifstream inFIndex, inRIndex;
	if (files[2] != "") { File::openInputFile(files[2], inFIndex); }
	if (files[3] != "") { File::openInputFile(files[3], inRIndex); }


	//fill convert table - goes from solexa to sanger. Used fq_all2std.pl as a reference.
	for (int i = -64; i < 65; i++) {
		char temp = (char)((int)(33 + 10 * log(1 + pow(10, (i / 10.0))) / log(10) + 0.499));
		convertTable.push_back(temp);
	}

	int count = 0;
	while (!inf.eof() && !inr.eof()) {

		if (ctrlc_pressed) { break; }

		bool ignoref, ignorer;
		fastqRead2 thisfRead = readFastq(inf, ignoref);
		fastqRead2 thisrRead = readFastq(inr, ignorer);

		if (!ignoref && !ignorer) {
			vector<int> fqualScores;
			vector<int> rqualScores;
			if (qual) {
				fqualScores = convertQual(thisfRead.quality);
				outfQual << ">" << thisfRead.seq.getName() << endl;
				for (int i = 0; i < fqualScores.size(); i++) { outfQual << fqualScores[i] << " "; }
				outfQual << endl;

				rqualScores = convertQual(thisrRead.quality);
				outrQual << ">" << thisrRead.seq.getName() << endl;
				for (int i = 0; i < rqualScores.size(); i++) { outrQual << rqualScores[i] << " "; }
				outrQual << endl;
			}

			if (ctrlc_pressed) { break; }

			if (pacbio) {
				if (!qual) { rqualScores = convertQual(thisrRead.quality); fqualScores = convertQual(thisfRead.quality); } //convert if not done
				string sequence = thisfRead.seq.getAligned();
				for (int i = 0; i < fqualScores.size(); i++) {
					if (fqualScores[i] == 0) { sequence[i] = 'N'; }
				}
				thisfRead.seq.setAligned(sequence);

				sequence = thisrRead.seq.getAligned();
				for (int i = 0; i < rqualScores.size(); i++) {
					if (rqualScores[i] == 0) { sequence[i] = 'N'; }
				}
				thisrRead.seq.setAligned(sequence);
			}

			//print sequence info to files
			if (fasta) { thisfRead.seq.printSequence(outfFasta); thisrRead.seq.printSequence(outrFasta); }

			if (split > 1) {

				Sequence findexBarcode("findex", "NONE");  Sequence rindexBarcode("rindex", "NONE");
				if (fileOption == 4) {
					bool ignorefi, ignoreri;

					if (files[2] != "") {
						fastqRead2 thisfiRead = readFastq(inFIndex, ignorefi);
						if (!ignorefi) { findexBarcode.setAligned(thisfiRead.seq.getAligned()); }
					}

					if (files[3] != "") {
						fastqRead2 thisriRead = readFastq(inRIndex, ignoreri);
						if (!ignoreri) { rindexBarcode.setAligned(thisriRead.seq.getAligned()); }
					}
				}

				int barcodeIndex, primerIndex, trashCodeLength;
				if (oligosfile != "") {
					if ((files[2] != "") || (files[3] != "")) {
						Sequence tempF = thisfRead.seq;
						Sequence tempR = thisrRead.seq;
						thisfRead.seq = findexBarcode;
						thisrRead.seq = rindexBarcode;
						trashCodeLength = findGroup(thisfRead, thisrRead, barcodeIndex, primerIndex, trimOligos, rtrimOligos, numBarcodes, numPrimers);
						thisfRead.seq = tempF;
						thisrRead.seq = tempR;
					}
					else {
						trashCodeLength = findGroup(thisfRead, thisrRead, barcodeIndex, primerIndex, trimOligos, rtrimOligos, numBarcodes, numPrimers);
					}
				}
				else if (groupfile != "") { trashCodeLength = findGroup(thisfRead, barcodeIndex, primerIndex, "groupMode"); }
				else { LOG(LOGERROR) << "uh oh, we shouldn't be here...\n"; }

				if (trashCodeLength == 0) {
					ofstream out;
					File::openOutputFileAppend(fastqFileNames[barcodeIndex][primerIndex], out);
					out << thisfRead.wholeRead;
					out.close();

					ofstream out2;
					File::openOutputFileAppend(rfastqFileNames[barcodeIndex][primerIndex], out2);
					out2 << thisrRead.wholeRead;
					out2.close();

					//print no match fasta, if wanted
					if (fasta) {
						ofstream outf, outr;
						File::openOutputFileAppend(fastaFileNames[barcodeIndex][primerIndex], outf);
						thisfRead.seq.printSequence(outf);
						outf.close();

						File::openOutputFileAppend(rfastaFileNames[barcodeIndex][primerIndex], outr);
						thisrRead.seq.printSequence(outr);
						outr.close();
					}

					//print no match quality parse, if wanted
					if (qual) {
						ofstream outq, outq2;
						File::openOutputFileAppend(qualFileNames[barcodeIndex][primerIndex], outq);
						outq << ">" << thisfRead.seq.getName() << endl;
						for (int i = 0; i < fqualScores.size(); i++) { outq << fqualScores[i] << " "; }
						outq << endl;
						outq.close();

						File::openOutputFileAppend(rqualFileNames[barcodeIndex][primerIndex], outq2);
						outq2 << ">" << thisrRead.seq.getName() << endl;
						for (int i = 0; i < rqualScores.size(); i++) { outq2 << rqualScores[i] << " "; }
						outq2 << endl;
						outq2.close();
					}

				}
				else {
					//print no match fastq
					ofstream out, out2;
					File::openOutputFileAppend(ffqnoMatchFile, out);
					out << thisfRead.wholeRead;
					out.close();

					File::openOutputFileAppend(rfqnoMatchFile, out2);
					out2 << thisrRead.wholeRead;
					out2.close();

					//print no match fasta, if wanted
					if (fasta) {
						ofstream outf, outr;
						File::openOutputFileAppend(ffnoMatchFile, outf);
						thisfRead.seq.printSequence(outf);
						outf.close();

						File::openOutputFileAppend(rfnoMatchFile, outr);
						thisrRead.seq.printSequence(outr);
						outr.close();
					}

					//print no match quality parse, if wanted
					if (qual) {
						ofstream outq, outq2;
						File::openOutputFileAppend(fqnoMatchFile, outq);
						outq << ">" << thisfRead.seq.getName() << endl;
						for (int i = 0; i < fqualScores.size(); i++) { outq << fqualScores[i] << " "; }
						outq << endl;
						outq.close();

						File::openOutputFileAppend(rqnoMatchFile, outq2);
						outq2 << ">" << thisrRead.seq.getName() << endl;
						for (int i = 0; i < rqualScores.size(); i++) { outq2 << rqualScores[i] << " "; }
						outq2 << endl;
						outq2.close();
					}
				}
			}
			//report progress
			if ((count + 1) % 10000 == 0) { LOG(INFO) << toString(count + 1) << '\n'; }
			count++;
		}
	}

	inf.close(); inr.close();
	if (files[2] != "") { inFIndex.close(); }
	if (files[3] != "") { inRIndex.close(); }

	if (fasta) { outfFasta.close(); outrFasta.close(); }
	if (qual) { outfQual.close();	outrQual.close(); }

	//report progress
	if (!ctrlc_pressed) { if ((count) % 10000 != 0) { LOG(INFO) << toString(count) << '\n'; } }


	return 0;
}
//**********************************************************************************************************************
int ParseFastaQCommand::processFile(string inputfile, TrimOligos*& trimOligos, TrimOligos*& rtrimOligos) {

	//open Output Files
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
	string fastaFile = getOutputFileName("fasta", variables);
	string qualFile = getOutputFileName("qfile", variables);
	ofstream outFasta, outQual;

	//fasta and quality files for whole input file
	if (fasta) { File::openOutputFile(fastaFile, outFasta);  outputNames.push_back(fastaFile); outputTypes["fasta"].push_back(fastaFile); }
	if (qual) { File::openOutputFile(qualFile, outQual);	outputNames.push_back(qualFile);  outputTypes["qfile"].push_back(qualFile); }

	ifstream in;
	File::openInputFile(inputfile, in);

	//fill convert table - goes from solexa to sanger. Used fq_all2std.pl as a reference.
	for (int i = -64; i < 65; i++) {
		char temp = (char)((int)(33 + 10 * log(1 + pow(10, (i / 10.0))) / log(10) + 0.499));
		convertTable.push_back(temp);
	}


	int count = 0;
	while (!in.eof()) {

		if (ctrlc_pressed) { break; }

		bool ignore;
		fastqRead2 thisRead = readFastq(in, ignore);

		if (!ignore) {
			vector<int> qualScores;
			if (qual) {
				qualScores = convertQual(thisRead.quality);
				outQual << ">" << thisRead.seq.getName() << endl;
				for (int i = 0; i < qualScores.size(); i++) { outQual << qualScores[i] << " "; }
				outQual << endl;
			}

			if (ctrlc_pressed) { break; }

			if (pacbio) {
				if (!qual) { qualScores = convertQual(thisRead.quality); } //convert if not done
				string sequence = thisRead.seq.getAligned();
				for (int i = 0; i < qualScores.size(); i++) {
					if (qualScores[i] == 0) { sequence[i] = 'N'; }
				}
				thisRead.seq.setAligned(sequence);
			}

			//print sequence info to files
			if (fasta) { thisRead.seq.printSequence(outFasta); }

			if (split > 1) {
				int barcodeIndex, primerIndex, trashCodeLength;
				if (oligosfile != "") { trashCodeLength = findGroup(thisRead, barcodeIndex, primerIndex, trimOligos, rtrimOligos, numBarcodes, numPrimers); }
				else if (groupfile != "") { trashCodeLength = findGroup(thisRead, barcodeIndex, primerIndex, "groupMode"); }
				else { LOG(LOGERROR) << "uh oh, we shouldn't be here...\n"; }

				if (trashCodeLength == 0) { //files in here are per group
					//print fastq to barcode and primer match
					ofstream out;
					File::openOutputFileAppend(fastqFileNames[barcodeIndex][primerIndex], out);
					out << thisRead.wholeRead;
					out.close();

					//print fasta match if wanted
					if (fasta) {
						ofstream outf;
						File::openOutputFileAppend(fastaFileNames[barcodeIndex][primerIndex], outf);
						thisRead.seq.printSequence(outf);
						outf.close();
					}

					//print qual match, if wanted
					if (qual) {
						ofstream outq;
						File::openOutputFileAppend(qualFileNames[barcodeIndex][primerIndex], outq);
						outq << ">" << thisRead.seq.getName() << endl;
						for (int i = 0; i < qualScores.size(); i++) { outq << qualScores[i] << " "; }
						outq.close();
					}
				}
				else {
					//print no match fastq
					ofstream out;
					File::openOutputFileAppend(ffqnoMatchFile, out);
					out << thisRead.wholeRead;
					out.close();

					//print no match fasta, if wanted
					if (fasta) {
						ofstream outf;
						File::openOutputFileAppend(ffnoMatchFile, outf);
						thisRead.seq.printSequence(outf);
						outf.close();
					}

					//print no match quality parse, if wanted
					if (qual) {
						ofstream outq;
						File::openOutputFileAppend(fqnoMatchFile, outq);
						outq << ">" << thisRead.seq.getName() << endl;
						for (int i = 0; i < qualScores.size(); i++) { outq << qualScores[i] << " "; }
						outq.close();
					}
				}
			}
			//report progress
			if ((count + 1) % 10000 == 0) { LOG(INFO) << toString(count + 1) << '\n'; }
			count++;
		}
	}

	in.close();
	if (fasta) { outFasta.close(); }
	if (qual) { outQual.close(); }

	//report progress
	if (!ctrlc_pressed) { if ((count) % 10000 != 0) { LOG(INFO) << toString(count) << '\n'; } }


	return 0;
}
//**********************************************************************************************************************
fastqRead2 ParseFastaQCommand::readFastq(ifstream& in, bool& ignore) {
	ignore = false;
	string wholeRead = "";

	//read sequence name
	string line = File::getline(in); File::gobble(in); if (split > 1) { wholeRead += line + "\n"; }
	vector<string> pieces = m->splitWhiteSpace(line);
	string name = "";  if (pieces.size() != 0) { name = pieces[0]; }
	if (name == "") { LOG(WARNING) << "Blank fasta name, ignoring read." << '\n'; ignore = true; }
	else if (name[0] != '@') { LOG(WARNING) << "reading " + name + " expected a name with @ as a leading character, ignoring read." << '\n'; ignore = true; }
	else { name = name.substr(1); }

	//read sequence
	string sequence = File::getline(in); File::gobble(in); if (split > 1) { wholeRead += sequence + "\n"; }
	if (sequence == "") { LOG(WARNING) << "missing sequence for " + name + ", ignoring."; ignore = true; }

	//read sequence name
	line = File::getline(in); File::gobble(in); if (split > 1) { wholeRead += line + "\n"; }
	pieces = m->splitWhiteSpace(line);
	string name2 = "";  if (pieces.size() != 0) { name2 = pieces[0]; }
	if (name2 == "") { LOG(WARNING) << "expected a name with + as a leading character, ignoring."; ignore = true; }
	else if (name2[0] != '+') { LOG(WARNING) << "reading " + name2 + " expected a name with + as a leading character, ignoring."; ignore = true; }
	else { name2 = name2.substr(1); if (name2 == "") { name2 = name; } }


	//read quality scores
	string quality = File::getline(in); File::gobble(in); if (split > 1) { wholeRead += quality + "\n"; }
	if (quality == "") { LOG(WARNING) << "missing quality for " + name2 + ", ignoring."; ignore = true; }

	//sanity check sequence length and number of quality scores match
	if (name2 != "") { if (name != name2) { LOG(WARNING) << "names do not match. read " + name + " for fasta and " + name2 + " for quality, ignoring."; ignore = true; } }
	if (quality.length() != sequence.length()) { LOG(WARNING) << "Lengths do not match for sequence " + name + ". Read " + toString(sequence.length()) + " characters for fasta and " + toString(quality.length()) + " characters for quality scores, ignoring read."; ignore = true; }

	m->checkName(name);
	Sequence seq(name, sequence);
	fastqRead2 read(seq, quality, wholeRead);

	if (app.isDebug) { LOG(DEBUG) << "" + read.seq.getName() + " " + read.seq.getAligned() + " " + quality + "\n"; }

	return read;
}

//**********************************************************************************************************************
vector<int> ParseFastaQCommand::convertQual(string qual) {
	vector<int> qualScores;

	bool negativeScores = false;

	for (int i = 0; i < qual.length(); i++) {

		int temp = 0;
		temp = int(qual[i]);
		if (format == "illumina") {
			temp -= 64; //char '@'
		}
		else if (format == "illumina1.8+") {
			temp -= int('!'); //char '!'
		}
		else if (format == "solexa") {
			temp = int(convertTable[temp]); //convert to sanger
			temp -= int('!'); //char '!'
		}
		else {
			temp -= int('!'); //char '!'
		}
		if (temp < -5) { negativeScores = true; }
		qualScores.push_back(temp);
	}

	if (negativeScores) { LOG(LOGERROR) << "finding negative quality scores, do you have the right format selected? http://en.wikipedia.org/wiki/FASTQ_format#Encoding \n";  ctrlc_pressed = true; }

	return qualScores;
}
//**********************************************************************************************************************
int ParseFastaQCommand::findGroup(fastqRead2 thisRead, int& barcode, int& primer, TrimOligos*& trimOligos, TrimOligos*& rtrimOligos, int numBarcodes, int numPrimers) {
	int success = 1;
	string trashCode = "";
	int currentSeqsDiffs = 0;

	Sequence currSeq(thisRead.seq.getName(), thisRead.seq.getAligned());
	QualityScores currQual; currQual.setScores(convertQual(thisRead.quality));

	//for reorient
	Sequence savedSeq(currSeq.getName(), currSeq.getAligned());
	QualityScores savedQual(currQual.getName(), currQual.getScores());

	if (numLinkers != 0) {
		success = trimOligos->stripLinker(currSeq, currQual);
		if (success > ldiffs) { trashCode += 'k'; }
		else { currentSeqsDiffs += success; }

	}

	if (numBarcodes != 0) {
		vector<int> results = trimOligos->stripBarcode(currSeq, currQual, barcode);
		if (pairedOligos) { success = results[0] + results[2]; }
		else { success = results[0]; }
		if (success > bdiffs) { trashCode += 'b'; }
		else { currentSeqsDiffs += success; }
	}

	if (numSpacers != 0) {
		success = trimOligos->stripSpacer(currSeq, currQual);
		if (success > sdiffs) { trashCode += 's'; }
		else { currentSeqsDiffs += success; }

	}

	if (numPrimers != 0) {
		vector<int> results = trimOligos->stripForward(currSeq, currQual, primer, true);
		if (pairedOligos) { success = results[0] + results[2]; }
		else { success = results[0]; }
		if (success > pdiffs) { trashCode += 'f'; }
		else { currentSeqsDiffs += success; }
	}

	if (numRPrimers != 0) {
		vector<int> results = trimOligos->stripReverse(currSeq, currQual);
		success = results[0];
		if (success > pdiffs) { trashCode += 'r'; }
		else { currentSeqsDiffs += success; }
	}

	if (currentSeqsDiffs > tdiffs) { trashCode += 't'; }

	if (reorient && (trashCode != "")) { //if you failed and want to check the reverse
		int thisSuccess = 0;
		string thisTrashCode = "";
		int thisCurrentSeqsDiffs = 0;

		int thisBarcodeIndex = 0;
		int thisPrimerIndex = 0;
		//cout << currSeq.getName() << '\t' << savedSeq.getUnaligned() << endl;
		if (numBarcodes != 0) {
			vector<int> results = rtrimOligos->stripBarcode(savedSeq, savedQual, thisBarcodeIndex);
			if (pairedOligos) { thisSuccess = results[0] + results[2]; }
			else { thisSuccess = results[0]; }
			if (thisSuccess > bdiffs) { thisTrashCode += "b"; }
			else { thisCurrentSeqsDiffs += thisSuccess; }
		}
		//cout << currSeq.getName() << '\t' << savedSeq.getUnaligned() << endl;
		if (numPrimers != 0) {
			vector<int> results = rtrimOligos->stripForward(savedSeq, savedQual, thisPrimerIndex, true);
			if (pairedOligos) { thisSuccess = results[0] + results[2]; }
			else { thisSuccess = results[0]; }
			if (thisSuccess > pdiffs) { thisTrashCode += "f"; }
			else { thisCurrentSeqsDiffs += thisSuccess; }
		}

		if (thisCurrentSeqsDiffs > tdiffs) { thisTrashCode += 't'; }

		if (thisTrashCode == "") {
			trashCode = thisTrashCode;
			success = thisSuccess;
			currentSeqsDiffs = thisCurrentSeqsDiffs;
			barcode = thisBarcodeIndex;
			primer = thisPrimerIndex;
			//savedSeq.reverseComplement();
			//currSeq.setAligned(savedSeq.getAligned());
			//savedQual.flipQScores();
			//currQual.setScores(savedQual.getScores());
		}
		else { trashCode += "(" + thisTrashCode + ")"; }
	}

	if (trashCode.length() == 0) { //is this sequence in the ignore group
		string thisGroup = oligos.getGroupName(barcode, primer);

		int pos = thisGroup.find("ignore");
		if (pos != string::npos) { trashCode += "i"; }
	}


	return trashCode.length();
}
//**********************************************************************************************************************
int ParseFastaQCommand::findGroup(fastqRead2 thisRead, int& barcode, int& primer, string groupMode) {
	string trashCode = "";
	primer = 0;

	string group = groupMap->getGroup(thisRead.seq.getName());
	if (group == "not found") { trashCode += "g"; } //scrap for group
	else { barcode = GroupToFile[group]; }

	return trashCode.length();
}
//**********************************************************************************************************************
int ParseFastaQCommand::findGroup(fastqRead2 thisfRead, fastqRead2 thisrRead, int& barcode, int& primer, TrimOligos*& trimOligos, TrimOligos*& rtrimOligos, int numBarcodes, int numPrimers) {
	int success = 1;
	string trashCode = "";
	int currentSeqsDiffs = 0;

	Sequence fcurrSeq(thisfRead.seq.getName(), thisfRead.seq.getAligned());
	QualityScores fcurrQual; fcurrQual.setScores(convertQual(thisfRead.quality));
	Sequence rcurrSeq(thisrRead.seq.getName(), thisrRead.seq.getAligned());
	QualityScores rcurrQual; rcurrQual.setScores(convertQual(thisrRead.quality));

	//for reorient
	Sequence fsavedSeq(fcurrSeq.getName(), fcurrSeq.getAligned());
	QualityScores fsavedQual(fcurrQual.getName(), fcurrQual.getScores());
	Sequence rsavedSeq(rcurrSeq.getName(), rcurrSeq.getAligned());
	QualityScores rsavedQual(rcurrQual.getName(), rcurrQual.getScores());

	if (numBarcodes != 0) {
		vector<int> results = trimOligos->stripBarcode(fcurrSeq, rcurrSeq, fcurrQual, rcurrQual, barcode);
		if (pairedOligos) { success = results[0] + results[2]; }
		else { success = results[0]; }
		if (success > bdiffs) { trashCode += 'b'; }
		else { currentSeqsDiffs += success; }
	}

	if (numPrimers != 0) {
		vector<int> results = trimOligos->stripForward(fcurrSeq, rcurrSeq, fcurrQual, rcurrQual, primer);
		if (pairedOligos) { success = results[0] + results[2]; }
		else { success = results[0]; }
		if (success > pdiffs) { trashCode += 'f'; }
		else { currentSeqsDiffs += success; }
	}

	if (currentSeqsDiffs > tdiffs) { trashCode += 't'; }

	if (reorient && (trashCode != "")) { //if you failed and want to check the reverse
		int thisSuccess = 0;
		string thisTrashCode = "";
		int thisCurrentSeqsDiffs = 0;

		int thisBarcodeIndex = 0;
		int thisPrimerIndex = 0;

		if (numBarcodes != 0) {

			vector<int> results = rtrimOligos->stripBarcode(fsavedSeq, rsavedSeq, fsavedQual, rsavedQual, thisBarcodeIndex);
			if (pairedOligos) { thisSuccess = results[0] + results[2]; }
			else { thisSuccess = results[0]; }

			if (thisSuccess > bdiffs) { thisTrashCode += 'b'; }
			else { thisCurrentSeqsDiffs += thisSuccess; }
		}

		if (numPrimers != 0) {

			vector<int> results = rtrimOligos->stripForward(fsavedSeq, rsavedSeq, fsavedQual, rsavedQual, thisPrimerIndex);
			if (pairedOligos) { thisSuccess = results[0] + results[2]; }
			else { thisSuccess = results[0]; }

			if (thisSuccess > pdiffs) { thisTrashCode += 'f'; }
			else { thisCurrentSeqsDiffs += thisSuccess; }
		}

		if (thisCurrentSeqsDiffs > tdiffs) { thisTrashCode += 't'; }

		if (thisTrashCode == "") {
			trashCode = thisTrashCode;
			success = thisSuccess;
			currentSeqsDiffs = thisCurrentSeqsDiffs;
			barcode = thisBarcodeIndex;
			primer = thisPrimerIndex;
			//fsavedSeq.reverseComplement();
			//rsavedSeq.reverseComplement();
			//fcurrSeq.setAligned(fsavedSeq.getAligned());
			//rcurrSeq.setAligned(rsavedSeq.getAligned());
			//fsavedQual.flipQScores(); rsavedQual.flipQScores();
		   // fcurrQual.setScores(fsavedQual.getScores()); rcurrQual.setScores(rsavedQual.getScores());

		}
		else { trashCode += "(" + thisTrashCode + ")"; }
	}

	if (trashCode.length() == 0) { //is this sequence in the ignore group
		string thisGroup = oligos.getGroupName(barcode, primer);

		int pos = thisGroup.find("ignore");
		if (pos != string::npos) { trashCode += "i"; }
	}

	return trashCode.length();
}
//**********************************************************************************************************************
/*

 file option 1

 ffastqfile1 rfastqfile1
 ffastqfile2 rfastqfile2
 ...

 file option 2

 group ffastqfile  rfastqfile
 group ffastqfile  rfastqfile
 group ffastqfile  rfastqfile
 ...

 file option 3

 My.forward.fastq My.reverse.fastq none My.rindex.fastq //none is an option is no forward or reverse index file

 */

 //lines can be 2, 3, or 4 columns
 // forward.fastq reverse.fastq -> 2 column
 // groupName forward.fastq reverse.fastq -> 3 column
 // forward.fastq reverse.fastq forward.index.fastq  reverse.index.fastq  -> 4 column
 // forward.fastq reverse.fastq none  reverse.index.fastq  -> 4 column
 // forward.fastq reverse.fastq forward.index.fastq  none  -> 4 column
vector< vector<string> > ParseFastaQCommand::readFile() {
	vector< vector<string> > files;
	string forward, reverse, findex, rindex;

	ifstream in;
	File::openInputFile(inputfile, in);

	while (!in.eof()) {

		if (ctrlc_pressed) { return files; }

		string line = File::getline(in);  File::gobble(in);
		vector<string> pieces = m->splitWhiteSpace(line);

		string group = "";
		if (pieces.size() == 2) {
			forward = pieces[0];
			reverse = pieces[1];
			group = "";
			findex = "";
			rindex = "";
			fileOption = 2;
		}
		else if (pieces.size() == 3) {
			if (oligosfile != "") { LOG(LOGERROR) << "You cannot have an oligosfile and 3 column file option at the same time. Aborting. \n"; ctrlc_pressed = true; }
			if (groupfile != "") { LOG(LOGERROR) << "You cannot have an groupfile and 3 column file option at the same time. Aborting. \n"; ctrlc_pressed = true; }
			group = pieces[0];
			forward = pieces[1];
			reverse = pieces[2];
			findex = "";
			rindex = "";
			createFileGroup = true;
			fileOption = 3;
		}
		else if (pieces.size() == 4) {
			if (oligosfile == "") { LOG(LOGERROR) << "You must have an oligosfile with the index file option. Aborting. \n"; ctrlc_pressed = true; }
			forward = pieces[0];
			reverse = pieces[1];
			findex = pieces[2];
			rindex = pieces[3];
			fileOption = 4;
			hasIndex = true;
			if ((findex == "none") || (findex == "NONE")) { findex = ""; }
			if ((rindex == "none") || (rindex == "NONE")) { rindex = ""; }
		}
		else {
			LOG(LOGERROR) << "file lines can be 2, 3, or 4 columns. The forward fastq files in the first column and their matching reverse fastq files in the second column, or a groupName then forward fastq file and reverse fastq file, or forward fastq file then reverse fastq then forward index and reverse index file.  If you only have one index file add 'none' for the other one. \n"; ctrlc_pressed = true;
		}

		if (app.isDebug) { LOG(DEBUG) << "group = " + group + ", forward = " + forward + ", reverse = " + reverse + ", forwardIndex = " + findex + ", reverseIndex = " + rindex + ".\n"; }

		if (inputDir != "") {
			string path = File::getPath(forward);
			if (path == "") { forward = inputDir + forward; }

			path = File::getPath(reverse);
			if (path == "") { reverse = inputDir + reverse; }

			if (findex != "") {
				path = File::getPath(findex);
				if (path == "") { findex = inputDir + findex; }
			}

			if (rindex != "") {
				path = File::getPath(rindex);
				if (path == "") { rindex = inputDir + rindex; }
			}
		}

		//check to make sure both are able to be opened
		ifstream in2;
		int openForward = File::openInputFile(forward, in2, "noerror");

		//if you can't open it, try default location
		if (openForward == 1) {
			if (settings.getDefaultPath() != "") { //default path is set
				string tryPath = settings.getDefaultPath() + File::getSimpleName(forward);
				LOG(INFO) << "Unable to open " + forward + ". Trying default " + tryPath << '\n';
				ifstream in3;
				openForward = File::openInputFile(tryPath, in3, "noerror");
				in3.close();
				forward = tryPath;
			}
		}

		//if you can't open it, try output location
		if (openForward == 1) {
			if (settings.getOutputDir() != "") { //default path is set
				string tryPath = settings.getOutputDir() + File::getSimpleName(forward);
				LOG(INFO) << "Unable to open " + forward + ". Trying output directory " + tryPath << '\n';
				ifstream in4;
				openForward = File::openInputFile(tryPath, in4, "noerror");
				forward = tryPath;
				in4.close();
			}
		}

		if (openForward == 1) { //can't find it
			LOG(WARNING) << "can't find " + forward + ", ignoring pair.\n";
		}
		else { in2.close(); }

		ifstream in3;
		int openReverse = File::openInputFile(reverse, in3, "noerror");

		//if you can't open it, try default location
		if (openReverse == 1) {
			if (settings.getDefaultPath() != "") { //default path is set
				string tryPath = settings.getDefaultPath() + File::getSimpleName(reverse);
				LOG(INFO) << "Unable to open " + reverse + ". Trying default " + tryPath << '\n';
				ifstream in3;
				openReverse = File::openInputFile(tryPath, in3, "noerror");
				in3.close();
				reverse = tryPath;
			}
		}

		//if you can't open it, try output location
		if (openReverse == 1) {
			if (settings.getOutputDir() != "") { //default path is set
				string tryPath = settings.getOutputDir() + File::getSimpleName(reverse);
				LOG(INFO) << "Unable to open " + reverse + ". Trying output directory " + tryPath << '\n';
				ifstream in4;
				openReverse = File::openInputFile(tryPath, in4, "noerror");
				reverse = tryPath;
				in4.close();
			}
		}

		if (openReverse == 1) { //can't find it
			LOG(WARNING) << "can't find " + reverse + ", ignoring pair.\n";
		}
		else { in3.close(); }

		int openFindex = 0;
		if (findex != "") {
			ifstream in4;
			openFindex = File::openInputFile(findex, in4, "noerror"); in4.close();

			//if you can't open it, try default location
			if (openFindex == 1) {
				if (settings.getDefaultPath() != "") { //default path is set
					string tryPath = settings.getDefaultPath() + File::getSimpleName(findex);
					LOG(INFO) << "Unable to open " + findex + ". Trying default " + tryPath << '\n';
					ifstream in5;
					openFindex = File::openInputFile(tryPath, in5, "noerror");
					in5.close();
					findex = tryPath;
				}
			}

			//if you can't open it, try output location
			if (openFindex == 1) {
				if (settings.getOutputDir() != "") { //default path is set
					string tryPath = settings.getOutputDir() + File::getSimpleName(findex);
					LOG(INFO) << "Unable to open " + findex + ". Trying output directory " + tryPath << '\n';
					ifstream in6;
					openFindex = File::openInputFile(tryPath, in6, "noerror");
					findex = tryPath;
					in6.close();
				}
			}

			if (openFindex == 1) { //can't find it
				LOG(WARNING) << "can't find " + findex + ", ignoring pair.\n";
			}
		}

		int openRindex = 0;
		if (rindex != "") {
			ifstream in7;
			openRindex = File::openInputFile(rindex, in7, "noerror"); in7.close();

			//if you can't open it, try default location
			if (openRindex == 1) {
				if (settings.getDefaultPath() != "") { //default path is set
					string tryPath = settings.getDefaultPath() + File::getSimpleName(rindex);
					LOG(INFO) << "Unable to open " + rindex + ". Trying default " + tryPath << '\n';
					ifstream in8;
					openRindex = File::openInputFile(tryPath, in8, "noerror");
					in8.close();
					rindex = tryPath;
				}
			}

			//if you can't open it, try output location
			if (openRindex == 1) {
				if (settings.getOutputDir() != "") { //default path is set
					string tryPath = settings.getOutputDir() + File::getSimpleName(rindex);
					LOG(INFO) << "Unable to open " + rindex + ". Trying output directory " + tryPath << '\n';
					ifstream in9;
					openRindex = File::openInputFile(tryPath, in9, "noerror");
					rindex = tryPath;
					in9.close();
				}
			}

			if (openRindex == 1) { //can't find it
				LOG(WARNING) << "can't find " + rindex + ", ignoring pair.\n";
			}
		}


		if ((openForward != 1) && (openReverse != 1) && (openFindex != 1) && (openRindex != 1)) { //good pair
			file2Group[files.size()] = group;
			vector<string> pair;
			pair.push_back(forward);
			pair.push_back(reverse);
			pair.push_back(findex);
			pair.push_back(rindex);
			if (((findex != "") || (rindex != "")) && (oligosfile == "")) { LOG(LOGERROR) << "You need to provide an oligos file if you are going to use an index file.\n"; ctrlc_pressed = true; }
			files.push_back(pair);
		}
	}
	in.close();

	return files;
}
//***************************************************************************************************************

bool ParseFastaQCommand::readOligos(string oligoFile) {
	bool allBlank = false;

	if (fileOption) { oligos.read(oligosfile, false); } // like make.contigs
	else { oligos.read(oligosfile); }

	if (ctrlc_pressed) { return false; } //error in reading oligos

	if (oligos.hasPairedPrimers() || oligos.hasPairedBarcodes()) {
		pairedOligos = true;
		numPrimers = oligos.getPairedPrimers().size();
		numBarcodes = oligos.getPairedBarcodes().size();
	}
	else {
		pairedOligos = false;
		numPrimers = oligos.getPrimers().size();
		numBarcodes = oligos.getBarcodes().size();
	}

	numLinkers = oligos.getLinkers().size();
	numSpacers = oligos.getSpacers().size();
	numRPrimers = oligos.getReversePrimers().size();

	vector<string> groupNames = oligos.getGroupNames();
	if (groupNames.size() == 0) { allBlank = true; }

	if (ctrlc_pressed) { return false; }

	fastqFileNames.resize(oligos.getBarcodeNames().size());
	for (int i = 0;i < fastqFileNames.size();i++) {
		for (int j = 0;j < oligos.getPrimerNames().size();j++) { fastqFileNames[i].push_back(""); }
	}
	if (pairedOligos) { rfastqFileNames = fastqFileNames; }

	if (qual) { qualFileNames = fastqFileNames;	 if (pairedOligos) { rqualFileNames = fastqFileNames; } }
	if (fasta) { fastaFileNames = fastqFileNames; if (pairedOligos) { rfastaFileNames = fastqFileNames; } }

	set<string> uniqueNames; //used to cleanup outputFileNames
	if (pairedOligos) {
		map<int, oligosPair> barcodes = oligos.getPairedBarcodes();
		map<int, oligosPair> primers = oligos.getPairedPrimers();
		for (map<int, oligosPair>::iterator itBar = barcodes.begin();itBar != barcodes.end();itBar++) {
			for (map<int, oligosPair>::iterator itPrimer = primers.begin();itPrimer != primers.end(); itPrimer++) {

				string primerName = oligos.getPrimerName(itPrimer->first);
				string barcodeName = oligos.getBarcodeName(itBar->first);

				if ((primerName == "ignore") || (barcodeName == "ignore")) {} //do nothing
				else if ((primerName == "") && (barcodeName == "")) {} //do nothing
				else {
					string comboGroupName = "";
					string comboName = "";

					if (primerName == "") {
						comboGroupName = barcodeName;
					}
					else {
						if (barcodeName == "") {
							comboGroupName = primerName;
						}
						else {
							comboGroupName = barcodeName + "." + primerName;
						}
					}

					if (((itPrimer->second).forward + (itPrimer->second).reverse) == "") {
						if ((itBar->second).forward != "NONE") { comboName += (itBar->second).forward; }
						if ((itBar->second).reverse != "NONE") {
							if (comboName == "") { comboName += (itBar->second).reverse; }
							else { comboName += ("." + (itBar->second).reverse); }
						}
					}
					else {
						if (((itBar->second).forward + (itBar->second).reverse) == "") {
							if ((itPrimer->second).forward != "NONE") { comboName += (itPrimer->second).forward; }
							if ((itPrimer->second).reverse != "NONE") {
								if (comboName == "") { comboName += (itPrimer->second).reverse; }
								else { comboName += ("." + (itPrimer->second).reverse); }
							}
						}
						else {
							if ((itBar->second).forward != "NONE") { comboName += (itBar->second).forward; }
							if ((itBar->second).reverse != "NONE") {
								if (comboName == "") { comboName += (itBar->second).reverse; }
								else { comboName += ("." + (itBar->second).reverse); }
							}
							if ((itPrimer->second).forward != "NONE") {
								if (comboName == "") { comboName += (itPrimer->second).forward; }
								else { comboName += ("." + (itPrimer->second).forward); }
							}
							if ((itPrimer->second).reverse != "NONE") {
								if (comboName == "") { comboName += (itPrimer->second).reverse; }
								else { comboName += ("." + (itPrimer->second).reverse); }
							}
						}
					}


					if (comboName != "") { comboGroupName += "_" + comboName; }
					ofstream temp;
					map<string, string> variables;
					if (fileOption) { variables["[tag]"] = "forward"; }

					variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
					variables["[group]"] = comboGroupName;
					string fastqFileName = getOutputFileName("fastq", variables);
					if (uniqueNames.count(fastqFileName) == 0) {
						outputNames.push_back(fastqFileName);
						uniqueNames.insert(fastqFileName);
					}

					fastqFileNames[itBar->first][itPrimer->first] = fastqFileName;
					File::openOutputFile(fastqFileName, temp);		temp.close();

					if (fileOption) {
						variables["[tag]"] = "reverse";
						variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
						string rfastqFileName = getOutputFileName("fastq", variables);
						if (uniqueNames.count(rfastqFileName) == 0) {
							outputNames.push_back(rfastqFileName);
							uniqueNames.insert(rfastqFileName);
						}

						ofstream temp2;
						rfastqFileNames[itBar->first][itPrimer->first] = rfastqFileName;
						File::openOutputFile(rfastqFileName, temp2);		temp2.close();
					}

					if (fasta) {
						variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
						variables["[group]"] = comboGroupName;

						if (fileOption) { variables["[tag]"] = "forward"; }
						string fastaFileName = getOutputFileName("fasta", variables);
						if (uniqueNames.count(fastaFileName) == 0) {
							outputNames.push_back(fastaFileName);
							outputTypes["fasta"].push_back(fastaFileName);
						}

						ofstream temp3;
						fastaFileNames[itBar->first][itPrimer->first] = fastaFileName;
						File::openOutputFile(fastaFileName, temp3);		temp3.close();

						if (fileOption) {
							variables["[tag]"] = "reverse";
							string fastaFileName2 = getOutputFileName("fasta", variables);
							if (uniqueNames.count(fastaFileName2) == 0) {
								outputNames.push_back(fastaFileName2);
								outputTypes["fasta"].push_back(fastaFileName2);
							}

							ofstream temp4;
							rfastaFileNames[itBar->first][itPrimer->first] = fastaFileName2;
							File::openOutputFile(fastaFileName2, temp4);		temp4.close();
						}
					}

					if (qual) {
						variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
						variables["[group]"] = comboGroupName;
						if (fileOption) { variables["[tag]"] = "forward"; }
						string qualFileName = getOutputFileName("qfile", variables);
						if (uniqueNames.count(qualFileName) == 0) {
							outputNames.push_back(qualFileName);
							outputTypes["qfile"].push_back(qualFileName);
						}

						ofstream temp4;
						qualFileNames[itBar->first][itPrimer->first] = qualFileName;
						File::openOutputFile(qualFileName, temp4);		temp4.close();

						if (fileOption) {
							variables["[tag]"] = "reverse";
							string qualFileName2 = getOutputFileName("qfile", variables);
							if (uniqueNames.count(qualFileName2) == 0) {
								outputNames.push_back(qualFileName2);
								outputTypes["qfile"].push_back(qualFileName2);
							}

							ofstream temp5;
							rqualFileNames[itBar->first][itPrimer->first] = qualFileName2;
							File::openOutputFile(qualFileName2, temp5);		temp5.close();
						}
					}

				}
			}
		}

		//make blank files for no matches
		ofstream temp, tempff, tempfq, rtemp, temprf, temprq;
		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
		variables["[group]"] = "scrap";
		if (fileOption) { variables["[tag]"] = "forward"; }
		ffqnoMatchFile = getOutputFileName("fastq", variables);
		File::openOutputFile(ffqnoMatchFile, temp);		temp.close();

		if (fileOption) {
			variables["[tag]"] = "reverse";
			rfqnoMatchFile = getOutputFileName("fastq", variables);
			File::openOutputFile(rfqnoMatchFile, rtemp);		rtemp.close();
		}

		if (fasta) {
			if (fileOption) { variables["[tag]"] = "forward"; }
			ffnoMatchFile = getOutputFileName("fasta", variables);
			File::openOutputFile(ffnoMatchFile, tempff);		tempff.close();

			if (fileOption) {
				variables["[tag]"] = "reverse";
				rfnoMatchFile = getOutputFileName("fasta", variables);
				File::openOutputFile(rfnoMatchFile, temprf);		temprf.close();
			}
		}

		if (qual) {
			if (fileOption) { variables["[tag]"] = "forward"; }
			fqnoMatchFile = getOutputFileName("qfile", variables);
			File::openOutputFile(fqnoMatchFile, tempfq);		tempfq.close();

			if (fileOption) {
				variables["[tag]"] = "reverse";
				rqnoMatchFile = getOutputFileName("qfile", variables);
				File::openOutputFile(rqnoMatchFile, temprq);		temprq.close();
			}
		}

	}
	else {
		map<string, int> barcodes = oligos.getBarcodes();
		map<string, int> primers = oligos.getPrimers();
		for (map<string, int>::iterator itBar = barcodes.begin();itBar != barcodes.end();itBar++) {
			for (map<string, int>::iterator itPrimer = primers.begin();itPrimer != primers.end(); itPrimer++) {

				string primerName = oligos.getPrimerName(itPrimer->second);
				string barcodeName = oligos.getBarcodeName(itBar->second);

				if ((primerName == "ignore") || (barcodeName == "ignore")) {} //do nothing
				else if ((primerName == "") && (barcodeName == "")) {} //do nothing
				else {
					string comboGroupName = "";
					string comboName = "";

					if (primerName == "") {
						comboGroupName = barcodeName;
					}
					else {
						if (barcodeName == "") {
							comboGroupName = primerName;
						}
						else {
							comboGroupName = barcodeName + "." + primerName;
						}
					}

					if (itPrimer->first == "") {
						comboName = itBar->first;
					}
					else {
						if (itBar->first == "") {
							comboName = itPrimer->first;
						}
						else {
							comboName = itBar->first + "." + itPrimer->first;
						}
					}

					if (comboName != "") { comboGroupName += "_" + comboName; }


					ofstream temp;
					map<string, string> variables;
					variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
					variables["[group]"] = comboGroupName;
					string fastqFileName = getOutputFileName("fastq", variables);
					if (uniqueNames.count(fastqFileName) == 0) {
						outputNames.push_back(fastqFileName);
						uniqueNames.insert(fastqFileName);
					}

					fastqFileNames[itBar->second][itPrimer->second] = fastqFileName;
					File::openOutputFile(fastqFileName, temp);		temp.close();

					if (fasta) {
						variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
						variables["[group]"] = comboGroupName;
						string fastaFileName = getOutputFileName("fasta", variables);
						if (uniqueNames.count(fastaFileName) == 0) {
							outputNames.push_back(fastaFileName);
							outputTypes["fasta"].push_back(fastaFileName);
						}

						ofstream temp3;
						fastaFileNames[itBar->second][itPrimer->second] = fastaFileName;
						File::openOutputFile(fastaFileName, temp3);		temp3.close();
					}

					if (qual) {
						variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
						variables["[group]"] = comboGroupName;
						string qualFileName = getOutputFileName("qfile", variables);
						if (uniqueNames.count(qualFileName) == 0) {
							outputNames.push_back(qualFileName);
							outputTypes["qfile"].push_back(qualFileName);
						}

						ofstream temp4;
						qualFileNames[itBar->second][itPrimer->second] = qualFileName;
						File::openOutputFile(qualFileName, temp4);		temp4.close();
					}
				}
			}
		}

		//make blank files for no matches
		ofstream temp, tempff, tempfq;
		map<string, string> variables;
		variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
		variables["[group]"] = "scrap";
		ffqnoMatchFile = getOutputFileName("fastq", variables);
		File::openOutputFile(ffqnoMatchFile, temp);		temp.close();

		if (fasta) {
			ffnoMatchFile = getOutputFileName("fasta", variables);
			File::openOutputFile(ffqnoMatchFile, tempff);		tempff.close();
		}

		if (qual) {
			fqnoMatchFile = getOutputFileName("qfile", variables);
			File::openOutputFile(fqnoMatchFile, tempfq);		tempfq.close();
		}
	}

	if (allBlank) {
		LOG(WARNING) << "your oligos file does not contain any group names.  mothur will not create a groupfile." << '\n';
		return false;
	}

	return true;

}
//***************************************************************************************************************
bool ParseFastaQCommand::readGroup(string groupfile) {
	fastqFileNames.clear();

	groupMap = new GroupMap();
	groupMap->readMap(groupfile);

	//like barcodeNameVector - no primer names
	vector<string> groups = groupMap->getNamesOfGroups();

	fastqFileNames.resize(groups.size());
	for (int i = 0; i < fastqFileNames.size(); i++) {
		for (int j = 0; j < 1; j++) {

			map<string, string> variables;
			variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(fastaQFile));
			variables["[group]"] = groups[i];
			string thisFilename = getOutputFileName("fastq", variables);
			outputNames.push_back(thisFilename);

			ofstream temp;
			File::openOutputFileBinary(thisFilename, temp); temp.close();
			fastqFileNames[i].push_back(thisFilename);
			GroupToFile[groups[i]] = i;
		}
	}

	//make blank files for no matches
	ofstream temp, tempff, tempfq;
	map<string, string> variables;
	variables["[filename]"] = outputDir + File::getRootName(File::getSimpleName(inputfile));
	variables["[group]"] = "scrap";
	ffqnoMatchFile = getOutputFileName("fastq", variables);
	File::openOutputFile(ffqnoMatchFile, temp);		temp.close();

	if (fasta) {
		ffnoMatchFile = getOutputFileName("fasta", variables);
		File::openOutputFile(ffqnoMatchFile, tempff);		tempff.close();
	}

	if (qual) {
		fqnoMatchFile = getOutputFileName("qfile", variables);
		File::openOutputFile(fqnoMatchFile, tempfq);		tempfq.close();
	}

	return true;

}
//**********************************************************************************************************************



