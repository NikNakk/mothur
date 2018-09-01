#pragma once

#include <string>
#include <map>
#include "filehandling/file.h"
#include "mothurdefs.h"

class Settings {
public:
	Settings() {};
	~Settings() {};

	std::string getDefaultPath() { return defaultPath; }
	void setDefaultPath(std::string defaultPath) { this->defaultPath = defaultPath; }
	std::string getOutputDir() { return outputDir; }
	void setOutputDir(std::string) { this->outputDir = outputDir; }
	std::string getInputDir() { return inputDir; }
	void setInputDir(std::string inputDir) { this->inputDir = inputDir; }
	std::string getLogFileName() { return logFileName; }
	void setLogFileName(std::string logFileName, bool append) { this->logFileName = logFileName; this->append = append; }
	bool getAppend() { return append; }

	std::string getReleaseDate() { return releaseDate; }
	void setReleaseDate(std::string r) { releaseDate = r; }
	std::string getVersion() { return version; }
	void setVersion(std::string r) { version = r; }
	std::string getProgramPath(std::string path) { return programPath; }
	void setProgramPath(std::string path) { programPath = path;  }


	std::string getCurrent(std::string type) { return currentfiles[type]; }
	void setCurrent(std::string type, std::string value);
	std::string getAllCurrent();
	bool hasCurrentFiles() { return currentfiles.size() > 0 || processors > 1; }
	void clearCurrentFiles() { currentfiles.clear(); }
	int getProcessors() { return processors; }
	bool getMothurCalling() { return mothurCalling; }
	bool getModifyNames() { return modifyNames; }

	void setProcessors(int p) { processors = p; LOG(INFO) << "Using " << p << " processors."; }
	void setDebug(bool debug) { this->debug = debug; }
	void setModifyNames(bool modifyNames) { this->modifyNames = modifyNames; }

	bool getQuietMode() { return quietMode; }
	void setQuietMode(bool quietMode) { this->quietMode = quietMode; }

	bool getGui() { return gui; }
	void setGui(bool gui) { this->gui = gui; }
private:
	std::string logFileName;
	std::string programPath;
	std::string defaultPath, outputDir, inputDir;
	std::string releaseDate, version;
	std::string groupMode = "group";

	int processors = 1;

	bool gui = false, printedSharedHeaders = false, printedListHeaders = false;
	bool mothurCalling = false, debug = false, quietMode = false, changedSeqNames = false, modifyNames = true, append = false;

	std::string saveNextLabel, argv, sharedHeaderMode;

	std::map<std::string, std::string> currentfiles;

};

