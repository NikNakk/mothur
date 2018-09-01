#include "inputdirectoryparameter.h"
#include "filehandling/file.h"
#include "settings.h"

void InputDirectoryParameter::validateAndSet(std::string newValue) {
	DirectoryParameter::validateAndSet(newValue);
	this->value = newValue;
	settings.setInputDir(newValue);
}