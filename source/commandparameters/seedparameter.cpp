#include "seedparameter.h"
#include "mothurdefs.h"
#include "utility.h"
#include <stdexcept>

void SeedParameter::validateAndSet(std::string newValue)
{
	unsigned int random = 0;
	bool seed = false;
	if (newValue == "clear") {
		random = static_cast<unsigned int>(time(NULL));
		seed = true;
	}
	else {
		if (Utility::isInteger(newValue)) {
			random = static_cast<unsigned int>(std::stoul(newValue));
			seed = true;
		}
		else {
			throw(std::out_of_range("Seed must be an integer or 'clear'"));
		}
	}

	if (seed) {
		srand(random);
		LOG(INFO) << "Setting random seed to " << toString(random) << '.';
	}
}
