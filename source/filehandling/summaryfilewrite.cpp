#include "summaryfilewrite.h"

void SummaryFileWrite::writeHeader()
{
	if (this->good()) {
		*fileStream << "seqname\tstart\tend\tnbases\tambigs\tpolymer\tnumSeqs\n";
	}
}

void SummaryFileWrite::writeSequenceSummary(const Sequence & seq, const SequenceSummary & seqSum)
{
	if (this->good()) {
		*fileStream << seq.getName() <<seqSum.startPosition << '\t' << seqSum.endPosition << '\t' << seqSum.seqLength << '\t' <<
			seqSum.ambigBases << '\t' << seqSum.longHomoPolymer << seqSum.numSeqs << '\n';
	}
}