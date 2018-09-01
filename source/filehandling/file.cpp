#include <fstream>
#include <string>
#include "mothur.h"
#include "file.h"
#include "utility.h"
#include <stdlib.h>

#if defined (UNIX)
#include <unistd.h>
#else
#include <direct.h>
#endif

bool File::FileExists(const std::string& name)
{
	std::ifstream f(name.c_str());
	bool isGood = f.good();
	f.close();
	return isGood;
}

std::string File::getRootName(std::string longName) {

	std::string rootName = longName;

	if (rootName.find_last_of(".") != rootName.npos) {
		size_t pos = rootName.find_last_of('.') + 1;
		rootName = rootName.substr(0, pos);
	}

	return rootName;
}
/***********************************************************************/

std::string File::getSimpleName(std::string longName) {
	std::string simpleName = longName;

	size_t found;
	found = longName.find_last_of("/\\");

	if (found != longName.npos) {
		simpleName = longName.substr(found + 1);
	}

	return simpleName;
}

std::string File::getPath(std::string longName) {
	std::string path = "";

	size_t found;
	found = longName.find_last_of("~/\\");

	if (found != longName.npos) {
		path = longName.substr(0, found + 1);
	}

	return path;

}


/***********************************************************************/


std::string File::getFullPathName(std::string fileName) {

	std::string path = getPath(fileName);
	std::string newFileName;
	size_t pos;

	if (path == "") { return fileName; } //its a simple name
	else { //we need to complete the pathname
		   // ex. ../../../filename 
		   // cwd = /user/work/desktop

		std::string cwd;
		//get current working directory 
		if (path.find("~") != -1) { //go to home directory
			std::string homeDir;

			char *homepath = NULL;
#if defined (UNIX)
			homepath = getenv("HOME");
#else
			homepath = getenv("HOMEPATH");
#endif
			if (homepath != NULL) { homeDir = homepath; }
			else { homeDir = ""; }
			free(homepath);

			newFileName = homeDir + fileName.substr(fileName.find("~") + 1);
			return newFileName;
		}
		else { //find path
			if (path.rfind("." + path_delimiter) == std::string::npos) { return fileName; } //already complete name
			else { newFileName = fileName.substr(fileName.rfind("." + path_delimiter) + 2); } //save the complete part of the name

			char *cwdpath = NULL;
			cwdpath = getcwd(NULL, 0); // or _getcwd
			if (cwdpath != NULL) { cwd = cwdpath; }
			else { cwd = ""; }
			free(cwdpath);

#if defined (UNIX)
			//rip off first '/'
			if (cwd.length() > 0) { cwd = cwd.substr(1); }
#endif

			//break apart the current working directory
			std::vector<std::string> dirs;
			while (cwd.find_first_of(path_delimiter) != std::string::npos) {
				std::string dir = cwd.substr(0, cwd.find_first_of(path_delimiter));
				cwd = cwd.substr(cwd.find_first_of(path_delimiter) + 1, cwd.length());
				dirs.push_back(dir);
			}
			//get last one              // ex. ../../../filename = /user/work/desktop/filename
			dirs.push_back(cwd);  //ex. dirs[0] = user, dirs[1] = work, dirs[2] = desktop


			int index = dirs.size() - 1;

			while ((pos = path.rfind("." + path_delimiter)) != std::string::npos) { //while you don't have a complete path
				if (pos == 0) {
					break;  //you are at the end
				}
				else if (path[(pos - 1)] == '.') { //you want your parent directory ../
					path = path.substr(0, pos - 1);
					index--;
					if (index == 0) { break; }
				}
				else if (path[(pos - 1)] == path_delimiter) { //you want the current working dir ./
					path = path.substr(0, pos);
				}
				else if (pos == 1) {
					break;  //you are at the end
				}
				else { File("cannot resolve path for " + fileName + "\n"); return fileName; }
			}

			for (int i = index; i >= 0; i--) {
				newFileName = dirs[i] + path_delimiter + newFileName;
			}

#if defined (UNIX)
			newFileName = "/" + newFileName;
#endif
			return newFileName;
		}
	}
}

int File::remove(std::string filename) {
	filename = getFullPathName(filename);
	int error = std::remove(filename.c_str());
	LOG(DEBUG) << "removed " + filename;
	//if (error != 0) { 
	//	if (errno != ENOENT) { //ENOENT == file does not exist
	//		std::string message = "Error deleting file " + filename;
	//		perror(message.c_str()); 
	//	}
	//}
	return error;
}

int File::openInputFile(std::string fileName, std::ifstream& fileHandle, std::string m) {
	//get full path name
	std::string completeFileName = getFullPathName(fileName);
	fileHandle.open(completeFileName.c_str());
	if (!fileHandle) {
		//LOG(LOGERROR) << "Could not open " + completeFileName;
		return 1;
	}
	else {
		//check for blank file
		zapGremlins(fileHandle);
		gobble(fileHandle);
		return 0;
	}
}
/***********************************************************************/

int File::openInputFile(std::string fileName, std::ifstream& fileHandle) {

	//get full path name
	std::string completeFileName = getFullPathName(fileName);
	fileHandle.open(completeFileName.c_str());
	if (!fileHandle) {
		LOG(LOGERROR) << "Could not open " + completeFileName;
		return 1;
	}
	else {
		//check for blank file
		zapGremlins(fileHandle);
		gobble(fileHandle);
		if (fileHandle.eof()) { LOG(LOGERROR) << completeFileName + " is blank. Please correct."; }

		return 0;
	}
}
/***********************************************************************/
int File::openInputFileBinary(std::string fileName, std::ifstream& fileHandle) {

	//get full path name
	std::string completeFileName = getFullPathName(fileName);
	fileHandle.open(completeFileName.c_str(), std::ios::binary);
	if (!fileHandle) {
		LOG(LOGERROR) << "Could not open " + completeFileName;
		return 1;
	}
	else {
		//check for blank file
		zapGremlins(fileHandle);
		gobble(fileHandle);
		if (fileHandle.eof()) { LOG(LOGERROR) << completeFileName + " is blank. Please correct."; }

		return 0;
	}
}
/***********************************************************************/
int File::openInputFileBinary(std::string fileName, std::ifstream& fileHandle, std::string noerror) {

	//get full path name
	std::string completeFileName = getFullPathName(fileName);
	fileHandle.open(completeFileName.c_str(), std::ios::binary);
	if (!fileHandle) {
		//LOG(LOGERROR) << "Could not open " + completeFileName;
		return 1;
	}
	else {
		//check for blank file
		zapGremlins(fileHandle);
		gobble(fileHandle);
		//if (fileHandle.eof()) { LOG(LOGERROR) << "" + completeFileName + " is blank. Please correct.";  }

		return 0;
	}
}
/***********************************************************************/
#ifdef USE_BOOST
int File::openInputFileBinary(std::string fileName, std::ifstream& file, boost::iostreams::filtering_istream& in) {

	//get full path name
	std::string completeFileName = getFullPathName(fileName);

	file.open(completeFileName.c_str(), ios_base::in | ios_base::binary);

	if (!file) {
		LOG(LOGERROR) << "Could not open " + completeFileName;
		return 1;
	}
	else {
		//check for blank file
		in.push(boost::iostreams::gzip_decompressor());
		in.push(file);
		if (file.eof()) { LOG(LOGERROR) << "" + completeFileName + " is blank. Please correct."; }

		return 0;
	}
}
/***********************************************************************/
int File::openInputFileBinary(std::string fileName, std::ifstream& file, boost::iostreams::filtering_istream& in, std::string noerror) {

	//get full path name
	std::string completeFileName = getFullPathName(fileName);

	file.open(completeFileName.c_str(), ios_base::in | ios_base::binary);

	if (!file) {
		return 1;
	}
	else {
		//check for blank file
		in.push(boost::iostreams::gzip_decompressor());
		in.push(file);
		return 0;
	}
}
#endif
/***********************************************************************/
//results[0] = allGZ, results[1] = allNotGZ
std::vector<bool> File::allGZFiles(std::vector<std::string> & files) {
	std::vector<bool> results;
	bool allGZ = true;
	bool allNOTGZ = true;

	for (int i = 0; i < files.size(); i++) {
		if (ctrlc_pressed) { break; }

		//ignore none and blank filenames
		if ((files[i] != "") || (files[i] != "NONE")) {
			if (isGZ(files[i])[1]) { allNOTGZ = false; }
			else { allGZ = false; }
		}
	}

	if (!allGZ && !allNOTGZ) { //mixed bag
		LOG(LOGERROR) << "Cannot mix .gz and non compressed files. Please decompress your files and rerun."; ctrlc_pressed = true;;
	}

	results.push_back(allGZ);
	results.push_back(allNOTGZ);

	return results;
}

/***********************************************************************/
std::vector<bool> File::isGZ(std::string filename) {
	std::vector<bool> results; results.resize(2, false);
#ifdef USE_BOOST
	std::ifstream fileHandle;
	boost::iostreams::filtering_istream gzin;

	if (getExtension(filename) != ".gz") { return results; } // results[0] = false; results[1] = false;

	int ableToOpen = openInputFileBinary(filename, fileHandle, gzin, ""); //no error

	if (ableToOpen == 1) { return results; } // results[0] = false; results[1] = false;
	else { results[0] = true; }

	char c;
	try
	{
		gzin >> c;
		results[1] = true;
	}
	catch (boost::iostreams::gzip_error & e)
	{
		gzin.pop();
		fileHandle.close();
		return results;  // results[0] = true; results[1] = false;
	}
	fileHandle.close();
#else
	LOG(LOGERROR) << "cannot test for gz format without enabling boost libraries."; ctrlc_pressed = true;
#endif
	return results; //results[0] = true; results[1] = true;
}

/***********************************************************************/

int File::renameFile(std::string oldName, std::string newName) {

	if (oldName == newName) { return 0; }

	std::ifstream inTest;
	int exist = openInputFile(newName, inTest, "");
	inTest.close();

#if defined (UNIX)		
	if (exist == 0) { //you could open it so you want to delete it
		std::string command = "rm " + newName;
		system(command.c_str());
	}

	std::string command = "mv " + oldName + " " + newName;
	system(command.c_str());
#else
	File::remove(newName);
	int renameOk = rename(oldName.c_str(), newName.c_str());
#endif
	return 0;

}

/***********************************************************************/

int File::openOutputFile(std::string fileName, std::ofstream& fileHandle) {

	std::string completeFileName = getFullPathName(fileName);
	fileHandle.open(completeFileName.c_str(), std::ios::trunc);
	if (!fileHandle) {
		LOG(LOGERROR) << "Could not open " + completeFileName;
		return 1;
	}
	else {
		return 0;
	}

}
/***********************************************************************/

int File::openOutputFileBinary(std::string fileName, std::ofstream& fileHandle) {

	std::string completeFileName = getFullPathName(fileName);
	fileHandle.open(completeFileName.c_str(), std::ios::trunc | std::ios::binary);
	if (!fileHandle) {
		LOG(LOGERROR) << "Could not open " + completeFileName;
		return 1;
	}
	else {
		return 0;
	}

}
/**************************************************************************************************/
int File::appendFiles(std::string temp, std::string filename) {
	std::ofstream output;
	std::ifstream input;

	//open output file in append mode
	openOutputFileBinaryAppend(filename, output);
	int ableToOpen = openInputFileBinary(temp, input, "no error");
	//int ableToOpen = openInputFile(temp, input);

	int numLines = 0;
	if (ableToOpen == 0) { //you opened it

		char buffer[4096];
		while (!input.eof()) {
			input.read(buffer, 4096);
			output.write(buffer, input.gcount());
			//count number of lines
			for (int i = 0; i < input.gcount(); i++) { if (buffer[i] == '\n') { numLines++; } }
		}
		input.close();
	}

	output.close();

	return numLines;
}
int File::appendFilesAndRemove(std::string temp, std::string filename)
{
	int numLines = appendFiles(temp, filename);
	File::remove(temp);
	return numLines;
}
/**************************************************************************************************/
int File::appendBinaryFiles(std::string temp, std::string filename) {
	std::ofstream output;
	std::ifstream input;

	//open output file in append mode
	openOutputFileBinaryAppend(filename, output);
	int ableToOpen = openInputFileBinary(temp, input, "no error");

	if (ableToOpen == 0) { //you opened it

		char buffer[4096];
		while (!input.eof()) {
			input.read(buffer, 4096);
			output.write(buffer, input.gcount());
		}
		input.close();
	}

	output.close();

	return ableToOpen;
}
/**************************************************************************************************/
int File::appendSFFFiles(std::string temp, std::string filename) {
	std::ofstream output;
	std::ifstream input;
	int ableToOpen = 0;

	//open output file in append mode
	std::string fullFileName = getFullPathName(filename);

	output.open(fullFileName.c_str(), std::ios::app | std::ios::binary);
	if (!output) {
		LOG(LOGERROR) << "Could not open " + fullFileName;
		return 1;
	}
	else {
		//get full path name
		std::string completeFileName = getFullPathName(temp);

		input.open(completeFileName.c_str(), std::ios::binary);
		if (!input) {
			//LOG(LOGERROR) << "Could not open " + completeFileName;
			return 1;
		}
		else {
			char buffer[4096];
			while (!input.eof()) {
				input.read(buffer, 4096);
				output.write(buffer, input.gcount());
			}
			input.close();
		}
		output.close();
	}

	return ableToOpen;
}
/**************************************************************************************************/
int File::appendFilesWithoutHeaders(std::string temp, std::string filename) {
	std::ofstream output;
	std::ifstream input;

	//open output file in append mode
	openOutputFileAppend(filename, output);
	int ableToOpen = openInputFile(temp, input, "no error");
	//int ableToOpen = openInputFile(temp, input);

	int numLines = 0;
	if (ableToOpen == 0) { //you opened it

		std::string headers = getline(input); gobble(input);
		LOG(DEBUG) << " skipping headers " + headers + '\n';

		char buffer[4096];
		while (!input.eof()) {
			input.read(buffer, 4096);
			output.write(buffer, input.gcount());
			//count number of lines
			for (int i = 0; i < input.gcount(); i++) { if (buffer[i] == '\n') { numLines++; } }
		}
		input.close();
	}

	output.close();

	return numLines;
}
/**************************************************************************************************/
std::string File::sortFile(std::string distFile, std::string outputDir) {

	//if (outputDir == "") {  outputDir += hasPath(distFile);  }
	std::string outfile = getRootName(distFile) + "sorted.dist";


	//if you can, use the unix sort since its been optimized for years
#if defined (UNIX)
	std::string command = "sort -n -k +3 " + distFile + " -o " + outfile;
	system(command.c_str());
#else //you are stuck with my best attempt...
		//windows sort does not have a way to specify a column, only a character in the line
		//since we cannot assume that the distance will always be at the the same character location on each line
		//due to variable sequence name lengths, I chose to force the distance into first position, then sort and then put it back.

		//read in file line by file and put distance first
	std::string tempDistFile = distFile + ".temp";
	std::ifstream input;
	std::ofstream output;
	openInputFile(distFile, input);
	openOutputFile(tempDistFile, output);

	std::string firstName, secondName;
	float dist;
	while (!input.eof()) {
		input >> firstName >> secondName >> dist;
		output << dist << '\t' << firstName << '\t' << secondName << std::endl;
		gobble(input);
	}
	input.close();
	output.close();


	//sort using windows sort
	std::string tempOutfile = outfile + ".temp";
	std::string command = "sort " + tempDistFile + " /O " + tempOutfile;
	system(command.c_str());

	//read in sorted file and put distance at end again
	std::ifstream input2;
	std::ofstream output2;
	openInputFile(tempOutfile, input2);
	openOutputFile(outfile, output2);

	while (!input2.eof()) {
		input2 >> dist >> firstName >> secondName;
		output2 << firstName << '\t' << secondName << '\t' << dist << std::endl;
		gobble(input2);
	}
	input2.close();
	output2.close();

	//remove temp files
	File::remove(tempDistFile);
	File::remove(tempOutfile);
#endif

	return outfile;
}
/**************************************************************************************************/
std::vector<unsigned long long> File::setFilePosFasta(std::string filename, long long& num) {
	std::vector<unsigned long long> positions;
	std::ifstream inFASTA;
	//openInputFileBinary(filename, inFASTA);
	std::string completeFileName = getFullPathName(filename);
	inFASTA.open(completeFileName.c_str(), std::ios::binary);

	std::string input;
	unsigned long long count = 0;
	while (!inFASTA.eof()) {
		//input = getline(inFASTA); 
		//cout << input << '\t' << inFASTA.tellg() << endl;
		//if (input.length() != 0) {
		//	if(input[0] == '>'){	unsigned long int pos = inFASTA.tellg(); positions.push_back(pos - input.length() - 1);	 cout << (pos - input.length() - 1) << endl; }
		//}
		//gobble(inFASTA); //has to be here since windows line endings are 2 characters and mess up the positions
		char c = inFASTA.get(); count++;
		if (c == '>') {
			positions.push_back(count - 1);
			LOG(DEBUG) << " numSeqs = " + toString(positions.size()) + " count = " + toString(count) + ".\n";
		}
	}
	inFASTA.close();

	num = positions.size();
	LOG(DEBUG) << " num = " + toString(num) + ".\n";
	FILE * pFile;
	unsigned long long size;

	//get num bytes in file
	pFile = fopen(completeFileName.c_str(), "rb");
	if (pFile == NULL) perror("Error opening file");
	else {
		fseek(pFile, 0, SEEK_END);
		size = ftell(pFile);
		fclose(pFile);
	}

	/*unsigned long long size = positions[(positions.size()-1)];
	std::ifstream in;
	openInputFile(filename, in);

	in.seekg(size);

	while(in.get()){
	if(in.eof())		{	break;	}
	else				{	size++;	}
	}
	in.close();*/

	LOG(DEBUG) << " size = " + toString(size) + ".\n";

	positions.push_back(size);
	positions[0] = 0;

	return positions;
}
/**************************************************************************************************/
std::vector<unsigned long long> File::setFilePosFasta(std::string filename, long long& num, char delim) {
	std::vector<unsigned long long> positions;
	std::ifstream inFASTA;
	std::string completeFileName = getFullPathName(filename);
	inFASTA.open(completeFileName.c_str(), std::ios::binary);
	int nameLine = 2;
	if (delim == '@') { nameLine = 4; }
	else if (delim == '>') { nameLine = 2; }
	else { LOG(LOGERROR) << "unknown file deliminator, quitting.\n"; ctrlc_pressed = true; }

	unsigned long long count = 0;
	long long numLines = 0;
	while (!inFASTA.eof()) {
		char c = inFASTA.get(); count++;
		std::string input = ""; input += c;
		while ((c != '\n') && (c != '\r') && (c != '\f') && (c != EOF)) {
			c = inFASTA.get(); count++;
			input += c;
		}
		numLines++;
		//gobble
		while (isspace(c = inFASTA.get())) { input += c; count++; }
		if (!inFASTA.eof()) { inFASTA.putback(c); count--; }


		if (input.length() != 0) {
			if ((input[0] == delim) && (((numLines - 1) % nameLine) == 0)) { //this is a name line
																			 //mothurOut(input + '\t' + toString(count+numLines-input.length()) + '\n');// << endl;
				positions.push_back(count + numLines - input.length());
				LOG(DEBUG) << " numSeqs = " + toString(positions.size()) + " count = " + toString(count) + input + ".\n";
			}
			else if (int(c) == -1) { break; }
			else {
				input = "";
			}
		}
	}
	inFASTA.close();

	num = positions.size();
	LOG(DEBUG) << " num = " + toString(num) + ".\n";
	FILE * pFile;
	unsigned long long size;

	//get num bytes in file
	pFile = fopen(completeFileName.c_str(), "rb");
	if (pFile == NULL) perror("Error opening file");
	else {
		fseek(pFile, 0, SEEK_END);
		size = ftell(pFile);
		fclose(pFile);
	}

	LOG(DEBUG) << " size = " + toString(size) + ".\n";

	positions.push_back(size);
	positions[0] = 0;

	return positions;
}
/**************************************************************************************************/
std::vector<unsigned long long> File::setFilePosFasta(std::string filename, int& num) {
	std::vector<unsigned long long> positions;
	std::ifstream inFASTA;
	//openInputFile(filename, inFASTA);
	std::string completeFileName = getFullPathName(filename);
	inFASTA.open(completeFileName.c_str(), std::ios::binary);

	std::string input;
	unsigned long long count = 0;
	while (!inFASTA.eof()) {
		//input = getline(inFASTA);
		//cout << input << '\t' << inFASTA.tellg() << endl;
		//if (input.length() != 0) {
		//	if(input[0] == '>'){	unsigned long int pos = inFASTA.tellg(); positions.push_back(pos - input.length() - 1);	 cout << (pos - input.length() - 1) << endl; }
		//}
		//gobble(inFASTA); //has to be here since windows line endings are 2 characters and mess up the positions
		char c = inFASTA.get(); count++;
		if (c == '>') {
			positions.push_back(count - 1);
			LOG(DEBUG) << " numSeqs = " + toString(positions.size()) + " count = " + toString(count) + ".\n";
		}
	}
	inFASTA.close();

	num = positions.size();
	LOG(DEBUG) << " num = " + toString(num) + ".\n";
	FILE * pFile;
	unsigned long long size;

	//get num bytes in file
	pFile = fopen(completeFileName.c_str(), "rb");
	if (pFile == NULL) perror("Error opening file");
	else {
		fseek(pFile, 0, SEEK_END);
		size = ftell(pFile);
		fclose(pFile);
	}

	/*unsigned long long size = positions[(positions.size()-1)];
	std::ifstream in;
	openInputFile(filename, in);

	in.seekg(size);

	while(in.get()){
	if(in.eof())		{	break;	}
	else				{	size++;	}
	}
	in.close();*/

	LOG(DEBUG) << " size = " + toString(size) + ".\n";

	positions.push_back(size);
	positions[0] = 0;

	return positions;
}

//**********************************************************************************************************************
//std::vector<consTax> File::readConsTax(std::string inputfile) {
//
//	std::vector<consTax> taxes;
//
//	std::ifstream in;
//	openInputFile(inputfile, in);
//
//	//read headers
//	getline(in);
//
//	while (!in.eof()) {
//
//		if (ctrlc_pressed) { break; }
//
//		std::string otu = ""; std::string tax = "unknown";
//		int size = 0;
//
//		in >> otu >> size >> tax; gobble(in);
//		consTax temp(otu, tax, size);
//		taxes.push_back(temp);
//	}
//	in.close();
//
//	return taxes;
//}
//**********************************************************************************************************************
//int File::readConsTax(std::string inputfile, map<int, consTax2>& taxes) {
///*	std::ifstream in;
//	openInputFile(inputfile, in);
//
//	//read headers
//	getline(in);
//
//	while (!in.eof()) {
//
//		if (ctrlc_pressed) { break; }
//
//		std::string otu = ""; std::string tax = "unknown";
//		int size = 0;
//
//		in >> otu >> size >> tax; gobble(in);
//		consTax2 temp(otu, tax, size);
//		std::string simpleBin = m->getSimpleLabel(otu);
//		int bin;
//		convert(simpleBin, bin);
//		taxes[bin] = temp;
//	}
//	in.close();
//
//	*/
//	return 0;
//	
//}
/**************************************************************************************************/
std::vector<unsigned long long> File::setFilePosEachLine(std::string filename, int& num) {
	filename = getFullPathName(filename);

	std::vector<unsigned long long> positions;
	std::ifstream in;
	//openInputFile(filename, in);
	openInputFileBinary(filename, in);

	std::string input;
	unsigned long long count = 0;
	positions.push_back(0);

	while (!in.eof()) {
		//getline counting reads
		char d = in.get(); count++;
		while ((d != '\n') && (d != '\r') && (d != '\f') && (!in.eof())) {
			//get next character
			d = in.get();
			count++;
		}

		if (!in.eof()) {
			d = in.get(); count++;
			while (isspace(d) && (!in.eof())) { d = in.get(); count++; }
		}
		positions.push_back(count - 1);
		//cout << count-1 << endl;
	}
	in.close();

	num = static_cast<int>(positions.size() - 1);

	FILE * pFile;
	unsigned long long size;

	//get num bytes in file
	pFile = fopen(filename.c_str(), "rb");
	if (pFile == NULL) perror("Error opening file");
	else {
		fseek(pFile, 0, SEEK_END);
		size = ftell(pFile);
		fclose(pFile);
	}

	positions[(positions.size() - 1)] = size;

	return positions;
}
/**************************************************************************************************/
std::vector<unsigned long long> File::setFilePosEachLine(std::string filename, unsigned long long& num) {
	filename = getFullPathName(filename);

	std::vector<unsigned long long> positions;
	std::ifstream in;
	//openInputFile(filename, in);
	openInputFileBinary(filename, in);

	std::string input;
	unsigned long long count = 0;
	positions.push_back(0);

	while (!in.eof()) {
		//getline counting reads
		char d = in.get(); count++;
		while ((d != '\n') && (d != '\r') && (d != '\f') && (!in.eof())) {
			//get next character
			d = in.get();
			count++;
		}

		if (!in.eof()) {
			d = in.get(); count++;
			while (isspace(d) && (!in.eof())) { d = in.get(); count++; }
		}
		positions.push_back(count - 1);
		//cout << count-1 << endl;
	}
	in.close();

	num = positions.size() - 1;

	FILE * pFile;
	unsigned long long size;

	//get num bytes in file
	pFile = fopen(filename.c_str(), "rb");
	if (pFile == NULL) perror("Error opening file");
	else {
		fseek(pFile, 0, SEEK_END);
		size = ftell(pFile);
		fclose(pFile);
	}

	positions[(positions.size() - 1)] = size;

	return positions;
}

/**************************************************************************************************/

std::vector<unsigned long long> File::divideFile(std::string filename, int& proc) {
	std::vector<unsigned long long> filePos;
	filePos.push_back(0);

	FILE * pFile;
	unsigned long long size;

	filename = getFullPathName(filename);

	//get num bytes in file
	pFile = fopen(filename.c_str(), "rb");
	if (pFile == NULL) perror("Error opening file");
	else {
		fseek(pFile, 0, SEEK_END);
		size = ftell(pFile);
		fclose(pFile);
	}

	//estimate file breaks
	unsigned long long chunkSize = 0;
	chunkSize = size / proc;

	//file to small to divide by processors
	if (chunkSize == 0) { proc = 1;	filePos.push_back(size); return filePos; }

	if (proc > 1) {
		//for each process seekg to closest file break and search for next '>' char. make that the filebreak
		for (int i = 0; i < proc; i++) {
			unsigned long long spot = (i + 1) * chunkSize;

			std::ifstream in;
			openInputFile(filename, in);
			in.seekg(spot);

			//look for next '>'
			unsigned long long newSpot = spot;
			while (!in.eof()) {
				char c = in.get();

				if (c == '>') { in.putback(c); newSpot = in.tellg(); break; }
				else if (int(c) == -1) { break; }

			}

			//there was not another sequence before the end of the file
			unsigned long long sanityPos = in.tellg();

			if (sanityPos == -1) { break; }
			else { filePos.push_back(newSpot); }

			in.close();
		}
	}
	//save end pos
	filePos.push_back(size);

	//sanity check filePos
	for (int i = 0; i < (filePos.size() - 1); i++) {
		if (filePos[(i + 1)] <= filePos[i]) { filePos.erase(filePos.begin() + (i + 1)); i--; }
	}

	proc = static_cast<int>(filePos.size() - 1);
	return filePos;
}
/**************************************************************************************************/

std::vector<unsigned long long> File::divideFile(std::string filename, int& proc, char delimChar) {
	std::vector<unsigned long long> filePos;
	filePos.push_back(0);

	FILE * pFile;
	unsigned long long size;

	filename = getFullPathName(filename);

	//get num bytes in file
	pFile = fopen(filename.c_str(), "rb");
	if (pFile == NULL) perror("Error opening file");
	else {
		fseek(pFile, 0, SEEK_END);
		size = ftell(pFile);
		fclose(pFile);
	}

	char secondaryDelim = '>';
	if (delimChar == '@') { secondaryDelim = '+'; }

	//estimate file breaks
	unsigned long long chunkSize = 0;
	chunkSize = size / proc;

	//file to small to divide by processors
	if (chunkSize == 0) { proc = 1;	filePos.push_back(size); return filePos; }

	//for each process seekg to closest file break and search for next delimChar char. make that the filebreak
	for (int i = 0; i < proc; i++) {
		unsigned long long spot = (i + 1) * chunkSize;

		std::ifstream in;
		openInputFile(filename, in);
		in.seekg(spot);

		getline(in); //get to end of line in case you jump into middle of line where the delim char happens to fall.

					 //look for next delimChar
		unsigned long long newSpot = spot;
		while (!in.eof()) {
			char c = in.get();
			std::string input = ""; input += c;
			while ((c != '\n') && (c != '\r') && (c != '\f') && (c != EOF)) {
				c = in.get();
				input += c;
			}

			if (input.length() != 0) {
				if (input[0] == delimChar) { //this is a potential name line
					newSpot = in.tellg();
					newSpot -= input.length();
					//get two lines and look for secondary delim
					//inf a fasta file this would be a new sequence, in fastq it will be the + line, if this was a nameline.
					getline(in); gobble(in);
					if (!in.eof()) {
						std::string secondInput = getline(in); gobble(in);
						LOG(DEBUG) << " input= " + input + "\n secondaryInput = " + secondInput + "\n";
						if (secondInput[0] == secondaryDelim) { break; } //yes, it was a nameline so stop
						else { input = ""; gobble(in); } //nope it was a delim at the beginning of a non nameline, keep looking.
					}
				}
				else if (int(c) == -1) { break; }
				else { input = ""; gobble(in); }
			}
		}

		//there was not another sequence before the end of the file
		unsigned long long sanityPos = in.tellg();

		if (sanityPos == -1) { break; }
		else { filePos.push_back(newSpot); }

		in.close();
	}

	//save end pos
	filePos.push_back(size);

	//sanity check filePos
	for (int i = 0; i < (filePos.size() - 1); i++) {
		if (filePos[(i + 1)] <= filePos[i]) { filePos.erase(filePos.begin() + (i + 1)); i--; }
	}

	proc = (filePos.size() - 1);
	return filePos;
}

/**************************************************************************************************/

std::vector<unsigned long long> File::divideFilePerLine(std::string filename, int& proc) {
	std::vector<unsigned long long> filePos;
	filePos.push_back(0);

	FILE * pFile;
	unsigned long long size;

	filename = getFullPathName(filename);

	//get num bytes in file
	pFile = fopen(filename.c_str(), "rb");
	if (pFile == NULL) perror("Error opening file");
	else {
		fseek(pFile, 0, SEEK_END);
		size = ftell(pFile);
		fclose(pFile);
	}

	//estimate file breaks
	unsigned long long chunkSize = 0;
	chunkSize = size / proc;

	//file to small to divide by processors
	if (chunkSize == 0) { proc = 1;	filePos.push_back(size); return filePos; }

	//for each process seekg to closest file break and search for next '>' char. make that the filebreak
	for (int i = 0; i < proc; i++) {
		unsigned long long spot = (i + 1) * chunkSize;

		std::ifstream in;
		openInputFile(filename, in);
		in.seekg(spot);

		//look for next line break
		unsigned long long newSpot = spot;
		while (!in.eof()) {
			char c = in.get();

			if ((c == '\n') || (c == '\r') || (c == '\f')) { gobble(in); newSpot = in.tellg(); break; }
			else if (int(c) == -1) { break; }
		}

		//there was not another line before the end of the file
		unsigned long long sanityPos = in.tellg();

		if (sanityPos == -1) { break; }
		else { filePos.push_back(newSpot); }

		in.close();
	}

	//save end pos
	filePos.push_back(size);

	//sanity check filePos
	for (int i = 0; i < (filePos.size() - 1); i++) {
		if (filePos[(i + 1)] <= filePos[i]) { filePos.erase(filePos.begin() + (i + 1)); i--; }
	}

	proc = (filePos.size() - 1);
	return filePos;
}
/**************************************************************************************************/
int File::divideFile(std::string filename, int& proc, std::vector<std::string>& files) {

	std::vector<unsigned long long> filePos = divideFile(filename, proc);

	for (int i = 0; i < (filePos.size() - 1); i++) {

		//read file chunk
		std::ifstream in;
		openInputFile(filename, in);
		in.seekg(filePos[i]);
		unsigned long long size = filePos[(i + 1)] - filePos[i];
		char* chunk = new char[size];
		in.read(chunk, size);
		in.close();

		//open new file
		std::string fileChunkName = filename + "." + toString(i) + ".tmp";
		std::ofstream out;
		openOutputFile(fileChunkName, out);

		out << chunk << std::endl;
		out.close();
		delete[] chunk;

		//save name
		files.push_back(fileChunkName);
	}

	return 0;
}
/***********************************************************************/
bool File::isBlank(std::string fileName) {

	fileName = getFullPathName(fileName);

	std::ifstream fileHandle;
	fileHandle.open(fileName.c_str());
	if (!fileHandle) {
		LOG(LOGERROR) << "Could not open " + fileName;
		return false;
	}
	else {
		//check for blank file
		zapGremlins(fileHandle);
		gobble(fileHandle);
		if (fileHandle.eof()) { fileHandle.close(); return true; }
		fileHandle.close();
	}
	return false;
}

/***********************************************************************/
int File::openOutputFileAppend(std::string fileName, std::ofstream& fileHandle) {
	fileName = getFullPathName(fileName);

	fileHandle.open(fileName.c_str(), std::ios::app);
	if (!fileHandle) {
		LOG(LOGERROR) << "Could not open " + fileName;
		return 1;
	}
	else {
		return 0;
	}
}
/***********************************************************************/
int File::openOutputFileBinaryAppend(std::string fileName, std::ofstream& fileHandle) {
	fileName = getFullPathName(fileName);

	fileHandle.open(fileName.c_str(), std::ios::app | std::ios::binary);
	if (!fileHandle) {
		LOG(LOGERROR) << "Could not open " + fileName;
		return 1;
	}
	else {
		return 0;
	}
}

/***********************************************************************/
void File::gobble(std::istream& f) {

	char d;
	while (isspace(d = f.get())) { ; }
	if (!f.eof()) { f.putback(d); }
}
/***********************************************************************/
void File::gobble(std::istringstream& f) {
	char d;
	while (isspace(d = f.get())) { ; }
	if (!f.eof()) { f.putback(d); }
}
/***********************************************************************/
void File::zapGremlins(std::istream& f) {

	char d;
	while ('\0' == (d = f.get())) { ; }
	if (!f.eof()) { f.putback(d); }
}
/***********************************************************************/
void File::zapGremlins(std::istringstream& f) {
	char d;
	while ('\0' == (d = f.get())) { ; }
	if (!f.eof()) { f.putback(d); }
}

/***********************************************************************/

std::string File::getline(std::istringstream& fileHandle) {

	std::string line = "";

	while (!fileHandle.eof()) {
		//get next character
		char c = fileHandle.get();

		//are you at the end of the line
		if ((c == '\n') || (c == '\r') || (c == '\f')) { break; }
		else { line += c; }
	}

	return line;

}
/***********************************************************************/

std::string File::getline(std::ifstream& fileHandle) {

	std::string line = "";

	while (fileHandle) {
		//get next character
		char c = fileHandle.get();

		//are you at the end of the line
		if ((c == '\n') || (c == '\r') || (c == '\f') || (c == EOF)) { break; }
		else { line += c; }
	}

	return line;

}
/***********************************************************************/
int File::getNumSeqs(std::ifstream& file) {
	int numSeqs = count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '>');
	file.seekg(0);
	return numSeqs;
}
/***********************************************************************/
void File::getNumSeqs(std::ifstream& file, int& numSeqs) {
	std::string input;
	numSeqs = 0;
	while (!file.eof()) {
		input = getline(file);
		if (input.length() != 0) {
			if (input[0] == '>') { numSeqs++; }
		}
	}
}
/***********************************************************************/
std::string File::findProgramPath(std::string programName) {
	std::string pPath = "";

	//look in ./
	//is this the programs path?
	std::ifstream in5;
	std::string tempIn = ".";
	tempIn += path_delimiter + programName;
	openInputFile(tempIn, in5, "");

	//if this file exists
	if (in5) { in5.close(); pPath = getFullPathName(tempIn); LOG(DEBUG) << "found it, programPath = " + pPath + "\n"; return pPath; }

	char* cEnvPath = getenv("PATH");
	std::string envPath = cEnvPath;
	free(cEnvPath);
	//delimiting path char
	char delim;
#if defined (UNIX)
	delim = ':';
#else
	delim = ';';
#endif

	//break apart path variable by ':'
	std::vector<std::string> dirs = Utility::split(envPath, delim);

	LOG(DEBUG) << "dir's in path: \n";

	//get path related to mothur
	for (int i = 0; i < dirs.size(); i++) {

		LOG(DEBUG) << "" + dirs[i] + "\n";

		//to lower so we can find it
		std::string tempLower = "";
		for (int j = 0; j < dirs[i].length(); j++) { tempLower += tolower(dirs[i][j]); }

		//is this mothurs path?
		if (tempLower.find(programName) != -1) { pPath = dirs[i]; break; }
	}

	LOG(DEBUG) << "programPath = " + pPath + "\n";

	if (pPath != "") {
		//add programName so it looks like what argv would look like
		pPath += path_delimiter + programName;
	}
	else {
		//okay programName is not in the path, so the folder programName is in must be in the path
		//lets find out which one

		//get path related to the program
		for (int i = 0; i < dirs.size(); i++) {

			LOG(DEBUG) << "looking in " + dirs[i] + " for " + programName + " \n";

			//is this the programs path?
			std::ifstream in;
			std::string tempIn = dirs[i];
			tempIn += path_delimiter + programName;
			openInputFile(tempIn, in, "");

			//if this file exists
			if (in) { in.close(); pPath = tempIn; LOG(DEBUG) << "found it, programPath = " + pPath + "\n"; break; }
		}
	}

	return pPath;

}

bool File::dirCheck(std::string& dirName, bool reportError) {

	if (dirName == "") { return false; }

	std::string tag = "";

	//add / to name if needed
	if (dirName[dirName.length() - 1] != path_delimiter) { dirName += path_delimiter; }

	//test to make sure directory exists
	dirName = getFullPathName(dirName);
	std::string outTemp = dirName + tag + "temp" + toString(time(NULL));
	std::ofstream out;
	out.open(outTemp.c_str(), std::ios::trunc);
	if (!out && reportError) {
		LOG(LOGERROR) << dirName << " directory does not exist or is not writable.";
	}
	else {
		out.close();
		File::remove(outTemp);
		return true;
	}

	return false;

}
/***********************************************************************/
std::string File::getExtension(std::string longName) {
	std::string extension = "";

	if (longName.find_last_of('.') != longName.npos) {
		int pos = longName.find_last_of('.');
		extension = longName.substr(pos, longName.length());
	}

	return extension;
}
