#pragma once
#include "IFileWriter.hpp"
#include <string>
#include <atomic>
#include <fcntl.h>
#include <cstdio>
#include <stdexcept>
#include <sys/mman.h>

template <class ChunkType>
class MMapFileWriter : public IFileWriter<ChunkType>
{
public:
    explicit MMapFileWriter(std::string fileName, size_t fileSize) 
        : fileName(std::move(fileName)), fileSize(fileSize), currentOffset(0), writeFinished(false)
    {}

    void write(ChunkType buf) override
    {
        if (currentOffset + buf.size() > fileSize)
        {
            throw std::runtime_error("Writer: writing more than expected");
        }
        
        std::copy(buf.begin(), buf.end(), mmappedFile + currentOffset);

        currentOffset += buf.size();
    }

    void create() override
    {
        fileDesc = open(fileName.data(), O_RDWR | O_CREAT, 0644);

        if (fileDesc == -1)
        {
            throw std::runtime_error("can`t create a file");
        }

        if (ftruncate(fileDesc, fileSize) == -1)
        {
            throw std::runtime_error("can`t resize a file");
        }

        mmappedFile = static_cast<unsigned char*>(mmap(nullptr, fileSize,
            PROT_READ | PROT_WRITE, MAP_SHARED, fileDesc, 0));
        
        if (mmappedFile == MAP_FAILED)
        {
            throw std::runtime_error("Writer: Can`t create a memory mapping of input file");
        }
    }

    void finishWrite() override
    {
        if (mmappedFile)
        {
            writeFinished.store(true);
            fsync(fileDesc);
            munmap(mmappedFile, fileSize);
            close(fileDesc);
            mmappedFile = nullptr;
        }
    }

    bool isWriteFinished() override
    {
        return writeFinished.load();
    }
    
    virtual ~MMapFileWriter() = default;

private:
    int fileDesc = -1;
    size_t fileSize = 0;

    unsigned char *mmappedFile = nullptr;

    size_t currentOffset = 0;
    
    std::atomic_bool writeFinished{false};

    std::string fileName;
};