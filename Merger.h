#ifndef MERGER_H
#define MERGER_H
#include <list>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstdio>

namespace {
const std::string outputChunkPrefix{"outputChunk"};
} //namespace

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
        makeRawMerge(&firstFile, &secondFile, &outputFile);
        return true;
    }
    void makeRawMerge(std::ifstream *firstFile, std::ifstream *secondFile, std::ofstream *outputFile) const {
        firstFile->seekg(0, firstFile->beg);
        secondFile->seekg(0, secondFile->beg);
        uint64_t filesSize = fileSize(firstFile) + fileSize(secondFile);
        uint64_t itemsCount = filesSize/static_cast<uint64_t>(sizeof(uint32_t));
        uint32_t first = 0, second = 0;
        firstFile->read(reinterpret_cast<char*>(&first), static_cast<std::streamsize>(sizeof(uint32_t)));
        secondFile->read(reinterpret_cast<char*>(&second), static_cast<std::streamsize>(sizeof(uint32_t)));
        while(itemsCount > 0) {
            if(firstFile->eof() ||
                    first >= second) {
                writeData(second, outputFile);
                itemsCount--;
                secondFile->read(reinterpret_cast<char*>(&second), static_cast<std::streamsize>(sizeof(uint32_t)));
            } else if(secondFile->eof() ||
                      first < second) {
                writeData(first, outputFile);
                itemsCount--;
                firstFile->read(reinterpret_cast<char*>(&first), static_cast<std::streamsize>(sizeof(uint32_t)));
            }
        }
    }
    void writeData(uint32_t value, std::ofstream *outputFile) const {
        outputFile->write(reinterpret_cast<const char *>(&value), static_cast<std::streamsize>(sizeof (uint32_t)));
    }

    uint64_t fileSize(std::ifstream *stream) const {
        if(!stream->is_open()) {
            return 0;
        }
        stream->seekg(0, stream->end);
        uint64_t fileSize = static_cast<uint64_t>(stream->tellg());
        stream->seekg(0, stream->beg);
        return fileSize;
    }
};

#endif // MERGER_H
