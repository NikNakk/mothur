#include "summaryfileread.h"
#include <sstream>
#include "utility.h"

NamedSequenceSummary SummaryFileRead::readSequenceSummary()
{
	NamedSequenceSummary nss;
	if (fileStream) {
		*fileStream >> nss.name >> nss.startPosition >> nss.endPosition >> nss.seqLength >> nss.ambigBases >> nss.longHomoPolymer >> nss.numSeqs;
	}
	else {
		nss.valid = false;
	}
	return nss;
}

bool SummaryFileRead::checkValid(const std::string & fileName)
{
	if (!File::FileExists(fileName)) {
		return false;
	}
	else {
		return true;
	}
}

std::vector<SummaryFileRead> SummaryFileRead::split(int numSections)
{
	return TextFileRead::splitType<SummaryFileRead>(numSections, '\n', false);
}
