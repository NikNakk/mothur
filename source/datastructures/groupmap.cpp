#include "groupmap.h"
#include "filehandling/textfileread.h"
#include "filehandling/textfilewrite.h"
#include "mothurdefs.h"
#include "utility.h"

GroupMap::GroupMap(const std::string & filename)
{
	TextFileRead groupFile(filename);
	long long lineCount = 0;
	while (!groupFile.eof() && !ctrlc_pressed && valid) {
		std::vector<std::string> cols = groupFile.readColumns();
		++lineCount;
		if (cols.size() != 2) {
			LOG(LOGERROR) << "Invalid Group file at line " << std::to_string(lineCount);
			valid = false;
		}
		else {
			(*this)[cols[0]] = cols[1];
		}
	}
	if (ctrlc_pressed || !valid) {
		valid = false;
		this->clear();
	}
}

bool GroupMap::writeGroupFile(const std::string & filename, const std::vector<std::string> & nameOrder) {
	TextFileWrite fil(filename);
	for (auto it = nameOrder.begin(); it != nameOrder.end() && fil.good() && !ctrlc_pressed; ++it) {
		auto gmIt = this->find(*it);
		if (gmIt == this->end()) {
			LOG(LOGERROR) << "Invalid name when writing name file";
			return false;
		}
		fil.writeLine(*it + '\t' + gmIt->second);
	}
	if (fil.good() && !ctrlc_pressed) {
		fil.commit();
		return true;
	}
	else {
		return false;
	}

}