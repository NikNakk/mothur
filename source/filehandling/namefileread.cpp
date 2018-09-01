#include "namefileread.h"
#include "utility.h"

inline NameFileLine NameFileRead::readNameFileLine()
{
	NameFileLine result;
	lineCount++;
	std::vector<std::string> cols = readColumns();
	if (cols.size() == 2) {
		result.first = cols[0];
		result.second = Utility::split(cols[1], ',');
	}
	return result;
}

std::vector<std::string> NameFileRead::readRawNameFileLine()
{
	return readColumns();
}

NamesWithTotals NameFileRead::readNamesWithTotals()
{
	NamesWithTotals nwt;
	long long lineCount = 0;
	while (!this->eof() && !ctrlc_pressed && nwt.valid) {
		std::vector<std::string> columns = this->readColumns();
		++lineCount;
		if (columns.size() != 2) {
			LOG(LOGERROR) << "Invalid Name file at line " << std::to_string(lineCount);
			nwt.valid = false;
		}
		nwt[columns[0]] = std::count(columns[1].begin(), columns[1].end(), ',');
	}
	if (ctrlc_pressed || !nwt.valid) {
		nwt.valid = false;
		nwt.clear();
	}
	return nwt;
}

NameMapUniqueToDups NameFileRead::readNameMapUniqueToDups()
{
	long long lineCount = 0;
	NameMapUniqueToDups nmap;
	while (!this->eof() && !ctrlc_pressed && nmap.valid) {
		std::pair<std::string, std::vector<std::string>> nameFileLine = readNameFileLine();
		++lineCount;
		if (nameFileLine.first.length == 0) {
			LOG(LOGERROR) << "Invalid Name file at line " << std::to_string(lineCount);
			nmap.valid = false;
		}
		nmap.emplace(nameFileLine);
	}
	if (ctrlc_pressed || !nmap.valid) {
		nmap.valid = false;
		nmap.clear();
	}
	return nmap;
}
