#pragma once
#include <memory>
#include <optional>
#include <unordered_map>
#include <functional>
#include "../Common/Buffer.hpp"
#include "RequestHeader.hpp"
#include "../../common/Serializer.hpp"

namespace TCPIP {
    enum class RequestState : char {NEW, RECEIVING, RECEIVED, PROCESSING, FINISHED};

    class ClientRequest
    {
    public:
        ClientRequest(std::shared_ptr<class ConnectedClient> ownerClient);

        void parseHeader();

        void updateRequestState();

        bool isRequestReceived() const;

        RequestType getRequestType();

        std::shared_ptr<TCPIP::Buffer> buffer;

    private:
        friend class RequestHandler;
        friend class ConnectedClient;
        std::shared_ptr<ConnectedClient> ownerClient;
        std::optional<RequestHeader> header = std::nullopt;
        bool requestReceived = false;
        RequestState state = RequestState::NEW;

        static std::unordered_map<RequestState, std::function<bool(ClientRequest&)>> stateHandlers;

        bool newHandler();
        bool receivingHandler();
        bool receivedHandler();
    };
}
