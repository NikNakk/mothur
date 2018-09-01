#pragma once

#include "commandparameterbase.h"

class SeedParameter : public CommandParameterBase {
public:
	explicit SeedParameter(bool required = false, bool important = false) :
		CommandParameterBase("seed", CommandParameterType::Number, required, important, "", "", "")
	{}
	virtual std::string getValue() const override { return ""; }
	virtual void validateAndSet(std::string newValue) override;
};

