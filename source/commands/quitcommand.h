#pragma once
/*
 *  quitcommand.h
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "command.hpp"

 /* The quit() command:
	 The quit command terminates the mothur program.
	 The quit command should be in the following format: quit ().   */


class QuitCommand : public Command {

public:
	QuitCommand(Settings& settings, ParameterListToProcess ptp) : Command(settings, ptp) {}
	QuitCommand(Settings& settings) : Command(settings) {}
	~QuitCommand() {}

	string getCommandName() { return "quit"; }
	string getCommandCategory() { return "Hidden"; }
	string getHelpString() { return "The quit command will terminate mothur and should be in the following format: quit() or quit. \n"; }
	string getCitation() { return "no citation"; }
	string getDescription() { return "quit"; }

	virtual int execute() { return 1; }
};


