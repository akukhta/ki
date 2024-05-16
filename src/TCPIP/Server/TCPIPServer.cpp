#include "TCPIPServer.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <iostream>
#include <format>
#include <memory>
#include "../Common/TCPUtiles.hpp"
#include "../../common/Logger.hpp"
#include "../Request/ObtainBufferRequest.hpp"
#include "../Request/RequestHandler.hpp"
#include "../../common/Serializer.hpp"
#include "../Request/RequestParser.hpp"

TCPIP::TCPIPServer::TCPIPServer(std::shared_ptr<FixedBufferQueue> queue)
    : queue(std::move(queue))
{
    masterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(5505);
    socketAddress.sin_addr.s_addr = inet_addr("192.168.0.80");

    bind(masterSocket, reinterpret_cast<sockaddr*>(&socketAddress), sizeof(socketAddress));
    TCPIP::Utiles::setSocketNonBlock(masterSocket);
    listen(masterSocket, SOMAXCONN);
    epollFD = epoll_create1(0);

    masterSocketEvent.data.fd = masterSocket;
    masterSocketEvent.events = EPOLLIN;
    epoll_ctl(epollFD, EPOLL_CTL_ADD, masterSocket, &masterSocketEvent);
}

void TCPIP::TCPIPServer::run()
{
    serverThread = std::jthread(&TCPIPServer::runFunction, this);
}

void TCPIP::TCPIPServer::runFunction()
{
    while(true)
    {
        int eventsTriggered = epoll_wait(epollFD, events, MAX_EVENTS_PER_ITER, -1);

        if (eventsTriggered == -1)
        {
            std::cout << "epoll_wait error: " << std::strerror(errno) << std::endl;
        }

        for (int i = 0; i < eventsTriggered; i++)
        {
            if (events[i].events & EPOLLIN)
            {
                // Read Event Triggered

                if (events[i].data.fd == masterSocket)
                {
                    connectClient();
                }
                else
                {
                    auto &client = clients[events[i].data.fd];
                    receiveRequest(client);
                }
            }
            else if (events[i].events & EPOLLHUP)
            {
                // Disconnect triggered
            }
        }
    }
}

size_t TCPIP::TCPIPServer::getConnectedClientsAmount()
{
    return clients.size();
}

void TCPIP::TCPIPServer::connectClient()
{
    sockaddr_in clientAddress;
    socklen_t addrLen = sizeof(clientAddress);

    int slaveSocket = accept(masterSocket, reinterpret_cast<sockaddr*>(&clientAddress), &addrLen);
    TCPIP::Utiles::setSocketNonBlock(slaveSocket);

    epoll_event slaveSocketEvent;
    slaveSocketEvent.data.fd = slaveSocket;
    slaveSocketEvent.events = EPOLLIN;

    auto clientIP = inet_ntoa(*reinterpret_cast<in_addr*>(&clientAddress));

    epoll_ctl(epollFD, EPOLL_CTL_ADD, slaveSocket, &slaveSocketEvent);

    //clientRequests.insert({slaveSocket, TCPIP::ClientRequest{clientIP, clientAddress.sin_port, queue, slaveSocket}});
    auto &connectedClient = clients.emplace(slaveSocket, slaveSocket).first->second;
    Logger::log(std::format("New Client {}:{} connected", clientIP, clientAddress.sin_port));

    // Try to obtain a buffer for the client
    // in order to store requests
    RequestHandler::getInstance()->addRequest(std::make_shared<TCPIP::ObtainBufferRequest>(queue, connectedClient));
}

void TCPIP::TCPIPServer::validateRequest(ConnectedClient &client)
{
    auto buffer = client.getBuffer();

    // Check if buffer contains header
    if (buffer->bytesUsed >= sizeof(RequestHeader::type) + sizeof(RequestHeader::messageLength))
    {
        // Check if header has not been cached
        if (headerCache.find(client.socket) == headerCache.end())
        {
            headerCache.insert(std::make_pair(client.socket, RequestHeader{}));

            Serializer<SerializerType::NoBuffer>::deserialize(buffer->getData(), headerCache[client.socket].typeAsByte);
            Serializer<SerializerType::NoBuffer>::deserialize(buffer->getData() + sizeof(RequestHeader::type), headerCache[client.socket].messageLength);
        }

        // Check if we've already received the entire request from the client
        if (buffer->bytesUsed - RequestHeader::noAligmentSize() == headerCache[client.socket].messageLength)
        {
            RequestHandler::getInstance()->addRequest(std::make_shared<TCPIP::RequestParser>(client, headerCache[client.socket]));
            headerCache.erase(client.socket);
        }
    }
}

void TCPIP::TCPIPServer::receiveRequest(TCPIP::ConnectedClient &client)
{
    if (!client.isBufferAvailable() || (client.currentRequest && client.currentRequest->getType() != RequestType::RECEIVING_REQUEST))
    {
        // The client already has an active request to be handled, ignore it for now
        return;
    }

    auto buffer = client.getBuffer();

    size_t bytesRead = recv(client.socket, buffer->appendBufferData(), BUFFER_SIZE, MSG_NOSIGNAL);
    buffer->bytesUsed += bytesRead;

    validateRequest(client);

    //Logger::log(std::format("Client {}:{} : read {}, current request length {}", request.clientIP, request.clientPort, bytesRead, buffer->bytesUsed));
}

