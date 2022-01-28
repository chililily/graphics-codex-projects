#include "OFFdata_test.h"

int main() {
    bool success = OFFdataTest::TestWrite("OFFdataTest.off");
    if (success) std::cout << "Successful";
}