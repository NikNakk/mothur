#pragma once

#include "inputtypeparameter.h"
#include <memory>
#include "filehandling/fastafileread.h"
#include "mothurdefs.h"

class FastaFileParameter : public InputTypeParameter {
public:
	FastaFileParameter(std::string& fasta, Settings& settings, std::string name, bool required, bool important = false,
		std::string chooseOnlyOneGroup = "", std::string chooseAtLeastOneGroup = "", std::string linkedGroup = "") :
		InputTypeParameter(settings, name, required, important, chooseOnlyOneGroup, chooseAtLeastOneGroup, linkedGroup),
		value(fasta) {}
	virtual std::string getValue() const override {
		return value;
	}
	virtual void validateAndSet(std::string newValue) override;
	virtual bool validateRequiredMissing() override;
private:
	std::string & value;
};

