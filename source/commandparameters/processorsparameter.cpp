#include "processorsparameter.h"
#include "mothur.h"

void ProcessorsParameter::validateAndSet(std::string newValue)
{
	IntegerParameter::validateAndSet(newValue);
	settings.setProcessors(value);
}

bool ProcessorsParameter::validateRequiredMissing()
{
	validateAndSet(std::to_string(settings.getProcessors()));
	return true;
}