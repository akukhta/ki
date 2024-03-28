#pragma once
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

using CharAllocator =  boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager>;
using SharedString = boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator>;

struct ProcInfo
{
    ProcInfo(std::shared_ptr<CharAllocator> alloc) : dst(*alloc) {}

    void lock()
    {
        mutex.lock();
    }

    void unlock()
    {
        mutex.unlock();
    }

    ~ProcInfo()
    {
        unlock();
    }

    std::string getDst()
    {
        return dst;
    }

    boost::interprocess::interprocess_mutex mutex;
    SharedString dst;
};