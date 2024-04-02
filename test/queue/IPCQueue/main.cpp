#define BOOST_TEST_MODULE Test1
#include "../../../src/queue/BufferedQueue.hpp"
#include "../../../src/IPC/SharedMemoryManager.hpp"
#include "../../Utiles/TmpFile.hpp"
#include <boost/test/included/unit_test.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/containers/deque.hpp>

BOOST_AUTO_TEST_CASE(SharedMemoryManagerTest)
{
    boost::interprocess::shared_memory_object::remove("IPCQueueTest");

    auto shmem = std::make_shared<SharedMemoryManager>("IPCQueueTest");

    BOOST_ASSERT(shmem->isFirstProcess() == true);
    BOOST_ASSERT(shmem->getDequeAllocator() != nullptr);
    BOOST_ASSERT(shmem->getRawAllocator() != nullptr);
    BOOST_ASSERT(shmem->getQueue("ipcQueue") != nullptr);
}

BOOST_AUTO_TEST_CASE(EmptyIPCQueue)
{
    boost::interprocess::shared_memory_object::remove("IPCQueueTest");

    auto shmem = std::make_shared<SharedMemoryManager>("IPCQueueTest");

    auto queue = shmem->getQueue("ipcQueue");

    BOOST_ASSERT(queue->isEmpty() == true);
    BOOST_ASSERT(queue->getFilledBuffer().has_value() == false);
    BOOST_ASSERT(queue->getFreeBuffer().has_value() == true);
}
