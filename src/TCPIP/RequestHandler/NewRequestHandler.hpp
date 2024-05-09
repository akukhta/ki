#pragma once
#include "IHandler.hpp"
#include "../RequestHeader.hpp"

class NewRequestHandler : public IHandler
{
public:
    void handle(TCPIP::ClientRequest &request) override
    {
        // Early Return
        if (request.getState() != TCPIP::RequestState::NEW)
        {
            IHandler::handle(request);
        }

        auto buffer = request.getRequestBuffer();

        if (buffer->bytesUsed >= sizeof(TCPIP::RequestHeader))
        {
            request.state = TCPIP::RequestState::HEADER_RECEIVED;
        }
        else
        {
            request.state = TCPIP::RequestState::HEADER_RECEIVING;
        }

        IHandler::handle(request);
    }
};