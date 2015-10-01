/*
 *  fileoutput.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 11/18/08.
 *  Copyright 2008 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "fileoutput.h"

 /***********************************************************************/

ThreeColumnFile::~ThreeColumnFile() {

	inFile.close();
	outFile.close();
	File::remove(outName);
}

/***********************************************************************/

void ThreeColumnFile::initFile(string label) {
	if (counter != 0) {
		File::openOutputFile(outName, outFile);
		File::openInputFile(inName, inFile);

		string inputBuffer;
		inputBuffer = File::getline(inFile);

		outFile << inputBuffer << '\t' << label << "\tlci\thci" << endl;
	}
	else {
		File::openOutputFile(outName, outFile);
		outFile << "numsampled\t" << label << "\tlci\thci" << endl;
	}

	outFile.setf(ios::fixed, ios::floatfield);
	outFile.setf(ios::showpoint);
}

/***********************************************************************/

void ThreeColumnFile::output(int nSeqs, vector<double> data) {
	if (counter != 0) {
		string inputBuffer;
		inputBuffer = File::getline(inFile);

		outFile << inputBuffer << setprecision(4) << '\t' << data[0] << '\t' << data[1] << '\t' << data[2] << endl;
	}
	else {
		outFile << nSeqs << setprecision(4) << '\t' << data[0] << '\t' << data[1] << '\t' << data[2] << endl;
	}
}

/***********************************************************************/

void ThreeColumnFile::resetFile() {
	if (counter != 0) {
		outFile.close();
		inFile.close();
	}
	else {
		outFile.close();
	}
	counter = 1;

	File::remove(inName);
	renameOk = rename(outName.c_str(), inName.c_str());

	//renameFile(outName, inName);

	//checks to make sure user was able to rename and remove successfully
	if ((renameOk != 0)) {
		LOG(INFO) << "Unable to rename " + outName << '\n';
		perror(" : ");
	}
}

/***********************************************************************/
/***********************************************************************/

ColumnFile::~ColumnFile() {

	inFile.close();
	outFile.close();
	File::remove(outName);
}

/***********************************************************************/

void ColumnFile::initFile(string label, vector<string> tags) {
	if (counter != 0) {
		File::openOutputFile(outName, outFile);
		File::openInputFile(inName, inFile);

		string inputBuffer;
		inputBuffer = File::getline(inFile);

		outFile << inputBuffer << '\t';
		for (int i = 0; i < tags.size(); i++) {
			outFile << label + tags[i] << '\t';
		}
		outFile << endl;
	}
	else {
		File::openOutputFile(outName, outFile);
		for (int i = 0; i < tags.size(); i++) {
			outFile << label + tags[i] << '\t';
		}
		outFile << endl;
	}

	outFile.setf(ios::fixed, ios::floatfield);
	outFile.setf(ios::showpoint);
}

/***********************************************************************/

void ColumnFile::output(vector<double> data) {

	if (counter != 0) {
		string inputBuffer;
		inputBuffer = File::getline(inFile);

		outFile << inputBuffer << '\t' << setprecision(6) << data[0] << setprecision(iters.length());
		for (int i = 1; i < data.size(); i++) {
			outFile << '\t' << data[i];
		}
		outFile << endl;
	}
	else {
		outFile << setprecision(6) << data[0] << setprecision(iters.length());
		for (int i = 1; i < data.size(); i++) {
			outFile << '\t' << data[i];
		}
		outFile << endl;
	}

}

/***********************************************************************/

void ColumnFile::resetFile() {
	if (counter != 0) {
		outFile.close();
		inFile.close();
	}
	else {
		outFile.close();
	}
	counter = 1;

	File::remove(inName);
	renameOk = rename(outName.c_str(), inName.c_str());

	//renameFile(outName, inName);

	//checks to make sure user was able to rename and remove successfully
	if ((renameOk != 0)) {
		LOG(INFO) << "Unable to rename " + outName << '\n';
		perror(" : ");
	}
}

/***********************************************************************/
/***********************************************************************/

SharedThreeColumnFile::~SharedThreeColumnFile() {

	inFile.close();
	outFile.close();
	File::remove(outName);
}

/***********************************************************************/

void SharedThreeColumnFile::initFile(string label) {
	if (counter != 0) {
		File::openOutputFile(outName, outFile);
		File::openInputFile(inName, inFile);

		string inputBuffer;
		inputBuffer = File::getline(inFile);

		outFile << inputBuffer << '\t' << label << "\tlci\thci" << endl;
	}
	else {
		File::openOutputFile(outName, outFile);
		outFile << "numsampled\t" << groupLabel << '\t' << label << "\tlci\thci" << endl;
	}

	outFile.setf(ios::fixed, ios::floatfield);
	outFile.setf(ios::showpoint);
}

/***********************************************************************/

void SharedThreeColumnFile::output(int nSeqs, vector<double> data) {
	if (counter != 0) {
		string inputBuffer;
		inputBuffer = File::getline(inFile);

		outFile << inputBuffer << setprecision(4) << '\t' << data[0] << '\t' << data[1] << '\t' << data[2] << endl;
	}
	else {
		outFile << numGroup << setprecision(4) << '\t' << data[0] << '\t' << data[1] << '\t' << data[2] << endl;
		numGroup++;
	}
}

/***********************************************************************/

void SharedThreeColumnFile::resetFile() {
	if (counter != 0) {
		outFile.close();
		inFile.close();
	}
	else {
		outFile.close();
	}
	counter = 1;

	File::remove(inName);
	renameOk = rename(outName.c_str(), inName.c_str());

	//renameFile(outName, inName);

	//checks to make sure user was able to rename and remove successfully
	if ((renameOk != 0)) {
		LOG(INFO) << "Unable to rename " + outName << '\n';
		perror(" : ");
	}
}

/***********************************************************************/

/***********************************************************************/

OneColumnFile::~OneColumnFile() {

	inFile.close();
	outFile.close();
	File::remove(outName);
}

/***********************************************************************/

void OneColumnFile::initFile(string label) {
	if (counter != 0) {
		File::openOutputFile(outName, outFile);
		File::openInputFile(inName, inFile);

		string inputBuffer;
		inputBuffer = File::getline(inFile);

		outFile << inputBuffer << '\t' << label << endl;
	}
	else {
		File::openOutputFile(outName, outFile);
		outFile << "numsampled\t" << label << endl;
	}

	outFile.setf(ios::fixed, ios::floatfield);
	outFile.setf(ios::showpoint);
}

/***********************************************************************/

void OneColumnFile::output(int nSeqs, vector<double> data) {
	try {
		if (counter != 0) {
			string inputBuffer;
			inputBuffer = File::getline(inFile);

			outFile << inputBuffer << setprecision(4) << '\t' << data[0] << endl;
		}
		else {
			outFile << nSeqs << setprecision(4) << '\t' << data[0] << endl;
		}
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in OneColumnFile, output";
		exit(1);
	}
}

/***********************************************************************/

void OneColumnFile::resetFile() {
	if (counter != 0) {
		outFile.close();
		inFile.close();
	}
	else {
		outFile.close();
	}
	counter = 1;

	File::remove(inName);
	renameOk = rename(outName.c_str(), inName.c_str());

	//renameFile(outName, inName);

	//checks to make sure user was able to rename and remove successfully
	if ((renameOk != 0)) {
		LOG(INFO) << "Unable to rename " + outName << '\n';
		perror(" : ");
	}

}

/***********************************************************************/
/***********************************************************************/

SharedOneColumnFile::~SharedOneColumnFile() {

	inFile.close();
	outFile.close();
	File::remove(outName);
}

/***********************************************************************/

void SharedOneColumnFile::initFile(string label) {
	if (counter != 0) {
		File::openOutputFile(outName, outFile);
		File::openInputFile(inName, inFile);

		string inputBuffer;
		inputBuffer = File::getline(inFile);

		outFile << inputBuffer << '\t' << label << endl;

	}
	else {
		File::openOutputFile(outName, outFile);
		outFile << "sampled\t" << label << endl;

	}

	outFile.setf(ios::fixed, ios::floatfield);
	outFile.setf(ios::showpoint);
}

/***********************************************************************/

void SharedOneColumnFile::output(int nSeqs, vector<double> data) {
	try {
		string dataOutput;
		float sam;
		sam = data[0];
		dataOutput = "";
		for (int i = 0; i < data.size(); i++) {
			dataOutput = dataOutput + "\t" + toString(data[i]);
		}
		if (counter != 0) {
			string inputBuffer;
			inputBuffer = File::getline(inFile);

			outFile << inputBuffer << setprecision(2) << '\t' << dataOutput << endl;
		}
		else {
			outFile << nSeqs << setprecision(2) << '\t' << dataOutput << endl;
		}
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in SharedOneColumnFile, output";
		exit(1);
	}
}

/***********************************************************************/

void SharedOneColumnFile::resetFile() {
	if (counter != 0) {
		outFile.close();
		inFile.close();
	}
	else {
		outFile.close();
	}
	counter = 1;

	File::remove(inName);
	renameOk = rename(outName.c_str(), inName.c_str());

	//renameFile(outName, inName);

	//checks to make sure user was able to rename and remove successfully
	if ((renameOk != 0)) {
		LOG(INFO) << "Unable to rename " + outName << '\n';
		perror(" : ");
	}
}

/***********************************************************************/
