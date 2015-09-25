#ifndef CHIMERAUCHIMECOMMAND_H
#define CHIMERAUCHIMECOMMAND_H


/*
 *  chimerauchimecommand.h
 *  Mothur
 *
 *  Created by westcott on 5/13/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "mothur.h"
#include "command.hpp"
#include "sequenceparser.h"
#include "counttable.h"
#include "sequencecountparser.h"


/***********************************************************/

class ChimeraUchimeCommand : public Command {
public:
	ChimeraUchimeCommand(string);
	ChimeraUchimeCommand();
	~ChimeraUchimeCommand() {}
	
	vector<string> setParameters();
	string getCommandName()			{ return "chimera.uchime";		}
	string getCommandCategory()		{ return "Sequence Processing"; }
	
	string getHelpString();	
    string getOutputPattern(string);	
	string getCitation() { return "uchime by Robert C. Edgar\nhttp://drive5.com/usearch/manual/uchime_algo.html\nThis code was donated to the public domain.\nEdgar,R.C., Haas,B.J., Clemente,J.C., Quince,C. and Knight,R. (2011), UCHIME improves sensitivity and speed of chimera detection.  Bioinformatics 27:2194.\nhttp://www.mothur.org/wiki/Chimera.uchime\n"; }
	string getDescription()		{ return "detect chimeric sequences"; }
	
	int execute(); 
	void help() { m->mothurOut(getHelpString()); }		
	
private:
	vector<int> processIDS;   //processid
	int driver(string, string, string, string, int&);
	void driverWithCount(string outputFName, string filename, string accnos, string alns, int & numChimeras, int & numSeqs);
	int createProcesses(string, string, string, string, int&);
		
	bool abort, useAbskew, chimealns, useMinH, useMindiv, useXn, useDn, useXa, useChunks, useMinchunk, useIdsmoothwindow, useMinsmoothid, useMaxp, skipgaps, skipgaps2, useMinlen, useMaxlen, ucl, useQueryfract, hasCount, hasName, dups;
	string fastafile, groupfile, templatefile, outputDir, namefile, countfile, abskew, minh, mindiv, xn, dn, xa, chunks, minchunk, idsmoothwindow, minsmoothid, maxp, minlen, maxlen, queryfract, uchimeLocation, strand;
	int processors;
	
	SequenceParser* sparser;
    SequenceCountParser* cparser;
	vector<string> outputNames;
	vector<string> fastaFileNames;
	vector<string> nameFileNames;
	vector<string> groupFileNames;
	
	string getNamesFile(string&);
	int readFasta(string, map<string, string>&);
	int printFile(vector<seqPriorityNode>&, string);
	int deconvoluteResults(map<string, string>&, string, string, string);
	int driverGroups(string outputFName, string filename, string accnos, string alns, string countlist, unsigned long long start, unsigned long long end, vector<string> groups);
	void driverGroupsWithCount(string outputFName, string filename, string accnos, string alns, string countlist, unsigned long long start, unsigned long long end, vector<string> groups, int & numSeqs);
	int createProcessesGroups(string, string, string, string, string, vector<string>, string, string, string);
    int prepFile(string filename, string);


};

/***********************************************************/
/**************************************************************************************************/
//custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
struct uchimeData {
	string fastafile; 
	string namefile; 
	string groupfile;
	string outputFName;
	string accnos, alns, filename, templatefile, uchimeLocation, countlist;
	MothurOut* m;
	int start;
	int end;
	int threadID, count, numChimeras;
	vector<string> groups;
	bool dups, useAbskew, chimealns, useMinH, useMindiv, useXn, useDn, useXa, useChunks, useMinchunk, useIdsmoothwindow, useMinsmoothid, useMaxp, skipgaps, skipgaps2, useMinlen, useMaxlen, ucl, useQueryfract, hasCount;
	string abskew, minh, mindiv, xn, dn, xa, chunks, minchunk, idsmoothwindow, minsmoothid, maxp, minlen, maxlen, queryfract, strand;
	
	uchimeData(){}
	uchimeData(string o, string uloc, string t, string file, string f, string n, string g, string ac,  string al, string nc, vector<string> gr, MothurOut* mout, int st, int en, int tid) {
		fastafile = f;
		namefile = n;
		groupfile = g;
		filename = file;
		outputFName = o;
		templatefile = t;
		accnos = ac;
		alns = al;
		m = mout;
		start = st;
		end = en;
		threadID = tid;
		groups = gr;
		count = 0;
		numChimeras = 0;
        uchimeLocation = uloc;
        countlist = nc;
	}
	void setBooleans(bool dps, bool Abskew, bool calns, bool MinH, bool Mindiv, bool Xn, bool Dn, bool Xa, bool Chunks, bool Minchunk, bool Idsmoothwindow, bool Minsmoothid, bool Maxp, bool skipgap, bool skipgap2, bool Minlen, bool Maxlen, bool uc, bool Queryfract, bool hc) {
		useAbskew = Abskew;
		chimealns = calns;
		useMinH = MinH;
		useMindiv = Mindiv;
		useXn = Xn;
		useDn = Dn;
		useXa = Xa;
		useChunks = Chunks;
		useMinchunk = Minchunk;
		useIdsmoothwindow = Idsmoothwindow;
		useMinsmoothid = Minsmoothid;
		useMaxp = Maxp;
		skipgaps = skipgap;
		skipgaps2 = skipgap2;
		useMinlen = Minlen;
		useMaxlen = Maxlen;
		ucl = uc;
		useQueryfract = Queryfract;
        hasCount = hc;
        dups = dps;
	}
	
	void setVariables(string abske, string min, string mindi, string x, string d, string xa2, string chunk, string minchun, string idsmoothwindo, string minsmoothi, string max, string minle, string maxle, string queryfrac, string stra) {
		abskew = abske;
		minh = min;
		mindiv = mindi;
        strand = stra;
		xn = x;
		dn = d;
		xa = xa2;
		chunks = chunk;
		minchunk = minchun;
		idsmoothwindow = idsmoothwindo;
		minsmoothid = minsmoothi;
		maxp = max;
		minlen = minle;
		maxlen = maxle;
		queryfract = queryfrac;
	}
};

/**************************************************************************************************/

#endif


