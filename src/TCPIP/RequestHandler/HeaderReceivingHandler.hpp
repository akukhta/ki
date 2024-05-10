#pragma once
#include "IHandler.hpp"

class HeaderReceivingHandler : public IHandler
{
public:
    void handle(TCPIP::ClientRequest &request) override
    {
        if (request.state != TCPIP::RequestState::HEADER_RECEIVING)
        {
            IHandler::handle(request);
        }

        auto buffer = request.getRequestBuffer();

        if (buffer->bytesUsed >= sizeof(request.header))
        {
            request.state = TCPIP::RequestState::HEADER_RECEIVED;
            IHandler::handle(request);
        }
    }
};