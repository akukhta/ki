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
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#define BUFFERS_IN_QUEUE 500

struct NonIPCBase
{
    std::array<std::array<unsigned char, BUFFER_SIZE>, BUFFERS_IN_QUEUE> buffers{};
};

struct IPCBase
{

};

template <class T>
concept IsNonIPC = std::derived_from<T, NonIPCBase>;

template <class MutexType, class ConditionType, template<class> class RAIILockType, template<class> class DequeType>
class FixedBufferQueue : public std::conditional_t<std::is_same_v<MutexType, std::mutex>, NonIPCBase, IPCBase>
{
public:

    template<typename T = FixedBufferQueue<MutexType, ConditionType, RAIILockType, DequeType>>
        requires IsNonIPC<T>
    FixedBufferQueue()
    {
        for (auto &buffer : NonIPCBase::buffers)
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
    mutable MutexType queueMutex;
    ConditionType cv;
    std::atomic_bool isOpen{false};
};