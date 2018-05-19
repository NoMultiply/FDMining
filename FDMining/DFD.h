#pragma once

#include "FDMining.h"
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <set>
#include <map>
#include <unordered_map>
#include <stack>
#include <iostream>
#include <bitset>
enum DFDNodeType {
	UnVisited = 0,
	Dependency = 1, MinimalDependency = 2, CandidateDependency = 3,
	NonDependency = 4, MaximalNonDependency = 5, CandidateNonDependency = 6
};

using namespace std;
class DFDNode;
typedef vector<int> DFDKey;
typedef vector<DFDNode*> DFDNodeList;
typedef vector<DFDKey> DFDKeyList;
typedef unordered_map<DFDKey, DFDNode*> DFDTable;
namespace std
{
	template <>
	struct hash<DFDKey>
	{
		size_t operator()(const DFDKey &key) const
		{
			using std::size_t;
			using std::hash;
			int count = 0;
			int sum = 0;
			for (auto i : key) {
				sum += i << count++;
			}
			return hash<int>()(sum);
		}
	};

	template<>
	struct hash<vector<string>>
	{
		size_t operator()(const vector<string> &key) const
		{
			using std::size_t;
			using std::hash;
			string all;
			for (auto s : key)all += s;
			return hash<string>()(all);
		}
	};
}

class DFDNode {
private:
	DFDKey keys;
	DFDNodeType type;
	DFDKeyList SubSetList;
	DFDKeyList SuperSetList;
public:
	static DFDKey RHS;
	static DFDTable Category;
	static DFDKeyList RShip;
	static RTable *data;
	static void generateNextSeed(DFDKeyList &seeds);
	static DFDNode* getByKey(DFDKey key){
		auto findRes = Category.find(key);
		DFDNode *retValue = nullptr;
		if (findRes == Category.end()) {
			retValue = new DFDNode(key);
			Category.insert(make_pair(retValue->getKeys(), retValue));
		}
		else retValue = findRes->second;
		return retValue;
	}
	static int countPartitions(RTable &data){
		int rowSize = data[0].size();
		int colSize = data.size();
		unordered_set<vector<string>> setMap;
		for (int j = 0; j < rowSize; j++) {
			vector<string> newRow;
			for (int i = 0; i < colSize; i++) newRow.push_back(data[i][j]);
			setMap.insert(newRow);
		}
		return setMap.size();
	}
public:
	DFDNode(DFDKey key): keys(key) {
		type = UnVisited;
	}

	void initialize() {
		SuperSetList.clear();
		SubSetList.clear();
		setType(UnVisited);
	}

	DFDKey complement() {
		DFDKey dk;
		for (auto column : RShip) {
			if (column != RHS && column.size()) {
				if (!(*this >> column)) {
					dk.push_back(column[0]);
				}
			}
		}
		return dk;
	}

	DFDNode *pickNextNode();

	DFDKey getKeys() {
		return keys;
	}

	void setType(DFDNodeType get_type) {
		type = get_type;
	}

	void getDownSubsets() {
		if (keys.size() == 1 || keys.size() == 0)return;
		for (unsigned i = 0; i < keys.size(); i++) {
			auto SubSetKey = getSingleSubset(i, keys);
			SubSetList.push_back(SubSetKey);
		}
	}

	DFDKey getSingleSubset(int column, DFDKey key) {
		key.erase(key.begin() + column);
		return key;
	}

	void getUpSupersets() {
		for (auto column : RShip) {
			if (column != RHS && column.size()) {
				if (!(*this >> column)) {
					auto SuperSetKey = getSingleSuperset(column[0], keys);
					SuperSetList.push_back(SuperSetKey);
				}
			}
		}
	}

	DFDKey getSingleSuperset(int column, DFDKey key) {
		key.push_back(column);
		sort(key.begin(), key.end());
		return key;
	}

	void computePartitions() {
		if (keys.size() == 0)return;
		if ((*this) >> RHS)return;

		RTable tempData;
		for (auto k : keys)tempData.push_back((*data)[k]);
		int tempPartition = countPartitions(tempData);

		RTable tempLarger = tempData;
		tempLarger.push_back((*data)[RHS[0]]);
		int tempBigger = countPartitions(tempLarger);

		if (tempBigger == tempPartition)this->setType(CandidateDependency);
		else if (tempBigger > tempPartition)this->setType(CandidateNonDependency);
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
	void getFromCategory();
	bool operator>> (const DFDKey other);
	bool operator>> (const DFDNode* other);
};

void DFD(RTable & r, FDSet & FDs);
FDSet findLHSs(DFDKey RHS, DFDKeyList RShip);