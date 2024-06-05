#include "ConnectedClient.hpp"

TCPIP::ConnectedClient::ConnectedClient(int socket, std::string clientIP, int clientPort)
    : socket(socket), clientIP(std::move(clientIP))
    {}

TCPIP::ConnectedClient::ConnectedClient()
    : socket(-1)
    {}

bool TCPIP::ConnectedClient::isBufferAvailable() const noexcept
{
    return currentRequest && currentRequest->buffer;
}

void TCPIP::ConnectedClient::createRequest()
{
    // Once we've created a request, a buffer should be given to it
    currentRequest = std::make_shared<ClientRequest>(shared_from_this());
}