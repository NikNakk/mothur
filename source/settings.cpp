#include "settings.h"
#include <sstream>
#include <iterator>

void Settings::setCurrent(std::string type, std::string value)
{
	if (value == "") {
		currentfiles.erase(type);
	}
	else {
		currentfiles[type] = value;
		if (type == "counttable") {
			groupMode = "count";
		}
		else if (type == "group") {
			groupMode = "group";
		}
	}
}

std::string Settings::getAllCurrent()
{
	std::ostringstream os;
	for (auto it = currentfiles.begin(); it != currentfiles.end(); ++it) {
		os << it->first << '=' << it->second << '\n';
	}
	os << "processors=" << processors;
	return os.str();
}
