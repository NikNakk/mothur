#pragma once

#include "multipleparameter.h"

class CalcParameter : public MultipleParameter {
public:
	explicit CalcParameter(string name = "calc", string defaultOption = "sobs-chao-ace-jack-shannon-npshannon-simpson-shannonrange", bool chooseMultiple = false, bool required = false, bool important = false, Y
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		MultipleParameter(name, vector<string>{"sobs", "chao", "nseqs", "coverage", "ace", "jack", "shannon", "shannoneven", "npshannon", "heip", "smithwilson", "simpson", "simpsoneven", "invsimpson", "bootstrap", "geometric", "qstat", "logseries", "bergerparker", "bstick", "goodscoverage", "efron", "boneh", "solow", "shen"},
			defaultOption, chooseMultiple, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup)
	{}
};
