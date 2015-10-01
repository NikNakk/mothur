#pragma once

#include "commandparameterbase.h"

using namespace std;

class IntegerParameter : public CommandParameterBase {
public:
	explicit IntegerParameter(string name, int minValue = LONG_MIN, int maxValue = LONG_MAX, int defaultValue = LONG_MIN,
		bool required = false, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		CommandParameterBase(name, Number, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		minValue(minValue), maxValue(maxValue), defaultValue(defaultValue) {}
	virtual string getValue() {
		return std::to_string(value);
	}
	int getIntegerValue() {
		return value;
	}
	virtual void validateAndSet(string newValue);
protected:Y
	int value = 0;
		  int defaultValue = LONG_MIN;
		  int minValue = LONG_MIN;
		  int maxValue = LONG_MAX;
};
