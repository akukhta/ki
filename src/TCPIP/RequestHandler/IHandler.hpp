#pragma once
#include <memory>
#include "../ClientRequest.hpp"

class IHandler
{
public:
    std::shared_ptr<IHandler> nextHandler;

    virtual void handle(TCPIP::ClientRequest &request)
    {
        if (nextHandler)
        {
            nextHandler->handle(request);
        }
    }
};