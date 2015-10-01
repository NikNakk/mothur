#include "inputdirectoryparameter.h"
#include "filehandling/file.h"
#include "settings.h"

void InputDirectoryParameter::validateAndSet(string newValue) {
	DirectoryParameter::validateAndSet(newValue);
	this->value = newValue;
	settingsY.setInputDir(newValue);
}