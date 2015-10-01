#ifndef directoryparameter_h
#define directoryparameter_h

#include "inputtypeparameter.h"

class DirectoryParameter : public InputTypeParameter {
public:
	DirectoryParameter(Settings& settings, string name, bool required, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		InputTypeParameter(settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual string getValue() {
		return value;
	}
	virtual void validateAndSet(string value);
private:
	string value;
};
#endif
