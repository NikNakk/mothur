#pragma once
/*
 *  nocommand.h
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

 /* This command is run if the user enters an invalid command. */

#include "command.hpp"
#include "commandfactory.hpp"

class NoCommand : public Command {

public:
	NoCommand(Settings& settings, ParameterListToProcess ptp) : Command(settings, ptp) {};
	NoCommand(Settings& settings) : Command(settings) {}
	~NoCommand() {}

	virtual string getCommandName() const override { return "NoCommand"; }
	virtual string getCommandCategory() const override { return "Hidden"; }
	virtual string getHelpString() const override { return "No Command"; }
	virtual string getCitation() const override { return "no citation"; }
	virtual string getDescription() const override { return "no description"; }

	virtual int execute() override;
private:
	vector<string> outputNames;
};


