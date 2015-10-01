#pragma once 
/*
 *  engine.hpp
 *
 *
 *  Created by Pat Schloss on 8/15/08.
 *  Copyright 2008 Patrick D. Schloss. All rights reserved.
 *
 */



#include "mothur.h"
#include "commandoptionparser.hpp"
#include "command.hpp"
#include "commandfactory.hpp"
#include "settings.h"
#include "logsinks.h"
#include "g3log\sinkhandle.hpp"

class Engine {
public:
	Engine(Settings& settings);
	virtual ~Engine() {}
	virtual bool getInput() = 0;

	vector<string> getOptions() { return options; }
protected:
	vector<string> options;
	CommandFactory cFactory;
	Settings& settings;
};



class BatchEngine : public Engine {
public:
	BatchEngine(Settings& settings, string, string);
	~BatchEngine();
	virtual bool getInput();
	int openedBatch;
private:
	ifstream inputBatchFile;
	string getNextCommand(ifstream&);
};



class InteractEngine : public Engine {
public:
	InteractEngine(Settings& settings, string path, g3::SinkHandle<LogScreen>&);
	~InteractEngine();
	bool getInput();
	string getCommand();
private:
	g3::SinkHandle<LogScreen> & screenLogHandle;
};


class ScriptEngine : public Engine {
public:
	ScriptEngine(Settings& settings, string, string);
	~ScriptEngine();
	virtual bool getInput();
	int openedBatch;
private:
	string listOfCommands;
	string getNextCommand(string&);

};