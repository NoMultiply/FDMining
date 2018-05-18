#include <iostream>
#include "FDMining.h"
#include "TANE.h"
#include "DFD.h"
using namespace std;

void printLine() {
    cout << "--------------------" << endl;
}

int main() {
    RTable r;
    FDSet FDs_TANE, FDs_DFD;

    printLine();
    gb_loadDataFromFile("./BigData/data.txt", r);
    printLine();
    cout << endl;

    printLine();
    cout << "TANE" << endl;
    printLine();
    gb_runMinerWithTimeCost(TANE, r, FDs_TANE);
    gb_writeResultToFile("./Result/TANE_result.txt", FDs_TANE);
    printLine();
    cout << endl;

    printLine();
    cout << "DFD" << endl;
    printLine();
    gb_runMinerWithTimeCost(DFD, r, FDs_DFD);
    gb_writeResultToFile("./Result/DFD_result.txt", FDs_DFD);
    printLine();
    cout << endl;

    return 0;
}