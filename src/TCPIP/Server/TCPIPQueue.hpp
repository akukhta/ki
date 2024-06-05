#pragma once
#include "../../queue/BufferedQueue.hpp"
#include "../../queue/Buffer/BufferType.hpp"

namespace TCPIP
{
    /// Memory pool designed to work with TCPIP tool (specifically with the TCPIP buffer that also stores request header)
    class FixedBufferQueue : public ::FixedBufferQueue<TCPIPTag>
    {
    public:
        /// Try to gather a buffer in non-blocking manner
        /// It's used to obtain a buffer for a client
        /// If it's not possible, the processing of the client is postponed
        /// Untill there is a memory buffer can be allocated to it
        std::optional<TCPIP::Buffer> getFreeBufferNonBlock()
        {
            RAIILockType lm(queueMutex);
            // Check if we can actually return buffer
            // if we can`t, return nullopt
            if (readBuffers.empty())
            {
                return std::nullopt;
            }
            else
            {
                auto buffer = std::move(readBuffers.back());
                readBuffers.pop_back();
                buffer.setType(::BufferType::READ);
                return buffer;
            }
        }

        /// Release TCPIP buffer and return it back to the memory pool
        void releaseBuffer(TCPIP::Buffer buffer)
        {
            RAIILockType lm(queueMutex);

            if (buffer.getType() == ::BufferType::READ)
            {
                readBuffers.emplace_back(std::move(buffer));
            }
            else
            {
                writeBuffers.emplace_back(std::move(buffer));
            }
        }

        size_t getFreeBuffersAmount() const
        {
            RAIILockType lm(queueMutex);
            return readBuffers.size();
        }
    };
}