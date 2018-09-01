#include "taxonomyfileparameter.h"
#include "filehandling/file.h"

void TaxonomyFileParameter::validateAndSet(std::string newValue) {
	this->value = newValue;
}
