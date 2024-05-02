#pragma once
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include "../queue/Buffer.hpp"
#include "IPCProcInfo.hpp"

template <class T>
using ShmemAllocator =  boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;

using ShmemBuffer = Buffer<boost::interprocess::offset_ptr<unsigned char>>;

using SharedDeque = boost::interprocess::deque<ShmemBuffer, ShmemAllocator<ShmemBuffer>>;

template <class Tag>
class FixedBufferQueue;

class IPCTag;
using Queue = FixedBufferQueue<IPCTag>;

///Class handles:\n
///all allocations and deallocations done in shared memory:\n
///* Queue that chunks read from files\n
///* Process Info\n
///allocation and deallocation of shared memory region\n
///determines if the process is reader (first process creates shared memory region)
///or writer (shared memory region already exists)
class SharedMemoryManager : public std::enable_shared_from_this<SharedMemoryManager>
{
public:

    explicit SharedMemoryManager(std::string const& shObjName);
    ~SharedMemoryManager();

    /// Creates or Finds and returns a pointer to the queue
    /// \param name A name of queue stored in shared memory
    /// \return Returns raw pointer to IPCQueue
    Queue* getQueue(std::string const& name);

    /// Returns allocator that is used by the queue to allocate/deallocate elements of the queue
    /// \return returns shared pointer to the allocator of
    /// FixedBufferQueue<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>
    std::shared_ptr<ShmemAllocator<SharedDeque>> getDequeAllocator();

    /// Returns allocator that allocates char within the shared memory region\n
    /// Used for allocation of string\n
    /// \return Shared pointer to the char allocator
    std::shared_ptr<ShmemAllocator<unsigned char>> getRawAllocator();

    /// Creates or Finds and returns a pointer to the ProcInfo structure
    /// that is used for managing process state and proper resource deallocation
    /// \return Raw pointer to ProcInfo
    ProcInfo* getProcInfo();

    /// Determines if the process is first consumer of the shared object
    /// specified at the construction\n
    /// Used to determine if process is a reader or writer
    /// \return true if shared memory object has not been used before, otherwise false
    bool isFirstProcess() const;

    /// Tries to deallocate shared memory region
    /// if no one else uses it
    void tryRemoveActiveSharedMemoryObject();

private:
    boost::interprocess::managed_shared_memory segment;
    bool isFirstProcess_ = false;

    std::shared_ptr<ShmemAllocator<SharedDeque>> dequeAllocator = nullptr;
    std::shared_ptr<ShmemAllocator<unsigned char>> rawAllocator = nullptr;
    std::shared_ptr<CharAllocator> charAllocator = nullptr;
    ProcInfo* procInfo = nullptr;

    std::string shObjName;

    /// Calculates size of shared memory region to allocate
    /// to hold the buffer queue
    /// \return size in bytes to allocate
    static size_t calculateNeededSize();
};