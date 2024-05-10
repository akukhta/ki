#pragma once
#include "Buffer/BufferType.hpp"
#include "../TCPIP/RequestHeader.hpp"
#include <cstdlib>

namespace TCPIP
{
    class Buffer
    {
    public:
        explicit Buffer(unsigned char* data, BufferType type = BufferType::READ)
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

        unsigned char* appendBufferData()
        {
            return data + bytesUsed;
        }

        unsigned char* getData()
        {
            return data;
        }

        unsigned char const* getRequestData()
        {
            return data + sizeof(TCPIP::RequestHeader);
        }

        void setOwnerID(int id)
        {
            owningClientID = id;
        }

        Buffer(Buffer const&) = delete;
        Buffer& operator=(Buffer const&) = delete;

        unsigned char *data;
        size_t bytesUsed;
        int owningClientID = 0;

    private:
        BufferType bufferType;
    };
}