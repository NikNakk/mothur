#pragma once
#include "mothur.h"
#include <vector>
#include <string>

struct ParameterToProcess {
	ParameterToProcess() = default;
	ParameterToProcess(std::string parameterName, std::string parameterValue) : parameterName(parameterName), parameterValue(parameterValue) {}
	std::string parameterName;
	std::string parameterValue;
};
typedef std::vector<ParameterToProcess> ParameterListToProcess;

struct CommandToProcess {
	CommandToProcess() = default;
	CommandToProcess(std::string commandName) : commandName(commandName) {}
	std::string commandName;
	ParameterListToProcess parametersToProcess;
};

class CommandListToProcess : public std::vector<CommandToProcess> {
public:
	CommandListToProcess() = default;
	CommandListToProcess(size_t size) : std::vector<CommandToProcess>(size) {}
	CommandListToProcess(std::string message) : valid(false), errorMessage(message) {}
	std::string errorMessage;
	bool valid = true;
};