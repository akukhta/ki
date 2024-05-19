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
            return buffer != nullptr;
        }

        void createRequest()
        {
            currentRequest = std::make_shared<ClientRequest>(shared_from_this(), buffer);
        }

        std::shared_ptr<ClientRequest> currentRequest = nullptr;
        std::shared_ptr<TCPIP::Buffer> buffer;

        int socket;
    };
}
