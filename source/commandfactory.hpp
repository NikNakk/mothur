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
#include "mothurout.h"
#include "currentfile.h"
#include "commandmaker.h"

typedef map<string, unique_ptr<CommandMakerBase>> TMapCommands;

class CommandFactory {
public:
	CommandFactory();
	unique_ptr<Command> getCommand(string, string, string);
	unique_ptr<Command> getCommand(string, string);
	unique_ptr<Command> getCommand(string);
	bool isValidCommand(string);
	bool isValidCommand(string, string);
	void printCommands(ostream&);
    void printCommandsCategories(ostream&);
	unique_ptr<vector<string>> getListCommands();
	void Register(string command, unique_ptr<CommandMakerBase> creator);
	unique_ptr<Command> Create(string command, string optionString);
	unique_ptr<Command> Create(string command);

private:
	MothurOut* m;
	CurrentFile* currentFile;
	
	map<string, string>::iterator it;
	string outputDir, inputDir, logFileName;
	bool append;

	TMapCommands commandMakers;
	    
	CommandFactory( const CommandFactory& ); // Disable copy constructor
	void operator=( const CommandFactory& ); // Disable assignment operator

};


#endif
