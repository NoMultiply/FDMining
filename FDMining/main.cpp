#include <iostream>
#include "BitTane.h"
using namespace std;

int main() {
    for (size_t i = 0; i < 10; ++i) {
        BitTane tane("./BigData/data.txt", "./Result/TANE_result.txt");
        tane.run();
    }
    return 0;
}