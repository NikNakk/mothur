#ifndef inputtypeparameter_h
#define inputtypeparameter_h

#include "commandparameterbase.h"
#include "settings.h"

class InputTypeParameter : public CommandParameterBase {
public:
	InputTypeParameter(Settings& settings, string name, bool required, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		CommandParameterBase(name, InputType, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		settings(settings) {}
	virtual string getValue() {
		return value;
	}
	virtual void validateAndSet(string value);
	string setInputDir(string newInputDir) { inputDir = newInputDir; }
	string getFullPath();

protected:
	Settings& settings;
	string value;
	string inputDir;
};
#endif
