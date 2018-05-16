#include "DFD.h"

using namespace std;

void DFD(RTable & r, FDSet & FDs) {
    //如果要将算法函数改成算法类，也行
    FDs = {
        FDUnit({ 1 }, 3),
        FDUnit({ 2, 5}, 1)
    };
}