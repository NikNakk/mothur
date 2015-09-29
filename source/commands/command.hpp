#ifndef COMMAND_HPP
#define COMMAND_HPP
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
#include "optionparser.h"
#include "validparameter.h"
#include "mothurout.h"
#include "commandparameter.h"
#include "parametertypes.h"
#include "application.h"
#include "outputtype.h"

class Command {
	
	public:
		Command();
		Command(string option);
		
		//needed by gui
		virtual string getCommandName() = 0;
		virtual string getCommandCategory() = 0;
		virtual string getHelpString() = 0;
		virtual string getCitation() = 0;
		virtual string getDescription() = 0;
		bool aborted() { return abort; }
		
		virtual map<string, vector<string> > getOutputFiles() { return outputTypes; }
		string getOutputFileName(string type, map<string, string> variableParts);        
        virtual string getOutputPattern(string) = 0; //pass in type, returns something like: [filename],align or [filename],[distance],subsample.shared  strings in [] means its a variable.  This is used by the gui to predict output file names.  use variable keywords: [filename], [distance], [group], [extension], [tag]
		virtual vector<string> setParameters() { return vector<string>(); } //to fill parameters
		virtual vector<CommandParameter> getParameters() { return parameters; }
		virtual void setOutputTypes() {}	
		virtual int execute() = 0;
		virtual void help();
		void citation();
		virtual ~Command() { }
	
	protected:
		Application* app;
		MothurOut* m;
		bool abort;
		bool calledHelp;
			
		map<string, vector<string> > outputTypes;
		OutputTypeCollection nkOutputTypes;
		vector<CommandParameter> parameters;
		CommandParameterCollection nkParameters;
	
		map<string, vector<string> >::iterator itTypes;
};

#endif
