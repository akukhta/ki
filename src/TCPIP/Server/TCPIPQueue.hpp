#pragma once
#include "../../queue/BufferedQueue.hpp"
#include "../../queue/Buffer/BufferType.hpp"

namespace TCPIP
{
    class FixedBufferQueue : public ::FixedBufferQueue<TCPIPTag>
    {
    public:
        std::optional<TCPIP::Buffer> getFreeBufferNonBlock()
        {
            RAIILockType lm(queueMutex);
            // Check if we can actually return buffer
            // if we can`t, return nullopt
            if (readBuffers.empty() && !isOpen.load())
            {
                return std::nullopt;
            }

            auto buffer = std::move(readBuffers.back());
            readBuffers.pop_back();
            buffer.setType(::BufferType::READ);
            return buffer;
        }

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
    };
}