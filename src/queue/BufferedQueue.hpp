#pragma once
#include <condition_variable>
#include <sys/stat.h>
#include <unistd.h>
#include <atomic>
#include <array>
#include <mutex>
#include <deque>
#include <boost/interprocess/offset_ptr.hpp>
#include "IQueue.hpp"
#include "Buffer.hpp"
#include "../IPC/SharedMemoryManager.hpp"
#include "Buffer/BufferConfiguration.hpp"

/// Buffers that are used in non-ipc version of a queue\n
/// Allocated on the stack
struct NonIPCBase
{
    std::array<std::array<unsigned char, BUFFER_SIZE>, BUFFERS_IN_QUEUE> buffers{};
};

/// Buffers used for IPC-version of a queue\n
/// It's empty because IPC buffers are allocated within shared memory
/// and managed by SharedMemoryManager
struct IPCBase {};

/// Concept to determine if the tool works in non-ipc mode
/// TODO: Refactor it to use tags
template <class T>
concept IsNonIPC = std::is_same_v<T, std::mutex>;

/// Concept to determine if the tool works in ipc mode
template <class T>
concept IsIPC = std::is_same_v<T, boost::interprocess::interprocess_mutex>;

/// Queue that holds buffers\n
/// Provides following functionality:\n
/// 1. Get free buffer to read data into\n
/// 2. Get filled buffer to write data from\n
/// Note: Once buffer is no longer neeeded, it should be returned back to the queue. See returnBuffer function\n
/// P.S. not inherited from IQueue because IPC queue would store virtual function pointer
/// And this pointer wouldn`t work from another process, if we use queue allocated withing shared memory
template <class MutexType, class ConditionType, template<class> class RAIILockType, template<class> class DequeType>
class FixedBufferQueue : public std::conditional_t<std::is_same_v<MutexType, std::mutex>, NonIPCBase, IPCBase>
{
private:
    /// Typename to determine which buffers to use: ipc(allocated within shared memory) or non-ipc(allocated on the stack)
   using BufType = Buffer<std::conditional_t<std::is_same_v<MutexType, std::mutex>, unsigned char*, boost::interprocess::offset_ptr<unsigned char>>>;

public:

    /// Constructor for non-IPC tool queue
    template<typename T = MutexType>
        requires IsNonIPC<T>
    FixedBufferQueue()
    {
        // Put all buffers in the reader deque
        // Since the app has just started and no data written yet
        for (auto &buffer : NonIPCBase::buffers)
        {
            readBuffers.emplace_back(buffer.data());
        }
    }

    /// Constructor for IPC tool queue
    template<typename T = MutexType>
        requires IsIPC<T>
    explicit FixedBufferQueue(std::shared_ptr<SharedMemoryManager> shMemManager) :
        readBuffers(*shMemManager->getDequeAllocator()), writeBuffers(*shMemManager->getDequeAllocator())
    {
            // Get shared memory char allocator
            auto rawAllocator = shMemManager->getRawAllocator();

            for (size_t i = 0; i < BUFFERS_IN_QUEUE; i++)
            {
                // Allocate buffer within shared memory and put it to the reader buffers
                auto allocated = rawAllocator->allocate(BUFFER_SIZE);
                readBuffers.emplace_back(allocated);
            }
    };


    ~FixedBufferQueue() = default;

    /// Function to get a free/unused buffer to read data into
    /// \return Returns free buffer or std::nullopt if buffer can`t be given
    std::optional<BufType> getFreeBuffer()
    {
        RAIILockType lm(queueMutex);
        // Wait until readBuffers contains something
        // or queue is closed (nothing should be returned)
        cv.wait(lm, [this](){ return !readBuffers.empty() || !isOpen.load(); });

        // Check if we can actually return buffer
        // if we can`t, return nullopt
        if (readBuffers.empty() && !isOpen.load())
        {
            return std::nullopt;
        }

        auto buffer = std::move(readBuffers.back());
        readBuffers.pop_back();

        buffer.setType(BufferType::READ);

        return buffer;
    }

    /// Function to get a filled/used buffer to write data from
    /// \return Returns filled buffer or std::nullopt if buffer can`t be given
    std::optional<BufType> getFilledBuffer()
    {
        RAIILockType lm(queueMutex);
        // Wait until writeBuffers contains something
        // or queue is closed (nothing should be returned)
        cv.wait(lm, [this](){ return !writeBuffers.empty() || !isOpen.load(); });

        // Check if we can actually return buffer
        // if we can`t, return nullopt
        if (writeBuffers.empty() && !isOpen.load())
        {
            return std::nullopt;
        }

        auto buffer = std::move(writeBuffers.front());
        writeBuffers.pop_front();

        buffer.setType(BufferType::WRITE);
        return buffer;
    }

    /// Check if the queue contains a buffer that can be used to read into or to write from
    /// \return true if there is a buffer available, otherwise false
    bool isEmpty() const
    {
        RAIILockType lm(queueMutex);
        return writeBuffers.empty() && !isOpen.load();
    }

    /// Function to close the queue\n
    /// Should be used from reader thread to signalize to the writer thread
    /// That reading is done
    void close()
    {
        isOpen.store(false);
    }

    /// Function to open the queue\n
    /// Does not allocate any memory, used only to properly initialize flags
    void open()
    {
        isOpen.store(true);
    }

    /// Function to return buffer (that has been used to read into/write from) back to the queue
    /// \param buffer Buffer to return
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

    /// Function to check if reading has been finished
    /// \return true if reading is done, false otherwise
    bool isReadFinished() const
    {
        RAIILockType lm(queueMutex);
        return isReadFinished_;
    }

    /// Function to signalize that the reading is done
    void finishRead()
    {
        RAIILockType lm(queueMutex);
        isReadFinished_ = true;
    }

protected:
    /// Inner queue store buffers could be used for reading or writing
    std::conditional_t<std::is_same_v<MutexType, std::mutex>, DequeType<BufType>, SharedDeque> readBuffers, writeBuffers;

    /// Queue mutex
    mutable MutexType queueMutex;
    /// Conditional variable
    ConditionType cv;
    std::atomic_bool isOpen{false};
    bool isReadFinished_ = false;
};