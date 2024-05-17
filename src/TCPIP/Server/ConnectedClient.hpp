#pragma once
#include <memory>
#include <optional>
#include "../../queue/TCPIPBuffer.hpp"
#include "../Request/ICommand.h"

namespace TCPIP
{
    class ConnectedClient
    {
    public:
        ConnectedClient(int socket) : socket(socket){}
        ConnectedClient() : socket(-1) {}

        bool isBufferAvailable() const noexcept
        {
            return buffer != nullptr;
        }

        std::shared_ptr<ICommand> currentRequest = nullptr;

        auto getBuffer()
        {
            return buffer;
        }

        int socket;
    private:
        friend class std::hash<TCPIP::ConnectedClient>;
        friend class ObtainBufferRequest;
        std::shared_ptr<TCPIP::Buffer> buffer;
    };
}
