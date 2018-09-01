#pragma once
/*
 *  deconvolute.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 1/21/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "command.hpp"

 /* The unique.seqs command reads a fasta file, finds the duplicate sequences and outputs a names file
	 containing 2 columns.  The first being the groupname and the second the list of identical sequence names. */


class DeconvoluteCommand : public Command {

public:
	DeconvoluteCommand(Settings& settings, ParameterListToProcess& ptp);
	DeconvoluteCommand(Settings& settings) : Command(settings) {};
	~DeconvoluteCommand() = default;

	virtual void setParameters() override;
	virtual void setOutputTypes() override;
	virtual std::string getCommandName() const override { return "unique.seqs"; }
	virtual std::string getCommandCategory() const override { return "Sequence Processing"; }

	virtual string getHelpString() const override;
	virtual string getDescription() const override { return "creates a fasta containing the unique sequences as well as a namesfile with the names each sequence represents"; }

	virtual int execute() override;


private:
	std::string fastafile;
	std::string namefile;
	std::string countfile;
	std::string format;
};


