/*
 *  helpcommand.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "helpcommand.h"

 //**********************************************************************************************************************

HelpCommand::HelpCommand(Settings& settings, string option) : Command(settings, option) {
}
//**********************************************************************************************************************
int HelpCommand::execute() {
	CommandFactory commandFactory;
	LOG(SCREENONLY) << commandFactory.getCommands();
	LOG(INFO) << "For more information about a specific command type 'commandName(help)' i.e. 'read.dist(help)'\n";
	LOG(INFO) << "For further assistance please refer to the Mothur manual on our wiki at http://www.mothur.org/wiki, or contact Pat Schloss at mothur.bugs@gmail.com.";

	return 0;
}
//**********************************************************************************************************************/
