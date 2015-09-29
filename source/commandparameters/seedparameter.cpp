#include "seedparameter.h"
#include "mothur.h"

void SeedParameter::validateAndSet(string newValue)
{
	int random = 0;
	bool seed = false;
	if (newValue == "clear") {
		int random = time(NULL);
		seed = true;
	}
	else {
		NumberParameter::validateAndSet(newValue);
		seed = true;
		random = static_cast<unsigned int>(value);
	}

	if (seed) {
		srand(random);
		// TODO m->mothurOut("Setting random seed to " + toString(random) + ".\n\n");
	}
}
