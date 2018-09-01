#include "namemap.h"
#include "filehandling/textfileread.h"
#include "filehandling/textfilewrite.h"
#include "utility.h"
#include <algorithm>
#include "mothurdefs.h"

NamesWithTotals::NamesWithTotals(const std::string & filename) {
	TextFileRead fil(filename);
	long long lineCount = 0;
	while (fil.good() && !ctrlc_pressed && valid) {
		std::vector<std::string> columns = fil.readColumns();
		++lineCount;
		if (columns.size() != 2) {
			LOG(LOGERROR) << "Invalid Name file at line " << std::to_string(lineCount);
			valid = false;
		}
		(*this)[columns[0]] = std::count(columns[1].begin(), columns[1].end(), ',');
	}
	if (ctrlc_pressed || !valid) {
		valid = false;
		clear();
	}
}

long long NamesWithTotals::getTotal(std::string name, bool reportError) {
	long long count = 1;
	if (this->size() > 0) {
		auto it = this->find(name);
		if (it == this->end()) {
			if (reportError) {
				LOG(LOGERROR) << "'" + name + "' is not in your name or count file, please correct.";
				count = 0;
			}
		}
		else {
			count = it->second;
		}
	}
	return count;
}

//long long NameMapRawLine::countNames() const
//{
//	return std::count(this->begin(), this->end(), ',');
//}

NameMapUniqueToDups::NameMapUniqueToDups(const std::string & filename) {
	long long lineCount = 0;
	TextFileRead fil(filename);
	while (fil.good() && !ctrlc_pressed && valid) {
		std::vector<std::string> cols = fil.readColumns();
		if (cols.size() == 0) {
			LOG(LOGERROR) << "Invalid Name file at line " << std::to_string(lineCount);
			valid = false;
		}
		++lineCount;
		(*this)[cols[0]] = Utility::split(cols[1], ',');
	}
	if (ctrlc_pressed || !valid) {
		valid = false;
		clear();
	}
}

bool NameMapUniqueToDups::writeNameFile(const std::string & filename, const std::vector<std::string>& nameOrder)
{
	TextFileWrite fil(filename);
	for (auto it = nameOrder.begin(); it != nameOrder.end() && fil.good() && !ctrlc_pressed; ++it) {
		auto nmIt = this->find(*it);
		if (nmIt == this->end()) {
			LOG(LOGERROR) << "Invalid name when writing name file";
			return false;
		}
		fil.writeLine(*it + '\t' + Utility::join(nmIt->second, ","));
	}
	if (fil.good() && !ctrlc_pressed) {
		fil.commit();
		return true;
	}
	else {
		return false;
	}
}

std::unordered_set<std::string> NameMapUniqueToDups::removeNames(const std::string & namefile, const std::string & outNamefile, std::set<std::string> badNames)
{
	std::set<std::string> badAllNames;

	TextFileRead inputNames(namefile);
	TextFileWrite outputNames(outNamefile);
	while (!inputNames.eof()) {
		std::vector<std::string> cols = inputNames.readColumns();
		auto it = badNames.find(cols[0]);

		if (it != badNames.end()) {
			std::vector<std::string> seqNames = Utility::split(cols[1], ',');
			badAllNames.insert(seqNames.begin(), seqNames.end());
			badNames.erase(it);
		}
		else {
			outputNames.writeTwoColumns(cols[0], cols[1]);
		}
	}

	//we were unable to remove some of the bad sequences
	if (badNames.size() != 0) {
		for (auto badSeq : badNames) {
			LOG(WARNING) << "Your namefile does not include the sequence " + badSeq + " please correct.";
		}
	}
	return badAllNames;
}

