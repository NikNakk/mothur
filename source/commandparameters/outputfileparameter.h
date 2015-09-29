#ifndef outputfileparameter_h
#define outputfileparameter_h

#include "commandparameterbase.h"

class OutputFileParameter : public CommandParameterBase {
public:
	OutputFileParameter(string name, bool required, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		CommandParameterBase(name, OutputFile, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual string getValue() { return value;  }
	virtual void validateAndSet(string value);
protected:
	string value;
};

#endif
