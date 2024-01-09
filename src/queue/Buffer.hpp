#pragma once
#include <condition_variable>
#include <deque>

#define BUFFER_BLOCKS_AMOUNT 4
#define BUFFER_SIZE ((S_BLKSIZE) * BUFFER_BLOCKS_AMOUNT)

enum class BufferType : char { READ, WRITE };

class Buffer
{
public:
    Buffer(std::deque<Buffer>& readBuffers, std::deque<Buffer>& writeBuffers, std::condition_variable &cv, BufferType type = BufferType::READ)
        :   type(type), mreadBuffers(readBuffers), mwriteBuffers(writeBuffers), cv(cv) {}

    ~Buffer()
    {
        cv.notify_all();

        if (type == BufferType::READ)
        {
            isFree = false;
            mwriteBuffers.push_back(*this); 
        }
        else if (type == BufferType::WRITE)
        {
            isFree = true;
            mreadBuffers.push_back(*this);  
        }
    }

    void setType(BufferType type)
    {
        this->type = type;
    }

    char unsigned data[BUFFER_SIZE];
    size_t size;
    bool isFree = true;

protected:
    std::deque<Buffer>& mreadBuffers, mwriteBuffers;
    
private:
    std::condition_variable &cv;
    BufferType type;
};