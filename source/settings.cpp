#include "settings.h"
#include <sstream>
#include <iterator>

void Settings::setCurrent(string type, string value)
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

string Settings::getAllCurrent()
{
	ostringstream os;
	for (map<string, string>::iterator it = currentfiles.begin(); it != currentfiles.end(); it++) {
		os << it->first << '=' << it->second << '\n';
	}
	os << "processors=" << processors;
	return os.str();
}
