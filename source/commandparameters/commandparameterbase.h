#pragma once

#include <string>
#include <memory>

enum class CommandParameterType {
	Boolean, Multiple, Number, String, InputType, Directory, OutputFile
};
class CommandParameterBase {
public:
	CommandParameterBase::CommandParameterBase(std::string name, CommandParameterType type, bool required, bool important, std::string chooseOnlyOneGroup, std::string chooseAtLeastOneGroup,
		std::string linkedGroup) :
		name(name), type(type), chooseOnlyOneGroup(chooseOnlyOneGroup), chooseAtLeastOneGroup(chooseAtLeastOneGroup), linkedGroup(linkedGroup),
		required(required), important(important) {}
	virtual ~CommandParameterBase() {};
	std::string getName() const { return name; }
	CommandParameterType getType() const { return type; }
	std::string getChooseOnlyOneGroup() const { return chooseOnlyOneGroup; }
	std::string getChooseAtLeastOneGroup() const { return chooseAtLeastOneGroup; }
	std::string getLinkedGroup() const { return linkedGroup; }
	bool isRequired() const { return required; }
	bool isImportant() const { return important; }
	virtual std::string getValue() const = 0;
	virtual bool hasValue() const { return getValue() != ""; }
	virtual void validateAndSet(std::string newValue) = 0;
	virtual bool validateRequiredMissing() { return !isRequired() || hasValue(); }
private:
	std::string name;
	CommandParameterType type;
	std::string chooseOnlyOneGroup;
	std::string chooseAtLeastOneGroup;
	std::string linkedGroup;
	bool required = false;
	bool important = false;
};


