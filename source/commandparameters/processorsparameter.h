#pragma once
#include "numberparameter.h"
#include "settings.h"

using namespace std;

class ProcessorsParameter : public IntegerParameter {
public:
	explicit ProcessorsParameter(Settings& settings, bool required = false, bool important = true) :
		IntegerParameter("processors", 0, LONG_MAX, LONG_YMIN, required, important),
		settings(settings)
	{}
	virtual void validateAndSet(string newValue);
private:
	Settings& settings;
};
