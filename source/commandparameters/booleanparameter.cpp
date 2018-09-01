#include "booleanparameter.h"
#include <stdexcept>
#include <string>
#include <algorithm>

void BooleanParameter::validateAndSet(std::string newValue) {
	transform(newValue.begin(), newValue.end(), newValue.begin(), ::toupper);
	value = (newValue == "TRUE" || newValue == "T");
	valueSet = true;
}