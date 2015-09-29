#ifndef outputtypes_h
#define outputtypes_h

#include <string>
#include <vector>
#include <map>

using namespace std;

class OutputType {
public:
	OutputType() {};
	OutputType(string name, string filePattern) : name(name), filePattern(filePattern) {};
	string getOutputFileName(map<string, string> variableParts);
	vector<string> files;
private:
	string name;
	string filePattern;
};

class OutputTypeCollection : map<string, OutputType>{
public:
	OutputTypeCollection() {};
	void add(string name, string filePattern) { (*this)[name] = OutputType(name, filePattern); }
};

#endif
