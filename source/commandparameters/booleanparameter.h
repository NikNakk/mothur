#pragma once

#include "commandparameterbase.h"

class BooleanParameter : public CommandParameterBase {
public:
	explicit BooleanParameter(bool & value, std::string name, bool defaultValue = false, bool required = false, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		CommandParameterBase(name, CommandParameterType::Boolean, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		value(value), defaultValue(defaultValue)
	{}
	virtual std::string getValue() const override {
		return std::to_string(value);
	}
	virtual bool hasValue() { return valueSet; }
	bool getBooleanValue() {
		return value;
	}
	virtual void validateAndSet(std::string newValue);
protected:
	bool & value;
	bool defaultValue = false;
	bool valueSet = false;
};

