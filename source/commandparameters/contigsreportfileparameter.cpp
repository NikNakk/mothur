#include "contigsreportfileparameter.h"
#include "filehandling/file.h"

void ContigsReportFileParameter::validateAndSet(std::string newValue) {
	this->value = newValue;
}
