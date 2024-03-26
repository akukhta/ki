#include "SharedMemoryManager.hpp"
#include "../queue/BufferedQueue.hpp"

SharedMemoryManager::SharedMemoryManager(const std::string &shObjName) : shObjName(std::move(shObjName))
    {
        try
        {
            segment = boost::interprocess::managed_shared_memory(boost::interprocess::create_only, this->shObjName.c_str(), calculateNeededSize());
            isFirstProcess_ = true;
            dequeAllocator = std::make_shared<ShmemAllocator<SharedDeque>>(ShmemAllocator<SharedDeque>(segment.get_segment_manager()));
            rawAllocator = std::make_shared<ShmemAllocator<unsigned char>>(ShmemAllocator<unsigned char>(segment.get_segment_manager()));
        }
        catch (boost::interprocess::interprocess_exception const&)
        {
            segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, this->shObjName.c_str());
        }
    }

std::shared_ptr<ShmemAllocator<unsigned char>> SharedMemoryManager::getRawAllocator() {
    return rawAllocator;
}

std::shared_ptr<ShmemAllocator<SharedDeque>> SharedMemoryManager::getDequeAllocator() {
    return dequeAllocator;
}

Queue* SharedMemoryManager::getQueue(const std::string &name)
{
    if (isFirstProcess_)
    {
        return segment.construct<Queue>(name.c_str())(shared_from_this());
    }
    else
    {
        return segment.find<Queue>(name.c_str()).first;
    }
}

bool SharedMemoryManager::isFirstProcess() const
{
    return isFirstProcess_;
}

SharedMemoryManager::~SharedMemoryManager()
{
    boost::interprocess::shared_memory_object::remove(shObjName.c_str());
}

size_t SharedMemoryManager::calculateNeededSize() {
    return (BUFFER_SIZE * BUFFERS_IN_QUEUE + sizeof(FixedBufferQueue<boost::interprocess::interprocess_mutex,
            boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>)) * 1.7;
}
