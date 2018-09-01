#pragma once
#include "commandparameterbase.h"

class NumberParameter : public CommandParameterBase {
public:
	explicit NumberParameter(double & value, std::string name, double minValue = -INFINITY, double maxValue = INFINITY, double defaultValue = NAN,
		bool required = false, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		CommandParameterBase(name, CommandParameterType::Number, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		value(value), minValue(minValue), maxValue(maxValue), defaultValue(defaultValue) {}
	virtual std::string getValue() const override {
		return std::to_string(value);
	}
	double getNumericValue() {
		return value;
	}
	virtual void validateAndSet(std::string newValue);
protected:
	double & value;
	double defaultValue = NAN;
	double minValue = -INFINITY;
	double maxValue = INFINITY;
};
