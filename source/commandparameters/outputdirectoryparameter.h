#pragma once

#include "directoryparameter.h"

class OutputDirectoryParameter : public DirectoryParameter {
public:
	explicit OutputDirectoryParameter(std::string & value, Settings& settings, std::string name = "outputdir", bool required = false, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		DirectoryParameter(value, settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual void validateAndSet(std::string value);
private:
	std::string value;
};

