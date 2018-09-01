#pragma once

#include <string>
#include <vector>
#include <map>

class OutputType {
public:
	OutputType() {};
	OutputType(std::string name, std::string filePattern) : name(name), filePattern(filePattern) {};
	std::string getFilePattern() { return filePattern; }
	std::vector<std::string> files;
private:
	string name;
	string filePattern;
};

class OutputTypeCollection : public std::map<string, OutputType>{
public:
	OutputTypeCollection() {};
	void add(std::string name, std::string filePattern) { (*this)[name] = OutputType(name, filePattern); }
};


