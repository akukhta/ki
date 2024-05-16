#pragma once
#include <memory>
#include "IRequest.h"
#include "../Server/ConnectedClient.hpp"
#include "../Server/TCPIPQueue.hpp"

namespace TCPIP
{
    /// Main purpose of this request to obtain a buffer from the queue
    /// to store client's requests
    class ObtainBufferRequest : public IRequest
    {
    public:
        ObtainBufferRequest(std::shared_ptr<TCPIP::FixedBufferQueue> queue, ConnectedClient& client) : queue(std::move(queue)), client(client) {};

        void handle() override
        {
            // Try to obtain buffer in non-blocking manner
            auto buffer = queue->getFreeBufferNonBlock();

            if (buffer)
            {
                client.buffer = std::make_shared<TCPIP::Buffer>(std::move(buffer.value())); // Assign buffer
                client.currentRequest = nullptr; // Mark that the client does not have active request at the moment
            }
        }

    private:
        ConnectedClient& client;
        std::shared_ptr<TCPIP::FixedBufferQueue> queue;
    };

}