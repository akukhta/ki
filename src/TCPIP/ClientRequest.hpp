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

namespace TCPIP
{
    enum class RequestState : char {NEW, HEADER_RECEIVING, HEADER_RECEIVED, REQUEST_RECEIVING, REQUEST_RECEIVED};

    class ClientRequest
    {
    public:
        ClientRequest(std::string ip, std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue, int clientID)
            : ip(std::move(ip)), queue(queue), state(RequestState::NEW), clientID(clientID)
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

    private:
        std::string ip;
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
        std::shared_ptr<TCPIP::Buffer> buffer;
        RequestState state;
        int clientID;
    };
}