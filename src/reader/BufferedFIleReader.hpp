#pragma once
#include "IFileReader.hpp"
#include "../queue/BufferedQueue.hpp"
#include <filesystem>
#include <cstdio>

template <class ChunkType>
class BufferedReader;

template <>
class BufferedReader<Buffer>: public IFileReader<Buffer>
{
public:
    explicit BufferedReader(std::string fileName, std::shared_ptr<FixedBufferQueue<Buffer>> queue) : fileName(std::move(fileName))
    {
        fileSize = std::filesystem::file_size(this->fileName);
    }

    virtual void open() override
    {
        fileDesc = std::fopen(fileName.c_str(), "rb");

        if (fileDesc == nullptr)
        {
            throw std::runtime_error("Reader: Can`t open a file");
        }

        if (fileSize < currentOffset)
        {
            readFinished.store(false);
        }
    }

    virtual Buffer read() override
    {
        BufferWrapper buf = queue->getFreeBuffer();

        auto readCount = std::min(currentOffset + BUFFER_SIZE, fileSize);

        if (setvbuf(fileDesc, &buf.buffer.data, _IOFBF, BUFFER_SIZE))
        {
            throw std::runtime_error("can`t set buffering mode");
        }
        
        fgetc(fileDesc);

        currentOffset = readCount;

        if (currentOffset < fileSize)
        {
            fseek(fileDesc, BUFFER_SIZE - 1, SEEK_CUR);
        }
        else
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
    std::FILE* fileDesc = nullptr;

    size_t fileSize = 0;
    
    size_t currentOffset = 0;
    std::atomic_bool readFinished{false};

    std::string fileName{""};
    std::shared_ptr<FixedBufferQueue<Buffer>> queue;
};