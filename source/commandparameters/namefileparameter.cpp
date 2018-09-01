#include "namefileparameter.h"
#include "settings.h"
#include "filehandling/file.h"
#include "utility.h"

void NameFileParameter::validateAndSet(std::string newValue) {
	if (newValue == "current") {
		newValue = settings.getCurrent("name");
	}
	value = newValue;
	settings.setCurrent("name", newValue);
}

std::string NameFileParameter::getNameFile(Settings& settings, std::vector<std::string> files) {
	std::string output;
	std::string namefile = settings.getCurrent("name");
	bool match = false;

	if ((namefile != "") && (!(settings.getMothurCalling()))) {
		std::string temp = File::getRootName(File::getSimpleName(namefile));
		std::vector<std::string> rootName = Utility::split(temp, '.');

		for (int i = 0; i < files.size(); i++) {
			temp = File::getRootName(File::getSimpleName(files[i]));
			std::vector<std::string> root = Utility::split(temp, '.');

			size_t smallest = rootName.size();
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
	return output;
}
