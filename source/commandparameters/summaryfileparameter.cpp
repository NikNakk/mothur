#include "summaryfileparameter.h"
#include "filehandling/file.h"

void SummaryFileParameter::validateAndSet(std::string newValue) {
	this->value = newValue;
}
