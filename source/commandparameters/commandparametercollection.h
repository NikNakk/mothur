#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "commandparameterbase.h"
#include "settings.h"
#include "commandtoprocess.h"

class CommandParameterCollection : public map<std::string, unique_ptr<CommandParameterBase>> {
public:
	CommandParameterCollection(Settings& settings) : settings(settings) {};
	void add(CommandParameterBase * newParameter);
	void addStandardParameters(std::string & inputDir, std::string & outputDir);
	std::vector<std::string> getNames();
	void validateAndSet(ParameterListToProcess ptp);
private:
	void addToGroup(std::map<std::string, std::vector<std::string>> & parameterGroups, const std::string& groupNames, const std::string& parameterName);
	typedef map<std::string, std::vector<std::string>> ParameterGroup;
	ParameterGroup chooseOnlyOneGroups;
	ParameterGroup chooseAtLeastOneGroups;
	ParameterGroup linkedGroups;
	Settings& settings;
};
