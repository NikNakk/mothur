#ifndef numberparameter_h
#define numberparameter_h

#include "commandparameterbase.h"

using namespace std;

class NumberParameter : public CommandParameterBase {
public:
	explicit NumberParameter(string name, double minValue = -INFINITY, double maxValue = INFINITY, bool required = false, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		CommandParameterBase(name, Number, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		minValue(minValue), maxValue(maxValue) {}
	virtual string getValue() {
		return std::to_string(value);
	}
	double getNumericValue() {
		return value;
	}
	virtual void validateAndSet(string newValue);
protected:
	double value = 0;
	double minValue = -INFINITY;
	double maxValue = INFINITY;
};
#endif
