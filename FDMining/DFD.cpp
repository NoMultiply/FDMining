#include "DFD.h"

using namespace std;

DFDNode * DFDNode::RHS = nullptr;
DFDTable DFDNode::Category;
void DFDNode::getFromCategory()
{
	for (auto item : Category) {
		if ((*this) >> item.second) {
			if (item.second->isDependency()) {
				this->type = Dependency;
				return;
			}
		}
		else if ((*item.second) >> this) {
			if (item.second->isNonDependency()) {
				this->type = NonDependency;
				return;
			}
		}
	}
}

bool DFDNode::operator>>(const DFDNode* other)
{
	int tsize = this->key.size(),osize = other->key.size();
	if (tsize < osize)return false;
	int t = 0, o = 0;
	while (t < tsize && o < osize) {
		if (this->key[t] == other->key[o])t++, o++;
		else if (this->key[t] < other->key[o])t++;
		else {
			return false;
		}
	}
	if (o >= osize) return true;

	return false;
}

void DFD(RTable & r, FDSet & FDs) {

	vector<RTable::iterator> DelWaitingList;
	DFDNodeList RHSList;
	auto beg = r.begin();
	auto end = r.end();
	int  rSize = r.size();

	for (auto A = beg; A != end; A++) {
			int key = A - beg;
			auto column = (*A);
			auto column_unique = set<string>(column.begin(), column.end());
			if (column.size() == column_unique.size()) {
				for (int i = 0; i < rSize; i++) FDs.push_back(FDUnit({ key }, i));
				DelWaitingList.push_back(A);
			}
			else {
				RHSList.push_back(new DFDNode({ key }));
			}
	}

	for (auto DelColumn : DelWaitingList) {
		r.erase(DelColumn);
	}

	for (auto RHS : RHSList) {
		FDSet getLHS = findLHSs(RHS,RHSList,r);
		FDs.insert(FDs.end(), getLHS.begin(), getLHS.end());
	}
}

FDSet findLHSs(DFDNode *RHS,DFDNodeList RShip,RTable &r) {
	FDSet result;
	auto RHSPos = find(RShip.begin(), RShip.end(), RHS);
	RShip.erase(RHSPos);
	auto seeds = RShip;
	DFDNode::RHS = RHS;
	while (!seeds.empty()) {
		auto node = seeds[0];
		while (node) {
			if (node->isVisited()) {

			}
			else {
				node->getFromCategory();
				if (!node->isVisited()) {
					node->computePartitions();
				}
			}
		}
	}
	return result;
}
