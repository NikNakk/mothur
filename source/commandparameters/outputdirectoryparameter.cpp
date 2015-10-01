#include "outputdirectoryparameter.h"
#include "filehandling/file.h"
#include "settings.h"

void OutputDirectoryParameter::validateAndSet(string newValue) {
	DirectoryParameter::validateAndSet(newValue);
	this->value = newValue;
	settings.setOutputDir(newValue);
}