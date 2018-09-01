#pragma once

#include "inputtypeparameter.h"
#include "filehandling/namefileread.h"

class NameFileParameter : public InputTypeParameter {
public:
	NameFileParameter(std::string& namefile, Settings& settings, std::string name, bool required, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		InputTypeParameter(settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		value(namefile) {}
	virtual std::string getValue() const override {
		return value;
	}
	virtual void validateAndSet(std::string newValue);
	static std::string getNameFile(Settings & settings, std::vector<std::string> files);
private:
	std::string& value;
};

