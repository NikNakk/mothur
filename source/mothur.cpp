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
#include "referencedb.h"
#include <g3log/g3log.hpp>
#include "g3log/logworker.hpp"
#include "g3log/logmessage.hpp"
#include "logsinks.h"
#include <atomic>

 /**************************************************************************************************/

MothurOut* MothurOut::_uniqueInstance = 0;
/***********************************************************************/

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
	MothurOut* m = MothurOut::getInstance();
	mothur_executing = false;
	ctrlc_pressed = false;
	try {
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
		bool versionOnly = false, helpOnly = false;
		auto logWorker = g3::LogWorker::createLogWorker();
		auto screenLogHandle = logWorker->addSink(unique_ptr<LogScreen>(new LogScreen()), &LogScreen::screenWrite);
		unique_ptr<g3::SinkHandle<LogMainLogFile>> logFileHandle;

		if ((input == "--version") || (input == "-v")) {
			versionOnly = true;
		}
		else if (input == "--help" || input == "-h") {
			helpOnly = true;
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

#ifdef MOTHUR_FILES
		string temp = MOTHUR_FILES;

		//add / to name if needed
		string lastChar = temp.substr(temp.length() - 1);
		if (lastChar != path_delimiter) { temp += path_delimiter; }

		temp = File::getFullPathName(temp);
		m->setDefaultPath(temp);
#endif

		//get releaseDate from config
		settings.setReleaseDate(RELEASE_DATE);
		settings.setVersion(VERSION);

		//will make the gui output "pretty"
		bool outputHeader = true;
		if (argc > 1) {
			string guiInput = argv[1];
			if (guiInput[0] == '+') { outputHeader = false; }
			if (guiInput[0] == '-') { outputHeader = false; }

			if (argc > 2) { //is one of these -q for quiet mode?
				if (argc > 3) { LOG(LOGERROR) << "mothur only allows command inputs and the -q command line options.\n  i.e. ./mothur \"#summary.seqs(fasta=final.fasta);\" -q\n or ./mothur -q \"#summary.seqs(fasta=final.fasta);\"\n"; return 0; }
				else {
					string argv1 = argv[1];
					string argv2 = argv[2];
					if ((argv1 == "--quiet") || (argv1 == "-q")) {
						m->quietMode = true;
						argv[1] = argv[2];
					}
					else if ((argv2 == "--quiet") || (argv2 == "-q")) {
						m->quietMode = true;
					}
					else {
						LOG(LOGERROR) << "mothur only allows command inputs and the -q command line options.\n";
						LOG(LOGERROR) << "Unrecognized options: " + argv1 + " " + argv2 + "\n";
						return 0;
					}
				}
			}
		}

		if (outputHeader) {
			//version
#if defined (UNIX)
#if defined (__APPLE__) || (__MACH__)
			LOG(FILEONLY) << "Mac version\n";
#else
			LOG(FILEONLY) << "Linux version\n";
#endif
#else
			LOG(FILEONLY) << "Windows version\n";
#endif		

#if defined (USE_READLINE) || defined (USE_EDITLINE)
			LOG(FILEONLY) << "Using ReadLine";
#endif

#ifdef MOTHUR_FILES
			LOG(FILEONLY) << "Using default file location " + temp;
#endif

			if (sizeof(void*) == 8) {
				LOG(FILEONLY) << "Running 64Bit Version";
			}
			else if (sizeof(void*) == 4) {
				LOG(FILEONLY) << "Running 32Bit Version";
			}
			else {
				LOG(FILEONLY) << "Running unknown Version";
			}

			//header
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
			LOG(SCREENONLY) << "Type 'help()' for information on the commands that are available\n";
			LOG(SCREENONLY) << "Type 'quit()' to exit program";
			LOG(INFO) << "";
		}

		//srand(54321);
		srand((unsigned)time(NULL));

		unique_ptr<Engine> mothur;
		bool bail = 0;
		string input;

		if (argc > 1) {
			input = argv[1];
			//LOG(INFO) << "input = " + input << '\n';

			if (input[0] == '#') {
				LOG(FILEONLY) << "Script Mode\n";

				mothur = move(unique_ptr<Engine>(new ScriptEngine(settings, argv[0], input)));
			}
			else if (input[0] == '+') {
				mothur = move(unique_ptr<Engine>(new ScriptEngine(settings, argv[0], input)));
				m->gui = true;
			}
			else if (versionOnly) {
				string OS = "";
				//version
#if defined (UNIX)
#if defined (__APPLE__) || (__MACH__)
				OS = "Mac ";
#else
				OS = "Linux ";
#endif

#else
				OS = "Windows ";
#endif

				if (sizeof(void*) == 8) {
					OS += "64Bit Version";
				}
				else if (sizeof(void*) == 4) {
					OS += "32Bit Version";
				}
				else {
					OS += "unknown Version";
				}

				LOG(SCREENONLY) << OS + "\nMothur version=" << VERSION << "\nRelease Date=" << RELEASE_DATE << '\n' << '\n';
				return 0;

			}
			else if (helpOnly) {

				input = "#help();quit();";

				mothur = move(unique_ptr<Engine>(new ScriptEngine(settings, argv[0], input)));
			}
			else {
				LOG(FILEONLY) << "Batch Mode";
				LOG(INFO) << "" << '\n';

				mothur = move(unique_ptr<Engine>(new BatchEngine(settings, argv[0], input)));
			}
		}
		else {
			LOG(FILEONLY) << "Interactive Mode";
			LOG(INFO) << "" << '\n';

			mothur = move(unique_ptr<Engine>(new InteractEngine(settings, argv[0], *screenLogHandle.get())));
		}

		while (bail == 0) { bail = mothur->getInput(); }

		//closes logfile so we can rename
		if (createLogFile) {
			logFileHandle->call(&LogMainLogFile::stopAndClose);
			string outputDir = settings.getOutputDir();
			string tempLog = settings.getLogFileName();
			bool append = settings.getAppend();

			string newlogFileName;
			if (tempLog != "") {
				newlogFileName = outputDir + tempLog;

				if (!append) {
					//need this because the logfile is started before the name is set
					rename(logFileName.c_str(), newlogFileName.c_str()); //logfile with timestamp
				}
				else {
					ofstream outNewLog;
					File::openOutputFileAppend(newlogFileName, outNewLog);

					if (!m->gui) {
						outNewLog << endl << endl << "*********************************************************************************" << endl << endl;
					}
					else {
						outNewLog << endl;
					}
					outNewLog.close();

					File::appendFiles(logFileName, newlogFileName);
					File::remove(logFileName);
				}
			}
		}
		return 0;
	}
	catch (exception& e) {
		LOG(FATAL) << e.what() << " in mothur, main";
		exit(1);
	}
}

/**************************************************************************************************/

