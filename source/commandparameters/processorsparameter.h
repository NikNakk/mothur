#pragma once
#include "integerparameter.h"
#include "settings.h"

class ProcessorsParameter : public IntegerParameter {
public:
	explicit ProcessorsParameter(int& value, Settings& settings, bool required = false, bool important = true) :
		IntegerParameter(value, "processors", 0, LONG_MAX, 1, required, important),
		settings(settings)
	{}
	virtual void validateAndSet(std::string newValue);
	virtual bool validateRequiredMissing() override;
private:
	Settings& settings;
};
