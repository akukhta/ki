#pragma once
#include <condition_variable>
#include <deque>

#define BUFFER_BLOCKS_AMOUNT 4
#define BUFFER_SIZE (S_BLKSIZE * BUFFER_BLOCKS_AMOUNT)

class Buffer
{
public:
    Buffer() = default;

    char unsigned data[BUFFER_SIZE];
    size_t size;
    bool isFree = true;
};

enum class BufferType : char { READ, WRITE };

class BufferWrapper
{
public:
    BufferWrapper(BufferType type, Buffer& buffer, std::deque<Buffer&>& readBuffers, std::deque<Buffer&>& writeBuffers, std::condition_variable &cv)
        :   type(type),buffer(buffer), mreadBuffers(readBuffers), mwriteBuffers(writeBuffers), cv(cv) {}
    
    Buffer& buffer;

    ~BufferWrapper()
    {
        cv.notify_all();

        if (type == BufferType::READ)
        {
            buffer.isFree = false;
            mwriteBuffers.push_back(buffer); 
        }
        else if (type == BufferType::WRITE)
        {
            buffer.isFree = true;
            mreadBuffers.push_back(buffer);  
        }
    }

protected:
    std::deque<Buffer&>& mreadBuffers, mwriteBuffers;
    
private:
    std::condition_variable &cv;
    BufferType type;
};
