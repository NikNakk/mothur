//
//  testmergegroupscommand.h
//  Mothur
//
//  Created by Sarah Westcott on 7/29/15.
//  Copyright (c) 2015 Schloss Lab. All rights reserved.
//

#pragma once

#include "mergegroupscommand.h"

class TestMergeGroupsCommand : public MergeGroupsCommand {
    
public:
    
    //private functions
    using MergeGroupsCommand::process;
    using MergeGroupsCommand::processSharedFile;
    using MergeGroupsCommand::processGroupFile;
    using MergeGroupsCommand::processCountFile;
    
};

 /* defined(__Mothur__testmergegroupscommand__) */
