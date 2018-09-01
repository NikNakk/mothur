#pragma once

#include "textfileread.h"
#include "datastructures/sequence.h"

class SummaryFileRead : public TextFileRead {
public:
	SummaryFileRead() : TextFileRead() {}
	SummaryFileRead(const std::string & fileName, bool skipHeader = true) : TextFileRead(fileName, skipHeader) {}
	SummaryFileRead(const std::string & fileName, std::streampos start, std::streampos end, bool skipHeader = true) : TextFileRead(fileName, start, end, skipHeader) {}
	// Note the copy constructor will copy the start position, but the file will be opened at start with a fresh filestream
	SummaryFileRead(const SummaryFileRead& obj) : SummaryFileRead(obj.fileName, obj.start, obj.end, obj.skipHeader) {}
	SummaryFileRead& operator=(SummaryFileRead obj);
	SummaryFileRead(SummaryFileRead&& obj) = delete;

	NamedSequenceSummary readSequenceSummary();
	static bool checkValid(const std::string & fileName);
	long long getLineCount() const { return lineCount; }

	std::vector<SummaryFileRead> split(int numSections);
private:
	long long lineCount = 0;
};