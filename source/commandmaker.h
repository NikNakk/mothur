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
	virtual unique_ptr<Command> Create(string optionString) = 0;
	virtual unique_ptr<Command> Create() = 0;
};

template<class T>
class CommandMaker : public CommandMakerBase {
public:
	CommandMaker() {}
	virtual ~CommandMaker() {}
	virtual unique_ptr<Command> Create(string optionString) { return unique_ptr<Command>(new T(optionString)); }
	virtual unique_ptr<Command> Create() { return unique_ptr<Command>(new T()); }
};
#endif