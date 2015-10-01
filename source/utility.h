#ifndef utility_h
#define utility_h

#include <vector>
#include <string>
#include "mothur.h"

using namespace std;

class Utility {
public:
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	static std::set<std::string>& split(const std::string & s, char delim, std::set<std::string>& elems);
	static std::vector<std::string> split(const std::string &s, char delim);
	static std::string join(std::vector<std::string> &sv, const std::string& delim);
	static std::vector<char*>& to_c_strs(std::vector<std::string>& sv, std::vector<char*>& output);
	static bool mothurConvert(string item, int & num);
	static bool mothurConvert(string item, intDist & num);
	static bool isNumeric1(string stringToCheck);
	static bool isInteger(string stringToCheck);
	static bool containsAlphas(string stringToCheck);
	static bool mothurConvert(string item, float & num);
	static bool mothurConvert(string item, double & num);
	static vector<vector<double>> binomial(int maxOrder);
	static unsigned int fromBase36(string base36);
	static string findEdianness();
	static double median(vector<double> x);
	static double median(vector<int> x);
	static int average(vector<int> x);
	static int sum(vector<int> x);
	static int factorial(int num);
	static bool anyLabelsToProcess(string label, set<string>& userLabels, string errorOff);
};
#endif
