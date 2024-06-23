#pragma once
#include <memory>

namespace TCPIP
{
    /// An interface of request handler server should use to process data from clients
    /// Designed to have its own thread in order to process requests
    class IRequestHandler
    {
    public:
        virtual void addRequest(std::shared_ptr<class ClientRequest> request) = 0;
        virtual void startHandling() = 0;
        ~IRequestHandler() = default;

    protected:
        virtual void handle(std::stop_token token) = 0;
    };
}