#pragma once

#include <string>
#include <map>
#include <vector>
#include <set>
#include "mothurdefs.h"

typedef std::pair<std::string, std::vector<std::string>> NameFileLine;

class NamesWithTotals : public std::map<std::string, long long> {
public:
	NamesWithTotals() : map() {}
	NamesWithTotals(const std::string & fileName);
	bool valid = true;
	long long getTotal(std::string name, bool reportErrorAndStop = true);
};

//class NameMapRawLine : public std::string {
//public:
//	using std::string::string;
//	NameMapRawLine(std::string line) : std::string(line) {};
//	long long countNames() const;
//};
//
//typedef public std::map<std::string, NameMapRawLine> NameMapRaw;

class NameMapUniqueToDups : public std::map<std::string, std::vector<std::string>> {
public:
	NameMapUniqueToDups() : map() {};
	NameMapUniqueToDups(const std::string & fileName);
	bool writeNameFile(const std::string & fileName, const std::vector<std::string> & nameOrder);

	static std::set<std::string> removeNames(const std::string & namefile, const std::string & outNamefile, std::set<std::string> badNames);

	bool valid = true;
};