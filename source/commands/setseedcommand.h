//
//  setseedcommand.h
//  Mothur
//
//  Created by Sarah Westcott on 3/24/15.
//  Copyright (c) 2015 Schloss Lab. All rights reserved.
//

#pragma once

#include "command.hpp"
#include "commandfactory.hpp"

/**********************************************************/

class SetSeedCommand : public Command {

public:
	SetSeedCommand(Settings& settings, ParameterListToProcess ptp) : Command(settings, ptp) {}
	SetSeedCommand(Settings& settings) : Command(settings) {}
	~SetSeedCommand() {}

	virtual void setParameters() override;
	virtual std::string getCommandName() const override { return "set.seed"; }
	virtual std::string getCommandCategory() const override { return "General"; }

	virtual string getHelpString() const override;
	virtual string getDescription() const override { return "set random seed"; }

	virtual int execute();
};

/**********************************************************/

 /* defined(__Mothur__setseedcommand__) */
