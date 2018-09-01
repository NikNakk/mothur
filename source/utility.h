#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include "mothurdefs.h"

template<typename T, typename U>
struct MeanWithCentiles {
public:
	double mean = 0;
	std::vector<U> centilePositions;
	std::vector<T> centileValues;
};

namespace Utility {
	std::set<std::string>& split(const std::string & s, char delim, std::set<std::string>& elems);
	std::vector<std::string> split(const std::string &s, char delim, bool trimWhiteSpace = false);
	std::string trim(const std::string & str, const std::string & whitespace = " \t");
	std::vector<char*>& to_c_strs(std::vector<std::string>& sv, std::vector<char*>& output);
	bool mothurConvert(std::string item, float & num);
	bool mothurConvert(std::string item, double & num);
	bool mothurConvert(std::string item, int & num);
	bool mothurConvert(std::string item, intDist & num);
	bool isNumeric1(std::string stringToCheck);
	bool isInteger(std::string stringToCheck);
	bool containsAlphas(std::string stringToCheck);
	std::vector<std::vector<double>> binomial(int maxOrder);
	unsigned int fromBase36(std::string base36);
	std::string findEdianness();
	int average(std::vector<int> x);
	int sum(std::vector<int> x);
	int factorial(int num);
	bool anyLabelsToProcess(std::string label, std::set<std::string>& userLabels, std::string errorOff);

	template<typename T>
	std::string join(const std::vector<T> &vec, const std::string& delim) {
		std::ostringstream ss;
		for (auto it = vec.begin(); it != vec.end(); ++it) {
			ss << *it << delim;
		}
		std::string output;
		output = ss.str();
		if (output != "") {
			output = output.substr(0, output.length() - delim.length());
		}
		return output;
	}

	template<typename Iter>
	std::string join(Iter it, Iter end, const std::string& delim) {
		std::ostringstream ss;
		for (; it != end; ++it) {
			ss << *it << delim;
		}
		std::string output;
		output = ss.str();
		if (output != "") {
			output = output.substr(0, output.length() - delim.length());
		}
		return output;
	}

	template<typename T>
	T median(std::vector<T> x) {
		double value = 0.0;

		if (x.size() == 0) {} //error
		else {
			//For example, if a < b < c, then the median of the list {a, b, c} is b, and, if a < b < c < d, then the median of the list {a, b, c, d} is the mean of b and c; i.e., it is (b + c)/2.
			sort(x.begin(), x.end());
			//is x.size even?
			if ((x.size() % 2) == 0) { //size() is even. median = average of 2 midpoints
				size_t midIndex1 = (x.size() / 2) - 1;
				size_t midIndex2 = (x.size() / 2);
				value = (x[midIndex1] + x[midIndex2]) / 2.0;
			}
			else {
				size_t midIndex = (x.size() / 2);
				value = x[midIndex];
			}
		}
		return static_cast<T>value;
	}

	template<typename T, typename U>
	std::map<T, U>& addToMap(std::map<T, U>& currentMap, const std::map<T, U>& newMap)
	{
		for (auto it = newMap.begin(); it != newMap.end(); ++it) {
			currentMap[it->first] += it->second;
		}
		return currentMap;
	}

	template<typename T, typename U>
	MeanWithCentiles<T, U> findCentiles(const std::map<T, U>& map, const std::vector<double>& centiles, U numSeqs)
	{
		MeanWithCentiles<T, U> result;
		auto mapIt = map.begin();
		U totalCount = 0;
		U totalSum = 0;
		T previousValue = 0;
		if (mapIt != map.end()) {
			previousValue = mapIt->first;
		}
		for (vector<double>::const_iterator cenIt = centiles.begin(); cenIt != centiles.end(); ++cenIt) {
			U scaledCentile = static_cast<U>(*cenIt * numSeqs);
			if (*cenIt != 0 && *cenIt != 1) { scaledCentile++; }
			for ( ;totalCount < scaledCentile && mapIt != map.end(); ++mapIt) {
				previousValue = mapIt->first;
				totalCount += mapIt->second;
				totalSum += mapIt->first * mapIt->second;
			}
			result.centilePositions.push_back(scaledCentile);
			result.centileValues.push_back(previousValue);
		}
		result.mean = static_cast<double>(totalSum) / numSeqs;
		return result;
	}

	template<typename T, typename U>
	T findCentile(const std::map<T, U>& map, double centile, U numSeqs)
	{
		auto mapIt = map.begin();
		T previousValue = 0;
		if (mapIt != map.end()) {
			previousValue = mapIt->first;
		}
		U scaledCentile = static_cast<U>(centile * numSeqs);
		if (centile != 0.0 && centile != 1.0) { scaledCentile++; }
		for (;totalCount < scaledCentile && mapIt != map.end(); ++mapIt) {
			previousValue = mapIt->first;
			totalCount += mapIt->second;
		}
		return previousValue;
	}

};


