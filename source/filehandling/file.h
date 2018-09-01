#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <map>
#include "g3log/g3log.hpp"

class File{
public:
	File() = default;
	virtual ~File() = default;
	File(std::string fileName) : fileName(fileName) {}
	virtual std::string getFileName() const { return fileName; }
	virtual std::string getSimpleRootName() const { return getRootName(getSimpleName(fileName)); }
	virtual void setFileName(std::string fileName) { this->fileName = fileName; }
	static bool FileExists(const std::string& name);
	static std::string getSimpleRootName(std::string longName) { return getRootName(getSimpleName(longName)); }
	static std::string getRootName(std::string longName);
	static std::string getSimpleName(std::string longName);
	static std::string getPath(std::string longName);
	static std::string getFullPathName(std::string fileName);
	static int remove(std::string filename);
	static int openInputFile(std::string fileName, std::ifstream & fileHandle, std::string m);
	static int openInputFile(std::string fileName, std::ifstream & fileHandle);
	static int openInputFileBinary(std::string fileName, std::ifstream & fileHandle);
	static int openInputFileBinary(std::string fileName, std::ifstream & fileHandle, std::string noerror);
	static std::vector<bool> allGZFiles(std::vector<std::string>& files);
	static std::vector<bool> isGZ(std::string filename);
	static int renameFile(std::string oldName, std::string newName);
	static int openOutputFile(std::string fileName, std::ofstream & fileHandle);
	static int openOutputFileBinary(std::string fileName, std::ofstream & fileHandle);
	static int appendFiles(std::string temp, std::string filename);
	static int appendFilesAndRemove(std::string temp, std::string existing);
	static int appendBinaryFiles(std::string temp, std::string filename);
	static int appendSFFFiles(std::string temp, std::string filename);
	static int appendFilesWithoutHeaders(std::string temp, std::string filename);
	static std::string sortFile(std::string distFile, std::string outputDir);
	static std::vector<unsigned long long> setFilePosFasta(std::string filename, long long & num);
	static std::vector<unsigned long long> setFilePosFasta(std::string filename, long long & num, char delim);
	static std::vector<unsigned long long> setFilePosFasta(std::string filename, int & num);
	//static std::vector<consTax> readConsTax(std::string inputfile);
	//static int readConsTax(std::string inputfile, map<int, consTax2>& taxes);
	static std::vector<unsigned long long> setFilePosEachLine(std::string filename, int & num);
	static std::vector<unsigned long long> setFilePosEachLine(std::string filename, unsigned long long & num);
	static std::vector<unsigned long long> divideFile(std::string filename, int & proc);
	static std::vector<unsigned long long> divideFile(std::string filename, int & proc, char delimChar);
	static std::vector<unsigned long long> divideFilePerLine(std::string filename, int & proc);
	static int divideFile(std::string filename, int & proc, std::vector<std::string>& files);
	static bool isBlank(std::string fileName);
	static int openOutputFileAppend(std::string fileName, std::ofstream & fileHandle);
	static int openOutputFileBinaryAppend(std::string fileName, std::ofstream & fileHandle);
	static void gobble(std::istream & f);
	static void gobble(std::istringstream & f);
	static void zapGremlins(std::istream & f);
	static void zapGremlins(std::istringstream & f);
	static std::string getline(std::istringstream & fileHandle);
	static std::string getline(std::ifstream & fileHandle);
	static int getNumSeqs(std::ifstream & file);
	static void getNumSeqs(std::ifstream & file, int & numSeqs);
	static std::string findProgramPath(std::string programName);
	static bool dirCheck(std::string & dirName, bool reportError = true);
	static std::string getExtension(std::string longName);
protected:
	std::string fileName;
};

class InvalidFile : public std::logic_error {
public:
	InvalidFile(std::string message) : std::logic_error(message) {}
};