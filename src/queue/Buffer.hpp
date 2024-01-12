#pragma once
#include <condition_variable>
#include <deque>

#define BUFFER_BLOCKS_AMOUNT 4
#define BUFFER_SIZE ((S_BLKSIZE) * BUFFER_BLOCKS_AMOUNT)

enum class BufferType : char { READ, WRITE };

class Buffer
{
public:
    Buffer(std::deque<Buffer>& readBuffers, std::deque<Buffer>& writeBuffers, std::condition_variable &cv, unsigned char *data, BufferType type = BufferType::READ)
        : bufferType(type), readBuffers(readBuffers), writeBuffers(writeBuffers), cv(cv), bytesUsed{0}, data(data) {}

    Buffer(Buffer && other) noexcept
        : readBuffers(other.readBuffers), writeBuffers(other.writeBuffers), cv(other.cv),
            data(other.data), bytesUsed(other.bytesUsed), bufferType(other.bufferType)
    {
        other.data = nullptr;
        other.bufferHasBeenMoved = true;
    }

    Buffer(Buffer const&) = delete;

    ~Buffer()
    {
        if (bufferHasBeenMoved)
        {
            return;
        }

        cv.notify_all();

        if (bufferType == BufferType::READ)
        {
            writeBuffers.push_back(std::move(*this));
        }
        else if (bufferType == BufferType::WRITE)
        {
            readBuffers.push_back(std::move(*this));
        }
    }

    void setType(BufferType type)
    {
        this->bufferType = type;
    }

    char unsigned *data;
    size_t bytesUsed;

protected:
    std::deque<Buffer>& readBuffers;
    std::deque<Buffer>& writeBuffers;
    
private:
    std::condition_variable &cv;
    BufferType bufferType;

    bool bufferHasBeenMoved = false;
};