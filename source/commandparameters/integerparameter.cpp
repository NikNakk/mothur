#include "integerparameter.h"
#include <stdexcept>

void IntegerParameter::validateAndSet(std::string newValue) {
	int iNewValue = stoi(newValue);
	if (iNewValue < minValue || iNewValue > maxValue) {
		throw(std::out_of_range("Parameter out of range"));
	}
	value = iNewValue;
}