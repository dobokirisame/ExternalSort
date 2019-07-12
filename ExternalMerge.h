#ifndef EXTERNALMERGE_H
#define EXTERNALMERGE_H
#include <string>
#include <memory>
#include "MemoryUnits.h"
#include "Splitter.h"
#include "Merger.h"

class ExternalMerge {
public:
    ExternalMerge(const std::string &inputFileName, const std::string &outputFileName, std::unique_ptr<MemoryUnit> &&memoryUnits) {
        Splitter splitter(std::move(memoryUnits), inputFileName);
        Merger merger(outputFileName, splitter.chunkFiles());
    }
};

#endif // EXTERNALMERGE_H


