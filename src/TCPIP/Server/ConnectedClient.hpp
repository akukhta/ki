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
        ConnectedClient(int socket, std::string const &clientIP, int clientPort)
            : socket(socket), clientIP(clientIP), clientPort(clientPort) {}
        ConnectedClient() : socket(-1) {}
        ~ConnectedClient();

        bool isBufferAvailable() const noexcept
        {
            return buffer != nullptr || (currentRequest && currentRequest->buffer != nullptr);
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
        std::string clientIP;

        int clientPort;
        int socket;
    };
}
