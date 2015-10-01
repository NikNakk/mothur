#include "processorsparameter.h"
#include "mothur.h"

void ProcessorsParameter::validateAndSet(string newValue)
{
	IntegerParameter::validateAndSet(newValue);
	settings.setProcessors(value);
}
