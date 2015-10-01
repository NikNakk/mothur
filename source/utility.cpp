#include "utility.h"
#include <sstream>
#include <iterator>

// Split taken from http://stackoverflow.com/a/236803/4998761

std::vector<std::string>& Utility::split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::set<std::string>& Utility::split(const std::string &s, char delim, std::set<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.insert(item);
	}
	return elems;
}


std::vector<std::string> Utility::split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

std::string Utility::join(std::vector<std::string>& sv, const std::string& delim)
{
	std::ostringstream ss;
	for (std::vector<std::string>::iterator it = sv.begin(); it != sv.end(); it++) {
		ss << *it << delim;
	}
	std::string output;
	output = ss.str();
	if (output != "") {
		output = output.substr(0, output.length() - delim.length());
	}
	return output;
}

std::vector<char*>& Utility::to_c_strs(std::vector<std::string>& sv, std::vector<char*>& output) {
	for (std::vector<std::string>::iterator it = sv.begin(); it != sv.end(); it++) {
		output.push_back(const_cast<char*>(it->c_str()));
	}
	return output;
}

bool Utility::mothurConvert(string item, int& num) {
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
bool Utility::mothurConvert(string item, intDist& num) {
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
bool Utility::isNumeric1(string stringToCheck) {
	bool numeric = false;

	if (stringToCheck == "") { numeric = false; }
	else if (stringToCheck.find_first_not_of("0123456789.-") == string::npos) { numeric = true; }

	return numeric;

}
/***********************************************************************/
bool Utility::isInteger(string stringToCheck) {
	bool isInt = false;

	if (stringToCheck.find_first_not_of("0123456789-") == string::npos) { isInt = true; }

	return isInt;

}
/***********************************************************************/
bool Utility::containsAlphas(string stringToCheck) {
	bool containsAlpha = false;

	if (stringToCheck.find_first_of("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOopPQqRrSsTtUuVvWwXxYyZz") != string::npos) { containsAlpha = true; }

	return containsAlpha;

}
/***********************************************************************/
bool Utility::mothurConvert(string item, float& num) {
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
bool Utility::mothurConvert(string item, double& num) {
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

vector<vector<double> > Utility::binomial(int maxOrder) {
	vector<vector<double> > binomial(maxOrder + 1);

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
unsigned int Utility::fromBase36(string base36) {
	unsigned int num = 0;

	map<char, int> converts;
	converts['A'] = 0;
	converts['a'] = 0;
	converts['B'] = 1;
	converts['b'] = 1;
	converts['C'] = 2;
	converts['c'] = 2;
	converts['D'] = 3;
	converts['d'] = 3;
	converts['E'] = 4;
	converts['e'] = 4;
	converts['F'] = 5;
	converts['f'] = 5;
	converts['G'] = 6;
	converts['g'] = 6;
	converts['H'] = 7;
	converts['h'] = 7;
	converts['I'] = 8;
	converts['i'] = 8;
	converts['J'] = 9;
	converts['j'] = 9;
	converts['K'] = 10;
	converts['k'] = 10;
	converts['L'] = 11;
	converts['l'] = 11;
	converts['M'] = 12;
	converts['m'] = 12;
	converts['N'] = 13;
	converts['n'] = 13;
	converts['O'] = 14;
	converts['o'] = 14;
	converts['P'] = 15;
	converts['p'] = 15;
	converts['Q'] = 16;
	converts['q'] = 16;
	converts['R'] = 17;
	converts['r'] = 17;
	converts['S'] = 18;
	converts['s'] = 18;
	converts['T'] = 19;
	converts['t'] = 19;
	converts['U'] = 20;
	converts['u'] = 20;
	converts['V'] = 21;
	converts['v'] = 21;
	converts['W'] = 22;
	converts['w'] = 22;
	converts['X'] = 23;
	converts['x'] = 23;
	converts['Y'] = 24;
	converts['y'] = 24;
	converts['Z'] = 25;
	converts['z'] = 25;
	converts['0'] = 26;
	converts['1'] = 27;
	converts['2'] = 28;
	converts['3'] = 29;
	converts['4'] = 30;
	converts['5'] = 31;
	converts['6'] = 32;
	converts['7'] = 33;
	converts['8'] = 34;
	converts['9'] = 35;

	int i = 0;
	while (i < base36.length()) {
		char c = base36[i];
		num = 36 * num + converts[c];
		i++;
	}

	return num;

}
/***********************************************************************/
string  Utility::findEdianness() {
	// find real endian type
	string endianType = "unknown";
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
double  Utility::median(vector<double> x) {
	double value = 0.0;

	if (x.size() == 0) {} //error
	else {
		//For example, if a < b < c, then the median of the list {a, b, c} is b, and, if a < b < c < d, then the median of the list {a, b, c, d} is the mean of b and c; i.e., it is (b + c)/2.
		sort(x.begin(), x.end());
		//is x.size even?
		if ((x.size() % 2) == 0) { //size() is even. median = average of 2 midpoints
			int midIndex1 = (x.size() / 2) - 1;
			int midIndex2 = (x.size() / 2);
			value = (x[midIndex1] + x[midIndex2]) / 2.0;
		}
		else {
			int midIndex = (x.size() / 2);
			value = x[midIndex];
		}
	}
	return value;
}
/***********************************************************************/
int  Utility::median(vector<int> x) {
	double value = 0;

	if (x.size() == 0) {} //error
	else {
		//For example, if a < b < c, then the median of the list {a, b, c} is b, and, if a < b < c < d, then the median of the list {a, b, c, d} is the mean of b and c; i.e., it is (b + c)/2.
		sort(x.begin(), x.end());
		//is x.size even?
		if ((x.size() % 2) == 0) { //size() is even. median = average of 2 midpoints
			int midIndex1 = (x.size() / 2) - 1;
			int midIndex2 = (x.size() / 2);
			value = (x[midIndex1] + x[midIndex2]) / 2.0;
		}
		else {
			int midIndex = (x.size() / 2);
			value = x[midIndex];
		}
	}
	return (int)value;
}
/***********************************************************************/
int  Utility::average(vector<int> x) {
	int value = 0;

	for (int i = 0; i < x.size(); i++) {
		if (control_pressed) { break; }
		value += x[i];
	}

	return ((int)value / x.size());
}
/***********************************************************************/
int  Utility::sum(vector<int> x) {
	int value = 0;

	Application& app = Application::getApplication();

	for (int i = 0; i < x.size(); i++) {
		if (ctrlc_pressed) { break; }
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
bool Utility::anyLabelsToProcess(string label, set<string>& userLabels, string errorOff) {

	set<string>::iterator it;
	vector<float> orderFloat;
	map<string, float> userMap;  //the conversion process removes trailing 0's which we need to put back
	map<string, float>::iterator it2;
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
			it++;
		}
		else if (*it == "unique") {
			orderFloat.push_back(-1.0);
			userMap["unique"] = -1.0;
			it++;
		}
		else {
			if (errorOff == "") { mothurOut(*it + " is not a valid label."); mothurOutEndLine(); }
			userLabels.erase(it++);
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
				string s = "";
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
