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

template <class MutexType, class ConditionType, template<class> class RAIILockType, template<class> class DequeType>
class FixedBufferQueue;

using Queue = FixedBufferQueue<boost::interprocess::interprocess_mutex, boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>;

class SharedMemoryManager : public std::enable_shared_from_this<SharedMemoryManager>
{
public:

    explicit SharedMemoryManager(std::string const& shObjName);
    ~SharedMemoryManager();

    Queue* getQueue(std::string const& name);

    std::shared_ptr<ShmemAllocator<SharedDeque>> getDequeAllocator();

    std::shared_ptr<ShmemAllocator<unsigned char>> getRawAllocator();

    ProcInfo* getProcInfo();

    bool isFirstProcess() const;

private:
    boost::interprocess::managed_shared_memory segment;
    bool isFirstProcess_ = false;

    std::shared_ptr<ShmemAllocator<SharedDeque>> dequeAllocator = nullptr;
    std::shared_ptr<ShmemAllocator<unsigned char>> rawAllocator = nullptr;
    std::shared_ptr<CharAllocator> charAllocator = nullptr;
    ProcInfo* procInfo = nullptr;

    std::string shObjName;

    static size_t calculateNeededSize();
};