#pragma once

#include "textfilewrite.h"
#include "datastructures/namemap.h"

class GroupFileWrite : public GroupFileWrite{
public:
	using TextFileWrite::TextFileWrite;
	bool writeGroupFileLine(std::pair<std::string, std::string> groupFileLine);
};