#pragma once
#include <memory>

namespace TCPIP
{
    class IRequestHandler
    {
    public:
        virtual void addRequest(std::shared_ptr<class ClientRequest> request) = 0;
        virtual void startHandling() = 0;
        ~IRequestHandler() = default;
    };
}