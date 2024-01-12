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
        for (auto & buffer : buffers)
        {
            readBuffers.emplace_back(readBuffers, writeBuffers, cv, buffer.data());
        }
    }

    ~FixedBufferQueue() override = default;

    std::optional<Buffer> getFreeBuffer()
    {
        std::unique_lock lm(queueMutex);
        cv.wait(lm, [this](){ return !readBuffers.empty() || !isOpen.load(); });

        if (readBuffers.empty() && !isOpen.load())
        {
            return std::nullopt;
        }

        auto buffer = std::move(readBuffers.back());
        readBuffers.pop_back();

        buffer.setType(BufferType::READ);
        
        return buffer;
    }

    std::optional<Buffer> getFilledBuffer()
    {
        std::unique_lock lm(queueMutex);
        cv.wait(lm, [this](){ return !writeBuffers.empty() || !isOpen.load(); });

        if (writeBuffers.empty() && !isOpen.load())
        {
            return std::nullopt;
        }

        auto buffer = std::move(writeBuffers.front());
        writeBuffers.pop_front();
        
        buffer.setType(BufferType::WRITE);
        return buffer;
    }

    bool isEmpty() const override
    {
        //std::unique_lock lm(queueMutex);
        return writeBuffers.empty() && isOpen.load() == false;
    }

    void close() override
    {
        isOpen.store(false);
    }

    void open() override
    {
        isOpen.store(true);
    }

private:

    void push(Buffer buffer) override
    {
        throw std::runtime_error("not implemented");
    }

    std::optional<Buffer> pop() override
    {
        throw std::runtime_error("not implemented");
    }
private:    
    std::deque<Buffer> readBuffers, writeBuffers;
    std::array<std::array<unsigned char, BUFFER_SIZE>, BUFFERS_IN_QUEUE> buffers{};
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic_bool isOpen{false};
};