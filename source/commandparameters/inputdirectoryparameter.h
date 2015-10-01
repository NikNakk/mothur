#ifndef intputdirectoryparameter_h
#define inputdirectoryparameter_h

#include "directoryparameter.h"

class InputDirectoryParameter : public DirectoryParameter {
public:
	explicit InputDirectoryParameter(Settings& settings, string name = "inputdir", bool required = false, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		DirectoryParameter(settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual void validateAndSet(string value);
private:
	string value;
};
#endif
