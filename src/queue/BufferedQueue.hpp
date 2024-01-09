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

        for (size_t i = 0; i < BUFFERS_IN_QUEUE; i++)
        {
            readBuffers.emplace_back(readBuffers, writeBuffers, cv);
        }
    }

    Buffer getFreeBuffer()
    {
        std::unique_lock lm(queueMutex);
        cv.wait(lm, [this](){ return readBuffers.size() > 0; });

        auto &buffer = readBuffers.back();
        readBuffers.pop_back();

        buffer.setType(BufferType::READ);
        
        return buffer;
    }

    Buffer getFilledBuffer()
    {
        std::unique_lock lm(queueMutex);
        cv.wait(lm, [this](){ return writeBuffers.size() > 0;});

        auto &buffer = writeBuffers.back();
        writeBuffers.pop_back();
        
        buffer.setType(BufferType::WRITE);
        return buffer;
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
    std::deque<Buffer> readBuffers, writeBuffers;

    mutable std::mutex queueMutex;
    std::condition_variable cv;
};