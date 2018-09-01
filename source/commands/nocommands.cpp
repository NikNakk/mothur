/*
 *  nocommand.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "nocommands.h"

//**********************************************************************************************************************

int NoCommand::execute(){
	//Could choose to give more help here?fdsah
	LOG(LOGERROR) << "Invalid command.";
   
	LOG(INFO) << Utility::join((CommandFactory(settings)).getListCommands(), ", ");
	
	return 0;
}

//**********************************************************************************************************************
