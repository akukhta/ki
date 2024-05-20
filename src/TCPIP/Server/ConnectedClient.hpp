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
        ConnectedClient(int socket, std::string clientIP, int clientPort);
        ConnectedClient();

        bool isBufferAvailable() const noexcept;

        void createRequest();

        std::shared_ptr<ClientRequest> currentRequest = nullptr;
        std::string clientIP;

        int clientPort;
        int socket;
    };
}
