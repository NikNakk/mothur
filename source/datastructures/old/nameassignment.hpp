#pragma once

#include "mothur.h"
#include "listvector.hpp"

class NameAssignment : public map<string, int> {
public:
	NameAssignment(string);
	NameAssignment();
	~NameAssignment() {}
	void readMap();
	ListVector getListVector();
	int get(string);
	string get(int);
	void print(ostream&);
	void push_back(string);
private:
	ifstream fileHandle;
	ListVector list;
	map<int, string> reverse;
};





