#include "fastafileparameter.h"

void FastaFileParameter::validateAndSet(std::string newValue) {
	// TODO: Validation, handling of multiple files
	if (newValue == "current" || newValue == "") {
		newValue = settings.getCurrent("fasta");
		if (newValue != "") {
			LOG(INFO) << "Using current fasta file " << newValue << ".";
		}
		else if (isRequired()){
			throw(InvalidFile("Current fasta file requested but none available"));
		}
		else {
			return;
		}
	}
	if (File::FileExists(newValue)) {
		if (FastaFileRead::checkValid(newValue)) {
			this->value = newValue;
			settings.setCurrent("fasta", newValue);
		}
		else {
			throw(InvalidFile("Invalid fasta file '" + newValue + "'."));
		}
	}
	else {
		throw(InvalidFile("Could not find fasta file '" + newValue + "'."));
	}
}

bool FastaFileParameter::validateRequiredMissing()
{
	if (!isRequired()) {
		return true;
	}
	if (value == "") {
		std::string fileName = settings.getCurrent("fasta");
		if (fileName == "") {
			value = "";
			return false;
		}
		validateAndSet(fileName);
	}
	return true;
}
