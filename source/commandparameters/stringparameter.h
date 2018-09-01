#pragma once

#include "commandparameterbase.h"

class StringParameter : public CommandParameterBase {
public:
	StringParameter(std::string & value, std::string name, std::string defaultOption, bool required = false, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		CommandParameterBase(name, CommandParameterType::String, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		value(value), defaultValue(defaultValue)
	{}
	virtual std::string getValue() const override { return value; }
	virtual void validateAndSet(std::string newValue) { value = newValue; }
protected:
	std::string & value;
	std::string defaultValue;
};
