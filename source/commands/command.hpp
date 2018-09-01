#pragma once
//test2
/*
 *  command.h
 *  nast
 *
 *  Created by Pat Schloss on 10/23/08.
 *  Copyright 2008 Patrick D. Schloss. All rights reserved.
 *
 */

 /*This class is a parent to all the command classes.  */


#include "mothur.h"
#include "settings.h"
#include "outputtype.h"
#include "filehandling/file.h"
#include "utility.h"
#include "commandtoprocess.h"
#include "commandparameters/commandparametercollection.h"

class Command {

public:
	Command(Settings& settings);
	Command(Settings& settings, ParameterListToProcess ptp);

	void setup();

	virtual string getCommandName() const = 0;
	virtual string getCommandCategory() const = 0;
	virtual string getHelpString() const = 0;
	virtual string getCitation() const { return "http://www.mothur.org/wiki/" + getCommandName(); }
	virtual string getDescription() const = 0;
	bool aborted() const { return abort; }
	bool getCalledHelp() const { return calledHelp; }
	bool isValid() const { return valid; }
	std::string getErrorMessage() const { return errorMessage; }
	void setErrorMessage(std::string message) { valid = false; abort = true; errorMessage = message; }

	// virtual map<string, vector<string> > getOutputFiles() { return outputTypes; }
	std::string getOutputFileName(std::string type, std::map<std::string, std::string> variableParts, std::string existingName, std::map<std::string, std::string> vPartsIfExisting);
	string getOutputFileName(string type, map<string, string> variableParts);
	//virtual string getOutputPattern(string) { return ""; } //pass in type, returns something like: [filename],align or [filename],[distance],subsample.shared  strings in [] means its a variable.  This is used by the gui to predict output file names.  use variable keywords: [filename], [distance], [group], [extension], [tag]
	virtual void setParameters() {} //to fill parameters
	virtual vector<string> getParameters() { return parameters.getNames(); }
	virtual void setOutputTypes() {}
	virtual int execute() = 0;
	virtual ~Command() { }

	void help();
	void citation();

protected:
	virtual void validateSpecial() {};
	void removeAllOutput();

	Settings& settings;
	bool valid = true;
	bool abort = false;
	bool calledHelp = false;
	std::string errorMessage;
	std::string inputDir, outputDir;

	OutputTypeCollection outputTypes;
	CommandParameterCollection parameters;
	ParameterListToProcess paramsToProcess;

	map<string, vector<string> >::iterator itTypes;
};


