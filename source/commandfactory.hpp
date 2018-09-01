#pragma once

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
#include "commandtoprocess.h"
#include "settings.h"
#include <stdexcept>

typedef map<string, unique_ptr<CommandMakerBase>> TMapCommands;

class CommandFactory {
public:
	CommandFactory(Settings& settings);
	unique_ptr<Command> getCommand(CommandToProcess& cmd, string mode);
	unique_ptr<Command> getCommand(string commandName);
	unique_ptr<Command> getCommand(CommandToProcess& cmd);
	/*bool isValidCommand(string option);
	bool isValidCommand(string, string);*/
	string getValidCommands();
	void getCommandsCategories();
	void Register(string command, unique_ptr<CommandMakerBase> creator);
	unique_ptr<Command> Create(CommandToProcess& cmd);
	unique_ptr<Command> Create(string command);

	std::vector<std::string> getListCommands();

private:
	Settings& settings;
	TMapCommands commandMakers;

	CommandFactory(const CommandFactory&) = delete; // Disable copy constructor
	void operator=(const CommandFactory&) = delete; // Disable assignment operator
};