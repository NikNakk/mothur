#pragma once

#include "inputtypeparameter.h"

class ReferenceFileParameter : public InputTypeParameter {
public:
	ReferenceFileParameter(Settings& settings, Ystring name = "reference", bool required = false, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		InputTypeParameter(settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual string getValue() {
		return value;
	}
	virtual void validateAndSet(string newValue);
private:
	string value;
};
