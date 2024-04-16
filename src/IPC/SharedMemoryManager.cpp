#include "SharedMemoryManager.hpp"
#include "../queue/BufferedQueue.hpp"
#include <iostream>
#include <format>

SharedMemoryManager::SharedMemoryManager(const std::string &shObjName) : shObjName(shObjName)
    {
        try
        {
            segment = boost::interprocess::managed_shared_memory(boost::interprocess::create_only, this->shObjName.c_str(), calculateNeededSize());
            isFirstProcess_ = true;
            dequeAllocator = std::make_shared<ShmemAllocator<SharedDeque>>(ShmemAllocator<SharedDeque>(segment.get_segment_manager()));
            rawAllocator = std::make_shared<ShmemAllocator<unsigned char>>(ShmemAllocator<unsigned char>(segment.get_segment_manager()));
            charAllocator = std::make_shared<CharAllocator>(CharAllocator(segment.get_segment_manager()));
            procInfo = segment.construct<ProcInfo>("kiProcInfo")(charAllocator);
        }
        catch (boost::interprocess::interprocess_exception const&)
        {
            segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, this->shObjName.c_str());
            procInfo = segment.find<ProcInfo>("kiProcInfo").first;
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
    if (procInfo->writerProcessCount == 0 && procInfo->readerProcessCount == 0)
    {
        std::cout <<"Clearing shared memory\n";

        boost::interprocess::shared_memory_object::remove(shObjName.c_str());
    }
}

size_t SharedMemoryManager::calculateNeededSize() {
    return (BUFFER_SIZE * BUFFERS_IN_QUEUE + sizeof(FixedBufferQueue<boost::interprocess::interprocess_mutex,
            boost::interprocess::interprocess_condition, boost::interprocess::scoped_lock, boost::interprocess::deque>)) * 1.7;
}

ProcInfo* SharedMemoryManager::getProcInfo()
{
    return procInfo;
}

void SharedMemoryManager::tryRemoveActiveSharedMemoryObject()
{
    if (boost::interprocess::shared_memory_object::remove(shObjName.c_str()))
    {
        std::cout << std::format("Shared Memory Object {} successfully removed", shObjName) <<std::endl;
    }
}
