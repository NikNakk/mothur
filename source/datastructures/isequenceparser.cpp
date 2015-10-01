#include "isequenceparser.h"

int ISequenceParser::getNumSeqs(string g) {
	map<string, vector<Sequence> >::iterator it;
	int num = 0;

	it = seqs.find(g);
	if (it == seqs.end()) {
		LOG(LOGERROR) << "" + g + " is not a valid group, please correct." << '\n';
	}
	else {
		num = (it->second).size();
	}

	return num;
}

vector<Sequence> ISequenceParser::getSeqs(string g) {
	map<string, vector<Sequence> >::iterator it;
	vector<Sequence> seqForThisGroup;

	it = seqs.find(g);
	if (it == seqs.end()) {
		LOG(LOGERROR) << "No sequences available for group " + g + ", please correct." << '\n';
	}
	else {
		seqForThisGroup = it->second;
		if (app.isDebug) { LOG(DEBUG) << "group " + g + " fasta file has " + toString(seqForThisGroup.size()) + " sequences."; }
	}

	return seqForThisGroup;
}

map<string, string> ISequenceParser::getAllSeqsMap() {
	return allSeqsMap;
}