#pragma once

#include "commandparameterbase.h"

class IntegerParameter : public CommandParameterBase {
public:
	explicit IntegerParameter(int& value, std::string name, int minValue = INT_MIN, int maxValue = INT_MAX, int defaultValue = INT_MAX,
		bool required = false, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		CommandParameterBase(name, CommandParameterType::Number, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		value(value), minValue(minValue), maxValue(maxValue), defaultValue(defaultValue) {}
	virtual std::string getValue() const override {
		return std::to_string(value);
	}
	int getIntegerValue() {
		return value;
	}
	virtual void validateAndSet(std::string newValue) override;
protected:
	int& value;
	int defaultValue = INT_MIN;
	int minValue = INT_MIN;
	int maxValue = INT_MAX;
};
