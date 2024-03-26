#include "SharedMemoryManager.hpp"
#include "../queue/BufferedQueue.hpp"

SharedMemoryManager::SharedMemoryManager(const std::string &shObjName) : shObjName(std::move(shObjName))
    {
        try
        {
            segment = boost::interprocess::managed_shared_memory(boost::interprocess::create_only, this->shObjName.c_str(), 10000000);
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
        return segment.construct<Queue>("dataQueue")(shared_from_this());
    }
    else
    {
        return segment.find<Queue>("dataQueue").first;
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
