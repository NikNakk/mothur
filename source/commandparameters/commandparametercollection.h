#ifndef commandparametercollection_h
#define commandparametercollection_h

#include "commandparameterbase.h"
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "settings.h"

using namespace std;

class CommandParameterCollection : public map<string, unique_ptr<CommandParameterBase>> {
public:
	CommandParameterCollection(Settings& settings) : settings(settings) {};
	void add(CommandParameterBase * newParameter);
	void addStandardParameters();
	vector<string> getNames();
	void parseOptionString(string optionString);
private:
	typedef map<string, vector<string>> ParameterGroup;
	ParameterGroup chooseOnlyOneGroups;
	ParameterGroup chooseAtLeastOneGroups;
	ParameterGroup linkedGroups;
	Settings& settings;
};

#endif