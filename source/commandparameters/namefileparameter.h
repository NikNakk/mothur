#ifndef namefileparameter_h
#define namefileparameter_h

#include "inputtypeparameter.h"

class NameFileParameter : public InputTypeParameter {
public:
	NameFileParameter(string name, bool required, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		InputTypeParameter(name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual string getValue() {
		return value;
	}
	virtual void validateAndSet(string newValue);
	static bool getNameFile(vector<string> files, string output);
private:
	string value;
};
#endif
