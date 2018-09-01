#pragma once

#include <map>
#include <string>
#include <vector>

class GroupMap : public std::map<std::string, std::string> {
public:
	GroupMap() = default;
	GroupMap(const std::string & filename);

	bool writeGroupFile(const std::string & filename, const std::vector<std::string> & nameOrder);

	bool valid = true;
};