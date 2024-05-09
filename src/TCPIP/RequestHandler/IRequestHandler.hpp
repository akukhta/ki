#pragma once
#include "../ClientRequest.hpp"

namespace TCPIP {
    class IRequestHandler
    {
    public:
        virtual void handleRequest(ClientRequest &request) = 0;
        virtual ~IRequestHandler() = default;
    };
}