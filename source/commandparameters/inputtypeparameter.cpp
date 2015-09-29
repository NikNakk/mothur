#include "inputtypeparameter.h"
#include "filehandling/file.h"

void InputTypeParameter::validateAndSet(string newValue) {
	this->value = value;
}

string InputTypeParameter::getFullPath()
{
	string filePath = File::getPath(value);
	if (filePath == "") {
		return inputDir + value;
	}
	else {
		return value;
	}
}
