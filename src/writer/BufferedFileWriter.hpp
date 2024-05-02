#pragma once
#include <utility>

#include "IFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"

/// Class that performs writing to file\n
/// Writing is done in buffered(chunked) mode:\n
/// Takes a buffer from the queue and write it to the output file\n
/// \tparam MutexType Data type used as a mutex (std::mutex/boost::interprocess::mutex)
/// \tparam ConditionType Data type used as a conditional variables(std::conditional_variable/boost::interprocess::condition)
/// \tparam RAIILockType Data type used as a RAII lock (std::unique_lock/boost::interprocess::scoped_lock)
/// \tparam DequeType Data type used as an inner deque types (std::deque/boost::interprocess::deque)
template <class Tag>
class BufferedFileWriter : IFileWriter<void>
{
private:
    using QueueType = FixedBufferQueue<Tag>;

public:

    explicit BufferedFileWriter(std::string fileName,
        std::conditional_t<std::is_same_v<Tag, NonIPCTag>, std::shared_ptr<QueueType>, QueueType*> queue):   fileName(std::move(fileName)), queue(std::move(queue)) {}

    /// Writes a filled buffer from the queue to the file
    void write() override
    {
        auto buf = queue->getFilledBuffer();

        if (buf) {
            fwrite(buf->getData(), buf->bytesUsed, 1, fileDesc);
            queue->returnBuffer(std::move(*buf));
        }
    }

    /// Creates and open output file
    void create() override
    {
        fileDesc = std::fopen(fileName.c_str(), "wb");

        if (fileDesc == nullptr)
        {
            throw std::runtime_error("can`t create output file");
        }

        if (setvbuf(fileDesc, nullptr, _IOFBF, BUFFER_SIZE))
        {
            throw std::runtime_error("can`t set buffering mode for output file");
        }
    }

    /// Closes file handle and syncs the file disk's content
    void finishWrite() override
    {
        if (fileDesc)
        {
            writeFinished.store(true);
            fsync(fileDesc->_fileno);
            std::fclose(fileDesc);
            fileDesc = nullptr;
        }
    }

    /// Check if writing is finished
    /// \return true if writing if finished, otherwise false
    bool isWriteFinished() override
    {
        return writeFinished.load();
    }
    
    ~BufferedFileWriter() override
    {
        BufferedFileWriter::finishWrite();
    }

private:
    std::FILE* fileDesc{};

    std::atomic_bool writeFinished{false};

    std::string fileName;

    std::conditional_t<std::is_same_v<Tag, NonIPCTag>, std::shared_ptr<QueueType>, QueueType*> queue;
};