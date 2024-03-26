#pragma once
#include "IFileReader.hpp"
#include "../queue/BufferedQueue.hpp"
#include <filesystem>
#include <cstdio>
#include <utility>

template <class MutexType, class ConditionType, template<class> class RAIILockType, template<class> class DequeType>
class BufferedReader : public IFileReader<void>
{
private:
    using QueueType = FixedBufferQueue<MutexType, ConditionType, RAIILockType, DequeType>;
public:
    explicit BufferedReader(std::string fileName,
                            std::conditional_t<std::is_same_v<MutexType, std::mutex>, std::shared_ptr<QueueType>, QueueType*> queue) :
        fileName(std::move(fileName)), queue(std::move(queue))
    {
        fileSize = std::filesystem::file_size(this->fileName);
    }


    void open() override
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

    void read() override
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

        if (setvbuf(fileDesc, reinterpret_cast<char*>(buf.getData()), _IOFBF, BUFFER_SIZE))
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
            queue->finishRead();
        }

        queue->returnBuffer(std::move(buf));
    }

    bool isReadFinished() const override
    {
        return readFinished.load();
    }

    void finishRead() override
    {
        if (fileDesc)
        {
            std::fclose(fileDesc);
            fileDesc = nullptr;
        }
    }
    
    size_t getFileSize() const
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

    std::string fileName;
    std::conditional_t<std::is_same_v<MutexType, std::mutex>, std::shared_ptr<QueueType>, QueueType*> queue;
};