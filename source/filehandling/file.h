#pragma once

#include <string>
using namespace std;

class File {
public:
	static bool FileExists(const string& name);
	static string getRootName(string longName);
	static string getSimpleName(string longName);
	static string getPath(string longName);
	static string getFullPathName(string fileName);
	static int remove(string filename);
	static int openInputFile(string fileName, ifstream & fileHandle, string m);
	static int openInputFile(string fileName, ifstream & fileHandle);
	static int openInputFileBinary(string fileName, ifstream & fileHandle);
	static int openInputFileBinary(string fileName, ifstream & fileHandle, string noerror);
#ifdef USE_BOOST
	int openInputFileBinary(string fileName, ifstream & file, boost::iostreams::filtering_istream & in);
	int openInputFileBinary(string fileName, ifstream & file, boost::iostreams::filtering_istream & in, string noerror);
#endif
	static vector<bool> allGZFiles(vector<string>& files);
	static vector<bool> isGZ(string filename);
	static int renameFile(string oldName, string newName);
	static int openOutputFile(string fileName, ofstream & fileHandle);
	static int openOutputFileBinary(string fileName, ofstream & fileHandle);
	static int appendFiles(string temp, string filename);
	static int appendBinaryFiles(string temp, string filename);
	static int appendSFFFiles(string temp, string filename);
	static int appendFilesWithoutHeaders(string temp, string filename);
	static string sortFile(string distFile, string outputDir);
	static vector<unsigned long long> setFilePosFasta(string filename, long long & num);
	static vector<unsigned long long> setFilePosFasta(string filename, long long & num, char delim);
	static vector<unsigned long long> setFilePosFasta(string filename, int & num);
	static vector<consTax> readConsTax(string inputfile);
	static int readConsTax(string inputfile, map<int, consTax2>& taxes);
	static vector<unsigned long long> setFilePosEachLine(string filename, int & num);
	static vector<unsigned long long> setFilePosEachLine(string filename, unsigned long long & num);
	static vector<unsigned long long> divideFile(string filename, int & proc);
	static vector<unsigned long long> divideFile(string filename, int & proc, char delimChar);
	static vector<unsigned long long> divideFilePerLine(string filename, int & proc);
	static int divideFile(string filename, int & proc, vector<string>& files);
	static bool isBlank(string fileName);
	static int openOutputFileAppend(string fileName, ofstream & fileHandle);
	static int openOutputFileBinaryAppend(string fileName, ofstream & fileHandle);
	static void gobble(istream & f);
	static void gobble(istringstream & f);
	static void zapGremlins(istream & f);
	static void zapGremlins(istringstream & f);
	static string getline(istringstream & fileHandle);
	static string getline(ifstream & fileHandle);
	static int getNumSeqs(ifstream & file);
	static void getNumSeqs(ifstream & file, int & numSeqs);
	static string findProgramPath(string programName);
};
