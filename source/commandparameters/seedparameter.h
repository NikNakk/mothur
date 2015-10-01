#ifndef seedparameter_h
#define seedparameter_h

#include "numberparameter.h"

using namespace std;

class SeedParameter : public NumberParameter {
public:
	explicit SeedParameter(bool required = false, bool important = false) :
		NumberParameter("seed", 0, ULONG_MAX, NAN, required, important)
	{}
	virtual void validateAndSet(string newValue);
};
#endif
