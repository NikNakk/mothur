#ifndef inputtypeparameter_h
#define inputtypeparameter_h

#include "commandparameterbase.h"

class InputTypeParameter : public CommandParameterBase {
public:
	InputTypeParameter(string name, bool required, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		CommandParameterBase(name, InputType, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual string getValue() {
		return value;
	}
	virtual void validateAndSet(string value);
	string setInputDir(string newInputDir) { inputDir = newInputDir; }
	string getFullPath();

protected:
	string value;
	string inputDir;
};
#endif
