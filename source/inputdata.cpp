/*
 *  inputdata.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 11/18/08.
 *  Copyright 2008 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "inputdata.h"
#include "ordervector.hpp"
#include "listvector.hpp"
#include "rabundvector.hpp"

 /***********************************************************************/

InputData::InputData(string fName, string f) : format(f) {
	File::openInputFile(fName, fileHandle);
	filename = fName;
	m->saveNextLabel = "";
}
/***********************************************************************/

InputData::~InputData() {
	fileHandle.close();
	m->saveNextLabel = "";
}

/***********************************************************************/

InputData::InputData(string fName, string orderFileName, string f) : format(f) {
	ifstream ofHandle;
	File::openInputFile(orderFileName, ofHandle);
	string name;

	int count = 0;

	while (ofHandle) {
		ofHandle >> name;
		orderMap[name] = count;
		count++;
		File::gobble(ofHandle);
	}
	ofHandle.close();

	File::openInputFile(fName, fileHandle);
	m->saveNextLabel = "";

}
/***********************************************************************/

ListVector* InputData::getListVector() {
	if (!fileHandle.eof()) {
		if (format == "list") {
			list = new ListVector(fileHandle);
		}
		else { list = NULL; }

		File::gobble(fileHandle);
		return list;
	}
	else {
		return NULL;
	}
}

/***********************************************************************/
ListVector* InputData::getListVector(string label) {
	ifstream in;
	string  thisLabel;
	File::openInputFile(filename, in);
	m->saveNextLabel = "";

	if (in) {

		if (format == "list") {

			while (in.eof() != true) {

				list = new ListVector(in);
				thisLabel = list->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete list; }
				File::gobble(in);
			}
		}
		else { list = NULL; }

		in.close();
		return list;
	}
	else {
		return NULL;
	}
}
/***********************************************************************/
ListVector* InputData::getListVector(string label, bool resetFP) {
	string  thisLabel;
	fileHandle.clear();
	fileHandle.seekg(0);
	m->saveNextLabel = "";

	if (fileHandle) {

		if (format == "list") {

			while (fileHandle.eof() != true) {

				list = new ListVector(fileHandle); File::gobble(fileHandle);
				thisLabel = list->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete list; }
			}
		}
		else { list = NULL; }

		return list;
	}
	else {
		return NULL;
	}
}

/***********************************************************************/

SharedListVector* InputData::getSharedListVector() {
	if (fileHandle) {
		if (format == "shared") {
			SharedList = new SharedListVector(fileHandle);
		}
		else { SharedList = NULL; }

		File::gobble(fileHandle);
		return SharedList;
	}
	else {
		return NULL;
	}
}
/***********************************************************************/

SharedListVector* InputData::getSharedListVector(string label) {
	ifstream in;
	string  thisLabel;
	File::openInputFile(filename, in);

	if (in) {

		if (format == "shared") {

			while (in.eof() != true) {

				SharedList = new SharedListVector(in);
				thisLabel = SharedList->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete SharedList; }
				File::gobble(in);
			}

		}
		else { SharedList = NULL; }

		in.close();
		return SharedList;

	}
	else {
		return NULL;
	}
}



/***********************************************************************/

SharedOrderVector* InputData::getSharedOrderVector() {
	if (fileHandle) {
		if (format == "sharedfile") {
			SharedOrder = new SharedOrderVector(fileHandle);
		}
		else { SharedOrder = NULL; }

		File::gobble(fileHandle);
		return SharedOrder;

	}
	else {
		return NULL;
	}
}

/***********************************************************************/

SharedOrderVector* InputData::getSharedOrderVector(string label) {
	ifstream in;
	string  thisLabel;
	File::openInputFile(filename, in);
	m->saveNextLabel = "";

	if (in) {

		if (format == "sharedfile") {

			while (in.eof() != true) {

				SharedOrder = new SharedOrderVector(in);
				thisLabel = SharedOrder->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete SharedOrder; }
				File::gobble(in);
			}

		}
		else { SharedOrder = NULL; }

		in.close();
		return SharedOrder;

	}
	else {
		return NULL;
	}
}



/***********************************************************************/

OrderVector* InputData::getOrderVector() {
	if (fileHandle) {
		if ((format == "list") || (format == "listorder")) {
			input = new ListVector(fileHandle);
		}
		else if (format == "shared") {
			input = new SharedListVector(fileHandle);
		}
		else if (format == "rabund") {
			input = new RAbundVector(fileHandle);
		}
		else if (format == "order") {
			input = new OrderVector(fileHandle);
		}
		else if (format == "sabund") {
			input = new SAbundVector(fileHandle);
		}

		File::gobble(fileHandle);

		output = new OrderVector();
		*output = (input->getOrderVector());

		return output;
	}
	else {
		return NULL;
	}
}

/***********************************************************************/
OrderVector* InputData::getOrderVector(string label) {

	ifstream in;
	string  thisLabel;
	File::openInputFile(filename, in);

	if (in) {
		if ((format == "list") || (format == "listorder")) {
			m->saveNextLabel = "";
			while (in.eof() != true) {

				input = new ListVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}
		}
		else if (format == "shared") {
			m->saveNextLabel = "";
			while (in.eof() != true) {

				input = new SharedListVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}

		}
		else if (format == "rabund") {

			while (in.eof() != true) {

				input = new RAbundVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}

		}
		else if (format == "order") {

			while (in.eof() != true) {

				input = new OrderVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}

		}
		else if (format == "sabund") {

			while (in.eof() != true) {

				input = new SAbundVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);

			}

		}

		in.close();

		output = new OrderVector();
		*output = (input->getOrderVector());

		return output;

	}
	else {
		return NULL;
	}
}

/***********************************************************************/
//this is used when you don't need the order vector
vector<SharedRAbundVector*> InputData::getSharedRAbundVectors() {
	if (fileHandle) {
		if (format == "sharedfile") {
			SharedRAbundVector* SharedRAbund = new SharedRAbundVector(fileHandle);
			if (SharedRAbund != NULL) {
				return SharedRAbund->getSharedRAbundVectors();
			}
		}
		else if (format == "shared") {
			SharedList = new SharedListVector(fileHandle);

			if (SharedList != NULL) {
				return SharedList->getSharedRAbundVector();
			}
		}
		File::gobble(fileHandle);
	}

	//this is created to signal to calling function that the input file is at eof
	vector<SharedRAbundVector*> null;  null.push_back(NULL);
	return null;

}
/***********************************************************************/
vector<SharedRAbundVector*> InputData::getSharedRAbundVectors(string label) {
	ifstream in;
	string  thisLabel;

	File::openInputFile(filename, in);
	m->saveNextLabel = "";

	if (in) {
		if (format == "sharedfile") {
			while (in.eof() != true) {

				SharedRAbundVector* SharedRAbund = new SharedRAbundVector(in);
				if (SharedRAbund != NULL) {
					thisLabel = SharedRAbund->getLabel();

					//if you are at the last label
					if (thisLabel == label) { in.close(); return SharedRAbund->getSharedRAbundVectors(); }
					else {
						//so you don't loose this memory
						vector<SharedRAbundVector*> lookup = SharedRAbund->getSharedRAbundVectors();
						for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
						delete SharedRAbund;
					}
				}
				else { break; }
				File::gobble(in);

			}
		}
		else if (format == "shared") {
			while (in.eof() != true) {

				SharedList = new SharedListVector(in);

				if (SharedList != NULL) {
					thisLabel = SharedList->getLabel();
					//if you are at the last label
					if (thisLabel == label) { in.close(); return SharedList->getSharedRAbundVector(); }
					else {
						//so you don't loose this memory
						delete SharedList;
					}
				}
				else { break; }
				File::gobble(in);

			}

		}
	}

	//this is created to signal to calling function that the input file is at eof
	vector<SharedRAbundVector*> null;  null.push_back(NULL);
	in.close();
	return null;

}

/***********************************************************************/
//this is used when you don't need the order vector
vector<SharedRAbundFloatVector*> InputData::getSharedRAbundFloatVectors() {
	if (fileHandle) {
		if (format == "relabund") {
			SharedRAbundFloatVector* SharedRelAbund = new SharedRAbundFloatVector(fileHandle);
			if (SharedRelAbund != NULL) {
				return SharedRelAbund->getSharedRAbundFloatVectors();
			}
		}
		else if (format == "sharedfile") {
			SharedRAbundVector* SharedRAbund = new SharedRAbundVector(fileHandle);
			if (SharedRAbund != NULL) {
				vector<SharedRAbundVector*> lookup = SharedRAbund->getSharedRAbundVectors();
				vector<SharedRAbundFloatVector*> lookupFloat = SharedRAbund->getSharedRAbundFloatVectors(lookup);
				for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } lookup.clear();
				return lookupFloat;
			}

		}
		File::gobble(fileHandle);
	}

	//this is created to signal to calling function that the input file is at eof
	vector<SharedRAbundFloatVector*> null;  null.push_back(NULL);
	return null;

}
/***********************************************************************/
vector<SharedRAbundFloatVector*> InputData::getSharedRAbundFloatVectors(string label) {
	ifstream in;
	string  thisLabel;

	File::openInputFile(filename, in);
	m->saveNextLabel = "";

	if (in) {
		if (format == "relabund") {
			while (in.eof() != true) {

				SharedRAbundFloatVector* SharedRelAbund = new SharedRAbundFloatVector(in);
				if (SharedRelAbund != NULL) {
					thisLabel = SharedRelAbund->getLabel();
					//if you are at the last label
					if (thisLabel == label) { in.close(); return SharedRelAbund->getSharedRAbundFloatVectors(); }
					else {
						//so you don't loose this memory
						vector<SharedRAbundFloatVector*> lookup = SharedRelAbund->getSharedRAbundFloatVectors();
						for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; }
						delete SharedRelAbund;
					}
				}
				else { break; }
				File::gobble(in);
			}
		}
		else if (format == "sharedfile") {
			while (in.eof() != true) {

				SharedRAbundVector* SharedRAbund = new SharedRAbundVector(in);
				if (SharedRAbund != NULL) {
					thisLabel = SharedRAbund->getLabel();

					//if you are at the last label
					if (thisLabel == label) {
						in.close();
						vector<SharedRAbundVector*> lookup = SharedRAbund->getSharedRAbundVectors();
						vector<SharedRAbundFloatVector*> lookupFloat = SharedRAbund->getSharedRAbundFloatVectors(lookup);
						for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } lookup.clear();
						return lookupFloat;
					}
					else {
						//so you don't loose this memory
						vector<SharedRAbundVector*> lookup = SharedRAbund->getSharedRAbundVectors();
						for (int i = 0; i < lookup.size(); i++) { delete lookup[i]; } lookup.clear();
						delete SharedRAbund;
					}
				}
				else { break; }
				File::gobble(in);
			}
		}
	}


	//this is created to signal to calling function that the input file is at eof
	vector<SharedRAbundFloatVector*> null;  null.push_back(NULL);
	in.close();
	return null;

}
/***********************************************************************/

SAbundVector* InputData::getSAbundVector() {
	if (fileHandle) {
		if (format == "list") {
			input = new ListVector(fileHandle);
		}
		else if (format == "shared") {
			input = new SharedListVector(fileHandle);
		}
		else if (format == "rabund") {
			input = new RAbundVector(fileHandle);
		}
		else if (format == "order") {
			input = new OrderVector(fileHandle);
		}
		else if (format == "sabund") {
			input = new SAbundVector(fileHandle);
		}
		File::gobble(fileHandle);

		sabund = new SAbundVector();
		*sabund = (input->getSAbundVector());

		return sabund;
	}
	else {
		return NULL;
	}
}
/***********************************************************************/
SAbundVector* InputData::getSAbundVector(string label) {

	ifstream in;
	string  thisLabel;
	File::openInputFile(filename, in);

	if (in) {
		if (format == "list") {
			m->saveNextLabel = "";
			while (in.eof() != true) {

				input = new ListVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}
		}
		else if (format == "shared") {
			m->saveNextLabel = "";
			while (in.eof() != true) {

				input = new SharedListVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}

		}
		else if (format == "rabund") {

			while (in.eof() != true) {

				input = new RAbundVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}

		}
		else if (format == "order") {

			while (in.eof() != true) {

				input = new OrderVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}

		}
		else if (format == "sabund") {

			while (in.eof() != true) {

				input = new SAbundVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);

			}

		}

		in.close();

		sabund = new SAbundVector();
		*sabund = (input->getSAbundVector());

		return sabund;

	}
	else {
		return NULL;
	}
}

/***********************************************************************/
RAbundVector* InputData::getRAbundVector() {
	if (fileHandle) {
		if (format == "list") {
			input = new ListVector(fileHandle);
		}
		else if (format == "shared") {
			input = new SharedListVector(fileHandle);
		}
		else if (format == "rabund") {
			input = new RAbundVector(fileHandle);
		}
		else if (format == "order") {
			input = new OrderVector(fileHandle);
		}
		else if (format == "sabund") {
			input = new SAbundVector(fileHandle);
		}

		File::gobble(fileHandle);

		rabund = new RAbundVector();
		*rabund = (input->getRAbundVector());

		return rabund;
	}
	else {
		return NULL;
	}
}
/***********************************************************************/
RAbundVector* InputData::getRAbundVector(string label) {

	ifstream in;
	string  thisLabel;
	File::openInputFile(filename, in);

	if (in) {
		if (format == "list") {
			m->saveNextLabel = "";

			while (in.eof() != true) {

				input = new ListVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}
		}
		else if (format == "shared") {
			m->saveNextLabel = "";

			while (in.eof() != true) {

				input = new SharedListVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}

		}
		else if (format == "rabund") {

			while (in.eof() != true) {

				input = new RAbundVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}

		}
		else if (format == "order") {

			while (in.eof() != true) {

				input = new OrderVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);
			}

		}
		else if (format == "sabund") {

			while (in.eof() != true) {

				input = new SAbundVector(in);
				thisLabel = input->getLabel();

				//if you are at the last label
				if (thisLabel == label) { break; }
				//so you don't loose this memory
				else { delete input; }
				File::gobble(in);

			}

		}


		in.close();

		rabund = new RAbundVector();
		*rabund = (input->getRAbundVector());

		return rabund;
	}
	else {
		return NULL;
	}
}

/***********************************************************************/



