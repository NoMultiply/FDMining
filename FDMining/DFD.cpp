#include "DFD.h"

DFDKey DFDNode::RHS;
DFDTable DFDNode::Category;
DFDKeyList DFDNode::RShip;
DFDKeyList minDeps;
DFDKeyList maxNonDeps;
RTable* DFDNode::data = nullptr;
stack<DFDNode*> DFDTrace;
typedef bitset<32> BITSET32;

// generate new seeds list
bool compare(BITSET32 a, BITSET32 b) {
	return a.to_ulong() >  b.to_ulong();
}

void DFDNode::generateNextSeed(DFDKeyList &seeds)
{
	vector<BITSET32> container;
	vector<BITSET32> initial;
	if (!maxNonDeps.size())return;
	for (auto maxNonDep : maxNonDeps) {
		auto _maxNonDep_ = Category.find(maxNonDep)->second->complement();
		if (!initial.size()) {
			for (auto i : _maxNonDep_) {
				BITSET32  newBit;
				newBit.set(i);
				initial.push_back(newBit);
			}
		} else {
			for (auto dep : initial) {
				for (auto i : _maxNonDep_) {
					BITSET32 newBit;
					newBit.set(i);
					container.push_back(dep | newBit);
				}
			}
			
			sort(container.begin(), container.end(),compare);
			for (size_t i = 0; i < container.size(); i++) {
				for (size_t j = i + 1; j < container.size(); j++) {
					if ((container[i] & container[j]) == container[i]) {
						container.erase(container.begin() + j);
						j--;
						continue;
					}
				}
			}

			initial.clear();
			initial = container;
			container.clear();
		}
	}

	for (auto newSeed : initial) {
		DFDKey newKey;
		string bitString = newSeed.to_string();
		size_t bitLen = bitString.length() - 1;
		for (size_t i = bitLen ; i <= 31; i--) {
			if (bitString[i] == '1') {
				newKey.push_back(31 - i);
			}
		}
		if ((find(minDeps.begin(), minDeps.end(), newKey) == minDeps.end())) {
			seeds.push_back(newKey);
		}
	}
	auto u_begin = unique(seeds.begin(), seeds.end());
	seeds.erase(u_begin, seeds.end());
	initial.clear();
}

// pick the next node to traverse
DFDNode* DFDNode::pickNextNode() {
	if (isCandidateDependency()) {
		if (!this->SubSetList.size()) {
			this->getDownSubsets();
		}
		if (this->SubSetList.size()) {
			for (auto s : SubSetList) {
				DFDNode *uncheckedNode = getByKey(s);
				if (uncheckedNode && uncheckedNode->isVisited())continue;

				uncheckedNode->getFromCategory();
				if (uncheckedNode->isNonDependency())continue;
				else {
					DFDTrace.push(this);
					return uncheckedNode;
				}
			}
		}
		this->setType(MinimalDependency);
		minDeps.push_back(this->getKeys());
	}
	else if (isCandidateNonDependency()) {
		if (!this->SuperSetList.size()) {
			this->getUpSupersets();
		}

		if (this->SuperSetList.size()) {
			for (auto s : SuperSetList) {
				DFDNode *uncheckedNode = getByKey(s);
				if (uncheckedNode && uncheckedNode->isVisited())continue;

				uncheckedNode->getFromCategory();
				if (uncheckedNode->isDependency())continue;
				else {
					DFDTrace.push(this);
					return uncheckedNode;
				}
			}
		}
		this->setType(MaximalNonDependency);
		maxNonDeps.push_back(this->getKeys());
	}
	
	if (!DFDTrace.empty()){
		auto top = DFDTrace.top();
		DFDTrace.pop();
		return top;
	}
	return nullptr;
}

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
bool DFDNode::operator>>(const DFDKey other)
{
	int tsize = this->keys.size(), osize = other.size();
	if (tsize < osize)return false;
	int t = 0, o = 0;
	while (t < tsize && o < osize) {
		if (this->keys[t] == other[o])t++, o++;
		else if (this->keys[t] < other[o])t++;
		else {
			return false;
		}
	}
	if (o >= osize) return true;

	return false;
}

bool DFDNode::operator>>(const DFDNode* other)
{
	int tsize = this->keys.size(),osize = other->keys.size();
	if (tsize < osize)return false;
	int t = 0, o = 0;
	while (t < tsize && o < osize) {
		if (this->keys[t] == other->keys[o])t++, o++;
		else if (this->keys[t] < other->keys[o])t++;
		else {
			return false;
		}
	}
	if (o >= osize) return true;

	return false;
}

bool DFDNode::isMinimal()
{
	if (this->type == MinimalDependency)return true;

	if (!this->SubSetList.size()) {
		this->getDownSubsets();
	}
	
	if (!this->SubSetList.size()) {
		this->setType(MinimalDependency);
		return true;
	}

	for (auto key : SubSetList) {
		auto subset = Category.find(key);
		if (subset == Category.end())return false;
		if (subset->second->isDependency()) {
			this->setType(Dependency);
			return false;
		}
	}

	this->setType(MinimalDependency);
	return true;
}

bool DFDNode::isMaximal()
{
	if (this->type == MaximalNonDependency)return true;
	if (!this->SuperSetList.size()) {
		this->getUpSupersets();
	}

	if (!this->SuperSetList.size()) {
		this->setType(MaximalNonDependency);
		return true;
	}

	for (auto key : SuperSetList) {
		auto superset = Category.find(key);
		if (superset == Category.end())return false;
		if (superset->second->isNonDependency()) {
			this->setType(NonDependency);
			return false;
		}
	}
	this->setType(MaximalNonDependency);
	return true;
}

void DFD(RTable & r, FDSet & FDs) {
	DFDNode::data = &r;
	DFDKeyList RHSKeyList;
	auto beg = r.begin();
	auto end = r.end();
	int  rSize = r.size();

	for (auto A = beg; A != end; A++) {
			int key = A - beg;
			//auto column = (*A);
			//auto column_unique = set<string>(column.begin(), column.end());
			//if (column.size() == column_unique.size()) {
			//	for (int i = 0; i < rSize; i++) FDs.push_back(FDUnit({ key }, i));
			//} else {
				DFDKey newNode = { key };
				RHSKeyList.push_back(newNode);
			//}
	}

	for (auto RHS : RHSKeyList) {
		FDSet getLHS = findLHSs(RHS,RHSKeyList);
		FDs.insert(FDs.end(), getLHS.begin(), getLHS.end());
	}
	for (unsigned f = 0; f < FDs.size(); f++) {
		for (unsigned i = 0; i < FDs[f].first.size(); i++) {
			FDs[f].first[i] += 1;
		}
		FDs[f].second += 1;
	}
}

FDSet findLHSs(DFDKey RHS,DFDKeyList RShip) {
	// clear last rhs data
	for (auto rs : DFDNode::Category) {
		rs.second->initialize();
	}
	DFDNode::RShip = RShip;
	DFDNode::RHS = RHS;
	minDeps.clear();
	maxNonDeps.clear();
	// initialize
	FDSet result;
	auto RHSPos = find(RShip.begin(), RShip.end(), RHS);
	RShip.erase(RHSPos);
	auto seeds = RShip;
	int maxLast = INT_MIN, minLast = INT_MAX;
	//main loop pick seed
	while (!seeds.empty()) {
		auto node = DFDNode::getByKey(seeds[0]);
		seeds.erase(seeds.begin());

		while (node) {
			if (node->isVisited()) {
				if (node->isCandidateDependency()) {
					if (node->isMinimal()) {
						minDeps.push_back(node->getKeys());
					}
				}
				else if (node->isCandidateNonDependency()) {
					if (node->isMaximal()) {
						maxNonDeps.push_back(node->getKeys());
					}
				}
			} else {
				node->getFromCategory();
				if (!node->isVisited()) {
					node->computePartitions();
				}
			}
			node = node->pickNextNode();
		}
		if (seeds.empty()) {
			if (maxLast == maxNonDeps.size() && minLast == minDeps.size())break;
			maxLast = maxNonDeps.size();
			minLast = minDeps.size();
			DFDNode::generateNextSeed(seeds);
		}
	}

	for (auto item : minDeps) {
		result.push_back(FDUnit(item, RHS[0]));
	}
	return result;
}

