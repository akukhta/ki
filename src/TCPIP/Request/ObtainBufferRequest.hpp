#pragma once
#include <memory>
#include "ICommand.h"
#include "../Server/ConnectedClient.hpp"
#include "../Server/TCPIPQueue.hpp"

namespace TCPIP
{
    /// Main purpose of this request to obtain a buffer from the queue
    /// to store client's requests
    class ObtainBufferRequest : public ICommand
    {
    public:
        ObtainBufferRequest(std::shared_ptr<TCPIP::FixedBufferQueue> queue)
            : queue(std::move(queue))
        {};

        void execute(std::shared_ptr<class ConnectedClient> client) override
        {
            if (!client)
            {
                // Early return if no client associated with the request
                return;
            }

            // Try to obtain buffer in non-blocking manner
            auto buffer = queue->getFreeBufferNonBlock();

            if (buffer)
            {
                client->buffer = std::make_shared<TCPIP::Buffer>(std::move(buffer.value())); // Assign buffer
                client->currentRequest = nullptr; // Mark that the client does not have active request at the moment
            }
        }

    private:
        std::shared_ptr<TCPIP::FixedBufferQueue> queue;
    };

}