#pragma once 
/*
 *  engine.hpp
 *
 *
 *  Created by Pat Schloss on 8/15/08.
 *  Copyright 2008 Patrick D. Schloss. All rights reserved.
 *
 */

#include "mothurdefs.h"
#include "command.hpp"
#include "commandfactory.hpp"
#include "settings.h"
#include "logsinks.h"
#include "g3log/sinkhandle.hpp"
#include "commandtoprocess.h"
#include "filehandling/textfileread.h"

class Engine {
public:
	Engine(Settings& settings, std::string path);
	virtual ~Engine() = default;
	void processCommands();
	virtual std::string getCommands() = 0;
protected:
	CommandFactory cFactory;
	Settings& settings;
	bool interactive = false;
private:
	CommandListToProcess parseCommands(string & input);
};

class BatchEngine : public Engine {
public:
	BatchEngine(Settings& settings, std::string path, string);
	virtual std::string getCommands();
private:
	int openedBatch;
	TextFileRead batchFile;
};

class InteractEngine : public Engine {
public:
	InteractEngine::InteractEngine(Settings& settings, string path, g3::SinkHandle<LogScreen>& screenLogHandle) :
		Engine(settings, path), screenLogHandle(screenLogHandle) {interactive = true; }
	virtual std::string getCommands();
protected:
	g3::SinkHandle<LogScreen> & screenLogHandle;
};


class ScriptEngine : public Engine {
public:
	ScriptEngine(Settings& settings, std::string path, string);
	virtual std::string getCommands();
private:
	std::string script;
};