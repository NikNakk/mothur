#ifndef intputdirectoryparameter_h
#define inputdirectoryparameter_h

#include "directoryparameter.h"

class InputDirectoryParameter : public DirectoryParameter {
public:
	InputDirectoryParameter(std::string & value, Settings& settings, std::string name = "inputdir", bool required = false, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		DirectoryParameter(value, settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup) {}
	virtual void validateAndSet(std::string value) override;
private:
	std::string value;
};
#endif
