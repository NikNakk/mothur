#pragma once

#include "textfilewrite.h"
#include "sequence.h"

class FastaFileWrite : public TextFileWrite{
public:
	using TextFileWrite::TextFileWrite;
	bool writeSequence(const Sequence & seq);
};