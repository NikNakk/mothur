#include "groupfileparameter.h"
#include "filehandling/file.h"

void GroupFileParameter::validateAndSet(std::string newValue) {
	this->value = newValue;
}
