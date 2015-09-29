#ifndef booleanparameter_h
#define booleanparameter_h

#include "commandparameterbase.h"

using namespace std;

class BooleanParameter : public CommandParameterBase {
public:
	explicit BooleanParameter(string name, bool defaultValue = false, bool required = false, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		CommandParameterBase(name, Boolean, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		defaultValue(defaultValue)
	{}
	virtual string getValue() {
		return std::to_string(value);
	}
	virtual bool hasValue() { return valueSet; }
	bool getBooleanValue() {
		return value;
	}
	virtual void validateAndSet(string newValue);
protected:
	bool value = false;
	bool defaultValue = false;
	bool valueSet = false;
};
#endif
