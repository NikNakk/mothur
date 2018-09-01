#include "qfileparameter.h"
#include "filehandling/file.h"

void QFileParameter::validateAndSet(std::string newValue) {
	this->value = newValue;
}
