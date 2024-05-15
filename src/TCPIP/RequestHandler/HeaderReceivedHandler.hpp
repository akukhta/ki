#pragma once
#include "IHandler.hpp"
#include "../../common/Serializer.hpp"

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

        Serializer<SerializerType::NoBuffer>::deserialize(buffer->getData(), request.header.requestTypeAsByte);
        Serializer<SerializerType::NoBuffer>::deserialize(buffer->getData() + sizeof(request.header.requestType), request.header.requestLength);

        request.state = TCPIP::RequestState::REQUEST_RECEIVING;

        IHandler::handle(request);
    }
};