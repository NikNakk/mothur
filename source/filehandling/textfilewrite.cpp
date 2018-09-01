#include "textfilewrite.h"

TextFileWrite::~TextFileWrite()
{
	if (!committed) {
		this->close();
		File::remove(this->getFileName());
	}
}

TextFileWrite::TextFileWrite(const std::string & fileName)
{
	this->fileName = fileName;
	fileStream = std::make_unique<std::ofstream>(fileName);
	if (!fileStream->good()) {
		fileStream.reset();
	}
}

bool TextFileWrite::writeLine(const std::string & line)
{
	if (fileStream) {
		*fileStream << line << '\n';
		return fileStream->good();
	}
	else {
		return false;
	}
}

bool TextFileWrite::write(const std::string & line) {
	if (fileStream) {
		*fileStream << line;
		return fileStream->good();
	}
	else {
		return false;
	}
}

void TextFileWrite::close()
{
	if (fileStream) {
		fileStream.reset();
	}
}

void TextFileWrite::commit()
{
	this->close();
	committed = true;
}