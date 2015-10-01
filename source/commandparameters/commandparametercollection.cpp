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

void CommandParameterCollection::add(CommandParameterBase * newParameter)
{
	(*this)[newParameter->getName()] = unique_ptr<CommandParameterBase>(newParameter);
	string group;
	if ((group = newParameter->getChooseAtLeastOneGroup()) != "") {
		chooseAtLeastOneGroups[group].push_back(newParameter->getName());
	}
	if ((group = newParameter->getChooseOnlyOneGroup()) != "") {
		chooseOnlyOneGroups[group].push_back(newParameter->getName());
	}
	if ((group = newParameter->getLinkedGroup()) != "") {
		linkedGroups[group].push_back(newParameter->getName());
	}
}

void CommandParameterCollection::addStandardParameters()
{
	this->add(new SeedParameter());
	Y	this->add(new OutputDirectoryParameter(settings));
	this->add(new InputDirectoryParameter(settings));
}

vector<string> CommandParameterCollection::getNames()
{
	vector<string> names;
	for (CommandParameterCollection::iterator it = this->begin(); it != this->end(); ++it) {
		names.push_back(it->first);
	}
	return names;
}

void CommandParameterCollection::parseOptionString(string optionString)
{
	vector<string> optionStrings;
	bool abort = false;
	try {
		Utility::split(optionString, ',', optionStrings);
		// Split option string into parameters
		for (vector<string>::iterator it = optionStrings.begin(); it != optionStrings.end(); it++) {
			vector<string> opt;
			Utility::split(optionString, '=', opt);
			if (opt.size() != 2) {
				throw(invalid_argument(string("Invalid parameter provided: ") + *it));
			}
			if ((opt[0] == "candidate") || (opt[0] == "query")) {
				opt[0] = "fasta";
			}
			else if (opt[0] == "template") {
				opt[0] = "reference";
			}
			CommandParameterCollection::iterator paramIt = this->find(opt[0]);
			if (paramIt == this->end()) {
				throw(invalid_argument(opt[0] + " is not a valid parameter"));
			}
			else {
				paramIt->second->validateAndSet(opt[1]);
			}
		}
		// Check required parameters provided
		for (CommandParameterCollection::iterator paramIt = this->begin(); paramIt != this->end(); paramIt++) {
			CommandParameterBase * param = paramIt->second.get();
			if (param->validateRequiredMissing()) {
				throw(invalid_argument(string("Parameter ") + param->getName() + " is required but was not provided"));
			}
		}
		for (ParameterGroup::iterator pgIt = chooseOnlyOneGroups.begin(); pgIt != chooseOnlyOneGroups.end(); pgIt++) {
			bool oneSet = false;
			for (vector<string>::iterator vIt = pgIt->second.begin(); vIt != pgIt->second.end(); vIt++) {
				if ((*this)[*vIt]->hasValue()) {
					if (oneSet) {
						throw(invalid_argument(string("Only one of " + Utility::join(pgIt->second, ", ") + " should be provided")));
					}
					else {
						oneSet = true;
					}
				}
			}
		}
		for (ParameterGroup::iterator pgIt = chooseAtLeastOneGroups.begin(); pgIt != chooseAtLeastOneGroups.end(); pgIt++) {
			bool atLeastOneSet = false;
			for (vector<string>::iterator vIt = pgIt->second.begin(); vIt != pgIt->second.end(); vIt++) {
				if ((*this)[*vIt]->hasValue()) {
					atLeastOneSet = true;
					break;
				}
			}
			if (!atLeastOneSet) {
				throw(invalid_argument(string("At least one of " + Utility::join(pgIt->second, ", ") + " should be provided")));
			}
		}
		for (ParameterGroup::iterator pgIt = chooseAtLeastOneGroups.begin(); pgIt != chooseAtLeastOneGroups.end(); pgIt++) {
			bool anySet = false;
			bool allSet = true;
			for (vector<string>::iterator vIt = pgIt->second.begin(); vIt != pgIt->second.end(); vIt++) {
				if ((*this)[*vIt]->hasValue()) {
					anySet = true;
				}
				else {
					allSet = false;
				}
				if (anySet && !allSet) {
					throw(invalid_argument(string("If any of " + Utility::join(pgIt->second, ", ") + " are provided, all should be")));
				}
			}
		}
	}
	catch (exception& e)
	{
		throw;
	}
}