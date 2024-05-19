#pragma once
#include <memory>
#include <optional>
#include "../../queue/TCPIPBuffer.hpp"
#include "RequestHeader.hpp"
#include "../../common/Serializer.hpp"

namespace TCPIP {
    enum class RequestState : char {NEW, RECEIVING, RECEIVED, PROCESSING, FINISHED};

    class ClientRequest
    {
    public:
        ClientRequest(std::shared_ptr<class ConnectedClient> ownerClient, std::shared_ptr<TCPIP::Buffer> buffer)
                : ownerClient(std::move(ownerClient)), buffer(std::move(buffer))
        {
            ;
        }

        void parseHeader()
        {
            header = RequestHeader();

            Serializer<SerializerType::NoBuffer>::deserialize(buffer->getData(), header->typeAsByte);
            Serializer<SerializerType::NoBuffer>::deserialize(buffer->getData() + sizeof(RequestHeader::type), header->messageLength);
        }

        void updateRequestState()
        {
            if (!header)
            {
                state = RequestState::NEW;
                return;
            }

            if (buffer->bytesUsed - RequestHeader::noAligmentSize() == header->messageLength)
            {
                state = RequestState::RECEIVED;
            }
            else
            {
                state = RequestState::RECEIVING;
            }
        }

        RequestType getRequestType()
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

        RequestState state = RequestState::NEW;

    private:
        std::shared_ptr<ConnectedClient> ownerClient;
        std::shared_ptr<TCPIP::Buffer> buffer;
        std::optional<RequestHeader> header = std::nullopt;

    };
}
