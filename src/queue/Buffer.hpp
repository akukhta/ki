#pragma once
#include <condition_variable>
#include <deque>

//#define BUFFER_BLOCKS_AMOUNT 8
//#define BUFFER_SIZE ((S_BLKSIZE) * BUFFER_BLOCKS_AMOUNT)
#define BUFFER_SIZE 4096

enum class BufferType : char { READ, WRITE };

class Buffer
{
public:
    Buffer(std::deque<Buffer> &readBuffers, std::deque<Buffer> &writeBuffers, std::condition_variable &cv,
           std::mutex &parentMutex, unsigned char *data, BufferType type = BufferType::READ)
        : bufferType(type), readBuffers(readBuffers), writeBuffers(writeBuffers),
        cv(cv), parentMtx(parentMutex), bytesUsed{0}, data(data) {}

    Buffer(Buffer && other) noexcept
        :   readBuffers(other.readBuffers), writeBuffers(other.writeBuffers), cv(other.cv),
            parentMtx(other.parentMtx), data(other.data), bytesUsed(other.bytesUsed),
            bufferType(other.bufferType)
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

        {
            std::unique_lock lm(parentMtx);

            if (bufferType == BufferType::READ)
            {
                writeBuffers.push_back(std::move(*this));
            }
            else if (bufferType == BufferType::WRITE)
            {
                readBuffers.push_back(std::move(*this));
            }
        }

        cv.notify_all();
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
    std::mutex& parentMtx;
    std::condition_variable &cv;
    BufferType bufferType;

    bool bufferHasBeenMoved = false;
};