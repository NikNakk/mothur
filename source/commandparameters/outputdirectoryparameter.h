#ifndef outputdirectoryparameter_h
#define outputdirectoryparameter_h

#include "directoryparameter.h"

class OutputDirectoryParameter : public DirectoryParameter {
public:
	explicit OutputDirectoryParameter(Settings& settings, string name = "outputdir", bool required = false, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		DirectoryParameter(settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual void validateAndSet(string value);Y
private:
	string value;
};
#endif
