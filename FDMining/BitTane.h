#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <list>

typedef size_t Attr;
typedef size_t Row;
typedef int AttrSet;
typedef std::set<AttrSet> AttrSetSet;
typedef std::vector<AttrSetSet> AttrSetSetVector;
typedef std::vector<Row> RowVector;
typedef std::list<RowVector> RowVectorList;
typedef std::vector<RowVector> RowVectorVector;
typedef std::vector<AttrSet> AttrSetVector;
typedef std::vector<AttrSetVector> AttrSetVectorVector;
typedef std::pair<AttrSet, AttrSet> FDUnit;
typedef std::vector<FDUnit> FDSet;

class BitTane {
public:
    BitTane(std::string infile, std::string outfile);

public:
    void run();

private:
    void loadData();
    void writeData();
    void tane();
    void compute_dependencies(AttrSetSet & L);
    void prune(AttrSetSet & L);
    void generate_next_level(AttrSetSet & L, AttrSetSet & nextL);
    void calStrippedPi(const AttrSet & A, const AttrSet & B, const AttrSet & X);
    void prefix_blocks(AttrSetSet & L, AttrSetVectorVector & pBlocks);

private:
    /*inline AttrSet extract(const AttrSet & t) {
        return t ^ (t & (t - 1));
    }*/
    /*inline void next(AttrSet & t) {
        t &= (t - 1);
    }*/
    inline bool prefix_equal(const AttrSet & a, const AttrSet & b) {
        return (a & (a - 1)) == (b & (b - 1));
    }
    //inline size_t e(const AttrSet & X)
    //{
    //    // I THINK IT DOSN'T MATTER NOT TO DIV |r|
    //    if (E.find(X) != E.end()) {
    //        return E[X];
    //    }
    //    size_t t = 0;
    //    RowVectorVector & Pi_X = Pi[X];
    //    for (auto & C : Pi_X) {
    //        t += C.size();
    //    }
    //    E[X] = (t -= Pi_X.size());
    //    return t;
    //}
    //inline bool isValid(const AttrSet & X, const AttrSet & X_U_A)
    //{
    //    return e(X) == e(X_U_A);
    //}
    inline AttrSet calC(const AttrSet & X);

private:
    std::string m_infile;
    std::string m_outfile;
    std::unordered_map<AttrSet, RowVectorVector> Pi;
    AttrSetSetVector L;
    std::unordered_map<AttrSet, AttrSet> C;
    int l;
    FDSet FDs;
    size_t nR;
    size_t nC;
    AttrSet R;
    std::unordered_map<AttrSet, size_t> E;
    std::unordered_map<AttrSet, std::vector<Row>> Ts;
    std::unordered_map<AttrSet, Attr> setMap;
};