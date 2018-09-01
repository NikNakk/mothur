#include "alignreportfileparameter.h"
#include "filehandling/file.h"

void AlignReportFileParameter::validateAndSet(std::string newValue) {
	this->value = newValue;
}
