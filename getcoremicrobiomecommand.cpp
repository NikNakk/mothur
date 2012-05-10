//
//  GetCoreMicroBiomeCommand.cpp
//  Mothur
//
//  Created by John Westcott on 5/8/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "GetCoreMicroBiomeCommand.h"


//**********************************************************************************************************************
vector<string> GetCoreMicroBiomeCommand::setParameters(){	
	try {
        CommandParameter pshared("shared", "InputTypes", "", "", "SharedRel", "SharedRel", "none",false,false); parameters.push_back(pshared);
		CommandParameter prelabund("relabund", "InputTypes", "", "", "SharedRel", "SharedRel", "none",false,false); parameters.push_back(prelabund);
        CommandParameter pgroups("groups", "String", "", "", "", "", "",false,false); parameters.push_back(pgroups);
		CommandParameter plabel("label", "String", "", "", "", "", "",false,false); parameters.push_back(plabel);
		CommandParameter poutput("output", "Multiple", "fraction-count", "fraction", "", "", "",false,false); parameters.push_back(poutput);
        CommandParameter pabund("abundance", "Number", "", "-1", "", "", "",false,false); parameters.push_back(pabund);
		CommandParameter psamples("samples", "Number", "", "-1", "", "", "",false,false); parameters.push_back(psamples);
		CommandParameter pinputdir("inputdir", "String", "", "", "", "", "",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "",false,false); parameters.push_back(poutputdir);
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "GetCoreMicroBiomeCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string GetCoreMicroBiomeCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The get.coremicrobiome determines the fraction of OTUs that are found in varying numbers of samples for different minimum relative abundances.\n";
		helpString += "The get.coremicrobiome parameters are: shared, relabund, groups, label, output, abundance and samples. Shared or relabund is required.\n";
		helpString += "The label parameter is used to analyze specific labels in your input.\n";
		helpString += "The groups parameter allows you to specify which of the groups you would like analyzed.\n";
        helpString += "The output parameter is used to specify whether you would like the fraction of OTU's or OTU count outputted. Options are fraction or count. Default=fraction.\n";
		helpString += "The abundance parameter allows you to specify an abundance you would like the OTU names outputted for. Must be an integer between 0 and 100, indicating the relative abundance. \n";
        helpString += "The samples parameter allows you to specify the minimum number of samples you would like the OTU names outputted for. Must be an interger between 1 and number of samples in your file.\n";
		helpString += "The new command should be in the following format: get.coremicrobiome(shared=yourSharedFile)\n";
		helpString += "get.coremicrobiom(shared=final.an.shared, abund=30)\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "GetCoreMicroBiomeCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
GetCoreMicroBiomeCommand::GetCoreMicroBiomeCommand(){	
	try {
		abort = true; calledHelp = true;
		setParameters();
        vector<string> tempOutNames;
		outputTypes["coremicrobiom"] = tempOutNames; 
	}
	catch(exception& e) {
		m->errorOut(e, "GetCoreMicroBiomeCommand", "GetCoreMicroBiomeCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
GetCoreMicroBiomeCommand::GetCoreMicroBiomeCommand(string option)  {
	try {
		abort = false; calledHelp = false;   allLines = 1;
		
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
		
		else {
			//valid paramters for this command
			vector<string> myArray = setParameters();
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			map<string,string>::iterator it;
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
                
				string path;
				it = parameters.find("relabund");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["relabund"] = inputDir + it->second;		}
				}
                
                it = parameters.find("shared");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["shared"] = inputDir + it->second;		}
				}
            }
           
        
			//check for parameters
            sharedfile = validParameter.validFile(parameters, "shared", true);
			if (sharedfile == "not open") { abort = true; }
			else if (sharedfile == "not found") { sharedfile = ""; }
			else { inputFileName = sharedfile; format = "sharedfile"; m->setSharedFile(sharedfile); }
			
			relabundfile = validParameter.validFile(parameters, "relabund", true);
			if (relabundfile == "not open") { abort = true; }
			else if (relabundfile == "not found") { relabundfile = ""; }
			else { inputFileName = relabundfile; format = "relabund"; m->setRelAbundFile(relabundfile); }
            
            if ((relabundfile == "") && (sharedfile == "")) { 
				//is there are current file available for either of these?
				//give priority to shared, then relabund
				sharedfile = m->getSharedFile(); 
				if (sharedfile != "") {  inputFileName = sharedfile; format="sharedfile"; m->mothurOut("Using " + sharedfile + " as input file for the shared parameter."); m->mothurOutEndLine(); }
				else { 
					relabundfile = m->getRelAbundFile(); 
					if (relabundfile != "") {  inputFileName = relabundfile; format="relabund"; m->mothurOut("Using " + relabundfile + " as input file for the relabund parameter."); m->mothurOutEndLine(); }
					else { 
						m->mothurOut("No valid current files. You must provide a shared or relabund."); m->mothurOutEndLine(); 
						abort = true;
					}
				}
			}
            
            //if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	
				outputDir = m->hasPath(inputFileName); //if user entered a file with a path then preserve it	
			}
            
            string groups = validParameter.validFile(parameters, "groups", false);			
			if (groups == "not found") { groups = ""; }
			else { m->splitAtDash(groups, Groups); }
			m->setGroups(Groups);
            
            string label = validParameter.validFile(parameters, "label", false);			
			if (label == "not found") { label = ""; }
			else { 
				if(label != "all") {  m->splitAtDash(label, labels);  allLines = 0;  }
				else { allLines = 1;  }
			}
            
            output = validParameter.validFile(parameters, "output", false);		if(output == "not found"){	output = "fraction"; }
						
			if ((output != "fraction") && (output != "count")) { m->mothurOut(output + " is not a valid output form. Options are fraction and count. I will use fraction."); m->mothurOutEndLine(); output = "fraction"; }
            
            string temp = validParameter.validFile(parameters, "abundance", false);	if (temp == "not found"){	temp = "-1";	}
			m->mothurConvert(temp, abund);
            
            if (abund != -1) { 
                if ((abund < 0) || (abund > 100)) { m->mothurOut(toString(abund) + " is not a valid number for abund. Must be an integer between 0 and 100.\n"); }
            }
            
            temp = validParameter.validFile(parameters, "samples", false);	if (temp == "not found"){	temp = "-1";	}
			m->mothurConvert(temp, samples);

		}
		
	}
	catch(exception& e) {
		m->errorOut(e, "GetCoreMicroBiomeCommand", "GetCoreMicroBiomeCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int GetCoreMicroBiomeCommand::execute(){
	try {
		
		if (abort == true) { if (calledHelp) { return 0; }  return 2;	}
        
        InputData input(inputFileName, format);
        vector<SharedRAbundFloatVector*> lookup = input.getSharedRAbundFloatVectors();
        string lastLabel = lookup[0]->getLabel();
        
        if (samples != -1) { 
            if ((samples < 1) || (samples > lookup.size())) { m->mothurOut(toString(samples) + " is not a valid number for samples. Must be an integer between 1 and the number of samples in your file. Your file contains " + toString(lookup.size()) + " samples, so I will use that.\n"); samples = lookup.size(); }
        }

        
        //if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
        set<string> processedLabels;
        set<string> userLabels = labels;
        
        //as long as you are not at the end of the file or done wih the lines you want
        while((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
            
            if (m->control_pressed) { for (int i = 0; i < lookup.size(); i++) {  delete lookup[i];  }  for (int i = 0; i < outputNames.size(); i++) { m->mothurRemove(outputNames[i]); } return 0; }
            
            if(allLines == 1 || labels.count(lookup[0]->getLabel()) == 1){			
                
                m->mothurOut(lookup[0]->getLabel()); m->mothurOutEndLine();
                
                createTable(lookup);
                
                processedLabels.insert(lookup[0]->getLabel());
                userLabels.erase(lookup[0]->getLabel());
            }
            
            if ((m->anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
                string saveLabel = lookup[0]->getLabel();
                
                for (int i = 0; i < lookup.size(); i++) {  delete lookup[i];  }  
                lookup = input.getSharedRAbundFloatVectors(lastLabel);
                m->mothurOut(lookup[0]->getLabel()); m->mothurOutEndLine();
                
                createTable(lookup);
                
                processedLabels.insert(lookup[0]->getLabel());
                userLabels.erase(lookup[0]->getLabel());
                
                //restore real lastlabel to save below
                lookup[0]->setLabel(saveLabel);
            }
            
            lastLabel = lookup[0]->getLabel();
            //prevent memory leak
            for (int i = 0; i < lookup.size(); i++) {  delete lookup[i]; lookup[i] = NULL; }
            
            if (m->control_pressed) { for (int i = 0; i < outputNames.size(); i++) { m->mothurRemove(outputNames[i]); }  return 0; }
            
            //get next line to process
            lookup = input.getSharedRAbundFloatVectors();				
        }
        
        if (m->control_pressed) {  for (int i = 0; i < outputNames.size(); i++) { m->mothurRemove(outputNames[i]); }  return 0; }
        
        //output error messages about any remaining user labels
        set<string>::iterator it;
        bool needToRun = false;
        for (it = userLabels.begin(); it != userLabels.end(); it++) {  
            m->mothurOut("Your file does not include the label " + *it); 
            if (processedLabels.count(lastLabel) != 1) {
                m->mothurOut(". I will use " + lastLabel + "."); m->mothurOutEndLine();
                needToRun = true;
            }else {
                m->mothurOut(". Please refer to " + lastLabel + "."); m->mothurOutEndLine();
            }
        }
        
        //run last label if you need to
        if (needToRun == true)  {
            for (int i = 0; i < lookup.size(); i++) { if (lookup[i] != NULL) { delete lookup[i]; } }  
            lookup = input.getSharedRAbundFloatVectors(lastLabel);
            
            m->mothurOut(lookup[0]->getLabel()); m->mothurOutEndLine();
            
            createTable(lookup);
            
            for (int i = 0; i < lookup.size(); i++) {  delete lookup[i];  }
        }
        
        if (m->control_pressed) { for (int i = 0; i < outputNames.size(); i++) { m->mothurRemove(outputNames[i]); }  return 0; }
        
        //output files created by command
		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
		m->mothurOutEndLine();
        return 0;
		
    }
	catch(exception& e) {
		m->errorOut(e, "GetCoreMicroBiomeCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************

int GetCoreMicroBiomeCommand::createTable(vector<SharedRAbundFloatVector*>& lookup){
	try {
        
        string outputFileName = outputDir + m->getRootName(m->getSimpleName(inputFileName)) + lookup[0]->getLabel() + ".core.microbiom";
        outputNames.push_back(outputFileName);  outputTypes["coremicrobiom"].push_back(outputFileName);
		ofstream out;
		m->openOutputFile(outputFileName, out);
        
        int numSamples = lookup.size();
        int numOtus = lookup[0]->getNumBins();
        
        //table is 100 by numsamples
        //question we are answering is: what fraction of OTUs in a study have a relative abundance at or above %X
        //in at least %Y samples. x goes from 0 to 100, y from 1 to numSamples
        vector< vector<double> > table; table.resize(101);
        for (int i = 0; i < table.size(); i++) { table[i].resize(numSamples, 0.0); }
        
        map<int, vector<string> > otuNames;
        if ((abund != -1) && (samples == -1)) { //fill with all samples
            for (int i = 0; i < numSamples; i++) {
                vector<string> temp;
                otuNames[i+1] = temp;
            }
        }else if ((abund == -1) && (samples != -1)) { //fill with all relabund
            for (int i = 0; i < 101; i++) {
                vector<string> temp;
                otuNames[i] = temp;
            }
        }else if ((abund != -1) && (samples != -1)) { //only one line is wanted
            vector<string> temp;
            otuNames[abund] = temp;
        }
        
        for (int i = 0; i < numOtus; i++) {
            
            if (m->control_pressed) { break; }
            
            //count number of samples in this otu with a relabund >= spot in count
            vector<int> counts; counts.resize(101, 0);
            
            for (int j = 0; j < lookup.size(); j++) {
                double relabund = lookup[j]->getAbundance(i);
                int wholeRelabund = (int) (floor(relabund*100));
                for (int k = 0; k < wholeRelabund+1; k++) { counts[k]++; }
            }
            
            //add this otus info to table
            for (int j = 0; j < table.size(); j++) {
                for (int k = 0; k < counts[j]; k++) { table[j][k]++; }
                
                if ((abund == -1) && (samples != -1)) { //we want all OTUs with this number of samples
                    if (counts[j] >= samples) { otuNames[j].push_back(m->currentBinLabels[i]); }
                }else if ((abund != -1) && (samples == -1)) { //we want all OTUs with this relabund
                    if (j == abund) {  
                        for (int k = 0; k < counts[j]; k++) {  otuNames[k+1].push_back(m->currentBinLabels[i]); }
                    }
                }else if ((abund != -1) && (samples != -1)) { //we want only OTUs with this relabund for this number of samples
                    if ((j == abund) && (counts[j] >= samples)) {  
                        otuNames[j].push_back(m->currentBinLabels[i]); 
                    }
                }
            }
        }
		
        //format output
		if (output == "fraction") { out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint); }
        out << "NumSamples\t";
        
        //convert table counts to percents
        for (int i = 0; i < table.size(); i++) {
            out << "Relabund-" << i << "%\t";
            if (m->control_pressed) { break; }
            for (int j = 0; j < table[i].size(); j++) {  if (output == "fraction") { table[i][j] /= (double) numOtus; } }
        }
        out << endl;
        
        for (int i = 0; i < numSamples; i++) {
            if (m->control_pressed) { break; }
            out << i+1 << '\t';
            for (int j = 0; j < table.size(); j++) {  out << table[j][i] << '\t'; }
            out << endl;
        }

        out.close();
        
        if (m->control_pressed) { return 0; }
        
        if ((samples != -1) || (abund != -1))  {
            string outputFileName2 = outputDir + m->getRootName(m->getSimpleName(inputFileName)) + lookup[0]->getLabel() + ".core.microbiomlist";
            outputNames.push_back(outputFileName2);  outputTypes["coremicrobiom"].push_back(outputFileName2);
            ofstream out2;
            m->openOutputFile(outputFileName2, out2);
            
            if ((abund == -1) && (samples != -1)) { //we want all OTUs with this number of samples
                out2 << "Relabund%\tOTUList_for_samples=" << samples << "\n";
            }else if ((abund != -1) && (samples == -1)) { //we want all OTUs with this relabund
                out2 << "Samples\tOTUList_for_abund=" << abund << "\n";
            }else if ((abund != -1) && (samples != -1)) { //we want only OTUs with this relabund for this number of samples
                out2 << "Relabund%\tOTUList_for_samples=" << samples << "\n";
            }

            for (map<int, vector<string> >::iterator it = otuNames.begin(); it != otuNames.end(); it++) {
                if (m->control_pressed) { break; }
                
                vector<string> temp = it->second;
                string list = m->makeList(temp);
                
                out2 << it->first << '\t' << list << endl;
            }
            
            out2.close();
        }
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "GetCoreMicroBiomeCommand", "createTable");
		exit(1);
	}
}

//**********************************************************************************************************************

