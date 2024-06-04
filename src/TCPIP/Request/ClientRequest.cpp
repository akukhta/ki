#include "ClientRequest.hpp"

std::unordered_map<TCPIP::RequestState, std::function<bool(TCPIP::ClientRequest&)>> TCPIP::ClientRequest::stateHandlers =
{
        {RequestState::NEW, std::bind(&TCPIP::ClientRequest::newHandler, std::placeholders::_1)},
        {RequestState::RECEIVING, std::bind(&TCPIP::ClientRequest::receivingHandler, std::placeholders::_1)},
        {RequestState::RECEIVED, std::bind(&TCPIP::ClientRequest::receivedHandler, std::placeholders::_1)},
};

TCPIP::ClientRequest::ClientRequest(std::shared_ptr<struct ConnectedClient> ownerClient)
        : ownerClient(std::move(ownerClient))
{
    ;
}

void TCPIP::ClientRequest::parseHeader()
{
    header = RequestHeader();

    Serializer<SerializerType::NoBuffer>::deserialize(buffer->getData(), header->typeAsByte);
    Serializer<SerializerType::NoBuffer>::deserialize(buffer->getData() + sizeof(RequestHeader::type), header->messageLength);
}

void TCPIP::ClientRequest::updateRequestState()
{
    while (true)
    {
        if (stateHandlers.find(state) == stateHandlers.end() || stateHandlers[state](*this) == false)
        {
            return;
        }
    }
}

bool TCPIP::ClientRequest::isRequestReceived() const
{
    return requestReceived;
}

TCPIP::RequestType TCPIP::ClientRequest::getRequestType()
{
    if (header)
    {
        return header->type;
    }
    else
    {
        throw std::runtime_error("Header is not parsed");
    }
}

bool TCPIP::ClientRequest::newHandler()
{
    if (buffer->bytesUsed >= sizeof(RequestHeader::type) + sizeof(RequestHeader::messageLength))
    {
        parseHeader();
        state = RequestState::RECEIVING;
        return true;
    }
    else
    {
        return false;
    }
}

bool TCPIP::ClientRequest::receivingHandler()
{
    if (buffer->bytesUsed - RequestHeader::noAligmentSize() == header->messageLength)
    {
        state = RequestState::RECEIVED;
        return true;
    }
    else
    {
        return false;
    }
}

bool TCPIP::ClientRequest::receivedHandler()
{
    requestReceived = true;
    return false;
}
