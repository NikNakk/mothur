#pragma once

#include "textfilewrite.h"
#include "datastructures/sequence.h"

class SummaryFileWrite : public TextFileWrite {
public:
	using TextFileWrite::TextFileWrite;
	void writeHeader();
	void writeSequenceSummary(const Sequence& seq, const SequenceSummary& seqSum);
};