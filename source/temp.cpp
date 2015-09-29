int Settings::checkForRedirects(string optionString) {
	try {

		int pos = optionString.find("outputdir");
		if (pos != string::npos) { //user has set outputdir in command option string
			string outputOption = "";
			bool foundEquals = false;
			for (int i = pos;i<optionString.length();i++) {
				if (optionString[i] == ',') { break; }
				else if (optionString[i] == '=') { foundEquals = true; }
				if (foundEquals) { outputOption += optionString[i]; }
			}
			if (outputOption[0] == '=') { outputOption = outputOption.substr(1); }
			if (m->mkDir(outputOption)) {
				setOutputDirectory(outputOption);
				m->mothurOut("Setting output directory to: " + outputOption); m->mothurOutEndLine();
			}
		}

		pos = optionString.find("inputdir");
		if (pos != string::npos) { //user has set inputdir in command option string
			string intputOption = "";
			bool foundEquals = false;
			for (int i = pos;i<optionString.length();i++) {
				if (optionString[i] == ',') { break; }
				else if (optionString[i] == '=') { foundEquals = true; }
				if (foundEquals) { intputOption += optionString[i]; }
			}
			if (intputOption[0] == '=') { intputOption = intputOption.substr(1); }
			if (m->dirCheck(intputOption)) {
				setInputDirectory(intputOption);
				m->mothurOut("Setting input directory to: " + intputOption); m->mothurOutEndLine();
			}
		}

		pos = optionString.find("seed=");
		if (pos != string::npos) { //user has set seed in command option string
			string intputOption = "";
			bool foundEquals = false;
			for (int i = pos;i<optionString.length();i++) {
				if (optionString[i] == ',') { break; }
				else if (optionString[i] == '=') { foundEquals = true; }
				if (foundEquals) { intputOption += optionString[i]; }
			}
			if (intputOption[0] == '=') { intputOption = intputOption.substr(1); }
			bool seed = false; int random;
			if (intputOption == "clear") {
				random = time(NULL);
				seed = true;
			}
			else {
				if (m->isNumeric1(intputOption)) { m->mothurConvert(intputOption, random); seed = true; }
				else { m->mothurOut("[ERROR]: Seed must be an integer."); m->mothurOutEndLine(); seed = false; }
			}

			if (seed) {
				srand(random);
				m->mothurOut("Setting random seed to " + toString(random) + ".\n\n");
			}
		}


		return 0;
	}
	catch (exception& e) {
		m->errorOut(e, "CommandFactory", "getCommand");
		exit(1);
	}
}
/***********************************************************/
