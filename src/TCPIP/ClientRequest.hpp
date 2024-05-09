#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <utility>
#include <span>
#include <memory>
#include "../queue/BufferedQueue.hpp"
#include "../queue/Buffer.hpp"
#include "RequestHeader.hpp"

namespace TCPIP
{
    enum class RequestState : char {NEW, HEADER_RECEIVING, HEADER_RECEIVED, REQUEST_RECEIVING, REQUEST_RECEIVED};

    class ClientRequest
    {
    public:
        ClientRequest(std::string ip, int clientPort, std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue, int clientID)
            : clientIP(std::move(ip)), clientPort(clientPort), queue(queue), state(RequestState::NEW), clientID(clientID)
        {
        }

        std::shared_ptr<TCPIP::Buffer> getRequestBuffer()
        {
            if (state == RequestState::NEW || !buffer)
            {
                buffer = std::make_shared<TCPIP::Buffer>(queue->getFreeBuffer().value());
                buffer->setOwnerID(clientID);
            }

            return buffer;
        }

        void completeRequest()
        {
            buffer = nullptr;
            state = RequestState::NEW;
        }

        RequestState getState()
        {
            return state;
        }

        int getClientID()
        {
            return clientID;
        }

        size_t bytesToRead = BUFFER_SIZE;
        RequestState state;
        RequestHeader header;
        int clientID;

        std::string clientIP;
        int clientPort;

    private:
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
        std::shared_ptr<TCPIP::Buffer> buffer;
    };
}