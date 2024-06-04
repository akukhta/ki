#pragma once
#include "../../queue/Buffer/BufferType.hpp"
#include "../Request/RequestHeader.hpp"
#include "../../queue/Buffer/BufferConfiguration.hpp"
#include <cstdlib>

namespace TCPIP
{
    /// Buffer to store the request (header + the request's data)
    /// Storing everything alongside it allows to avoid
    /// dynamic memory allocation (buffers are being reused when the request has been handled)
    /// and can be easily integrated to the existing code base
    class Buffer
    {
    public:
        explicit Buffer(unsigned char* data, BufferType type = BufferType::READ)
                : bytesUsed{0z}, data(data), bufferType(type) {}

        Buffer(Buffer && other) noexcept
                :   data(other.data), bytesUsed(other.bytesUsed),
                    bufferType(other.bufferType), owningClientID(other.owningClientID)
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

        /// Returns a pointer to the request's not used memory
        /// In order to append the request
        /// \return nullptr if the buffer is filled, otherwise pointer to the available memory
        unsigned char* appendBufferData()
        {
            return bytesUsed < BUFFER_SIZE ? data + bytesUsed : nullptr;
        }

        /// Get pointer to the buffer
        unsigned char* getData()
        {
            return data;
        }

        /// Get pointer to the request's data
        /// skipping the request's header
        unsigned char* getRequestData()
        {
            return data + TCPIP::RequestHeader::noAligmentSize();
        }

        /// Get size of the request's payload data (without header)
        /// \return size of the request data
        size_t getRequestDataSize() const
        {
            return bytesUsed - TCPIP::RequestHeader::noAligmentSize();
        }

        /// Set the id of the owning client (the client that currently owns the buffer and its data)
        void setOwnerID(int id)
        {
            owningClientID = id;
        }

        /// Reset the buffer after the request has been processed
        void reset()
        {
            bytesUsed = 0;
        }

        Buffer(Buffer const&) = delete;
        Buffer& operator=(Buffer const&) = delete;

        unsigned char *data;
        size_t bytesUsed;
        unsigned int owningClientID = 0;

    private:
        BufferType bufferType;
    };
}