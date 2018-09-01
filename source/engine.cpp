/*
 *  engine.cpp
 *
 *
 *  Created by Pat Schloss on 8/15/08.
 *  Copyright 2008 Patrick D. Schloss. All rights reserved.
 *
 */

#include "engine.hpp"
#include <stdlib.h>
#include <iterator>

 /***********************************************************************/
Engine::Engine(Settings& settings, std::string path) : settings(settings), cFactory(settings) {
	string temppath = path.substr(0, (path.find_last_of("othur") - 5));

	//this will happen if you set the path variable to contain mothur's exe location
	if (temppath == "") { path = File::findProgramPath("mothur"); }

	settings.setProgramPath(path);

	//if you haven't set your own location
#ifdef MOTHUR_FILES
	string defaultPath = MOTHUR_FILES;

	//add / to name if needed
	if (defaultPath.substr(defaultPath.length() - 1) != path_delimiter) { defaultPath += path_delimiter; }

	defaultPath = File::getFullPathName(defaultPath);
#else
	//set default location to search for files to mothur's executable location.  This will resolve issue of double-clicking on the executable which opens mothur and sets pwd to your home directory instead of the mothur directory and leads to "unable to find file" errors.
	std::string defaultPath = path.substr(0, (path.find_last_of('m')));
#endif
	if (defaultPath != "") { settings.setDefaultPath(defaultPath); }
}

/***********************************************************************/

void Engine::processCommands()
{
	bool finished = false;
	while (!finished) {
		std::string input = getCommands();
		CommandListToProcess cList;
		cList = parseCommands(input);
		if (!cList.valid) {
			LOG(LOGERROR) << "Invalid command: " << cList.errorMessage;
		}
		else {
			// Handles multiple commands returned from a single call (e.g. Batch input, ; used to separate commands on a single line
			for (CommandListToProcess::iterator cListIt = cList.begin(); cListIt != cList.end() & !finished; ++cListIt) {
				int quitCommandCalled = 0;
				unique_ptr<Command> command;
				command = cFactory.getCommand(*cListIt);
				command->setup();
				if (!(command->isValid())) {
					LOG(LOGERROR) << "Invalid command: " << command->getErrorMessage();
					command.reset();
				}
				if (command && !(command->aborted())) {
					if (interactive) {
						mothur_executing = true;
					}
					ctrlc_pressed = false;
					quitCommandCalled = command->execute();
					mothur_executing = false;
				}

				//if we aborted command
				if (quitCommandCalled == 2) {
					LOG(LOGERROR) << "Did not complete " + cListIt->commandName + ".\n";
				}
				else if (quitCommandCalled == 1) {
					finished = true;
				}
			}
		}
	}
	return;
}
CommandListToProcess Engine::parseCommands(std::string & input)
{
	CommandListToProcess cList;
	// If comment, return empty CommandListToProcess
	if (input[0] == '#') {
		return cList;
	}
	// Quit is an odd exception that is permitted without brackets
	else if (input == "quit") {
		cList.push_back(CommandToProcess("quit"));
		return cList;
	}

	std::string::iterator it = input.begin();
	// Loop until no further commands in string
	while (it != input.end()) {
		ostringstream os;
		// Find first ( which indicates end of command name
		for (;it != input.end() && *it != '(';it++) {
			os << *it;
		}
		if (it == input.end()) {
			return CommandListToProcess("Command missing (");
		}
		++it;
		CommandToProcess cmd(os.str());
		for (;it != input.end() && *it == ' ';++it) {}
		while (it != input.end() && *it != ')') {
			os.str("");
			for (;it != input.end() && *it != '=' && *it != ')' && *it != ',';++it) {
				os << *it;
			}
			if (it == input.end()) {
				return CommandListToProcess("Command missing )");
			}
			std::string paramName = os.str();
			std::vector<std::string> paramValues;
			if (*it == '=') {
				++it;
				for (;it != input.end() && *it == ' ';++it) {}
				os.str("");
				bool inSingleQuote = false;
				bool inDoubleQuote = false;
				bool inBackSlash = false;
				for (;it != input.end() && (inSingleQuote || inDoubleQuote || inBackSlash || (*it != ')' && *it != ',' && *it != '-'));++it) {
					// At this stage pass through all characters, but noting that quotes and backslashes can be used
					// to escape separators
					os << *it;
					if (inBackSlash) {
						inBackSlash = false;
					}
					else if (inSingleQuote) {
						if (*it == '\'') {
							inSingleQuote = false;
						}
					}
					else if (inDoubleQuote) {
						if (*it == '"') {
							inDoubleQuote = false;
						}
					}
					else if (*it == '\\') {
						inBackSlash = true;
					}
					else if (*it == '\'') {
						inSingleQuote = true;
					}
					else if (*it == '"') {
						inDoubleQuote = true;
					}
				}
				if (inSingleQuote) {
					return CommandListToProcess("Unmatched '");
				}
				else if (inDoubleQuote) {
					return CommandListToProcess("Unmatched \"");
				}
				else if (it == input.end()) {
					return CommandListToProcess("Command missing )");
				}
			}
			else {
				os.str("");
			}
			cmd.parametersToProcess.push_back(ParameterToProcess(paramName, os.str()));
			if (*it == ',') {
				it++;
				for (;it != input.end() && *it == ' ';++it) {}
			}
		}
		cList.push_back(cmd);
		++it;
		for (;it != input.end() && *it == ' ';++it) {}
		if (it != input.end() && *it != ';') {
			return CommandListToProcess("Unexpected input after command");
		}
		else if (it != input.end() && *it == ';') {
			it++;
			for (;it != input.end() && *it == ' ';++it) {}
		}
	}
	return cList;
}
/***********************************************************************/
string InteractEngine::getCommands() {
	string nextCommand;
#if defined(USE_READLINE) || defined(USE_EDITLINE)
	char* cNextCommand = NULL;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	screenLogHandle.setWaitOver(false);
	screenLogHandle.call(&LogScreen::notifyCinWaiting, ref(screenLogHandle));
	screenLogHandle.cinWaiting();
	screenLogHandle.waitUntilClear();

	std::cout << '\n';
	cNextCommand = readline("mothur > ");
	nextCommand = string(cNextCommand);

	if (cNextCommand != NULL) { add_history(cNextCommand); }
	else { //^D causes null string and we want it to quit mothur
		nextCommand = "quit()";
		LOG(INFO) << nextCommand;
	}
	free(cNextCommand);

#else
	std::cout << '\nmothur > ';
	getline(cin, nextCommand);
#endif	
	LOG(FILEONLY) << "mothur > " << nextCommand;
	return nextCommand;
	}
/***********************************************************************/
//This function opens the batchfile to be used by BatchEngine::getInput.
BatchEngine::BatchEngine(Settings& settings, string path, string batchFileName) :
	Engine(settings, path), batchFile(batchFileName)
{
	if (!batchFile.good()) {
		LOG(LOGERROR) << "Unable to open batchfile";
		exit(1);
	}
}

std::string BatchEngine::getCommands() {
	if (batchFile.eof()) {
		return "quit()";
	}
	else {
		std::string nextCommand = batchFile.readLine();
		LOG(INFO) << "mothur > " << nextCommand;
		return nextCommand;
	}
}

/***********************************************************************/
ScriptEngine::ScriptEngine(Settings& settings, std::string path, std::string commandString) :
	Engine(settings, path),
	script(commandString.substr(1, (commandString.length() - 1)))
{}

/***********************************************************************/

std::string ScriptEngine::getCommands()
{
	std::string nextCommand;
	if (script != "") {
		nextCommand = script;
		script = "";
	}
	else {
		nextCommand = "quit()";
	}
	return nextCommand;
}
