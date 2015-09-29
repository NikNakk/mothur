#include "logsinks.h"
#include <sstream>
#include <iostream>

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
	if (msg.level() != "SCREENONLY") {
		out << msg.message() << std::endl;
	}
}


void LogScreen::screenWrite(g3::LogMessageMover message) {
	std::cout << message.get().message() << std::endl;
}
