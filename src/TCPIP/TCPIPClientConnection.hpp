#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <utility>
#include <span>
#include "../queue/BufferedQueue.hpp"
#include "../queue/Buffer.hpp"

namespace TCPIP
{
    class ClientConnection
    {
    public:
        ClientConnection(std::string ip, std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue)
            : ip(std::move(ip)), queue(queue)
        {
            auto b = queue->getFreeBuffer();

            if (b)
            {
                buffer = std::span<unsigned char>{b->getData(), BUFFER_SIZE};
            }
        }

        unsigned char* getBuffer()
        {
            if (buffer.empty())
            {
                return nullptr;
            }
            else
            {
                return buffer.data() + bytesUsed;
            }
        }

        void returnBuffer(size_t bytesUsed)
        {
            this->bytesUsed += bytesUsed;
        }

    private:
        std::string ip;
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
        std::span<unsigned char> buffer;
        size_t bytesUsed = 0;
    };
}