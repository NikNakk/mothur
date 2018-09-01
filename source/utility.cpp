#include "utility.h"
#include "g3log\g3log.hpp"
#include <sstream>
#include <iterator>
#include <vector>
#include <map>
#include <algorithm>

// Split taken from http://stackoverflow.com/a/236803/4998761

std::set<std::string>& Utility::split(const std::string &s, char delim, std::set<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.insert(item);
	}
	return elems;
}


std::vector<std::string> Utility::split(const std::string &s, char delim, bool trimWhiteSpace) {
	std::vector<std::string> elems;
	std::istringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if (trimWhiteSpace) {
			elems.push_back(trim(item));
		}
		else {
			elems.push_back(item);
		}
	}
	return elems;
}

// Taken from http://stackoverflow.com/a/1798170/4998761
std::string Utility::trim(const std::string& str, const std::string& whitespace)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::vector<char*>& Utility::to_c_strs(std::vector<std::string>& sv, std::vector<char*>& output) {
	for (std::vector<std::string>::iterator it = sv.begin(); it != sv.end(); ++it) {
		output.push_back(const_cast<char*>(it->c_str()));
	}
	return output;
}

bool Utility::mothurConvert(std::string item, int& num) {
	bool error = false;

	if (isNumeric1(item)) {
		convert(item, num);
	}
	else {
		num = 0;
		error = true;
		LOG(LOGERROR) << "cannot convert " + item + " to an integer.";
		//    commandInputsConvertError = true;
	}

	return error;
}
/***********************************************************************/
bool Utility::mothurConvert(std::string item, intDist& num) {
	bool error = false;

	if (isNumeric1(item)) {
		num = static_cast<short>(std::stoi(item));
	}
	else {
		num = 0;
		error = true;
		LOG(LOGERROR) << "cannot convert " + item + " to an integer.";
		//    commandInputsConvertError = true;
	}

	return error;
}

/***********************************************************************/
bool Utility::isNumeric1(std::string stringToCheck) {
	bool numeric = false;

	if (stringToCheck == "") { numeric = false; }
	else if (stringToCheck.find_first_not_of("0123456789.-") == std::string::npos) { numeric = true; }

	return numeric;

}
/***********************************************************************/
bool Utility::isInteger(std::string stringToCheck) {
	bool isInt = false;

	if (stringToCheck.find_first_not_of("0123456789-") == std::string::npos) { isInt = true; }

	return isInt;

}
/***********************************************************************/
bool Utility::containsAlphas(std::string stringToCheck) {
	bool containsAlpha = false;

	if (stringToCheck.find_first_of("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOopPQqRrSsTtUuVvWwXxYyZz") != std::string::npos) { containsAlpha = true; }

	return containsAlpha;

}
/***********************************************************************/
bool Utility::mothurConvert(std::string item, float& num) {
	bool error = false;

	if (isNumeric1(item)) {
		convert(item, num);
	}
	else {
		num = 0;
		error = true;
		LOG(LOGERROR) << "cannot convert " + item + " to a float.";
		//    commandInputsConvertError = true;
	}

	return error;
}
/***********************************************************************/
bool Utility::mothurConvert(std::string item, double& num) {
	bool error = false;

	if (isNumeric1(item)) {
		convert(item, num);
	}
	else {
		num = 0;
		error = true;
		LOG(LOGERROR) << "cannot convert " + item + " to a double.";
		//    commandInputsConvertError = true;
	}

	return error;
}
/**************************************************************************************************/

std::vector<std::vector<double>> Utility::binomial(int maxOrder) {
	std::vector<std::vector<double>> binomial(maxOrder + 1);

	for (int i = 0;i <= maxOrder;i++) {
		binomial[i].resize(maxOrder + 1);
		binomial[i][0] = 1;
		binomial[0][i] = 0;
	}
	binomial[0][0] = 1;

	binomial[1][0] = 1;
	binomial[1][1] = 1;

	for (int i = 2;i <= maxOrder;i++) {
		binomial[1][i] = 0;
	}

	for (int i = 2;i <= maxOrder;i++) {
		for (int j = 1;j <= maxOrder;j++) {
			if (i == j) { binomial[i][j] = 1; }
			if (j > i) { binomial[i][j] = 0; }
			else { binomial[i][j] = binomial[i - 1][j - 1] + binomial[i - 1][j]; }
		}
	}

	return binomial;

}
/**************************************************************************************************/
unsigned int Utility::fromBase36(std::string base36) {
	unsigned int num = 0;

	std::map<char, int> converts = {
	{'A', 0},
	{'a', 0},
	{'B', 1},
	{'b', 1},
	{'C', 2},
	{'c', 2},
	{'D', 3},
	{'d', 3},
	{'E', 4},
	{'e', 4},
	{'F', 5},
	{'f', 5},
	{'G', 6},
	{'g', 6},
	{'H', 7},
	{'h', 7},
	{'I', 8},
	{'i', 8},
	{'J', 9},
	{'j', 9},
	{'K', 10},
	{'k', 10},
	{'L', 11},
	{'l', 11},
	{'M', 12},
	{'m', 12},
	{'N', 13},
	{'n', 13},
	{'O', 14},
	{'o', 14},
	{'P', 15},
	{'p', 15},
	{'Q', 16},
	{'q', 16},
	{'R', 17},
	{'r', 17},
	{'S', 18},
	{'s', 18},
	{'T', 19},
	{'t', 19},
	{'U', 20},
	{'u', 20},
	{'V', 21},
	{'v', 21},
	{'W', 22},
	{'w', 22},
	{'X', 23},
	{'x', 23},
	{'Y', 24},
	{'y', 24},
	{'Z', 25},
	{'z', 25},
	{'0', 26},
	{'1', 27},
	{'2', 28},
	{'3', 29},
	{'4', 30},
	{'5', 31},
	{'6', 32},
	{'7', 33},
	{'8', 34},
	{'9', 35} };

	int i = 0;
	while (i < base36.length()) {
		char c = base36[i];
		num = 36 * num + converts[c];
		i++;
	}

	return num;

}
/***********************************************************************/
std::string  Utility::findEdianness() {
	// find real endian type
	std::string endianType = "unknown";
	int num = 1;
	if (*(char *)&num == 1)
	{
		endianType = "LITTLE_ENDIAN";
	}
	else
	{
		endianType = "BIG_ENDIAN";
	}
	return endianType;
}
/***********************************************************************/
int Utility::average(std::vector<int> x) {
	int value = 0;

	for (int i = 0; i < x.size() && !ctrlc_pressed; i++) {
		value += x[i];
	}

	return static_cast<int>(value / static_cast<int>(x.size()));
}
/***********************************************************************/
int  Utility::sum(std::vector<int> x) {
	int value = 0;

	for (int i = 0; i < x.size() && !ctrlc_pressed; i++) {
		value += x[i];
	}

	return value;
}
/***********************************************************************/
int Utility::factorial(int num) {
	int total = 1;

	for (int i = 1; i <= num; i++) {
		total *= i;
	}

	return total;
}
/***********************************************************************/

//this function determines if the user has given us labels that are smaller than the given label.
//if so then it returns true so that the calling function can run the previous valid distance.
//it's a "smart" distance function.  It also checks for invalid labels.
bool Utility::anyLabelsToProcess(std::string label, std::set<std::string>& userLabels, std::string errorOff) {

	std::set<std::string>::iterator it;
	std::vector<float> orderFloat;
	std::map<std::string, float> userMap;  //the conversion process removes trailing 0's which we need to put back
	std::map<std::string, float>::iterator it2;
	float labelFloat;
	bool smaller = false;

	//unique is the smallest line
	if (label == "unique") { return false; }
	else {
		if (convertTestFloat(label, labelFloat)) {
			convert(label, labelFloat);
		}
		else { //cant convert 
			return false;
		}
	}

	//go through users set and make them floats
	for (it = userLabels.begin(); it != userLabels.end();) {

		float temp;
		if ((*it != "unique") && (convertTestFloat(*it, temp) == true)) {
			convert(*it, temp);
			orderFloat.push_back(temp);
			userMap[*it] = temp;
			++it;
		}
		else if (*it == "unique") {
			orderFloat.push_back(-1.0);
			userMap["unique"] = -1.0;
			++it;
		}
		else {
			if (errorOff == "") { LOG(LOGERROR) << *it << " is not a valid label."; }
			userLabels.erase(++it);
		}
	}

	//sort order
	sort(orderFloat.begin(), orderFloat.end());

	/*************************************************/
	//is this label bigger than any of the users labels
	/*************************************************/

	//loop through order until you find a label greater than label
	for (int i = 0; i < orderFloat.size(); i++) {
		if (orderFloat[i] < labelFloat) {
			smaller = true;
			if (orderFloat[i] == -1) {
				if (errorOff == "") { LOG(INFO) << "Your file does not include the label unique."; }
				userLabels.erase("unique");
			}
			else {
				std::string s = "";
				for (it2 = userMap.begin(); it2 != userMap.end(); it2++) {
					if (it2->second == orderFloat[i]) {
						s = it2->first;
						//remove small labels
						userLabels.erase(s);
						break;
					}
				}
				if (errorOff == "") { LOG(INFO) << "Your file does not include the label " << (s + ". I will use the next smallest distance. "); }
			}
			//since they are sorted once you find a bigger one stop looking
		}
		else { break; }
	}

	return smaller;

}
