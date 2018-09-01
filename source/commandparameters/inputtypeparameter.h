#pragma once

#include "commandparameterbase.h"
#include "settings.h"

class InputTypeParameter : public CommandParameterBase {
public:
	InputTypeParameter(Settings& settings, std::string name, bool required, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		CommandParameterBase(name, CommandParameterType::InputType, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		settings(settings) {}
	virtual std::string getValue() const override { return value; }
	virtual void validateAndSet(std::string value) override;
	std::string getFullPath();

protected:
	Settings& settings;
	std::string value;
};

