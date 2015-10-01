#include "integerparameter.h"
#include <stdexcept>

void IntegerParameter::validateAndSet(string newValue) {
	int iNewValue = stoi(newValue);
	if (iNewValue < minValue || iNewValue > maxValue) {
		throw(out_of_range("Parameter out of range"));
	}
	value = iNewValue;
}