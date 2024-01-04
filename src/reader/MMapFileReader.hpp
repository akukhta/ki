#pragma once
#include "IFileReader.hpp"
#include <string>
#include <unistd.h>
#include <atomic>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdexcept>
#include <algorithm>
#include <filesystem>

template <class ChunkType>
class MMapFileReader : public IFileReader<ChunkType>
{
public:
    explicit MMapFileReader(std::string fileName) : fileName(std::move(fileName))
    {
        fileDesc = ::open(this->fileName.data(), O_RDONLY, S_IRUSR);
    
        if (fileDesc == -1)
        {
            throw std::runtime_error("can`t open src file");
        }    

        fileSize = std::filesystem::file_size(this->fileName);
    }

    virtual void open() override
    {
        mmappedFile = static_cast<unsigned char*>(mmap(0, fileSize, PROT_READ, MAP_SHARED, fileDesc, 0));

        if (mmappedFile == MAP_FAILED)
        {
            throw std::runtime_error("Reader: Can`t create a memory mapping of input file");
        }

        if (fileSize < currentOffset)
        {
            readFinished.store(false);
        }
    }

    virtual ChunkType read() override
    {
        ChunkType buf;
        buf.reserve(defaultBufferSize);
        
        auto readCount = std::min(currentOffset + defaultBufferSize, fileSize);

        std::copy(mmappedFile + currentOffset, mmappedFile + readCount, std::back_inserter(buf));

        currentOffset = readCount;

        if (currentOffset >= fileSize)
        {
            readFinished.store(true);
        }

        return buf;
    }

    virtual bool isReadFinished() const override
    {
        return readFinished.load();
    }

    virtual void finishRead() override
    {
        munmap(mmappedFile, fileSize);
        close(fileDesc);
    }
    
    size_t getFileSize()
    {
        return fileSize;
    }

    ~MMapFileReader(){}

    static inline size_t defaultBufferSize = sysconf(_SC_PAGESIZE);

private:
    int fileDesc = -1;
    size_t fileSize = 0;
    unsigned char *mmappedFile;

    size_t currentOffset = 0;
    std::atomic_bool readFinished{false};

    std::string fileName{""};
};