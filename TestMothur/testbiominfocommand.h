//
//  testbiominfocommand.h
//  Mothur
//
//  Created by Sarah Westcott on 8/18/15.
//  Copyright (c) 2015 Schloss Lab. All rights reserved.
//

#pragma once

#include "biominfocommand.h"

class TestBiomInfoCommand : public BiomInfoCommand {
    
public:
    
    using BiomInfoCommand::getTag;
    using BiomInfoCommand::getName;
    using BiomInfoCommand::getTaxonomy;
    using BiomInfoCommand::readRows;
    using BiomInfoCommand::getDims;
    using BiomInfoCommand::readData;
    using BiomInfoCommand::getNamesAndTaxonomies;

};


 /* defined(__Mothur__testbiominfocommand__) */
