#ifndef ENGINE_HPP
#define ENGINE_HPP

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
#include "mothurout.h"
#include "application.h"
#include "settings.h"

class Engine {
public:
	Engine(); 
	virtual ~Engine(){}
	virtual bool getInput() = 0;
	virtual string getCommand();
	virtual string getOutputDir()			{	return app->getSettings().getOutputDir();	}
	virtual string getLogFileName()			{	return app->getSettings().getLogFileName(); 	}
	virtual bool getAppend()				{	return app->getSettings().getAppend();		}

	vector<string> getOptions()		{	return options;		}
protected:
	vector<string> options;
	CommandFactory cFactory;
	MothurOut* mout;
	Application* app;
};



class BatchEngine : public Engine {
public:
	BatchEngine(string, string);
	~BatchEngine();
	virtual bool getInput();
	int openedBatch;
private:
	ifstream inputBatchFile;
	string getNextCommand(ifstream&);

};



class InteractEngine : public Engine {
public:
	InteractEngine(string);
	~InteractEngine();
	virtual bool getInput();
private:
	
};


class ScriptEngine : public Engine {
public:
	ScriptEngine(string, string);
	~ScriptEngine();
	virtual bool getInput();
	int openedBatch;
private:
	string listOfCommands;
	string getNextCommand(string&);

};


#endif
