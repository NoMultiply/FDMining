#include "TANE.h"

using namespace std;

void TANE(RTable & r, FDSet & FDs) {
    FDs = {
        FDUnit({ 1 }, 3),
        FDUnit({ 2, 5 }, 1)
    };
}
