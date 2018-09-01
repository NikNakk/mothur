#pragma once

#include "textfilewrite.h"
#include "datastructures/counttable.h"

class CountFileWrite : public TextFileWrite{
public:
	using TextFileWrite::TextFileWrite;
	bool writeCountTable(const CountTable & ct, std::vector<std::string> nameOrder);
};