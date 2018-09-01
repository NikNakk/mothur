#pragma once

#include "namemap.h"
#include <string>
#include <vector>
#include <map>
#include <numeric>

struct CountTable {
public:
	CountTable() = default;
	CountTable(std::string filename);

	bool writeCountTable(std::string fileName, std::vector<std::string> nameOrder);

	void addGroupName(std::string groupName) { this->groupNames.push_back(groupName); }
	void setGroupNames(std::vector<std::string> groupNames) { this->groupNames = groupNames; }
	void setCounts(std::string name, std::vector<long long> counts) { nameCounts[name] = counts; nameTotals[name] = std::accumulate(counts.begin(), counts.end(), 0LL); }
	void setCounts(std::string name, std::vector<long long> counts, long long totalCount) { nameCounts[name] = counts; nameTotals[name] = totalCount; }
	void setTotalCount(std::string name, long long totalCount) { nameTotals[name] = totalCount; }

	std::vector<std::string> getGroupNames() const { return groupNames; }
	long long getNumSeqs() const { return nameCounts.size(); }
	long long getTotal(std::string name) const { auto it = nameTotals.find(name); if (it != nameTotals.end()) return it->second; else return 0; }
	std::vector<long long> getCounts(std::string name) const { auto it = nameCounts.find(name); if (it != nameCounts.end()) return it->second; else return std::vector<long long>(); }
	NamesWithTotals getNamesWithTotals() const { return nameTotals; }

	void clear() { using std::swap; CountTable ct; swap(ct, *this); }

	bool valid = true;

private:
	std::map<std::string, std::vector<long long>> nameCounts;
	NamesWithTotals nameTotals;
	std::vector<std::string> groupNames;
};