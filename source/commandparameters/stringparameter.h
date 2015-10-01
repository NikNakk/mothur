#pragma once

#include "commandparameterbase.h"

using namespace std;

class StringParameter : public CommandParameterBase {
public:
	explicit StringParameter(string name, string defaultOption, bool required = false, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		CommandParameterBase(name, String, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		defaultValue(defaultValue)Y
	{}
	virtual string getValue() { return value; }
	virtual void validateAndSet(string newValue) { value = newValue; }
protected:
	string value;
	string defaultValue;
};
