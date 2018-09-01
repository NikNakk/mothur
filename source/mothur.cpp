/*
 *  interface.cpp
 *
 *
 *  Created by Pat Schloss on 8/14/08.
 *  Copyright 2008 Patrick D. Schloss. All rights reserved.
 *
 */

#include "mothur.h"
#include "engine.hpp"
#include <g3log/g3log.hpp>
#include "g3log/logworker.hpp"
#include "g3log/logmessage.hpp"
#include "logsinks.h"
#include <atomic>

 /**************************************************************************************************/

std::atomic<bool> ctrlc_pressed;
std::atomic<bool> mothur_executing;

void ctrlc_handler(int sig) {
	ctrlc_pressed = true;

	if (mothur_executing) { //if mid command quit execution, else quit mothur
		LOG(INFO) << "quitting command...";
	}
	else {
		LOG(INFO) << "quitting mothur";
		exit(1);
	}
}
/***********************************************************************/
int main(int argc, char *argv[]) {
	mothur_executing = false;
	ctrlc_pressed = false;
	bool createLogFile = true;

	signal(SIGINT, ctrlc_handler);

	time_t ltime = time(NULL); /* calendar time */
	string logFileName = "mothur." + toString(ltime) + ".logfile";

	Settings settings;

	settings.setLogFileName(logFileName, false);
	string input;
	if (argc > 1) {
		input = argv[1];
	}
	bool versionOnly = false;
	auto logWorker = g3::LogWorker::createLogWorker();
	auto screenLogHandle = logWorker->addSink(unique_ptr<LogScreen>(new LogScreen()), &LogScreen::screenWrite);
	unique_ptr<g3::SinkHandle<LogMainLogFile>> logFileHandle;

	if ((input == "--version") || (input == "-v")) {
		versionOnly = true;
	}
	else if (input == "--help" || input == "-h") {
		input = "#help();quit();";
	}
	else {
		logFileHandle = move(logWorker->addSink(unique_ptr<LogMainLogFile>(new LogMainLogFile(logFileName)), &LogMainLogFile::fileWrite));
	}

	g3::initializeLogging(logWorker.get());

#if defined (UNIX)
	system("clear");
#else
	system("CLS");
#endif

	//get releaseDate from config
	settings.setReleaseDate(RELEASE_DATE);
	settings.setVersion(VERSION);

	//will make the gui output "pretty"
	bool outputHeader = true;
	if (argc > 1) {
		if (input[0] == '+' || input[0] == '-') { outputHeader = false; }

		if (argc > 2) { //is one of these -q for quiet mode?
			if (argc > 3) { LOG(LOGERROR) << "mothur only allows command inputs and the -q command line options.\n  i.e. ./mothur \"#summary.seqs(fasta=final.fasta);\" -q\n or ./mothur -q \"#summary.seqs(fasta=final.fasta);\"\n"; return 0; }
			else {
				string argv1 = argv[1];
				string argv2 = argv[2];
				if ((argv1 == "--quiet") || (argv1 == "-q")) {
					settings.setQuietMode(true);
					input = argv[2];
				}
				else if ((argv2 == "--quiet") || (argv2 == "-q")) {
					settings.setQuietMode(true);
				}
				else {
					LOG(LOGERROR) << "mothur only allows command inputs and the -q command line options.\n";
					LOG(LOGERROR) << "Unrecognized options: " + argv1 + " " + argv2 + "\n";
					return 0;
				}
			}
		}
	}

	if (outputHeader || versionOnly) {
		//version
		string OS = "";
		ostringstream header;
#if defined (UNIX)
#if defined (__APPLE__) || (__MACH__)
		OS = "Mac";
#else
		OS = "Linux";
#endif
#else
		OS = "Windows";
#endif
		header << OS << " version\n";

#if defined (USE_READLINE) || defined (USE_EDITLINE)
		header << "Using ReadLine\n";
#endif

#ifdef MOTHUR_FILES
		header << "Using default file location " << MOTHUR_FILES << '\n';
#endif

		std::string bits;
		if (sizeof(void*) == 8) {
			bits = "64Bit Version";
		}
		else if (sizeof(void*) == 4) {
			bits = "32Bit Version";
		}
		else {
			bits = "unknown Version";
		}

		header << "Running " << bits << '\n';

	//header
		if (outputHeader) {
			LOG(FILEONLY) << header.str();

			LOG(INFO) << "mothur v." << VERSION;
			LOG(INFO) << "Last updated: " << RELEASE_DATE;
			LOG(INFO) << "\nby";
			LOG(INFO) << "Patrick D. Schloss\n";
			LOG(INFO) << "Department of Microbiology & Immunology";
			LOG(INFO) << "University of Michigan";
			LOG(INFO) << "pschloss@umich.edu";
			LOG(INFO) << "http://www.mothur.org\n";
			LOG(INFO) << "When using, please cite:";
			LOG(INFO) << "Schloss, P.D., et al., Introducing mothur: Open-source, platform-independent, community-supported software for describing and comparing microbial communities. Appl Environ Microbiol, 2009. 75(23):7537-41.\n";
			LOG(INFO) << "Distributed under the GNU General Public License\n";
			LOG(SCREENONLY) << "Type 'help()' for information on the commands that are available";
			LOG(SCREENONLY) << "Type 'quit()' to exit program\n";
		}
		else {
			//version only
			LOG(SCREENONLY) << OS << ' ' << bits << "\nMothur version=" << VERSION << "\nRelease Date=" << RELEASE_DATE << '\n';
			return 0;
		}
	}

	//srand(54321);
	srand((unsigned)time(NULL));

	std::unique_ptr<Engine> mothur;

	if (argc > 1) {
		if (input[0] == '#') {
			LOG(FILEONLY) << "Script Mode\n";
			mothur = unique_ptr<Engine>(new ScriptEngine(settings, argv[0], input));
		}
		else if (input[0] == '+') {
			mothur = unique_ptr<Engine>(new ScriptEngine(settings, argv[0], input));
			settings.setGui(true);
		}
		else {
		LOG(FILEONLY) << "Batch Mode\n";
			mothur = unique_ptr<Engine>(new BatchEngine(settings, argv[0], input));
		}
	}
	else {
		LOG(FILEONLY) << "Interactive Mode\n";
		mothur = unique_ptr<Engine>(new InteractEngine(settings, argv[0], *screenLogHandle.get()));
	}

	// Enter main processing loop
	mothur->processCommands();

	//closes logfile so we can rename
	if (createLogFile) {
		logFileHandle->call(&LogMainLogFile::stopAndClose);
		string outputDir = settings.getOutputDir();
		string newLogFileName = settings.getLogFileName();
		bool append = settings.getAppend();

		if (newLogFileName != logFileName) {
			newLogFileName = outputDir + newLogFileName;

			if (!append) {
				//need this because the logfile is started before the name is set
				rename(logFileName.c_str(), newLogFileName.c_str()); //logfile with timestamp
			}
			else {
				ofstream outNewLog;
				File::openOutputFileAppend(newLogFileName, outNewLog);

				if (!settings.getGui()) {
					outNewLog << endl << endl << "*********************************************************************************" << endl << endl;
				}
				else {
					outNewLog << endl;
				}
				outNewLog.close();

				File::appendFiles(logFileName, newLogFileName);
				File::remove(logFileName);
			}
		}
	}
	return 0;
}

/**************************************************************************************************/

