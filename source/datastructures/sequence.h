#pragma once

#include <string>
#include <stdexcept>

struct SequenceSummary {
public:
	int startPosition = 0;
	int endPosition = 0;
	int seqLength = 0;
	int ambigBases = 0;
	int longHomoPolymer = 0;
	int numNs = 0;
	long long numSeqs = 1;
};

struct NamedSequenceSummary : public SequenceSummary {
public:
	std::string name;
	bool valid = true;
};

class Sequence {
public:
	Sequence() = default;
	Sequence(std::string name, std::string sequence, int numAmbigs = -1) : name(name), sequence(sequence), numAmbigs(numAmbigs) {};
	std::string getName() const { return name; }
	std::string getSequence() const { return sequence; }
	SequenceSummary getSequenceSummary() const;
	int getNumNs() const;
private:
	std::string name;
	std::string sequence;
	mutable int numAmbigs = -1;
};

class InvalidSequence : public std::logic_error {
public:
	InvalidSequence(std::string message) : std::logic_error(message) {}
};