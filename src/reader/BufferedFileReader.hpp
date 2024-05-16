#pragma once
#include "IFileReader.hpp"
#include "../queue/BufferedQueue.hpp"
#include <filesystem>
#include <cstdio>
#include <utility>

/// Class that performs reading from file\n
/// Reading is done in buffered(chunked) mode:\n
/// file is being read in small buffers/chunks and those buffers are being stored in the queue\n
/// Later on those buffers will be used to write data from\n
/// Reading is done with direct accessing of the io buffer (for more info check "Linux System Programming" by Robert Love, Chapter 3, pg 123)
/// \tparam MutexType Data type used as a mutex (std::mutex/boost::interprocess::mutex)
/// \tparam ConditionType Data type used as a conditional variables(std::conditional_variable/boost::interprocess::condition)
/// \tparam RAIILockType Data type used as a RAII lock (std::unique_lock/boost::interprocess::scoped_lock)
/// \tparam DequeType Data type used as an inner deque types (std::deque/boost::interprocess::deque)
template <class Tag>
class BufferedReader : public IFileReader<void>
{
private:
    using QueueType = FixedBufferQueue<Tag>;
public:
    explicit BufferedReader(std::string fileName,
                            typename Tag::QueueType queue) :
        fileName(std::move(fileName)), queue(std::move(queue))
    {
        fileSize = std::filesystem::file_size(this->fileName);
    }

    /// Opens the requested file for reading
    void open() override
    {
        fileDesc = std::fopen(fileName.c_str(), "rb");

        if (fileDesc == nullptr)
        {
            throw std::runtime_error("Reader: Can`t open a file");
        }

        if (fileSize < currentOffset)
        {
            readFinished.store(false);
        }
    }

    /// Reads one chunk/buffer from a file and stores it into the queue
    void read() override
    {
        // Get the buffer to read into
        auto buf = queue->getFreeBuffer().value();

        // Determine amount of bytes to read and store in the buffer
        if (currentOffset + BUFFER_SIZE <= fileSize)
        {
            buf.bytesUsed = BUFFER_SIZE;
        }
        else
        {
            buf.bytesUsed = fileSize - currentOffset;
        }

        auto readCount = std::min(currentOffset + BUFFER_SIZE, fileSize);

        if (setvbuf(fileDesc, reinterpret_cast<char*>(buf.getData()), _IOFBF, BUFFER_SIZE))
        {
            throw std::runtime_error("can`t set buffering mode");
        }

        fgetc(fileDesc);

        currentOffset = readCount;

        if (currentOffset < fileSize)
        {
            fseek(fileDesc, BUFFER_SIZE - 1, SEEK_CUR);
        }
        else
        {
            readFinished.store(true);
            queue->finishRead();
        }

        queue->returnBuffer(std::move(buf));
    }

    [[nodiscard]] bool isReadFinished() const override
    {
        return readFinished.load();
    }

    void finishRead() override
    {
        if (fileDesc)
        {
            std::fclose(fileDesc);
            fileDesc = nullptr;
        }
    }
    
    [[nodiscard]] size_t getFileSize() const
    {
        return fileSize;
    }

    ~BufferedReader() override
    {
        BufferedReader::finishRead();
    }

    //static inline size_t defaultBufferSize = sysconf(_SC_PAGESIZE);

protected:
    std::FILE* fileDesc = nullptr;

    size_t fileSize = 0;
    
    size_t currentOffset = 0;
    std::atomic_bool readFinished{false};

    std::string fileName;
    typename Tag::QueueType queue;
};