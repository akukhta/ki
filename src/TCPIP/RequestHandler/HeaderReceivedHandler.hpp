#pragma once
#include "IHandler.hpp"

class HeaderReceived : public IHandler
{
public:
    void handle(TCPIP::ClientRequest &request) override
    {
        // Early return
        if (request.state != TCPIP::RequestState::HEADER_RECEIVED)
        {
            IHandler::handle(request);
        }

        auto buffer = request.getRequestBuffer();
        request.header = *reinterpret_cast<TCPIP::RequestHeader const*>(buffer->getBufferData());

        request.state = TCPIP::RequestState::REQUEST_RECEIVING;

        IHandler::handle(request);
    }
};