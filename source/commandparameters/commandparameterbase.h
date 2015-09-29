#ifndef commandparameterbase_h
#define commandparameterbase_h

#include <string>
#include <memory>
#include <vector>
using namespace std;

enum CommandParameterType {
	Boolean, Multiple, Number, String, InputType, Directory, OutputFile
};
class CommandParameterBase {
public:
	CommandParameterBase::CommandParameterBase(string name, CommandParameterType type, bool required, bool important, string chooseOnlyOneGroup, string chooseAtLeastOneGroup,
		string linkedGroup) :
		name(name), type(type), chooseOnlyOneGroup(chooseOnlyOneGroup), chooseAtLeastOneGroup(chooseAtLeastOneGroup), linkedGroup(linkedGroup),
		required(required), important(important) {}
	virtual ~CommandParameterBase() {};
	string getName() { return name; }
	CommandParameterType getType() { return type; }
	string getChooseOnlyOneGroup() { return chooseOnlyOneGroup; }
	string getChooseAtLeastOneGroup() { return chooseAtLeastOneGroup; }
	string getLinkedGroup() { return linkedGroup; }
	bool isRequired() { return required; }
	bool isImportant() { return important; }
	virtual string getValue() = 0;
	virtual bool hasValue() { return getValue() != ""; }
	virtual void validateAndSet(string newValue) = 0;
	virtual bool validateRequiredMissing() { return !isRequired() || hasValue(); }
private:
	string name;
	CommandParameterType type;
	string chooseOnlyOneGroup;
	string chooseAtLeastOneGroup;
	string linkedGroup;
	bool required = false;
	bool important = false;
};

#endif
