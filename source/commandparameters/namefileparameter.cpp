#include "namefileparameter.h"
#include "application.h"
#include "settings.h"
#include "filehandling/file.h"
#include "utility.h"

void NameFileParameter::validateAndSet(string newValue) {
	this->value = value;
}

bool NameFileParameter::getNameFile(vector<string> files, string output) {
	try {
		Settings& settings = Application::getApplication()->getSettings();
		string namefile = settings.getNameFile();
		bool match = false;

		if ((namefile != "") && (!(settings.getMothurCalling()))) {
			string temp = File::getRootName(File::getSimpleName(namefile));
			vector<string> rootName;
			Utility::split(temp, '.', rootName);

			for (int i = 0; i < files.size(); i++) {
				temp = File::getRootName(File::getSimpleName(files[i]));
				vector<string> root;
				Utility::split(temp, '.', root);

				int smallest = rootName.size();
				if (root.size() < smallest) { smallest = root.size(); }

				int numMatches = 0;
				for (int j = 0; j < smallest; j++) {
					if (root[j] == rootName[j]) { numMatches++; }
				}

				if (smallest > 0) {
					if ((numMatches >= (smallest - 2)) && (root[0] == rootName[0])) {
						output = "[WARNING]: This command can take a namefile and you did not provide one. The current namefile is " + namefile + " which seems to match " + files[i] + ".";
						match = true;
						break;
					}
				}
			}
		}
		return match;
	}
	catch (exception& e) {
		//m->errorOut(e, "OptionParser", "getNameFile");
		//exit(1);
		throw;
	}
}
