#ifndef commandmaker_h
#define commandmaker_h

#include <memory>
#include "command.hpp"

// Command maker
// Based on code from stackoverflow.com/questions/16047560/creating-dynamic-type-in-c#16047779

using namespace std;

class CommandMakerBase {
public:
	CommandMakerBase() {}
	virtual ~CommandMakerBase() {}
	virtual unique_ptr<Command> Create(Settings& settings, string optionString) = 0;
	virtual unique_ptr<Command> Create(Settings& settings) = 0;
};

template<class T>
class CommandMaker : public CommandMakerBase {
public:
	CommandMaker() {}
	virtual ~CommandMaker() {}
	virtual unique_ptr<Command> Create(Settings& settings, string optionString) { return unique_ptr<Command>(new T(Settings& settings, optionString)); }
	virtual unique_ptr<Command> Create(Settings& settings) { return unique_ptr<Command>(new T(settings)); }
};
#endif