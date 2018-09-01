#include "sequence.h"
#include <iterator>
#include <vector>
#include <algorithm>


SequenceSummary Sequence::getSequenceSummary() const
{
	SequenceSummary seqSum;
	int i = 0;
	for (auto it = sequence.begin(); it != sequence.end() && (*it =='-' || *it == '.'); ++it, ++i) {}
	seqSum.startPosition = i + 1;
	i = static_cast<int>(sequence.size() - 1);
	for (auto it = sequence.rbegin(); it != sequence.rend() && (*it == '-' || *it == '.'); ++it, --i) {}
	seqSum.endPosition = i + 1;
	int curLongHomoPoly = 1;
	char prevChar = '-';
	if (numAmbigs == -1) {
		numAmbigs = std::count_if(sequence.begin(), sequence.end(), [](char c) {return c != 'A' && c != 'C' && c != 'T' && c != 'G' && c != 'U' && c != '-' && c != '.';});
	}
	seqSum.ambigBases = numAmbigs;
	for (i = seqSum.startPosition - 1; i < seqSum.endPosition; ++i) {
		switch (char c = sequence[i]) {
		case 'N':
			++seqSum.numNs;
		case 'A':
		case 'C':
		case 'T':
		case 'G':
			if (c == prevChar) {
				++curLongHomoPoly;
			}
			else {
				if (curLongHomoPoly > seqSum.longHomoPolymer) {
					seqSum.longHomoPolymer = curLongHomoPoly;
				}
				curLongHomoPoly = 1;
				prevChar = c;
			}
			++seqSum.seqLength;
			break;
		}
	}
	return seqSum;
}