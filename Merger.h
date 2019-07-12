#ifndef MERGER_H
#define MERGER_H
#include <list>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstdio>
#include <vector>

namespace {
const std::string outputChunkPrefix{"outputChunk"};
} //namespace

struct MergerHelper {
    uint32_t value;
    std::ifstream *sourceFile;
};

class Merger {
public:
    Merger(const std::string &outputFileName, const std::list<std::string> &chunkNames) {
        if(chunkNames.empty() || outputFileName.empty()) {
            return;
        }
        if(chunkNames.size() == 1) {
            std::rename(chunkNames.front().c_str(), outputFileName.c_str());
            return;
        }
        mergeFiles(outputFileName, chunkNames, 0);
    }
private:
    std::list<std::string> mergeFiles(const std::string &outputFileName, const std::list<std::string> &chunkNames, int iteration) {
        if(chunkNames.size() == 2) {
            merge({chunkNames.front(), chunkNames.back()}, outputFileName);
            removeFile(chunkNames.front());
            removeFile(chunkNames.back());
            return {outputFileName};
        }
        auto files = chunkNames;
        int chunkNumber = iteration;
        std::list<std::string> outputChunksNames;
        while(!files.empty()) {
            auto first = files.front();
            auto second = files.back();
            if(first == second) {
                outputChunksNames.emplace_back(first);
                return mergeFiles(outputFileName, outputChunksNames, chunkNumber++);
            }
            auto chunkName = outputChunkPrefix + std::to_string(chunkNumber++);
            if(!merge({first, second}, chunkName)) {
                return outputChunksNames;
            }
            removeFile(first);
            removeFile(second);
            outputChunksNames.emplace_back(chunkName);
            files.pop_front();
            files.pop_back();
        }
        return mergeFiles(outputFileName, outputChunksNames, chunkNumber++);
    }
    void removeFile(const std::string &fileName) {
        if(std::remove(fileName.c_str()) != 0) {
            std::cout << "Could not remove chunk file "<< fileName << std::endl;
        }
    }
    bool merge(const std::pair<std::string, std::string> &inputFiles, const std::string &outputFileName) const {
        std::ifstream firstFile(inputFiles.first, std::ios_base::binary);
        std::ifstream secondFile(inputFiles.second, std::ios_base::binary);
        if(!firstFile.is_open() || !secondFile.is_open()) {
            std::cout << "Could not open one of input files" << std::endl;
            return false;
        }
        std::ofstream outputFile;
        outputFile.open(outputFileName, std::ios_base::binary);
        std::merge(std::istreambuf_iterator<char>(firstFile), std::istreambuf_iterator<char>(),
                   std::istreambuf_iterator<char>(secondFile), std::istreambuf_iterator<char>(),
                   std::ostreambuf_iterator<char>(outputFile));

        return true;
    }
    void writeData(uint32_t value, std::ofstream *outputFile) const {
        outputFile->write(reinterpret_cast<const char *>(&value), static_cast<std::streamsize>(sizeof (uint32_t)));
    }
};

#endif // MERGER_H
