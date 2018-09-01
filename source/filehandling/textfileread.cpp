#include "textfileread.h"
#include "utility.h"

TextFileRead::TextFileRead(const std::string & fileName, bool skipHeader) :
	File(fileName), skipHeader(skipHeader)
{
	fileStream = std::make_unique<std::ifstream>(fileName);
	if (skipHeader && fileStream->good()) {
		this->readLine();
	}
	if (!(fileStream->good())) {
		fileStream.reset();
	}
}


TextFileRead::TextFileRead(const std::string & fileName, std::streampos start, std::streampos end, bool skipHeader) :
	File(fileName), start(start), end(end), skipHeader(skipHeader)
{
	fileStream = std::make_unique<std::ifstream>(fileName);
	if (start > 0 && fileStream->good()) {
		fileStream->seekg(start);
	}
	else {
		if (skipHeader && fileStream->good()) {
			this->readLine();
		}
	}
	if (!(fileStream->good())) {
		fileStream.reset();
	}
}

std::string TextFileRead::readLine()
{
	std::string line;
	if (this->good()) {
		std::getline(*fileStream, line);
	}
	return line;
}

std::vector<std::string> TextFileRead::readColumns(const char delimiter)
{
	std::string line = readLine();
	return Utility::split(line, delimiter);
}

bool TextFileRead::reachedEnd() {
	return !fileStream || fileStream->eof() || (fileStream->tellg() >= end && end > 0);
}

std::vector<TextFileRead> TextFileRead::split(int numSections, char delimiter, bool keepDelimiter)
{
}

void TextFileRead::swap(TextFileRead & obj)
{
	using std::swap;
	swap(this->fileName, obj.fileName);
	swap(this->start, obj.start);
	swap(this->end, obj.end);
	swap(this->skipHeader, obj.skipHeader);
}
