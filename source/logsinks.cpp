#include "logsinks.h"
#include <sstream>
#include <iostream>
#include <memory>
#include "g3log/sinkhandle.hpp"
#include "g3log/g3log.hpp"

LogMainLogFile::LogMainLogFile(std::string& filename) : filename(filename)
{
	out.open(filename);
	if (!out.is_open()) {
		std::ostringstream ss_error;
		ss_error << "FILE ERROR:  could not open log file:[" << filename << "]";
		std::cerr << ss_error.str() << std::endl;
		out.close();
	}
}

LogMainLogFile::~LogMainLogFile() {
	out << std::flush;
	out.close();
}


void LogMainLogFile::fileWrite(g3::LogMessageMover message)
{
	g3::LogMessage msg = message.get();
	if (msg.level() != "SCREENONLY" && !finished) {
		std::unique_lock<std::mutex> lock(m_);
		out << msg.message() << std::endl;
	}
}

void LogScreen::screenWrite(g3::LogMessageMover message) {
	g3::LogMessage msg = message.get();
	if (msg.level() != "FILEONLY") {
		std::cout << msg.message() << std::endl;
	}
}

void LogMainLogFile::stopAndClose() {
	std::unique_lock<std::mutex> lock(m_);
	finished = true;
	out.close();
}