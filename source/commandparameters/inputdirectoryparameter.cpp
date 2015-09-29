#include "inputdirectoryparameter.h"
#include "filehandling/file.h"
#include "application.h"
#include "settings.h"

void InputDirectoryParameter::validateAndSet(string newValue) {
	DirectoryParameter::validateAndSet(newValue);
	this->value = newValue;
	Application::getApplication()->getSettings().setInputDir(newValue);
}