#include "utility.h"
#include <sstream>
#include <iterator>

// Split taken from http://stackoverflow.com/a/236803/4998761

std::vector<std::string>& Utility::split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> Utility::split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

std::string Utility::join(std::vector<std::string>& sv, const std::string& delim)
{
	std::ostringstream ss;
	for (std::vector<std::string>::iterator it = sv.begin(); it != sv.end(); it++) {
		ss << *it << delim;
	}
	std::string output;
	output = ss.str();
	if (output != "") {
		output = output.substr(0, output.length() - delim.length());
	}
	return output;
}

std::vector<char*>& Utility::to_c_strs(std::vector<std::string>& sv, std::vector<char*>& output) {
	for (std::vector<std::string>::iterator it = sv.begin(); it != sv.end(); it++) {
		output.push_back(const_cast<char*>(it->c_str()));
	}
	return output;
}