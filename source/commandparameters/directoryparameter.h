#pragma once

#include "inputtypeparameter.h"

class DirectoryParameter : public InputTypeParameter {
public:
	DirectoryParameter(std::string & value, Settings & settings, std::string name, bool required, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		InputTypeParameter(settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		value(value), settings(settings) {}
	virtual std::string getValue() const override {
		return value;
	}
	virtual void validateAndSet(std::string value);
protected:
	std::string & value;
	Settings & settings;
};

