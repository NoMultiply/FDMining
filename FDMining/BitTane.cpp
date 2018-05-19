#include "BitTane.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

using namespace std;

#define BUF_SIZE 1000
#define UNIT_SIZE 100

BitTane::BitTane(string infile, string outfile)
    : m_infile(infile), m_outfile(outfile)
{
}

void BitTane::run()
{
    clock_t start = clock();
    loadData();
    tane();
    writeData();
    clock_t end = clock();
    cout << "Run Time: " << double(end - start) / CLOCKS_PER_SEC << "s\n";
}

void BitTane::loadData()
{
    // clock_t start = clock();
    fstream fin(m_infile, fstream::in);
    if (!fin.is_open()) {
        cout << "Error: Open file " << m_infile << " failed!" << endl;
        return;
    }
    nC = 0;
    vector<string> col;
    vector<unordered_map<string, pair<size_t, bool>>> T;
    unordered_map<AttrSet, RowVector> tPi;
    char buf[BUF_SIZE];
    char unit[UNIT_SIZE];
    size_t len = 0;
    size_t c;
    size_t bufL = 0;
    size_t i;
    while (!fin.eof()) {
        fin.getline(buf, BUF_SIZE);
        bufL = fin.gcount();
        if (bufL == 0)
            continue;
        buf[bufL++] = ',';
        buf[bufL++] = '#';
        buf[bufL] = '\0';
        ++nC;
        c = 0;
        for (size_t i = 0; i < bufL; ++i) {
            if (buf[i] == ',' && buf[i + 1] != ' ') {
                unit[len] = '\0';
                col.push_back(unit);
                len = 0;
            }
            else {
                unit[len++] = buf[i];
            }
        }
        R = (1 << col.size()) - 1;
        nR = col.size();
        T = vector<unordered_map<string, pair<size_t, bool>>>(nR, {});
        int m = 0;
        for (i = 0; i < nR; ++i) {
            m = 1 << i;
            tPi[m] = { nC };
            setMap[m] = i + 1;
            T[i][col[i]] = { tPi[m].size() - 1, false };
            E[m] = 0;
        }
        break;
    }
    int m = 0;
    while (!fin.eof()) {
        fin.getline(buf, BUF_SIZE);
        bufL = fin.gcount();
        if (bufL == 0)
            continue;
        buf[bufL++] = ',';
        buf[bufL++] = '#';
        buf[bufL] = '\0';
        ++nC;
        len = 0;
        c = 0;
        for (i = 0; i < bufL; ++i) {
            if (buf[i] == ',' && buf[i + 1] != ' ') {
                unit[len] = '\0';
                m = 1 << c;
                auto iT = T[c].find(unit);
                if (iT == T[c].end()) {
                    tPi[m].push_back(nC);
                    T[c][unit] = { tPi[m].size() - 1, false };
                }
                else {
                    ++E[m];
                    auto & u = (*iT).second;
                    if (u.second == false) {
                        u.second = true;
                        Pi[m].push_back({ tPi[m][u.first], nC });
                        u.first = Pi[m].size() - 1;
                    }
                    else {
                        Pi[m][u.first].push_back(nC);
                    }
                }
                ++c;
                len = 0;
            }
            else {
                unit[len++] = buf[i];
            }
        }
    }
    // clock_t end = clock();
    // cout << "Load Data Run Time: " << double(end - start) / CLOCKS_PER_SEC << "s\n";
}

void BitTane::writeData()
{
    // clock_t start = clock();
    AttrSet i, t;
    sort(FDs.begin(), FDs.end());
    fstream fout(m_outfile, fstream::out);
    if (!fout.is_open()) {
        cout << "Error: Open file " << m_outfile << " failed!" << endl;
        return;
    }
    for (auto FD : FDs) {
        for (t = FD.first; t != 0; t &= (t - 1)) {
            i = t ^ (t & (t - 1));
            fout << setMap[i] << " ";
        }
        fout << "-> " << setMap[FD.second] << endl;
    }
    // clock_t end = clock();
    // cout << "Write Data Time Cost: " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    cout << "FDs Num: " << FDs.size() << ", ";
}

void BitTane::tane()
{
    L.push_back({});
    L.push_back({});
    // cout << "R.count(): " << nR << endl;
    for (size_t i = 0; i < nR; ++i) {
        L[1].insert({ 1 << i });
    }
    l = 1;
    C[{}] = R;
    while (!L[l].empty()) {
        // cout << "level " << l << ": " << L[l].size() << endl;
        // cout << "Calculating level " << l << endl;
        compute_dependencies(L[l]);
        prune(L[l]);
        L.push_back({});
        generate_next_level(L[l], L[l + 1]);
        ++l;
    }
}

void BitTane::compute_dependencies(AttrSetSet & L)
{
    // clock_t start = clock();
    AttrSet X_A;
    for (auto X : L) {
        auto t = X;
        C[X] = C[X & (~(t ^ (t & (t - 1))))];
        for (t &= (t - 1); t != 0; t &= (t - 1)) {
            X_A = X & (~(t ^ (t & (t - 1))));
            C[X] &= C[X_A];
        }
    }
    if (l <= 1)
        return;
    AttrSet X_CX, A;
    for (auto X : L) {
        X_CX = X & C[X];
        for (auto t = X_CX; t != 0; t &= (t - 1)) {
            A = t ^ (t & (t - 1));
            X_A = X & (~A);
            // X\{A} -> A is valid
            if (E[X_A] == E[X]) {
                FDs.push_back({ X_A, A });
                C[X] &= ~(A | (R & (~X)));
            }
        }
    }
    // clock_t end = clock();
    // cout << "Compute Dependencies Run Time: " << double(end - start) / CLOCKS_PER_SEC << "s\n";
}

void BitTane::prune(AttrSetSet & L)
{
    // clock_t start = clock();
    AttrSet CX_D_X, A, X_U_A, X_U_A_D_B, tR;
    for (auto iX = L.begin(); iX != L.end();) {
        auto & X = (*iX);
        if (C[X] == 0) {
            // cout << "Yean! Prune!" << endl;
            L.erase(iX++);
        }
        // X is a (super)key
        else if (E[X] == 0) {
            // else if (m_cutSuperKey && e(X) == 0) {
            // else if (false) {
            CX_D_X = C[X] & (~X);
            for (auto t = CX_D_X; t != 0; t &= (t - 1)) {
                A = t ^ (t & (t - 1));
                X_U_A = X | A;
                auto k = X;
                X_U_A_D_B = X_U_A & (~(k ^ (k & (k - 1))));
                C[X_U_A_D_B] = calC(X_U_A_D_B);
                tR = C[X_U_A_D_B];
                for (k &= (k - 1); k != 0; k &= (k - 1)) {
                    X_U_A_D_B = X_U_A & (~(k ^ (k & (k - 1))));
                    C[X_U_A_D_B] = calC(X_U_A_D_B);
                    tR &= C[X_U_A_D_B];
                }
                if ((tR & A) != 0) {
                    FDs.push_back({ X, A });
                }
            }
            L.erase(iX++);
        }
        else {
            ++iX;
        }
    }
    // clock_t end = clock();
    // cout << "Prune Run Time: " << double(end - start) / CLOCKS_PER_SEC << "s\n";
}

// clock_t ta, tb;

void BitTane::generate_next_level(AttrSetSet & curL, AttrSetSet & nextL)
{
    // clock_t start = clock();
    AttrSetVectorVector pBlocks;
    prefix_blocks(curL, pBlocks);
    AttrSet X, X_D_A, t, ti, tj;
    // ta = 0, tb = 0;
    for (const AttrSetVector & K : pBlocks) {
        auto end = K.end();
        for (auto i = K.begin(); i != end; ++i) {
            auto j = i;
            ti = (*i);
            for (++j; j != end; ++j) {
                tj = (*j);
                X = ti | tj;
                for (t = X; t != 0; t &= (t - 1)) {
                    X_D_A = X & (~(t ^ (t & (t - 1))));
                    if (curL.find(X_D_A) == curL.end()) {
                        break;
                    }
                }
                if (t != 0)
                    continue;
                calStrippedPi(ti, tj, X);
                nextL.insert(X);
            }
        }
    }
    // clock_t end = clock();
    // cout << "ta: " << double(ta) / CLOCKS_PER_SEC << "s, tb: " << double(tb) / CLOCKS_PER_SEC << "s\n";
    // cout << "Generate Next Level Run Time: " << double(end - start) / CLOCKS_PER_SEC << "s\n";
}

void BitTane::calStrippedPi(const AttrSet & A, const AttrSet & B, const AttrSet & X)
{
    RowVectorVector & Pi_A = Pi[A];
    RowVectorVector & Pi_B = Pi[B];
    RowVectorVector & Pi_X = Pi[X];
    static vector<RowVector> S(nC + 1, {});
    size_t tI = 1, size;
    if (Ts.find(A) == Ts.end()) {
        Ts[A] = vector<Row>(nC + 1, 0);
        vector<Row> & T = Ts[A];
        for (const auto & c : Pi_A) {
            for (const auto t : c) {
                T[t] = tI;
            }
            ++tI;
        }
    }
    vector<Row> & T = Ts[A];
    E[X] = 0;
    for (const auto & c : Pi_B) {
        for (const auto t : c) {
            if (T[t] != 0) {
                S[T[t]].push_back(t);
            }
        }
        for (const auto t : c) {
            if (T[t] == 0)
                continue;
            auto & st = S[T[t]];
            if ((size = st.size()) >= 2) {
                Pi_X.push_back(st);
                E[X] += size - 1;
            }
            st.clear();
        }
    }
}

void BitTane::prefix_blocks(AttrSetSet & L, AttrSetVectorVector & pBlocks)
{
    // clock_t start = clock();
    if (L.empty())
        return;
    pBlocks = { { *L.begin() } };
    auto i = L.begin();
    for (++i; i != L.end(); ++i) {
        if (prefix_equal(pBlocks.back().back(), *i)) {
            pBlocks.back().push_back(*i);
        }
        else {
            pBlocks.push_back({ *i });
        }
    }
    // clock_t end = clock();
    // cout << "Prefix Blocks Run Time: " << double(end - start) / CLOCKS_PER_SEC << "s\n";
}

inline AttrSet BitTane::calC(const AttrSet & X)
{
    if (C.find(X) != C.end())
        return C[X];
    C[X] = R;
    for (auto t = X; t != 0; t &= (t - 1)) {
        C[X] &= calC(X & (~(t ^ (t & (t - 1)))));
    }
    return C[X];
}
