#pragma once

#include "textfileread.h"
#include "counttable.h"

class CountFileRead : public TextFileRead{
public:
	using TextFileRead::TextFileRead;
	CountTable readCountTable();
	NamesWithTotals getNamesWithTotals();
};