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

#define BUFFERS_IN_QUEUE 500

template <class MutexType, class ConditionType, template<class> class RAIILockType, template<class> class DequeType>
class FixedBufferQueue
{
public:
    FixedBufferQueue()
    {
        for (auto & buffer : buffers)
        {
            readBuffers.emplace_back(readBuffers, writeBuffers, cv, queueMutex, buffer.data());
        }
    }

    ~FixedBufferQueue()
    {
        for(auto &b : readBuffers)
        {
            b.isActive = false;
        }

        for (auto &b : writeBuffers)
        {
            b.isActive = false;
        }
    }

    std::optional<Buffer> getFreeBuffer()
    {
        RAIILockType lm(queueMutex);
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
        RAIILockType lm(queueMutex);
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

    bool isEmpty() const
    {
        RAIILockType lm(queueMutex);
        return writeBuffers.empty() && !isOpen.load();
    }

    void close()
    {
        isOpen.store(false);
    }

    void open()
    {
        isOpen.store(true);
    }

private:    
    DequeType<Buffer> readBuffers, writeBuffers;
    std::array<std::array<unsigned char, BUFFER_SIZE>, BUFFERS_IN_QUEUE> buffers{};
    mutable MutexType queueMutex;
    ConditionType cv;
    std::atomic_bool isOpen{false};
};