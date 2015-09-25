#include "isequenceparser.h"

int ISequenceParser::getNumSeqs(string g) {
	try {
		map<string, vector<Sequence> >::iterator it;
		int num = 0;

		it = seqs.find(g);
		if (it == seqs.end()) {
			m->mothurOut("[ERROR]: " + g + " is not a valid group, please correct."); m->mothurOutEndLine();
		}
		else {
			num = (it->second).size();
		}

		return num;
	}
	catch (exception& e) {
		m->errorOut(e, "ISequenceParser", "getNumSeqs");
		exit(1);
	}
}

vector<Sequence> ISequenceParser::getSeqs(string g) {
	try {
		map<string, vector<Sequence> >::iterator it;
		vector<Sequence> seqForThisGroup;

		it = seqs.find(g);
		if (it == seqs.end()) {
			m->mothurOut("[ERROR]: No sequences available for group " + g + ", please correct."); m->mothurOutEndLine();
		}
		else {
			seqForThisGroup = it->second;
			if (m->debug) { m->mothurOut("[DEBUG]: group " + g + " fasta file has " + toString(seqForThisGroup.size()) + " sequences."); }
		}

		return seqForThisGroup;
	}
	catch (exception& e) {
		m->errorOut(e, "ISequenceParser", "getSeqs");
		exit(1);
	}
}

map<string, string> ISequenceParser::getAllSeqsMap() {
	return allSeqsMap;
}