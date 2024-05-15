#pragma once
#include "IHandler.hpp"

class RequestReceivingHandler : public IHandler
{
public:
    void handle(TCPIP::ClientRequest &request) override
    {
        if (request.state != TCPIP::RequestState::REQUEST_RECEIVING)
        {
            IHandler::handle(request);
        }

        auto buffer = request.getRequestBuffer();

        if (buffer->bytesUsed - sizeof(request.header.requestType) - sizeof(request.header.requestLength) == request.header.requestLength)
        {
            request.state = TCPIP::RequestState::REQUEST_RECEIVED;
            IHandler::handle(request);
        }
        else
        {
            request.bytesToRead = request.header.requestLength - buffer->bytesUsed + sizeof(request.header);
        }
    }
};