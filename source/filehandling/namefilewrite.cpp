#include "namefilewrite.h"
#include "utility.h"

bool NameFileWrite::writeNameFileLine(NameFileLine line)
{
	if (fileStream->good()) {
		writeLine(line.first + "\t" + Utility::join(line.second, ","));
	}
	return fileStream->good();
}