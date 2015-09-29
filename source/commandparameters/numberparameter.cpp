#include "numberparameter.h"
#include "filehandling/file.h"
#include <stdexcept>

void NumberParameter::validateAndSet(string newValue) {
	double dNewValue = stod(newValue);
	if (dNewValue < minValue || dNewValue > maxValue) {
		throw(out_of_range("Parameter out of range"));
	}
	value = dNewValue;
}