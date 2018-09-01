#include "inputtypeparameter.h"
#include "filehandling/file.h"

void InputTypeParameter::validateAndSet(std::string newValue) {
	this->value = value;
}

std::string InputTypeParameter::getFullPath()
{
	std::string filePath = File::getPath(value);
	if (filePath == "") {
		return settings.getInputDir() + value;
	}
	else {
		return value;
	}
}
