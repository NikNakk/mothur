#ifndef multipleparameter_h
#define multipleparameter_h

#include "commandparameterbase.h"

class MultipleParameter : public CommandParameterBase {
public:
	explicit MultipleParameter(string name, vector<string> options, string defaultOption, bool chooseMultiple = false, bool required = false, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		CommandParameterBase(name, InputType, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		options(options), defaultOption(defaultOption), chooseMultiple(chooseMultiple) {}
	virtual string getValue() {
		return "";
	}
	virtual void validateAndSet(string newValue);
private:
	vector<string> values;
	vector<string> options;
	string defaultOption;
	bool chooseMultiple;
};
#endif
