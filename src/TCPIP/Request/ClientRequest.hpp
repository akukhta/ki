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
        ClientRequest(std::shared_ptr<class ConnectedClient> ownerClient)
                : ownerClient(std::move(ownerClient))
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
            switch (state)
            {
                case RequestState::NEW:
                {
                    if (buffer->bytesUsed >= sizeof(RequestHeader::type) + sizeof(RequestHeader::messageLength))
                    {
                        parseHeader();
                        state = RequestState::RECEIVING;
                    }
                }

                case RequestState::RECEIVING:
                {
                    if (buffer->bytesUsed - RequestHeader::noAligmentSize() == header->messageLength)
                    {
                        state = RequestState::RECEIVED;
                    }
                }

                case RequestState::RECEIVED:
                {
                    requestReceived = true;
                    break;
                }

                default:
                    break;
            }
        }

        bool isRequestReceived() const
        {
            return requestReceived;
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
        std::shared_ptr<TCPIP::Buffer> buffer;

    private:
        friend class RequestHandler;
        friend class ConnectedClient;
        std::shared_ptr<ConnectedClient> ownerClient;
        std::optional<RequestHeader> header = std::nullopt;
        bool requestReceived = false;
    };
}
