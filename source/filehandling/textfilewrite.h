#pragma once

#include "file.h"
#include <memory>

class TextFileWrite : public File {
public:
	TextFileWrite() = default;
	TextFileWrite(const TextFileWrite&) = default;
	TextFileWrite& operator=(const TextFileWrite&) = default;
	virtual ~TextFileWrite();
	TextFileWrite(const std::string & fileName);
	virtual bool writeLine(const std::string & line);
	virtual bool write(const std::string & line);
	virtual bool good() { return fileStream->good(); }
	virtual bool eof() { return fileStream->eof(); }
	virtual bool isValid() { return static_cast<bool>(fileStream); }
	virtual void close();
	virtual void commit();

	template <typename T, typename U>
	bool writeTwoColumns(T first, U second, std::string delimiter = std::string('\t')) {
		if (fileStream && fileStream->good()) {
			*fileStream << first << delimiter << second << '\n';
		}
		return fileStream->good();
	}
protected:
	std::unique_ptr<std::ostream> fileStream;
	bool committed = false;
};