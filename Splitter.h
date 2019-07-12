#ifndef SPLITTER_H
#define SPLITTER_H
#include <memory>
#include "MemoryUnits.h"
#include <list>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

namespace {
const std::string chunkFilePrefix{"chunk_"};
}
class Splitter
{
public:
    Splitter(std::unique_ptr<MemoryUnit> &&memoryUnits, const std::string &inputFile)
        : mUnits(std::move(memoryUnits)),
          mInputFileName(inputFile) {
        mIsValid = mUnits != nullptr &&
                !mInputFileName.empty();
        if(isValid()) {
            splitInputFile();
        }
    }
    bool isValid() const {
        return mIsValid;
    }
    std::list<std::string> chunkFiles() const {
        return mChunkFiles;
    }
private:
    void splitInputFile() {
        if(!isValid()) {
            return;
        }
        std::ifstream stream(mInputFileName, std::ios_base::binary);
        auto inputFileSize = fileSize(&stream);
        if(inputFileSize == 0) {
            std::cout << "Invalid input file size" << std::endl;
            mIsValid = false;
            return;
        }
        auto availableMemory = mUnits->memoryInBytes();
        auto chunksCount = inputFileSize/availableMemory;
        if(inputFileSize % availableMemory > 0) {
            chunksCount += 1;
        }
        auto itemsCount = static_cast<size_t>(availableMemory/sizeof(uint32_t));
        std::vector<uint32_t> data;
        data.resize(itemsCount);
        int chunkNumber = 0;
        while(stream) {
            stream.read(reinterpret_cast<char*>(data.data()),
                        static_cast<std::streamsize>(data.size()* sizeof (uint32_t)));
            auto count = static_cast<size_t>(stream.gcount());
            if (count < data.size()* sizeof (uint32_t)) {
                data.resize(count / sizeof(uint32_t));
            }
            if(count == 0) {
                break;
            }
            if(!data.empty()) {
                std::sort(data.begin(), data.end());
                writeDataToChunkFile(chunkFilePrefix + std::to_string(chunkNumber++), &data);
            }
        }
    }
    uint64_t fileSize(std::ifstream *stream) const {
        if(!stream->is_open()) {
            std::cout << "Could not open " << mInputFileName << std::endl;
            return 0;
        }
        stream->seekg(0, stream->end);
        auto fileSize = static_cast<uint64_t>(stream->tellg());
        stream->seekg(0, stream->beg);
        return fileSize;
    }
    void writeDataToChunkFile(const std::string &chunkFileName, std::vector<uint32_t> *data) {
        if(data == nullptr || chunkFileName.empty()) {
            return;
        }
        std::ofstream chunkFile;
        chunkFile.open(chunkFileName, std::ios_base::binary);
        chunkFile.write(reinterpret_cast<const char *>(data->data()),
                        static_cast<std::streamsize>(data->size()* sizeof (uint32_t)));
        mChunkFiles.emplace_back(chunkFileName);
    }
private:
    std::unique_ptr<MemoryUnit> mUnits;
    std::list<std::string> mChunkFiles;
    std::string mInputFileName;
    bool mIsValid;
};

#endif // SPLITTER_H
