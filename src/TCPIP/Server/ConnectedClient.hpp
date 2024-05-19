#pragma once
#include <memory>
#include <optional>
#include "../../queue/TCPIPBuffer.hpp"
#include "../Request/ClientRequest.hpp"

namespace TCPIP
{
    class ConnectedClient : public std::enable_shared_from_this<ConnectedClient>
    {
    public:
        ConnectedClient(int socket) : socket(socket){}
        ConnectedClient() : socket(-1) {}

        bool isBufferAvailable() const noexcept
        {
            return buffer != nullptr || currentRequest->buffer != nullptr;
        }

        void createRequest()
        {
            // Once we've created a request, a buffer should be given to it
            currentRequest = std::make_shared<ClientRequest>(shared_from_this(), buffer);
            buffer = nullptr;
        }

        std::shared_ptr<TCPIP::Buffer> getBuffer()
        {
            if (currentRequest)
            {
                return currentRequest->buffer;
            }
            else
            {
                return buffer;
            }
        }

        std::shared_ptr<ClientRequest> currentRequest = nullptr;
        std::shared_ptr<TCPIP::Buffer> buffer = nullptr;

        int socket;
    };
}
