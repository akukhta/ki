#pragma once
#include <utility>

#include "IFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"

template <class MutexType, class ConditionType, template<class> class RAIILockType, template<class> class DequeType>
class BufferedFileWriter : IFileWriter<void>
{
public:

    explicit BufferedFileWriter(std::string fileName, 
        std::shared_ptr<FixedBufferQueue<MutexType, ConditionType, RAIILockType, DequeType>> queue):   fileName(std::move(fileName)), queue(std::move(queue)) {}

    void write() override
    {
        auto buf = queue->getFilledBuffer();

        if (buf) {
            fwrite(buf->data, buf->bytesUsed, 1, fileDesc);
        }
    }

    void create() override
    {
        fileDesc = std::fopen(fileName.c_str(), "wb");

        if (fileDesc == nullptr)
        {
            throw std::runtime_error("can`t create output file");
        }

        if (setvbuf(fileDesc, nullptr, _IOFBF, BUFFER_SIZE))
        {
            throw std::runtime_error("can`t set buffering mode for output file");
        }
    }

    void finishWrite() override
    {
        if (fileDesc)
        {
            writeFinished.store(true);
            fsync(fileDesc->_fileno);
            std::fclose(fileDesc);
            fileDesc = nullptr;
        }
    }

    bool isWriteFinished() override
    {
        return writeFinished.load();
    }
    
    virtual ~BufferedFileWriter()
    {
        finishWrite();
    }

private:
    std::FILE* fileDesc{};

    std::atomic_bool writeFinished{false};

    std::string fileName;

    std::shared_ptr<FixedBufferQueue<MutexType, ConditionType, RAIILockType, DequeType>> queue;
};