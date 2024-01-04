#pragma once
#include "IQueue.hpp"
#include "Buffer.hpp"
#include <condition_variable>
#include <sys/stat.h>
#include <unistd.h>
#include <atomic>
#include <array>
#include <mutex>
#include <deque>

#define BUFFERS_IN_QUEUE 2

/*
consteval size_t getBlockSize()
{
    struct stat fi;
    stat("/", &fi);
    return fi.st_blksize;
}
*/

template<class ChunkType>
class FixedBufferQueue;

template <>
class FixedBufferQueue<Buffer> : public IQueue<Buffer>
{
public:
    FixedBufferQueue()
    {
        for (auto &buffer : buffers)
        {
            readBuffers.push_back(buffer);
        }
    }

    BufferWrapper getFreeBuffer()
    {
        std::unique_lock lm(queueMutex);
        cv.wait(lm, readBuffers.size() > 0);

        auto &buffer = readBuffers.back();
        readBuffers.pop_back();
        return BufferWrapper{ BufferType::READ, buffer, readBuffers, writeBuffers, cv};
    }

    BufferWrapper getFilledBuffer()
    {
        std::unique_lock lm(queueMutex);
        cv.wait(lm, writeBuffers.size() > 0);

        auto &buffer = writeBuffers.back();
        writeBuffers.pop_back();
        return BufferWrapper{BufferType::WRITE, buffer, readBuffers, writeBuffers, cv};
    }

    virtual void push(Buffer buffer) override
    {
        throw std::runtime_error("not implemented"); 
    }

    virtual std::optional<Buffer> pop() override
    {
        throw std::runtime_error("not implemented");
    }

    virtual bool isEmpty() const override
    {
        std::unique_lock lm(queueMutex);
        return writeBuffers.empty();
    }
    
private:
    // Are those interfaces apliable to queue itself? Should i segregate interfaces? 
    virtual void close() override
    {
        ;
    }

    virtual void open() override
    {
        ;
    }

private:    
    std::array<Buffer, BUFFERS_IN_QUEUE> buffers;  
    std::deque<Buffer&> readBuffers, writeBuffers;

    std::mutex queueMutex;
    std::condition_variable cv;
};