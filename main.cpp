#include "MemoryUnits.h"
#include "ExternalMerge.h"
#include <exception>
#include <iostream>

int main() {
    try {
        ExternalMerge merge("input", "output", std::make_unique<MegaByteMemoryUnit>(124));
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
