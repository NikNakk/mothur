#ifndef settings_h
#define settings_h

#include <string>
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


	string getPhylipFile() { return phylipfile; }
	string getColumnFile() { return columnfile; }
	string getListFile() { return listfile; }
	string getRabundFile() { return rabundfile; }
	string getSabundFile() { return sabundfile; }
	string getNameFile() { return namefile; }
	string getGroupFile() { return groupfile; }
	string getOrderFile() { return orderfile; }
	string getOrderGroupFile() { return ordergroupfile; }
	string getTreeFile() { return treefile; }
	string getSharedFile() { return sharedfile; }
	string getRelAbundFile() { return relabundfile; }
	string getDesignFile() { return designfile; }
	string getFastaFile() { return fastafile; }
	string getSFFFile() { return sfffile; }
	string getQualFile() { return qualfile; }
	string getOligosFile() { return oligosfile; }
	string getAccnosFile() { return accnosfile; }
	string getTaxonomyFile() { return taxonomyfile; }
	string getFlowFile() { return flowfile; }
	string getBiomFile() { return biomfile; }
	string getCountTableFile() { return counttablefile; }
	string getSummaryFile() { return summaryfile; }
	string getFileFile() { return filefile; }
	int getProcessors() { return processors; }
	bool getMothurCalling() { return mothurCalling; }
	bool getDebug() { return debug; }
	bool getModifyNames() { return modifyNames; }

	void setListFile(string f) { listfile = File::getFullPathName(f); }
	void setTreeFile(string f) { treefile = File::getFullPathName(f); }
	void setGroupFile(string f) { groupfile = File::getFullPathName(f);	groupMode = "group"; }
	void setPhylipFile(string f) { phylipfile = File::getFullPathName(f); }
	void setColumnFile(string f) { columnfile = File::getFullPathName(f); }
	void setNameFile(string f) { namefile = File::getFullPathName(f); }
	void setRabundFile(string f) { rabundfile = File::getFullPathName(f); }
	void setSabundFile(string f) { sabundfile = File::getFullPathName(f); }
	void setSharedFile(string f) { sharedfile = File::getFullPathName(f); }
	void setRelAbundFile(string f) { relabundfile = File::getFullPathName(f); }
	void setOrderFile(string f) { orderfile = File::getFullPathName(f); }
	void setOrderGroupFile(string f) { ordergroupfile = File::getFullPathName(f); }
	void setDesignFile(string f) { designfile = File::getFullPathName(f); }
	void setFastaFile(string f) { fastafile = File::getFullPathName(f); }
	void setSFFFile(string f) { sfffile = File::getFullPathName(f); }
	void setQualFile(string f) { qualfile = File::getFullPathName(f); }
	void setOligosFile(string f) { oligosfile = File::getFullPathName(f); }
	void setAccnosFile(string f) { accnosfile = File::getFullPathName(f); }
	void setTaxonomyFile(string f) { taxonomyfile = File::getFullPathName(f); }
	void setFlowFile(string f) { flowfile = File::getFullPathName(f); }
	void setBiomFile(string f) { biomfile = File::getFullPathName(f); }
	void setSummaryFile(string f) { summaryfile = File::getFullPathName(f); }
	void setFileFile(string f) { filefile = File::getFullPathName(f); }
	void setCountTableFile(string f) { counttablefile = File::getFullPathName(f);	groupMode = "count"; }
	void setProcessors(int p) { processors = p; }
	void setDebug(bool debug) { this->debug = debug; }
	void setModifyNames(bool modifyNames) { this->modifyNames = modifyNames; }

private:
	string logFileName;
	string defaultPath, outputDir, inputDir;
	string releaseDate, version;
	string groupMode = "group";
	int control_pressed = 0;

	int processors = 1;

	bool gui = false, printedSharedHeaders = false, printedListHeaders = false;
	bool mothurCalling = false, debug = false, quietMode = false, changedSeqNames = false, modifyNames = true, append = false;


	string accnosfile, phylipfile, columnfile, listfile, rabundfile, sabundfile, namefile, groupfile, designfile, taxonomyfile, biomfile, filefile;
	string orderfile, treefile, sharedfile, ordergroupfile, relabundfile, fastafile, qualfile, sfffile, oligosfile, flowfile, counttablefile, summaryfile;

};
#endif
