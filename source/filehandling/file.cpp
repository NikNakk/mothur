#include "file.h"
#include <fstream>
#include "mothur.h"

using namespace std;

bool File::FileExists(const string& name)
{
	ifstream f(name.c_str());
	bool isGood = f.good();
	f.close();
	return isGood;
}

#if defined (UNIX)
#ifdef USE_COMPRESSION
inline bool endsWith(string s, const char * suffix) {
	size_t suffixLength = strlen(suffix);
	return s.size() >= suffixLength && s.substr(s.size() - suffixLength, suffixLength).compare(suffix) == 0;
}
#endif
#endif

string File::getRootName(string longName) {
	try {

		string rootName = longName;

#if defined (UNIX)
#ifdef USE_COMPRESSION
		if (endsWith(rootName, ".gz") || endsWith(rootName, ".bz2")) {
			int pos = rootName.find_last_of('.');
			rootName = rootName.substr(0, pos);
			cerr << "shortening " << longName << " to " << rootName << "\n";
		}
#endif
#endif
		if (rootName.find_last_of(".") != rootName.npos) {
			int pos = rootName.find_last_of('.') + 1;
			rootName = rootName.substr(0, pos);
		}

		return rootName;
	}
	catch (exception& e) {
		//errorOut(e, "MothurOut", "getRootName");
		//exit(1);
		throw;
	}
}
/***********************************************************************/

string File::getSimpleName(string longName) {
	try {
		string simpleName = longName;

		size_t found;
		found = longName.find_last_of("/\\");

		if (found != longName.npos) {
			simpleName = longName.substr(found + 1);
		}

		return simpleName;
	}
	catch (exception& e) {
		// errorOut(e, "MothurOut", "getSimpleName");
		// exit(1);
		throw;
	}
}

string File::getPath(string longName) {
	string path = "";

	size_t found;
	found = longName.find_last_of("~/\\");

	if (found != longName.npos) {
		path = longName.substr(0, found + 1);
	}

	return path;

}


/***********************************************************************/


string File::getFullPathName(string fileName) {
	try {

		string path = getPath(fileName);
		string newFileName;
		int pos;

		if (path == "") { return fileName; } //its a simple name
		else { //we need to complete the pathname
			   // ex. ../../../filename 
			   // cwd = /user/work/desktop

			string cwd;
			//get current working directory 
#if defined (UNIX)	

			if (path.find("~") != -1) { //go to home directory
				string homeDir;

				char *homepath = NULL;
				homepath = getenv("HOME");
				if (homepath != NULL) { homeDir = homepath; }
				else { homeDir = ""; }

				newFileName = homeDir + fileName.substr(fileName.find("~") + 1);
				return newFileName;
			}
			else { //find path
				if (path.rfind("./") == string::npos) { return fileName; } //already complete name
				else { newFileName = fileName.substr(fileName.rfind("./") + 2); } //save the complete part of the name

																				  //char* cwdpath = new char[1024];
																				  //size_t size;
																				  //cwdpath=getcwd(cwdpath,size);
																				  //cwd = cwdpath;

				char *cwdpath = NULL;
				cwdpath = getcwd(NULL, 0); // or _getcwd
				if (cwdpath != NULL) { cwd = cwdpath; }
				else { cwd = ""; }


				//rip off first '/'
				string simpleCWD;
				if (cwd.length() > 0) { simpleCWD = cwd.substr(1); }

				//break apart the current working directory
				vector<string> dirs;
				while (simpleCWD.find_first_of('/') != string::npos) {
					string dir = simpleCWD.substr(0, simpleCWD.find_first_of('/'));
					simpleCWD = simpleCWD.substr(simpleCWD.find_first_of('/') + 1, simpleCWD.length());
					dirs.push_back(dir);
				}
				//get last one              // ex. ../../../filename = /user/work/desktop/filename
				dirs.push_back(simpleCWD);  //ex. dirs[0] = user, dirs[1] = work, dirs[2] = desktop


				int index = dirs.size() - 1;

				while ((pos = path.rfind("./")) != string::npos) { //while you don't have a complete path
					if (pos == 0) {
						break;  //you are at the end
					}
					else if (path[(pos - 1)] == '.') { //you want your parent directory ../
						path = path.substr(0, pos - 1);
						index--;
						if (index == 0) { break; }
					}
					else if (path[(pos - 1)] == '/') { //you want the current working dir ./
						path = path.substr(0, pos);
					}
					else if (pos == 1) {
						break;  //you are at the end
					}
					else { mothurOut("cannot resolve path for " + fileName + "\n"); return fileName; }
				}

				for (int i = index; i >= 0; i--) {
					newFileName = dirs[i] + "/" + newFileName;
				}

				newFileName = "/" + newFileName;
				return newFileName;
			}
#else
			if (path.find("~") != string::npos) { //go to home directory
				string homeDir = getenv("HOMEPATH");
				newFileName = homeDir + fileName.substr(fileName.find("~") + 1);
				return newFileName;
			}
			else { //find path
				if (path.rfind(".\\") == string::npos) { return fileName; } //already complete name
				else { newFileName = fileName.substr(fileName.rfind(".\\") + 2); } //save the complete part of the name

				char *cwdpath = NULL;
				cwdpath = getcwd(NULL, 0); // or _getcwd
				if (cwdpath != NULL) { cwd = cwdpath; }
				else { cwd = ""; }

				//break apart the current working directory
				vector<string> dirs;
				while (cwd.find_first_of('\\') != -1) {
					string dir = cwd.substr(0, cwd.find_first_of('\\'));
					cwd = cwd.substr(cwd.find_first_of('\\') + 1, cwd.length());
					dirs.push_back(dir);

				}
				//get last one
				dirs.push_back(cwd);  //ex. dirs[0] = user, dirs[1] = work, dirs[2] = desktop

				int index = dirs.size() - 1;

				while ((pos = path.rfind(".\\")) != string::npos) { //while you don't have a complete path
					if (pos == 0) {
						break;  //you are at the end
					}
					else if (path[(pos - 1)] == '.') { //you want your parent directory ../
						path = path.substr(0, pos - 1);
						index--;
						if (index == 0) { break; }
					}
					else if (path[(pos - 1)] == '\\') { //you want the current working dir ./
						path = path.substr(0, pos);
					}
					else if (pos == 1) {
						break;  //you are at the end
					}
					else {
						throw invalid_argument("cannot resolve path for " + fileName + "\n");
						//mothurOut("cannot resolve path for " + fileName + "\n"); return fileName; }
					}
				}

				for (int i = index; i >= 0; i--) {
					newFileName = dirs[i] + "\\" + newFileName;
				}

				return newFileName;
			}

#endif
		}
	}
	catch (exception& e) {
		// errorOut(e, "MothurOut", "getFullPathName");
		// exit(1);
		throw;
	}
}