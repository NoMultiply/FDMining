#pragma once

#include "FDMining.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <set>
enum DFDNodeType {
	UnVisited = 0,
	Dependency = 1, MinimalDependency = 2, CandidateDependency = 3,
	NonDependency = 4, MaximalNonDependency = 5, CandidateNonDependency = 6
};
class DFDNode;
typedef std::vector<int> DFDKey;
typedef std::vector<DFDNode*> DFDNodeList;
typedef std::unordered_map<DFDKey, DFDNode*> DFDTable;

class DFDNode {
private:
	DFDKey key;
	DFDNodeType type;
public:
	static DFDNode *RHS;
	static DFDTable Category;
	DFDNode(DFDKey key): key(key) {
		type = UnVisited;
	}

	void getFromCategory();

	bool operator>> (const DFDNode* other);

	void computePartitions() {

	}

	DFDNodeList getAllSubsets() {

	}

	DFDNodeList getAllSupersets() {

	}

	bool isVisited() {
		return type != UnVisited;
	}

	bool isCandidateDependency() {
		return type == CandidateDependency;
	}

	bool isCandidateNonDependency() {
		return type == CandidateNonDependency;
	}

	bool isDependency() {
		return type == Dependency || type == CandidateDependency || type == MinimalDependency;
	}

	bool isNonDependency() {
		return type == NonDependency || type == MaximalNonDependency || type == CandidateNonDependency;
	}

	bool isMinimal();
	bool isMaximal();
};

void DFD(RTable & r, FDSet & FDs);
FDSet findLHSs(DFDNode *RHS, DFDNodeList RShip, RTable &r);