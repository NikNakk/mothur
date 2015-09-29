#ifndef outputdirectoryparameter_h
#define outputdirectoryparameter_h

#include "directoryparameter.h"

class OutputDirectoryParameter : public DirectoryParameter {
public:
	explicit OutputDirectoryParameter(string name = "outputdir", bool required = false, bool important = false,
		string chooseOnlyOneGroup = "", string chooseAtLeastOneGroup = "", string linkedGroup = "") :
		DirectoryParameter(name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual void validateAndSet(string value);
private:
	string value;
};
#endif
