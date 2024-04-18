#pragma once
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

using CharAllocator =  boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager>;
using SharedString = boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator>;

/// Helper class that stores information about IPC copy tool
/// Should be created only in shared memory
/// Since all related IPC processes should access this object
struct ProcInfo
{
    explicit ProcInfo(std::shared_ptr<CharAllocator> alloc) : dst(*alloc) {}

    /// Creates RAII mutex to provide exclusive ownership under process info
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> createScopedLock()
    {
        return boost::interprocess::scoped_lock(mutex);
    }

    /// Function to get destination path
    /// Should be called from reader process
    /// Being used to reduce unnecessary command line arguments
    /// Since we start reader process and it already has all the necessary info
    /// there is no need to pass it to writer process
    std::string getDst() const
    {
        return dst;
    }

    boost::interprocess::interprocess_mutex mutex;
    SharedString dst; /// String that holds destination file path, allocated in shared memory
    bool isWritingStarted = false;
    size_t readerProcessCount = 0; /// counter of reader processes (for now holds 2 values: 0, 1)
    size_t writerProcessCount = 0; /// counter of writer processes (for now holds 2 values: 0, 1)
};