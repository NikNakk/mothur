

#include "nameassignment.hpp"

//**********************************************************************************************************************

NameAssignment::NameAssignment(string nameMapFile) {
	File::openInputFile(nameMapFile, fileHandle);

}
//**********************************************************************************************************************
NameAssignment::NameAssignment() { m = MothurOut::getInstance(); }
//**********************************************************************************************************************

void NameAssignment::readMap() {
	try {
		string firstCol, secondCol, skip;
		//	int index = 0;


		map<string, int>::iterator itData;
		int rowIndex = 0;

		while (fileHandle) {
			fileHandle >> firstCol;	File::gobble(fileHandle);			//read from first column
			fileHandle >> secondCol;			//read from second column

			if (app.isDebug) { LOG(DEBUG) << "firstCol = " + firstCol + ", secondCol= " + secondCol + "\n"; }

			itData = (*this).find(firstCol);
			if (itData == (*this).end()) {

				(*this)[firstCol] = rowIndex++;
				list.push_back(secondCol);		//adds data's value to list
				reverse[rowIndex] = firstCol;

			}
			else { LOG(INFO) << firstCol + " is already in namesfile. I will use first definition." << '\n'; }

			File::gobble(fileHandle);
		}
		fileHandle.close();

	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in NameAssignment, readMap";
		exit(1);
	}
}
//**********************************************************************************************************************
void NameAssignment::push_back(string name) {
	try {

		int num = (*this).size();
		(*this)[name] = num;
		reverse[num] = name;

		list.push_back(name);
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in NameAssignment, push_back";
		exit(1);
	}
}

//**********************************************************************************************************************

ListVector NameAssignment::getListVector(void) {

	return list;

}

//**********************************************************************************************************************

void NameAssignment::print(ostream& out) {
	map<string, int>::iterator it;
	//cout << (*this).size() << endl;
	for (it = (*this).begin(); it != (*this).end(); it++) {
		out << it->first << '\t' << it->second << endl;  //prints out keys and values of the map this.
		//out << it->first << '\t' <<  it->first << endl;
	}
}

//**********************************************************************************************************************

int NameAssignment::get(string key) {
	map<string, int>::iterator itGet = (*this).find(key);

	//if you can't find it
	if (itGet == (*this).end()) { return -1; }

	return	(*this)[key];
}
//**********************************************************************************************************************

string NameAssignment::get(int key) {

	map<int, string>::iterator itGet = reverse.find(key);

	if (itGet == reverse.end()) { return "not found"; }

	return	reverse[key];

}
//**********************************************************************************************************************

