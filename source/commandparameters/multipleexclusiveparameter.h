#pragma once

#include "commandparameterbase.h"
#include <vector>
#include "utility.h"

class MultipleExclusiveParameter : public CommandParameterBase {
public:
	explicit MultipleExclusiveParameter(std::string & value, std::string name, std::vector<std::string> options, std::string defaultOption, bool chooseMultiple = false, bool required = false, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		CommandParameterBase(name, CommandParameterType::Multiple, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		value(value), options(options), defaultOption(defaultOption), chooseMultiple(chooseMultiple) {}
	virtual std::string getValue() const override {
		return value;
	}
	virtual void validateAndSet(std::string newValue) override;
	virtual bool validateRequiredMissing();
private:
	std::string & value;
	std::vector<std::string> options;
	std::string defaultOption;
	bool chooseMultiple;
};

