#include "multipleparameter.h"

void MultipleParameter::validateAndSet(std::string newValue)
{
	values.clear();
	values.push_back(newValue);
}

bool MultipleParameter::validateRequiredMissing()
{
	if (isRequired()) {
		return false;
	}
	else {
		validateAndSet(defaultOption);
		return true;
	}
}
