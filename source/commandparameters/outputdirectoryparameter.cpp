#include "outputdirectoryparameter.h"
#include "filehandling/file.h"
#include "settings.h"

void OutputDirectoryParameter::validateAndSet(std::string newValue) {
	DirectoryParameter::validateAndSet(newValue);
	this->value = newValue;
	settings.setOutputDir(newValue);
}