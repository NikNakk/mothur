#include "commandparametercollection.h"
#include "outputdirectoryparameter.h"
#include "inputdirectoryparameter.h"
#include "seedparameter.h"

#include "utility.h"
#include <map>
#include <vector>
#include <string>
#include <iterator>
#include <sstream>

void CommandParameterCollection::addToGroup(std::map<std::string, std::vector<std::string>>& parameterGroups, std::string & const groupNames, std::string & const parameterName)
{
	std::vector<std::string> groupNamesVec;
	if (groupNames != "") {
		groupNamesVec = Utility::split(groupNames, '-');
		for (auto groupName : groupNamesVec) {
			parameterGroups[groupName].push_back(parameterName);
		}
	}
}

void CommandParameterCollection::add(CommandParameterBase * newParameter)
{
	(*this)[newParameter->getName()] = unique_ptr<CommandParameterBase>(newParameter);
	std::string group;
	addToGroup(chooseAtLeastOneGroups, newParameter->getChooseAtLeastOneGroup(), newParameter->getName());
	addToGroup(chooseOnlyOneGroups, newParameter->getChooseOnlyOneGroup(), newParameter->getName());
	addToGroup(linkedGroups, newParameter->getLinkedGroup(), newParameter->getName());
}

void CommandParameterCollection::addStandardParameters(std::string & inputDir, std::string & outputDir)
{
	this->add(new SeedParameter());
	this->add(new OutputDirectoryParameter(inputDir, settings));
	this->add(new InputDirectoryParameter(outputDir, settings));
}


std::vector<std::string> CommandParameterCollection::getNames()
{
	std::vector<std::string> names;
	for (CommandParameterCollection::iterator it = this->begin(); it != this->end(); ++it) {
		names.push_back(it->first);
	}
	return names;
}

void CommandParameterCollection::validateAndSet(ParameterListToProcess ptp)
{
	for (ParameterListToProcess::iterator it = ptp.begin(); it != ptp.end(); ++it) {
		if ((it->parameterName == "candidate") || (it->parameterName == "query")) {
			it->parameterName = "fasta";
		}
		else if (it->parameterName == "template") {
			it->parameterName = "reference";
		}
		CommandParameterCollection::iterator paramIt = this->find(it->parameterName);
		if (paramIt == this->end()) {
			throw(invalid_argument('\'' + it->parameterName + " is not a valid parameter"));
		}
		else {
			paramIt->second->validateAndSet(it->parameterValue);
		}
	}
	// Check required parameters provided
	for (CommandParameterCollection::iterator paramIt = this->begin(); paramIt != this->end(); ++paramIt) {
		CommandParameterBase * param = paramIt->second.get();
		if (!(param->validateRequiredMissing())) {
			throw(invalid_argument(std::string("Parameter '") + param->getName() + "' is required but was not provided"));
		}
	}
	for (ParameterGroup::iterator pgIt = chooseOnlyOneGroups.begin(); pgIt != chooseOnlyOneGroups.end(); ++pgIt) {
		bool oneSet = false;
		for (std::vector<std::string>::iterator vIt = pgIt->second.begin(); vIt != pgIt->second.end(); ++vIt) {
			if ((*this)[*vIt]->hasValue()) {
				if (oneSet) {
					throw(invalid_argument(std::string("Only one of " + Utility::join(pgIt->second, ", ") + " should be provided")));
				}
				else {
					oneSet = true;
				}
			}
		}
	}
	for (ParameterGroup::iterator pgIt = chooseAtLeastOneGroups.begin(); pgIt != chooseAtLeastOneGroups.end(); ++pgIt) {
		bool atLeastOneSet = false;
		for (std::vector<std::string>::iterator vIt = pgIt->second.begin(); vIt != pgIt->second.end(); ++vIt) {
			if ((*this)[*vIt]->hasValue()) {
				atLeastOneSet = true;
				break;
			}
		}
		if (!atLeastOneSet) {
			throw(invalid_argument(std::string("At least one of " + Utility::join(pgIt->second, ", ") + " should be provided")));
		}
	}
	for (ParameterGroup::iterator pgIt = chooseAtLeastOneGroups.begin(); pgIt != chooseAtLeastOneGroups.end(); ++pgIt) {
		bool anySet = false;
		bool allSet = true;
		for (std::vector<std::string>::iterator vIt = pgIt->second.begin(); vIt != pgIt->second.end(); ++vIt) {
			if ((*this)[*vIt]->hasValue()) {
				anySet = true;
			}
			else {
				allSet = false;
			}
			if (anySet && !allSet) {
				throw(invalid_argument(std::string("If any of " + Utility::join(pgIt->second, ", ") + " are provided, all should be")));
			}
		}
	}

}