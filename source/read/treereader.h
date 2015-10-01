#pragma once

//
//  treereader.h
//  Mothur
//
//  Created by Sarah Westcott on 4/11/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "tree.h"
#include "counttable.h"

class TreeReader {

public:

	TreeReader(string tf, string cf);
	TreeReader(string tf, string gf, string nf);
	~TreeReader() {}

	vector<Tree> getTrees() { return trees; }

private:
	vector<Tree> trees;
	CountTable ctY;
	//map<string, string> nameMap; //dupName -> uniqueName
   // map<string, string> names;

	string treefile, groupfile, namefile, countfile;

	bool readTrees();
	int readNamesFile();
};
