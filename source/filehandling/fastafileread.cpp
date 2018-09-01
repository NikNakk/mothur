#include "fastafileread.h"
#include <sstream>
#include "utility.h"

Sequence FastaFileRead::readSequence()
{
	std::string seqName = this->readLine();
	while (seqName[0] == '#') { seqName = this->readLine(); }
	if (seqName == "") {
		return Sequence();
	}
	else {
		if (seqName.length() > 0) {
			if (seqName[0] != '>') {
				throw(InvalidSequence("Error in reading your fastafile, at position " + std::to_string(fileStream->tellg()) + ". Malformed name."));
			}
			seqName = seqName.substr(1);
		}
		else {
			throw(InvalidSequence("Error in reading your fastafile, at position " + std::to_string(fileStream->tellg()) + ". Blank name."));
		}
		seqName = Utility::trim(seqName);
		std::ostringstream seqLines;
		int numAmbigs = 0;
		while (!(fileStream->eof())) {
			char c = fileStream->get();
			if (c == '>') {
				fileStream->putback(c);
				break;
			}
			else if (c == ' ') {}
			else if (isprint(c)) {
				c = toupper(c);
				if (c == 'U') { c = 'T'; }
				if (c != '.' && c != '-' && c != 'A' && c != 'T' && c != 'G'  && c != 'C' && c != 'N') {
					++numAmbigs;
					c = 'N';
				}
				seqLines << c;
			}
		}
		return Sequence(seqName, seqLines.str(), numAmbigs);
	}
}

std::vector<FastaFileRead> FastaFileRead::split(int numSections) {
	return TextFileRead::splitType<FastaFileRead>(numSections, '>', true);
}

bool FastaFileRead::checkValid(const std::string & fileName)
{
	if (!File::FileExists(fileName)) {
		return false;
	}
	FastaFileRead fil(fileName);
	Sequence seq = fil.readSequence();
	return seq.getName() != "";
}