#ifndef utility_h
#define utility_h

#include <vector>
#include <string>

class Utility {
public:
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	static std::vector<std::string> split(const std::string &s, char delim);
	static std::string join(std::vector<std::string> &sv, const std::string& delim);
	static std::vector<char*>& to_c_strs(std::vector<std::string>& sv, std::vector<char*>& output);
};
#endif
