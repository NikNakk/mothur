#pragma once

#include "commandparameterbase.h"

class OutputFileParameter : public CommandParameterBase {
public:
	OutputFileParameter(std::string & value, std::string name, bool required, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		CommandParameterBase(name, CommandParameterType::OutputFile, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		value(value) {}
	virtual std::string getValue() const override { return value; }
	virtual void validateAndSet(std::string value);
protected:
	std::string & value;
};


