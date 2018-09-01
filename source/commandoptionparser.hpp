#pragma once

#include "mothur.h"

//**********************************************************************************************************************

class CommandOptionParser {
public:
	CommandOptionParser(string);
	string getCommandString();
	string getOptionString();

private:
	string commandString, optionString;
};

//**********************************************************************************************************************


