#pragma once

#include "file.h"
#include <memory>
#include <vector>

class TextFileRead : public File {
public:
	TextFileRead() = default;
	TextFileRead(const std::string & fileName, bool skipHeader = false);
	TextFileRead(const std::string & fileName, std::streampos start, std::streampos end, bool skipHeader = false);
	// Note the copy constructor will copy the start position, but the file will be opened at start with a fresh filestream
	TextFileRead(const TextFileRead& obj) : TextFileRead(obj.fileName, obj.start, obj.end, obj.skipHeader) {}
	TextFileRead& operator=(TextFileRead obj) { this->swap(obj); return *this; }
	TextFileRead(TextFileRead&& obj) = delete;

	void swap(TextFileRead& obj);

	virtual std::string readLine();
	virtual std::vector<std::string> readColumns(const char delimiter = '\t');
	virtual bool good() const { return fileStream && fileStream->good(); }
	virtual bool eof() const { return !fileStream || fileStream->eof(); }
	virtual bool reachedEnd();
	virtual long long getStart() const { return start; }
	virtual long long getEnd() const { return end; }

	template<typename TextFileType>
	static std::vector<TextFileType> splitType(int numSections, char delimiter = '\n', bool keepDelimiter = false);
protected:
	std::unique_ptr<std::istream> fileStream;
	std::streampos start = 0;
	std::streampos end = 0;
	bool skipHeader = false;
};

template<typename TextFileType>
std::vector<TextFileType> TextFileRead::splitType(int numSections, char delimiter, bool keepDelimiter) {
	std::vector<TextFileType> textSplit;
	if (!fileStream) {
		return textSplit;
	}
	fileStream->seekg(0, fileStream->end);
	long long fileSize = fileStream->tellg();
	long long chunkSize = fileSize / numSections;
	if (chunkSize == 0 || numSections == 1) {
		textSplit.emplace_back(this->fileName);
		return textSplit;
	}
	long long start = 0;
	for (int i = 1; i < numSections && !(fileStream->eof()); i = static_cast<int>(start / chunkSize) + 1) {
		fileStream->seekg(chunkSize * i);
		char c = fileStream->get();
		while (!fileStream->eof()) {
			if (c == delimiter) {
				if (keepDelimiter) {
					fileStream->putback(c);
				}
				textSplit.emplace_back(this->fileName, start, fileStream->tellg());
				start = fileStream->tellg();
				break;
			}
			c = fileStream->get();
		}
	}
	textSplit.emplace_back(this->fileName, start, fileSize);
	return textSplit;
}
