#pragma once
#include "g3log/g3log/logmessage.hpp"
#include <fstream>
#include <string>

class LogMainLogFile {
public:
	LogMainLogFile(std::string& filename);
	~LogMainLogFile();
	void fileWrite(g3::LogMessageMover message);
private:
	std::ofstream out;
	std::string filename;
};

class LogScreen {
public:
	LogScreen() {};
	void screenWrite(g3::LogMessageMover message);
};