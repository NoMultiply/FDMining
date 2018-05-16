#include "FDMining.h"
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

bool gb_loadDataFromFile(string filename, RTable & r) {
    clock_t start = clock();
    fstream fin(filename, fstream::in);
    if (!fin.is_open()) {
        cout << "Error: Open file " << filename << " failed!" << endl;
        return false;
    }
    bool first = true;
    while (!fin.eof()) {
        string buf;
        getline(fin, buf);
        if (buf.empty())
            continue;
        buf += ',';
        string unit;
        size_t row = 0;
        for (size_t i = 0; i < buf.size(); ++i) {
            if (buf[i] == ',') {
                if (row >= r.size()) {
                    if (first)
                        r.push_back(vector<string>());
                    else {
                        cout << "Error: row >= r.size()" << endl;
                        return false;
                    }
                }
                r[row++].push_back(unit);
                unit.clear();
            }
            else {
                unit += buf[i];
            }
        }
        first = false;
    }
    clock_t end = clock();
    cout << "Load Data Time Cost: " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    cout << "Attribute Num: " << r.size() << endl;
    cout << "Row Num: " << (r.empty() ? 0 : r[0].size()) << endl;
    return true;
}

void gb_runMinerWithTimeCost(void miner(RTable &, FDSet &), RTable & r, FDSet & FDs) {
    clock_t start = clock();
    miner(r, FDs);
    clock_t end = clock();
    cout << "Run Miner Time Cost: " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
}

bool gb_writeResultToFile(string filename, FDSet & FDs, bool printOnConsole) {
    clock_t start = clock();
    fstream fout(filename, fstream::out);
    if (!fout.is_open()) {
        cout << "Error: Open file " << filename << " failed!" << endl;
        return false;
    }
    for (auto FD : FDs) {
        for (auto i : FD.first) {
            fout << i << " ";
            if (printOnConsole) {
                cout << i << " ";
            }
        }
        if (printOnConsole) {
            fout << "-> " << FD.second << endl;
            cout << "-> " << FD.second << endl;
        }
    }
    clock_t end = clock();
    cout << "Write Data Time Cost: " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    cout << "FDs Num: " << FDs.size() << endl;
    return true;
}
