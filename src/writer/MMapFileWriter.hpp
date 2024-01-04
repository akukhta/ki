#pragma once
#include "IFileWriter.hpp"
#include <string>
#include <atomic>
#include <fcntl.h>
#include <stdio.h>
#include <stdexcept>
#include <sys/mman.h>

template <class ChunkType>
class MMapFileWriter : public IFileWriter<ChunkType>
{
public:
    explicit MMapFileWriter(std::string fileName, size_t fileSize) 
        : fileName(std::move(fileName)), fileSize(fileSize), currentOffset(0), writeFinished(false)
    {}

    virtual void write(ChunkType buf) override
    {
        if (currentOffset + buf.size() > fileSize)
        {
            throw std::runtime_error("Writer: writing more than expected");
        }
        
        std::copy(buf.begin(), buf.end(), mmappedFile + currentOffset);

        currentOffset += buf.size();
    }

    virtual void create() override
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

        mmappedFile = static_cast<unsigned char*>(mmap(0, fileSize,
            PROT_READ | PROT_WRITE, MAP_SHARED, fileDesc, 0));
        
        if (mmappedFile == MAP_FAILED)
        {
            throw std::runtime_error("Writer: Can`t create a memory mapping of input file");
        }
    }

    virtual void finishWrite() override
    {
        writeFinished.store(true);
        fsync(fileDesc);
        close(fileDesc);
    }

    virtual bool isWriteFinished() override
    {
        return writeFinished.load();
    }
    
    virtual ~MMapFileWriter() {};

private:
    int fileDesc = -1;
    size_t fileSize = 0;

    unsigned char *mmappedFile = nullptr;

    size_t currentOffset = 0;
    
    std::atomic_bool writeFinished{false};

    std::string fileName{""};
};