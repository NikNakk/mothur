#pragma once

#include "file.h"

class TextFile{
public:
	TextFile() = default;
	virtual ~TextFile() = default;
	TextFile(std::string fileName) : fileName(fileName) {}
	virtual std::string getFileName() { return fileName; }
	virtual std::string getSimpleRootName() { return File::getRootName(File::getSimpleName(fileName)); }
	virtual void setFileName(std::string fileName) { this->fileName = fileName; }
protected:
	std::string fileName;
};

class InvalidFile : public std::logic_error {
public:
	InvalidFile(std::string message) : std::logic_error(message) {}
};