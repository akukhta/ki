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
    };
}