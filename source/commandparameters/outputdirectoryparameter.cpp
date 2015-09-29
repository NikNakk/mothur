#include "outputdirectoryparameter.h"
#include "filehandling/file.h"
#include "application.h"
#include "settings.h"

void OutputDirectoryParameter::validateAndSet(string newValue) {
	DirectoryParameter::validateAndSet(newValue);
	this->value = newValue;
	Application::getApplication()->getSettings().setOutputDir(newValue);
}