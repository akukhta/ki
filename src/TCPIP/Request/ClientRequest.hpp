#pragma once
#include <memory>
#include <optional>
#include <unordered_map>
#include <functional>
#include "../Common/Buffer.hpp"
#include "RequestHeader.hpp"
#include "../../common/Serializer.hpp"

namespace TCPIP {
    enum class RequestState : char {NEW, RECEIVING, RECEIVED, PROCESSING};

    /// Class represents request from a client
    class ClientRequest
    {
    public:
        explicit ClientRequest(std::shared_ptr<class ConnectedClient> ownerClient);

        /// Parse request header
        void parseHeader();

        /// Function to update the request state
        /// Since the request may be huge and may not be received by one read from socket operation
        /// the function should be called every time we receive client's data from the network
        void updateRequestState();

        /// Check if the request has been received
        /// (header received and request's payload data has been received completely)
        bool isRequestReceived() const;

        /// Get type of of the request
        RequestType getRequestType();

    private:
        friend class RequestHandler;
        friend class ConnectedClient;
        friend class TCPIPServer;

        /// Request's buffer
        std::shared_ptr<TCPIP::Buffer> buffer;

        /// Client has sent the request
        std::shared_ptr<ConnectedClient> ownerClient;

        std::optional<RequestHeader> header = std::nullopt;

        bool requestReceived = false;

        RequestState state = RequestState::NEW;

        /// Request sate handler
        /// State pattern is intentionally avoided because it requires more data than one byte of the request state
        /// and the amount of requests recived from clients may be huge under high load
        static std::unordered_map<RequestState, std::function<bool(ClientRequest&)>> stateHandlers;

        bool newStateHandler();
        bool receivingStateHandler();
        bool receivedStateHandler();
    };
}
