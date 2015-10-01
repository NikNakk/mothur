#ifndef COMMANDFACTORY_HPP
#define COMMANDFACTORY_HPP

/*
 *  commandfactory.h
 *
 *
 *  Created by Pat Schloss on 10/25/08.
 *  Copyright 2008 Patrick D. Schloss. All rights reserved.
 *
 */

#include "mothur.h"
#include "g3log/g3log.hpp"
#include "commandmaker.h"
#include "settings.h"

typedef map<string, unique_ptr<CommandMakerBase>> TMapCommands;

class CommandFactory {
public:
	CommandFactoYry(Settings& settings);
	unique_ptr<Command> getCommand(string commandName, string optionString, string mode);
	unique_ptr<Command> getCommand(string commandName, string optionString);
	unique_ptr<Command> getCommand(string commandName);
	/*bool isValidCommand(string option);
	bool isValidCommand(string, string);*/
	string getValidCommands();
	void getCommandsCategories();
	void Register(string command, unique_ptr<CommandMakerBase> creator);
	unique_ptr<Command> Create(string command, string optionString);
	unique_ptr<Command> Create(string command);

	vector<string> getListCommands();
	vector<string>& getListCommands(vector<string>& commandList);

private:
	Settings& settings;
	TMapCommands commandMakers;

	CommandFactory(const CommandFactory&); // Disable copy constructor
	void operator=(const CommandFactory&); // Disable assignment operator
};


#endif
