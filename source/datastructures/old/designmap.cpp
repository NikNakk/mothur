//
//  designmap.cpp
//  Mothur
//
//  Created by SarahsWork on 6/17/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#include "designmap.h"

/************************************************************/
DesignMap::DesignMap(string file) {
	defaultClass = "not found";
	read(file);
}
/************************************************************/
int DesignMap::read(string file) {
	ifstream in;
	File::openInputFile(file, in);

	string temp = "";
	in >> temp; File::gobble(in);

	vector<string> columnHeaders;
	vector<string> tempColumnHeaders;
	if (temp == "group") {
		string headers = File::getline(in); File::gobble(in);
		columnHeaders = m->splitWhiteSpace(headers);
		columnHeaders.insert(columnHeaders.begin(), "group");
	}
	else {
		string headers = File::getline(in); File::gobble(in);
		tempColumnHeaders = m->splitWhiteSpace(headers);
		int num = tempColumnHeaders.size();
		columnHeaders.push_back("group");
		for (int i = 0; i < num; i++) { columnHeaders.push_back("value" + toString(i)); }
	}

	namesOfCategories.clear();
	indexCategoryMap.clear();
	indexGroupNameMap.clear();
	designMap.clear();
	map<int, string> originalGroupIndexes;
	for (int i = 1; i < columnHeaders.size(); i++) { namesOfCategories.push_back(columnHeaders[i]);  originalGroupIndexes[i - 1] = columnHeaders[i]; }
	if (columnHeaders.size() > 1) { defaultClass = columnHeaders[1]; }
	else {
		LOG(LOGERROR) << "Your design file contains only one column. Please correct." << '\n'; ctrlc_pressed = true;
	}

	//sort groups to keep consistent with how we store the groups in groupmap
	sort(namesOfCategories.begin(), namesOfCategories.end());
	for (int i = 0; i < namesOfCategories.size(); i++) { indexCategoryMap[namesOfCategories[i]] = i; }
	int numCategories = namesOfCategories.size();

	bool error = false;
	string group;
	totalCategories.resize(numCategories);
	int count = 0;

	//file without headers, fix it
	if (temp != "group") {
		group = temp;
		if (app.isDebug) { LOG(DEBUG) << "group = " + group + "\n"; }

		//if group info, then read it
		vector<string> categoryValues; categoryValues.resize(numCategories, "not found");
		for (int i = 0; i < numCategories; i++) {
			int thisIndex = indexCategoryMap[originalGroupIndexes[i]]; //find index of this category because we sort the values.
			string temp = tempColumnHeaders[i];
			categoryValues[thisIndex] = temp;

			if (app.isDebug) { LOG(DEBUG) << "value = " + temp + "\n"; }

			//do we have this value for this category already
			map<string, int>::iterator it = totalCategories[thisIndex].find(temp);
			if (it == totalCategories[thisIndex].end()) { totalCategories[thisIndex][temp] = 1; }
			else { totalCategories[thisIndex][temp]++; }
		}


		map<string, int>::iterator it = indexGroupNameMap.find(group);
		if (it == indexGroupNameMap.end()) {
			groups.push_back(group);
			indexGroupNameMap[group] = count;
			designMap.push_back(categoryValues);
			count++;
		}
		else {
			error = true;
			LOG(LOGERROR) << "Your design file contains more than 1 group named " + group + ", group names must be unique. Please correct." << '\n';
		}
	}

	while (!in.eof()) {

		if (ctrlc_pressed) { break; }

		in >> group; File::gobble(in);
		if (app.isDebug) { LOG(DEBUG) << "group = " + group + "\n"; }

		//if group info, then read it
		vector<string> categoryValues; categoryValues.resize(numCategories, "not found");
		for (int i = 0; i < numCategories; i++) {
			int thisIndex = indexCategoryMap[originalGroupIndexes[i]]; //find index of this category because we sort the values.
			string temp = "not found";
			in >> temp; categoryValues[thisIndex] = temp; File::gobble(in);

			if (app.isDebug) { LOG(DEBUG) << "value = " + temp + "\n"; }

			//do we have this value for this category already
			map<string, int>::iterator it = totalCategories[thisIndex].find(temp);
			if (it == totalCategories[thisIndex].end()) { totalCategories[thisIndex][temp] = 1; }
			else { totalCategories[thisIndex][temp]++; }
		}


		map<string, int>::iterator it = indexGroupNameMap.find(group);
		if (it == indexGroupNameMap.end()) {
			groups.push_back(group);
			indexGroupNameMap[group] = count;
			designMap.push_back(categoryValues);
			count++;
		}
		else {
			error = true;
			LOG(LOGERROR) << "Your design file contains more than 1 group named " + group + ", group names must be unique. Please correct." << '\n';
		}
	}
	in.close();

	if (error) { ctrlc_pressed = true; }

	return 0;
}
/************************************************************/
////groupName, returns default categories value.
string DesignMap::get(string groupName) {
	string value = "not found";

	map<string, int>::iterator it2 = indexGroupNameMap.find(groupName);
	if (it2 == indexGroupNameMap.end()) {
		LOG(LOGERROR) << "group " + groupName + " is not in your design file. Please correct.\n"; ctrlc_pressed = true;
	}
	else {
		return designMap[it2->second][indexCategoryMap[defaultClass]];
	}

	return value;
}
/************************************************************/
////groupName, returns default categories value.
vector<string> DesignMap::getCategory() {
	//oldStyle design file  group -> treatment. returns treatments
	set<string> uniqueNames;

	for (int i = 0; i < groups.size(); i++) { uniqueNames.insert(get(groups[i])); }

	vector<string> values;
	for (set<string>::iterator it = uniqueNames.begin(); it != uniqueNames.end(); it++) { values.push_back(*it); }

	return values;
}
/************************************************************/
////categoryName, returns category values.
vector<string> DesignMap::getCategory(string catName) {
	vector<string> values;

	map<string, int>::iterator it2 = indexCategoryMap.find(catName);
	if (it2 == indexCategoryMap.end()) {
		LOG(LOGERROR) << "category " + catName + " is not in your design file. Please correct.\n"; ctrlc_pressed = true;
	}
	else {
		for (map<string, int>::iterator it = totalCategories[it2->second].begin(); it != totalCategories[it2->second].end(); it++) {
			values.push_back(it->first);
		}
	}

	return values;
}

/************************************************************/
////groupName, category returns value. example F000132, sex -> male
string DesignMap::get(string groupName, string categoryName) {
	string value = "not found";
	map<string, int>::iterator it = indexCategoryMap.find(categoryName);
	if (it == indexCategoryMap.end()) {
		LOG(LOGERROR) << "category " + categoryName + " is not in your design file. Please correct.\n"; ctrlc_pressed = true;
	}
	else {
		map<string, int>::iterator it2 = indexGroupNameMap.find(groupName);
		if (it2 == indexGroupNameMap.end()) {
			LOG(LOGERROR) << "group " + groupName + " is not in your design file. Please correct.\n"; ctrlc_pressed = true;
		}
		else {
			return designMap[it2->second][it->second];
		}
	}
	return value;
}
/************************************************************/
//add group, assumes order is correct
int DesignMap::push_back(string group, vector<string> values) {
	map<string, int>::iterator it = indexGroupNameMap.find(group);
	if (it == indexGroupNameMap.end()) {
		if (values.size() != getNumCategories()) { LOG(LOGERROR) << "Your design file has a " + toString(getNumCategories()) + " categories and " + group + " has " + toString(values.size()) + ", please correct." << '\n'; ctrlc_pressed = true;  return 0; }

		for (int i = 0; i < values.size(); i++) {
			//do we have this value for this category already
			map<string, int>::iterator it = totalCategories[i].find(values[i]);
			if (it == totalCategories[i].end()) { totalCategories[i][values[i]] = 1; }
			else { totalCategories[i][values[i]]++; }
		}
		int count = indexGroupNameMap.size();
		indexGroupNameMap[group] = count;
		designMap.push_back(values);
	}
	else {
		LOG(LOGERROR) << "Your design file contains more than 1 group named " + group + ", group names must be unique. Please correct." << '\n'; ctrlc_pressed = true;
	}

	return 0;
}
/************************************************************/
//set values for group, does not need to set all values. assumes group is in table already
int DesignMap::setValues(string group, map<string, string> values) {
	map<string, int>::iterator it = indexGroupNameMap.find(group);
	if (it != indexGroupNameMap.end()) {
		for (map<string, string>::iterator it2 = values.begin(); it2 != values.end(); it2++) {

			map<string, int>::iterator it3 = indexCategoryMap.find(it2->first); //do we have this category
			if (it3 == indexCategoryMap.end()) {
				LOG(LOGERROR) << "Your design file does not contain a category called " + it2->first + ". Please correct." << '\n'; ctrlc_pressed = true;
			}
			else {
				string oldCategory = designMap[it->second][it3->second];
				//adjust totals for old category
				int oldCount = totalCategories[it3->second][oldCategory];
				if (oldCount == 1) { totalCategories[it3->second].erase(oldCategory); }
				else { totalCategories[it3->second][oldCategory]--; }

				designMap[it->second][it3->second] = it2->second; //reset value

				//adjust totals for new category
				map<string, int>::iterator it4 = totalCategories[it3->second].find(it2->second);
				if (it4 == totalCategories[it3->second].end()) { totalCategories[it3->second][it2->second] = 1; }
				else { totalCategories[it3->second][it2->second]++; }
			}
		}
	}
	else {
		LOG(LOGERROR) << "Your design file does not contain a group named " + group + ". Please correct." << '\n'; ctrlc_pressed = true;
	}

	return 0;
}
/************************************************************/
//set defaultclass
void DesignMap::setDefaultClass(string dClass) {
	if (m->inUsersGroups(dClass, namesOfCategories)) {
		defaultClass = dClass;
	}
	else {
		LOG(WARNING) << "Your design file does not contain a category named " + dClass + ". Using default class " + defaultClass + " .\n\n";
	}
}
/************************************************************/
//get number of groups belonging to a category or set of categories, with value or a set of values. Must have all categories and values. Example:
//  map<treatment - > early, late>, <sex -> male> would return 1. Only one group is male and from early or late.
int DesignMap::getNumUnique(map<string, vector<string> > selected) {
	int num = 0;

	map<int, vector<string> > indexes;
	for (map<string, vector<string> >::iterator it = selected.begin(); it != selected.end(); it++) {
		map<string, int>::iterator it2 = indexCategoryMap.find(it->first);
		if (it2 == indexCategoryMap.end()) {
			LOG(LOGERROR) << "Your design file does not contain a category named " + it->first + ". Please correct." << '\n'; ctrlc_pressed = true; return 0;
		}
		else {
			indexes[it2->second] = it->second;
		}
	}

	for (int j = 0; j < designMap.size(); j++) {
		bool hasAll = true; //innocent til proven guilty
		for (map<int, vector<string> >::iterator it = indexes.begin(); it != indexes.end(); it++) {
			//column number is it->first
			if (!m->inUsersGroups(designMap[j][it->first], it->second)) { hasAll = false; }
		}
		if (hasAll) { num++; }
	}

	return num;
}
/************************************************************/
//get number of groups belonging to a category or set of categories, with value or a set of values. Must have at least one categories and values. Example:
//  map<treatment - > early, late>, <sex -> male> would return 3. All three group have are either male or from early or late.
int DesignMap::getNumShared(map<string, vector<string> > selected) {
	int num = 0;

	map<int, vector<string> > indexes;
	for (map<string, vector<string> >::iterator it = selected.begin(); it != selected.end(); it++) {
		map<string, int>::iterator it2 = indexCategoryMap.find(it->first);
		if (it2 == indexCategoryMap.end()) {
			LOG(LOGERROR) << "Your design file does not contain a category named " + it->first + ". Please correct." << '\n'; ctrlc_pressed = true; return 0;
		}
		else {
			indexes[it2->second] = it->second;
		}
	}

	for (int j = 0; j < designMap.size(); j++) {
		bool hasAny = false; //innocent til proven guilty
		for (map<int, vector<string> >::iterator it = indexes.begin(); it != indexes.end(); it++) {
			//column number is it->first
			if (m->inUsersGroups(designMap[j][it->first], it->second)) { hasAny = true; }
		}
		if (hasAny) { num++; }
	}

	return num;
}

/************************************************************/
//get names of groups belonging to a category or set of categories, with value or a set of values. Must have all categories and values. Example:
//  map<treatment - > early, late>, <sex -> male> would return F000132. F000132 is the only group which is male and from early or late.
vector<string> DesignMap::getNamesUnique(map<string, vector<string> > selected) {
	vector<string> names;

	map<int, vector<string> > indexes;
	for (map<string, vector<string> >::iterator it = selected.begin(); it != selected.end(); it++) {
		map<string, int>::iterator it2 = indexCategoryMap.find(it->first);
		if (it2 == indexCategoryMap.end()) {
			LOG(LOGERROR) << "Your design file does not contain a category named " + it->first + ". Please correct." << '\n'; ctrlc_pressed = true; return names;
		}
		else {
			indexes[it2->second] = it->second;
		}
	}

	//map int to name
	map<int, string> reverse;
	for (map<string, int>::iterator it = indexGroupNameMap.begin(); it != indexGroupNameMap.end(); it++) {
		reverse[it->second] = it->first;
	}

	for (int j = 0; j < designMap.size(); j++) {
		bool hasAll = true; //innocent til proven guilty
		for (map<int, vector<string> >::iterator it = indexes.begin(); it != indexes.end(); it++) {
			//column number is it->first
			if (!m->inUsersGroups(designMap[j][it->first], it->second)) { hasAll = false; }
		}
		if (hasAll) {
			map<int, string>::iterator it = reverse.find(j);
			if (it == reverse.end()) {
				LOG(LOGERROR) << "should never get here, oops. Please correct." << '\n'; ctrlc_pressed = true; return names;
			}
			else { names.push_back(it->second); }
		}
	}

	return names;
}
/************************************************************/
//get names of groups belonging to a category or set of categories, with value or a set of values. Must have all categories and values. Example:
//  map<treatment - > early, late>, <sex -> male> would return F000132. F000132 is the only group which is male and from early or late.
vector<string> DesignMap::getNamesShared(map<string, vector<string> > selected) {
	vector<string> names;

	map<int, vector<string> > indexes;
	for (map<string, vector<string> >::iterator it = selected.begin(); it != selected.end(); it++) {
		map<string, int>::iterator it2 = indexCategoryMap.find(it->first);
		if (it2 == indexCategoryMap.end()) {
			LOG(LOGERROR) << "Your design file does not contain a category named " + it->first + ". Please correct." << '\n'; ctrlc_pressed = true; return names;
		}
		else {
			indexes[it2->second] = it->second;
		}
	}

	//map int to name
	map<int, string> reverse;
	for (map<string, int>::iterator it = indexGroupNameMap.begin(); it != indexGroupNameMap.end(); it++) {
		reverse[it->second] = it->first;
	}

	for (int j = 0; j < designMap.size(); j++) {
		bool hasAny = false; //innocent til proven guilty
		for (map<int, vector<string> >::iterator it = indexes.begin(); it != indexes.end(); it++) {
			//column number is it->first
			if (m->inUsersGroups(designMap[j][it->first], it->second)) { hasAny = true; }
		}

		if (hasAny) {
			map<int, string>::iterator it = reverse.find(j);
			if (it == reverse.end()) {
				LOG(LOGERROR) << "should never get here, oops. Please correct." << '\n'; ctrlc_pressed = true; return names;
			}
			else { names.push_back(it->second); }
		}
	}

	return names;
}

/************************************************************/
//get names of groups belonging to a category or set of categories, with value or a set of values. Must have at least one categories and values. Example:
//  map<treatment - > early, late>, <sex -> male> would return F000132, F000142, F000138. All three group have are either male or from early or late.

vector<string> DesignMap::getNamesGroups(string category, string value) {
	vector<string> names;

	map<string, int>::iterator it = indexCategoryMap.find(category);
	if (it == indexCategoryMap.end()) {
		LOG(LOGERROR) << "category " + category + " is not in your design file. Please correct.\n"; ctrlc_pressed = true;
	}
	else {
		int column = it->second;

		//map int to name
		map<int, string> reverse;
		for (map<string, int>::iterator it2 = indexGroupNameMap.begin(); it2 != indexGroupNameMap.end(); it2++) {
			reverse[it2->second] = it2->first;
		}

		for (int i = 0; i < designMap.size(); i++) {
			if (designMap[i][column] == value) {
				map<int, string>::iterator it2 = reverse.find(i);
				if (it2 == reverse.end()) {
					LOG(LOGERROR) << "should never get here, oops. Please correct." << '\n'; ctrlc_pressed = true; return names;
				}
				else { names.push_back(it2->second); }
			}
		}
	}
	return names;

}
/************************************************************/
//assume default category and get names groups that match any values in vector passed in.  <early, late> = F000142, F000132.

vector<string> DesignMap::getNamesGroups(vector<string> sets) {
	vector<string> names;

	if (sets.size() == 0) { return names; }

	map<string, vector<string> > temp;

	temp[defaultClass] = sets;

	names = getNamesShared(temp);

	return names;

}

/************************************************************/
int DesignMap::print(ofstream& out) {

	out << "group";
	for (int i = 0; i < namesOfCategories.size(); i++) { out << '\t' << namesOfCategories[i]; }
	out << endl;

	map<int, string> reverse; //use this to preserve order
	for (map<string, int>::iterator it = indexGroupNameMap.begin(); it != indexGroupNameMap.end(); it++) { reverse[it->second] = it->first; }

	for (int i = 0; i < designMap.size(); i++) {
		map<int, string>::iterator itR = reverse.find(i);

		if (itR != reverse.end()) { //will equal end if seqs were removed because remove just removes from indexNameMap
			out << itR->second;

			for (int j = 0; j < namesOfCategories.size(); j++) {
				out << '\t' << designMap[i][j];
			}
			out << endl;
		}
	}
	out.close();

	return 0;
}
/************************************************************/
//print specific categories
int DesignMap::printCategories(ofstream& out, vector<string> cats) {

	out << "group";
	for (int i = 0; i < namesOfCategories.size(); i++) { if (m->inUsersGroups(namesOfCategories[i], cats)) { out << '\t' << namesOfCategories[i]; } }
	out << endl;

	map<int, string> reverse; //use this to preserve order
	for (map<string, int>::iterator it = indexGroupNameMap.begin(); it != indexGroupNameMap.end(); it++) { reverse[it->second] = it->first; }

	for (int i = 0; i < designMap.size(); i++) {
		map<int, string>::iterator itR = reverse.find(i);

		if (itR != reverse.end()) { //will equal end if seqs were removed because remove just removes from indexNameMap
			out << itR->second;

			for (int j = 0; j < namesOfCategories.size(); j++) {
				if (m->inUsersGroups(namesOfCategories[i], cats)) {
					out << '\t' << designMap[i][j];
				}
			}
			out << endl;
		}
	}
	out.close();

	return 0;
}
/************************************************************/
//print specific groups
int DesignMap::printGroups(ofstream& out, vector<string> groups) {
	int numSelected = 0;

	out << "group";
	for (int i = 0; i < namesOfCategories.size(); i++) { out << '\t' << namesOfCategories[i]; }
	out << endl;

	map<int, string> reverse; //use this to preserve order
	for (map<string, int>::iterator it = indexGroupNameMap.begin(); it != indexGroupNameMap.end(); it++) { reverse[it->second] = it->first; }

	for (int i = 0; i < designMap.size(); i++) {
		map<int, string>::iterator itR = reverse.find(i);

		if (itR != reverse.end()) { //will equal end if groups were removed because remove just removes from indexNameMap

			if (m->inUsersGroups(itR->second, groups)) {
				out << itR->second;

				for (int j = 0; j < namesOfCategories.size(); j++) {
					out << '\t' << designMap[i][j];
				}
				out << endl;

				numSelected++;
			}
		}
	}

	out.close();

	return numSelected;
}

/************************************************************/

