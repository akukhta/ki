#pragma once
#include "../ClientRequest.hpp"
#include <iostream>

namespace TCPIP {
    class IRequestHandler
    {
    public:
        virtual void handleRequest(ClientRequest &request) = 0;

        virtual ~IRequestHandler()
        {
            std::cout << "Just a message to stacktrace" << std::endl;
        }

    };
}