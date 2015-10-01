#pragma once
#include "g3log/g3log/logmessage.hpp"
#include "g3log/sinkhandle.hpp"
#include <fstream>
#include <string>
#include <mutex>
#include <atomic>

class LogMainLogFile {
public:
	LogMainLogFile(std::string& filename);
	~LogMainLogFile();
	void fileWrite(g3::LogMessageMover message);
	void stopAndClose();
private:
	std::ofstream out;
	std::string filename;
	bool finished = false;
	std::mutex m_;
};

class LogScreen {
public:
	LogScreen() {};
	void screenWrite(g3::LogMessageMover message);

	void notifyCinWaiting(g3::SinkHandle<LogScreen>& sh) {
		sh.notifyCinWaiting();
	}
};