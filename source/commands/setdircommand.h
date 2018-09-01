#pragma once

/*
 *  setoutdircommand.h
 *  Mothur
 *
 *  Created by westcott on 1/21/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "command.hpp"
#include "commandfactory.hpp"

 /**********************************************************/

class SetDirectoryCommand : public Command {

public:
	SetDirectoryCommand(Settings& settings, ParameterListToProcess ptp) : Command(settings, ptp) {}
	SetDirectoryCommand(Settings& settings) : Command(settings) {}
	~SetDirectoryCommand() {}

	void setParameters();
	virtual string getCommandName() const override { return "set.dir"; }
	virtual string getCommandCategory() const override { return "General"; }

	virtual string getHelpString() const override;
	virtual string getDescription() const override { return "set input, output and default directories"; }

	virtual int execute() override;

private:
	string output, input, tempdefault;
	bool abort, debugorSeedOnly, modifyNames, debug;
	int random;
	vector<string> outputNames;
};

/**********************************************************/



