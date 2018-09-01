#pragma once

#include "commandparameterbase.h"
#include <unordered_set>
#include "utility.h"

class MultipleParameter : public CommandParameterBase {
public:
	explicit MultipleParameter(std::vector<std::string> & values, std::string name, std::vector<std::string> options, std::string defaultOption, bool chooseMultiple = false, bool required = false, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		CommandParameterBase(name, CommandParameterType::Multiple, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		values(values), options(options), defaultOption(defaultOption), chooseMultiple(chooseMultiple) {}
	virtual std::string getValue() const override {
		return Utility::join(values.begin(), values.end(), std::string("-"));
	}
	virtual void validateAndSet(std::string newValue) override;
	virtual bool validateRequiredMissing();
private:
	std::vector<std::string> & values;
	std::vector<std::string> options;
	std::string defaultOption;
	bool chooseMultiple;
};

