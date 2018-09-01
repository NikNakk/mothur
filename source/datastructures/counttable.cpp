#include "counttable.h"
#include "filehandling/textfileread.h"
#include "filehandling/textfilewrite.h"
#include "utility.h"
#include "mothur.h"

CountTable::CountTable(std::string filename) {
	TextFileRead fil(filename);
	std::vector<std::string> cols = fil.readColumns();
	if (cols.size() < 2 || cols[0] != "Representative_Sequence" || cols[1] != "total") {
		LOG(LOGERROR) << "Invalid Count file, expected first line to begin Representative_Sequence\ttotal";
		valid = false;
		return;
	}
	std::vector<std::string> groupNames;
	if (cols.size() > 2) {
		for (size_t i = 2; i < cols.size(); i++) {
			addGroupName(cols[i]);
		}
	}
	size_t nGroups = groupNames.size();
	int lineCount = 0;
	std::string errorMessage;
	while (fil.good() && errorMessage.length() == 0) {
		++lineCount;
		cols = fil.readColumns();
		if (cols.size() > 0) {
			if (cols.size() != nGroups) {
				errorMessage = "Invalid Count file, each line should have the same number of groups";
				break;
			}
			std::string name = cols[0];
			std::vector<long long> counts;
			long long totalCount;
			long long runningTotal;
			try {
				totalCount = std::stoll(cols[1]);
				runningTotal = 0;
				for (size_t i = 2; i < cols.size(); i++) {
					long long count = std::stoll(cols[i]);
					counts.push_back(count);
					runningTotal += count;
				}
			}
			catch (std::invalid_argument&) {
				errorMessage = "Invalid Count file: invalid number at line " + lineCount;
				break;
			}
			catch (std::out_of_range&) {
				errorMessage = "Invalid Count file: invalid number at line " + lineCount;
				break;
			}
			if (runningTotal != totalCount) {
				errorMessage = "Invalid Count file: totals don't add up at line " + lineCount;
				break;
			}
			setCounts(name, counts, totalCount);
		}
	}
	if (errorMessage.length() != 0) {
		LOG(LOGERROR) << errorMessage;
		this->clear();
		valid = false;
		nameTotals.valid = false;
	}
}

bool CountTable::writeCountTable(std::string filename, std::vector<std::string> nameOrder) {
	TextFileWrite fil(filename);
	if (!fil.good()) {
		return false;
	}
	fil.write("Representative_Sequence\ttotal");
	bool hasGroups = (groupNames.size() > 0);
	if (hasGroups) {
		fil.writeLine(Utility::join(groupNames, "\t"));
	}
	if (!(fil.good())) {
		return false;
	}
	for (auto it = nameOrder.begin(); it != nameOrder.end() && fil.good() && !ctrlc_pressed; it++) {
		auto ntIt = nameTotals.find(*it);
		if (ntIt == nameTotals.end()) {
			LOG(LOGERROR) << "mismatch in countfile print.\n";
			return false;
		}
		else {
			fil.write(*it + '\t' + std::to_string(nameTotals[*it]) + '\t');
			if (hasGroups) {
				fil.write(Utility::join(nameCounts[*it], "\t"));
			}
			fil.write("\n");
		}
	}
	if (fil.good()) {
		fil.commit();
		return true;
	}
	else {
		return false;
	}
}