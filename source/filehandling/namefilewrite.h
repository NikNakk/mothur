#pragma once

#include "textfilewrite.h"
#include "datastructures/namemap.h"

class NameFileWrite : public TextFileWrite{
public:
	using TextFileWrite::TextFileWrite;
	bool writeNameFileLine(NameFileLine line);
};