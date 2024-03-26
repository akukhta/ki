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
#include <boost/interprocess/offset_ptr.hpp>
#include "../IPC/SharedMemoryManager.hpp"

#define BUFFERS_IN_QUEUE 500

struct NonIPCBase
{
    std::array<std::array<unsigned char, BUFFER_SIZE>, BUFFERS_IN_QUEUE> buffers{};
};

struct IPCBase
{

};

template <class T>
concept IsNonIPC = std::is_same_v<T, std::mutex>;

template <class T>
concept IsIPC = std::is_same_v<T, boost::interprocess::interprocess_mutex>;

template <class MutexType, class ConditionType, template<class> class RAIILockType, template<class> class DequeType>
class FixedBufferQueue : public std::conditional_t<std::is_same_v<MutexType, std::mutex>, NonIPCBase, IPCBase>
{
private:
    using QueueType = FixedBufferQueue<MutexType, ConditionType, RAIILockType, DequeType>;
    using BufType = Buffer<std::conditional_t<std::is_same_v<MutexType, std::mutex>, unsigned char*, boost::interprocess::offset_ptr<unsigned char>>>;

public:

    template<typename T = MutexType>
        requires IsNonIPC<T>
    FixedBufferQueue()
    {
        for (auto &buffer : NonIPCBase::buffers)
        {
            readBuffers.emplace_back(buffer.data());
        }
    }

    template<typename T = MutexType>
        requires IsIPC<T>
    FixedBufferQueue(std::shared_ptr<SharedMemoryManager> shMemManager) :
        readBuffers(*shMemManager->getDequeAllocator()), writeBuffers(*shMemManager->getDequeAllocator())
    {
            auto rawAllocator = shMemManager->getRawAllocator();

            for (size_t i = 0; i < BUFFERS_IN_QUEUE; i++)
            {
                auto allocated = rawAllocator->allocate(BUFFER_SIZE);
                readBuffers.emplace_back(allocated);
            }
    };


    ~FixedBufferQueue() = default;

    std::optional<BufType> getFreeBuffer()
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

    std::optional<BufType> getFilledBuffer()
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

    void returnBuffer(BufType buffer)
    {
        {
            RAIILockType lm(queueMutex);

            auto typeToSet = buffer.getType() == BufferType::READ ? BufferType::WRITE : BufferType::READ;
            auto &dequeToPush = typeToSet == BufferType::READ ? readBuffers : writeBuffers;

            dequeToPush.emplace_back(std::move(buffer)).setType(typeToSet);
        }

        cv.notify_all();
    }

    bool isReadFinished() const
    {
        RAIILockType lm(queueMutex);
        return isReadFinished_;
    }

    void finishRead()
    {
        RAIILockType lm(queueMutex);
        isReadFinished_ = true;
    }

protected:
    std::conditional_t<std::is_same_v<MutexType, std::mutex>, DequeType<BufType>, SharedDeque> readBuffers, writeBuffers;

    mutable MutexType queueMutex;
    ConditionType cv;
    std::atomic_bool isOpen{false};
    bool isReadFinished_ = false;
};