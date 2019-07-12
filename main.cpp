#include "MemoryUnits.h"
#include "ExternalMerge.h"

int main() {
    ExternalMerge merge("input", "output", std::make_unique<MegaByteMemoryUnit>(124));
    return 0;
}
