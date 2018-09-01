#include "countfilewrite.h"
#include "utility.h"

bool CountFileWrite::writeCountTable(const CountTable & ct, std::vector<std::string> nameOrder)
{
	if (!good()) {
		return false;
	}
	write("Representative_Sequence\ttotal");
	bool hasGroups = (ct.getGroupNames().size() > 0);
	if (hasGroups) {
		writeLine(Utility::join(ct.getGroupNames(), "\t"));
	}
	if (!good()) {
		return false;
	}
	for (auto it = nameOrder.begin(); it != nameOrder.end() && good() && !ctrlc_pressed; it++) {
		long long total = ct.getTotal(*it);
		if (total == 0) {
			LOG(LOGERROR) << "Mismatch in countfile print.";
			return false;
		}
		else {
			write(*it + '\t' + std::to_string(total) + '\t');
			if (hasGroups) {
				write(Utility::join(ct.getCounts(*it), "\t"));
			}
			write("\n");
		}
	}
	if (good() && !ctrlc_pressed) {
		commit();
		return true;
	}
	else {
		return false;
	}
}
