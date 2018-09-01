#pragma once

#include <memory>
#include "command.hpp"
#include "commandtoprocess.h"

// Command maker
// Based on code from stackoverflow.com/questions/16047560/creating-dynamic-type-in-c#16047779

class CommandMakerBase {
public:
	CommandMakerBase() {}
	virtual ~CommandMakerBase() {}
	virtual std::unique_ptr<Command> Create(Settings& settings, ParameterListToProcess parms) = 0;
	virtual std::unique_ptr<Command> Create(Settings& settings) = 0;
};

template<class T>
class CommandMaker : public CommandMakerBase {
public:
	CommandMaker() {}
	virtual ~CommandMaker() {}
	virtual std::unique_ptr<Command> Create(Settings& settings, ParameterListToProcess parms) { return std::unique_ptr<Command>(new T(settings, parms)); }
	virtual std::unique_ptr<Command> Create(Settings& settings) { return std::unique_ptr<Command>(new T(settings)); }
};