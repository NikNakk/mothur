#include "multipleexclusiveparameter.h"

void MultipleExclusiveParameter::validateAndSet(std::string newValue)
{
	this->value = newValue;
}

bool MultipleExclusiveParameter::validateRequiredMissing()
{
	if (isRequired()) {
		return false;
	}
	else {
		validateAndSet(defaultOption);
		return true;
	}
}
