#pragma once

#include "multipleparameter.h"

class CalcParameter : public MultipleParameter {
public:
	CalcParameter(std::string & value, std::string name = "calc", std::string defaultOption = "sobs-chao-ace-jack-shannon-npshannon-simpson-shannonrange", bool chooseMultiple = false, bool required = false, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		MultipleParameter(value, name, std::vector<std::string>{"sobs", "chao", "nseqs", "coverage", "ace", "jack", "shannon", "shannoneven", "npshannon", "heip", "smithwilson", "simpson", "simpsoneven", "invsimpson", "bootstrap", "geometric", "qstat", "logseries", "bergerparker", "bstick", "goodscoverage", "efron", "boneh", "solow", "shen"},
			defaultOption, chooseMultiple, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup)
	{}
};
