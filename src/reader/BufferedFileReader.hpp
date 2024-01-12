#pragma once
#include "IFileReader.hpp"
#include "../queue/BufferedQueue.hpp"
#include <filesystem>
#include <cstdio>
#include <utility>

class BufferedReader
{
public:
    explicit BufferedReader(std::string fileName, std::shared_ptr<FixedBufferQueue<Buffer>> queue) : fileName(std::move(fileName)), queue(std::move(queue))
    {
        fileSize = std::filesystem::file_size(this->fileName);
    }

    virtual void open()
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

    virtual void read()
    {
        auto buf = queue->getFreeBuffer().value();

        if (currentOffset + BUFFER_SIZE <= fileSize)
        {
            buf.bytesUsed = BUFFER_SIZE;
        }
        else
        {
            buf.bytesUsed = fileSize - currentOffset;
        }

        auto readCount = std::min(currentOffset + BUFFER_SIZE, fileSize);

        if (setvbuf(fileDesc, reinterpret_cast<char*>(buf.data), _IOFBF, BUFFER_SIZE))
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
    }

    virtual bool isReadFinished() const
    {
        return readFinished.load();
    }

    virtual void finishRead()
    {
        if (fileDesc)
        {
            std::fclose(fileDesc);
            fileDesc = nullptr;
        }
    }
    
    size_t getFileSize()
    {
        return fileSize;
    }

    ~BufferedReader()
    {
        finishRead();
    }

    static inline size_t defaultBufferSize = sysconf(_SC_PAGESIZE);

private:
    std::FILE* fileDesc = nullptr;

    size_t fileSize = 0;
    
    size_t currentOffset = 0;
    std::atomic_bool readFinished{false};

    std::string fileName{""};
    std::shared_ptr<FixedBufferQueue<Buffer>> queue;
};