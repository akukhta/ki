#include "ClientRequest.hpp"

std::unordered_map<TCPIP::RequestState, std::function<bool(TCPIP::ClientRequest&)>> TCPIP::ClientRequest::stateHandlers =
{
        {RequestState::NEW, [](TCPIP::ClientRequest& request){return request.newStateHandler();}},
        {RequestState::RECEIVING, [](TCPIP::ClientRequest& request){return request.receivingStateHandler();}},
        {RequestState::RECEIVED, [](TCPIP::ClientRequest& request){return request.receivedStateHandler();}},
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
    Serializer<SerializerType::NoBuffer>::deserialize(buffer->getData() + sizeof(RequestHeader::type), header->requestDataSize);
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

bool TCPIP::ClientRequest::newStateHandler()
{
    if (buffer->bytesUsed >= sizeof(RequestHeader::type) + sizeof(RequestHeader::requestDataSize))
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

bool TCPIP::ClientRequest::receivingStateHandler()
{
    if (buffer->bytesUsed - RequestHeader::noAligmentSize() == header->requestDataSize)
    {
        state = RequestState::RECEIVED;
        return true;
    }
    else
    {
        return false;
    }
}

bool TCPIP::ClientRequest::receivedStateHandler()
{
    requestReceived = true;
    return false;
}
