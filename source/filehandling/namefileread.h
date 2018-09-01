#pragma once

#include "textfileread.h"
#include "namemap.h"

class NameFileRead : public TextFileRead{
public:
	using TextFileRead::TextFileRead;
	NameFileLine readNameFileLine();
	std::vector<std::string> readRawNameFileLine();
	NamesWithTotals readNamesWithTotals();
	NameMapUniqueToDups readNameMapUniqueToDups();

	long long getLineCount() { return lineCount; }
private:
	long long lineCount = 0;
};