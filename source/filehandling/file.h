#ifndef file_h
#define file_h

#include <string>
using namespace std;

class File {
public:
	static bool FileExists(const string& name);
	static string getRootName(string longName);
	static string getSimpleName(string longName);
	static string getPath(string longName);
	static string getFullPathName(string fileName);
};
#endif
