#ifndef fastafileparameter_h
#define fastafileparameter_h

#include "inputtypeparameter.h"

class FastaFileParameter : public InputTypeParameter {
public:
	FastaFileParameter(string name, bool required, bool important = false,
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
