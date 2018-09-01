#pragma once

#include "textfileread.h"
#include "sequence.h"

class FastaFileRead : public TextFileRead{
public:
	using TextFileRead::TextFileRead;
	Sequence readSequence();
	std::vector<FastaFileRead> split(int numSections);
	static bool checkValid(const std::string & fileName);
};