#pragma once

#include "inputtypeparameter.h"

class ReferenceFileParameter : public InputTypeParameter {
public:
	ReferenceFileParameter(Settings& settings, std::string name = "reference", bool required = false, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		InputTypeParameter(settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual std::string getValue() const override {
		return value;
	}
	virtual void validateAndSet(std::string newValue);
private:
	std::string value;
};
