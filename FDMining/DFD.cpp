#include "DFD.h"

using namespace std;

void DFD(RTable & r, FDSet & FDs) {
    //���Ҫ���㷨�����ĳ��㷨�࣬Ҳ��
    FDs = {
        FDUnit({ 1 }, 3),
        FDUnit({ 2, 5}, 1)
    };
}