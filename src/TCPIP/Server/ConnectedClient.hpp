#pragma once
#include <memory>
#include <optional>
#include "../Common/Buffer.hpp"
#include "../Request/ClientRequest.hpp"

namespace TCPIP
{
    /// Class represents connected client
    class ConnectedClient : public std::enable_shared_from_this<ConnectedClient>
    {
    public:
        ConnectedClient(int socket, std::string clientIP);
        ConnectedClient();

        /// Check if the client has a buffer to use
        bool isBufferAvailable() const noexcept;

        /// Function to create a client's request
        void createRequest();

        /// Current requests received/being received from the client
        std::shared_ptr<ClientRequest> currentRequest = nullptr;
        /// Client ip
        std::string clientIP;

        /// Communication endpoint descriptor
        int socket;
    };
}
