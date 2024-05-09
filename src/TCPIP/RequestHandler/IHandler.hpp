#pragma once
#include <memory>
#include "../ClientRequest.hpp"

class IHandler
{
public:
    std::shared_ptr<IHandler> nextHandler = nullptr;

    void appendHandler(std::shared_ptr<IHandler> nextHandler)
    {
        if (this->nextHandler)
        {
            this->nextHandler->appendHandler(nextHandler);
        }
        else
        {
            this->nextHandler = nextHandler;
        }
    }

    virtual void handle(TCPIP::ClientRequest &request)
    {
        if (nextHandler)
        {
            nextHandler->handle(request);
        }
    }
};