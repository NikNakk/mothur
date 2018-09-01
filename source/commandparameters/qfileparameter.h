#pragma once

#include "inputtypeparameter.h"

class QFileFileParameter : public InputTypeParameter {
public:
	QFileFileParameter(std::string& countfile, Settings& settings, std::string name, bool required, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		InputTypeParameter(settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		value(countfile) {}
	virtual std::string getValue() const override {
		return value;
	}
	virtual void validateAndSet(std::string newValue);
private:
	std::string& value;
};

