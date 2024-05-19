#include "TCPIPServer.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <iostream>
#include <format>
#include <memory>
#include "../Common/TCPUtiles.hpp"
#include "../../common/Logger.hpp"
#include "../Request/RequestHandler.hpp"
#include "../../common/Serializer.hpp"

TCPIP::TCPIPServer::TCPIPServer(std::shared_ptr<FixedBufferQueue> queue, std::unique_ptr<IRequestHandler> requestHandler)
    : queue(std::move(queue)), requestHandler(std::move(requestHandler))
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
    auto &connectedClient = clients.emplace(slaveSocket, std::make_shared<TCPIP::ConnectedClient>(slaveSocket)).first->second;
    Logger::log(std::format("New Client {}:{} connected", clientIP, clientAddress.sin_port));

    tryGetClientBuffer(connectedClient);
    // Try to obtain a buffer for the client
    // in order to store requests
    //RequestHandler::getInstance()->addRequest(commandFactory->createCommand(TCPIP::RequestType::CLIENT_CONNECTED, connectedClient));
}

void TCPIP::TCPIPServer::validateRequest(std::shared_ptr<ConnectedClient> client)
{
    // Header has been just received
    if (!client->currentRequest)
    {
        if (client->getBuffer()->bytesUsed >= sizeof(RequestHeader::type) + sizeof(RequestHeader::messageLength))
        {
            client->createRequest();
            client->currentRequest->parseHeader();
        }
    }

    client->currentRequest->updateRequestState();

    switch(client->currentRequest->state)
    {
        case RequestState::RECEIVING:
        {
            // Request is still being received, nothing to do yet
            return;
            break;
        }
        case RequestState::RECEIVED:
        {
            // Request has been recived, can be processed by requestHandler
            requestHandler->addRequest(client->currentRequest);
            break;
        }

        default:
        {
            break;
        }
    }
}

void TCPIP::TCPIPServer::receiveRequest(std::shared_ptr<ConnectedClient> client)
{
    // If client does not have buffer already
    // try to obtain it
    // If the attempt has failed, leave and try next time
    if (!client->isBufferAvailable() && !tryGetClientBuffer(client))
    {
        return;
    }

    auto buffer = client->getBuffer();

    size_t bytesRead = recv(client->socket, buffer->appendBufferData(), BUFFER_SIZE, MSG_NOSIGNAL);
    buffer->bytesUsed += bytesRead;

    validateRequest(client);

    //Logger::log(std::format("Client {}:{} : read {}, current request length {}", request.clientIP, request.clientPort, bytesRead, buffer->bytesUsed));
}

bool TCPIP::TCPIPServer::tryGetClientBuffer(std::shared_ptr<ConnectedClient> client)
{
    if (client->buffer || client->currentRequest)
    {
        Logger::log("Client already owns a buffer or has active request");
        return false;
    }

    auto rv = queue->getFreeBufferNonBlock();

    if (rv)
    {
        client->buffer = std::make_shared<TCPIP::Buffer>(std::move(rv.value()));
        return true;
    }
    else
    {
        return false;
    }
}

