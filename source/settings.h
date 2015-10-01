#ifndef settings_h
#define settings_h

#include <string>
#include <map>
#include "filehandling/file.h"

using namespace std;

class Settings {
public:
	Settings() {};
	~Settings() {};

	string getDefaultPath() { return defaultPath; }
	void setDefaultPath(string defaultPath) { this->defaultPath = defaultPath; }
	string getOutputDir() { return outputDir; }
	void setOutputDir(string) { this->outputDir = outputDir; }
	string getInputDir() { return inputDir; }
	void setInputDir(string inputDir) { this->inputDir = inputDir; }
	string getLogFileName() { return logFileName; }
	void setLogFileName(string logFileName, bool append) { this->logFileName = logFileName; this->append = append; }
	bool getAppend() { return append; }

	string getReleaseDate() { return releaseDate; }
	void setReleaseDate(string r) { releaseDate = r; }
	string getVersion() { return version; }
	void setVersion(string r) { version = r; }

	string getCurrent(string type) { return currentfiles[type]; }
	void setCurrent(string type, string value);
	string getAllCurrent();
	bool hasCurrentFiles() { return currentfiles.size() > 0 || processors > 1; }
	void clearCurrentFiles() { currentfiles.clear(); }
	int getProcessors() { return processors; }
	bool getMothurCalling() { return mothurCalling; }
	bool getModifyNames() { return modifyNames; }

	void setProcessors(int p) { processors = p; LOG(INFO) << "Using " << p << " processors."; }
	void setDebug(bool debug) { this->debug = debug; }
	void setModifyNames(bool modifyNames) { this->modifyNames = modifyNames; }

	void addGroup(string g) { Groups.push_back(g); }
	void setGroups(vector<string>& g) { sort(g.begin(), g.end()); Groups = g; }
	void clearGroups() { Groups.clear(); }
	int getNumGroups() { return static_cast<int>(Groups.size()); }
	vector<string> getGroups() { sort(Groups.begin(), Groups.end()); return Groups; }
	void addAllGroup(string g) { namesOfGroups.push_back(g); }
	void setAllGroups(vector<string>& g) { sort(g.begin(), g.end()); namesOfGroups = g; }
	void clearAllGroups() { namesOfGroups.clear(); }
	int getNumAllGroups() { return static_cast<int>(namesOfGroups.size()); }

	vector<string> getAllGroups() { sort(namesOfGroups.begin(), namesOfGroups.end()); return namesOfGroups; }
	vector<string> Treenames;
	vector<string> sharedBinLabelsInFile;
	vector<string> currentSharedBinLabels;
	vector<string> listBinLabelsInFile;


private:
	string logFileName;
	string defaultPath, outputDir, inputDir;
	string releaseDate, version;
	string groupMode = "group";

	int processors = 1;

	bool gui = false, printedSharedHeaders = false, printedListHeaders = false;
	bool mothurCalling = false, debug = false, quietMode = false, changedSeqNames = false, modifyNames = true, append = false;

	vector<string> Groups;
	vector<string> namesOfGroups;
	string saveNextLabel, argv, sharedHeaderMode;

	map<string, string> currentfiles;

};
#endif
