#include "MemoryUnits.h"
#include "ExternalMerge.h"

int main() {
    ExternalMerge merge("small.in", "small.out", std::make_unique<MegaByteMemoryUnit>(128));
    return 0;
}
