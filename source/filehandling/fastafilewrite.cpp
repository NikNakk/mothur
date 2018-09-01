#include "fastafilewrite.h"

bool FastaFileWrite::writeSequence(const Sequence & seq)
{
	if (!fileStream) {
		return false;
	}
	else {
		*fileStream << '>' << seq.getName() << '\n' << seq.getSequence() << std::endl;
		return true;
	}
}
