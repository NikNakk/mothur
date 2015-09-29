#ifndef countfileparameter_h
#define countfileparameter_h

#include "inputtypeparameter.h"

class CountFileParameter : public InputTypeParameter {
public:
	CountFileParameter(string name, bool required, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		InputTypeParameter(name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual string getValue() {
		return value;
	}
	virtual void validateAndSet(string newValue);
private:
	string value;
};
#endif
