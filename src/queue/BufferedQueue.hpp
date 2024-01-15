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

class FixedBufferQueue
{
public:
    FixedBufferQueue();

    ~FixedBufferQueue() = default;

    std::optional<Buffer> getFreeBuffer();

    std::optional<Buffer> getFilledBuffer();

    bool isEmpty() const;

    void close();

    void open();

private:    
    std::deque<Buffer> readBuffers, writeBuffers;
    std::array<std::array<unsigned char, BUFFER_SIZE>, BUFFERS_IN_QUEUE> buffers{};
    mutable std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic_bool isOpen{false};
};