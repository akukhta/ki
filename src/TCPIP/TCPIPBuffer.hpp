#pragma once
#include <cstdlib>
#include "../queue/Buffer/BufferConfiguration.hpp"
#include "../queue/Buffer/BufferType.hpp"

namespace TCPIP
{
    class Buffer
    {
    public:
        explicit Buffer(unsigned char* data, BufferType type = BufferType::READ)
                : bytesUsed{0z}, data(data), bufferType(type), clientID(0) {}

        Buffer(Buffer && other) noexcept
                :   data(other.data), bytesUsed(other.bytesUsed),
                    bufferType(other.bufferType), clientID(other.clientID)
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
            return data;
        }

        Buffer(Buffer const&) = delete;
        Buffer& operator=(Buffer const&) = delete;

        unsigned char *data;
        size_t bytesUsed;
        size_t clientID;

    private:
        BufferType bufferType;
    };
}