#include "countfileparameter.h"
#include "filehandling/file.h"

void CountFileParameter::validateAndSet(std::string newValue) {
	this->value = newValue;
}
