//
//  setseedcommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 3/24/15.
//  Copyright (c) 2015 Schloss Lab. All rights reserved.
//

#include "setseedcommand.h"
#include "commandparameters/seedparameter.h"

//**********************************************************************************************************************
void SetSeedCommand::setParameters() {
	parameters.add(new SeedParameter(true, true));
}
//**********************************************************************************************************************
string SetSeedCommand::getHelpString() const {
	string helpString = "The set.seed command is used to seed random.\n"
		"The set.seed command parameter is seed, and it is required.\n"
		"To seed random set seed=yourRandomValue. By default mothur seeds random with the start time.\n"
		"Example set.seed(seed=12345).\n"
		"Note: No spaces between parameter labels (i.e. seed), '=' and parameters (i.e.yourSeedValue).\n";
	return helpString;
}
//**********************************************************************************************************************

int SetSeedCommand::execute() { return 0; }
//**********************************************************************************************************************/
