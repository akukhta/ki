#pragma once
#include <condition_variable>
#include <deque>
#include <boost/interprocess/offset_ptr.hpp>
#include "Buffer/BufferType.hpp"

/// Class represents a buffer
template <class DataType>
class Buffer
{
public:
    explicit Buffer(DataType data, BufferType type = BufferType::READ)
        : bytesUsed{0z}, data(data), bufferType(type) {}

    Buffer(Buffer && other) noexcept
        :   data(other.data), bytesUsed(other.bytesUsed),
            bufferType(other.bufferType)
    {
        other.data = nullptr;
    }

    void setType(BufferType type)
    {
        this->bufferType = type;
    }

    BufferType getType()
    {
        return bufferType;
    }

    unsigned char* getData()
    {
        if constexpr(std::is_same_v<boost::interprocess::offset_ptr<unsigned char>, DataType>)
        {
            return data.get();
        }
        else
        {
            return data;
        }
    }

    Buffer(Buffer const&) = delete;
    Buffer& operator=(Buffer const&) = delete;

    DataType data;
    size_t bytesUsed;

private:
    BufferType bufferType;
};