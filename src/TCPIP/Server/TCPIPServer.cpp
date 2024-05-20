#include "TCPIPServer.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
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
    socketAddress.sin_addr.s_addr = inet_addr("192.168.0.87");

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
    requestHandler->startHandling();
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
                    receiveData(events[i].data.fd);
                }
            }

            if ((events[i].events & EPOLLHUP) || (events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLERR))
            {
                clientDisconnected(events[i].data.fd);
                continue;
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

    int clientSocket = accept(masterSocket, reinterpret_cast<sockaddr*>(&clientAddress), &addrLen);
    TCPIP::Utiles::setSocketNonBlock(clientSocket);

    epoll_event slaveSocketEvent;
    slaveSocketEvent.data.fd = clientSocket;
    slaveSocketEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;

    auto clientIP = inet_ntoa(*reinterpret_cast<in_addr*>(&clientAddress.sin_addr));

    epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSocket, &slaveSocketEvent);

    clients.emplace(clientSocket, std::make_shared<TCPIP::ConnectedClient>(clientSocket, clientIP, htons(clientAddress.sin_port))).first->second;
    Logger::log(std::format("New Client {}:{} connected", clientIP, htons(clientAddress.sin_port)));

    tryGetClientBuffer(clientSocket);
}

void TCPIP::TCPIPServer::processReceivedData(int clientSocket)
{
    // Header has been just received
    auto client = clients[clientSocket];

    // Create a request for a given client and
    // parse header if possible
    if (!client->currentRequest)
    {
        // Check if the client has received header
        if (client->getBuffer()->bytesUsed >= sizeof(RequestHeader::type) + sizeof(RequestHeader::messageLength))
        {
            // Create request and parse header
            client->createRequest();
            client->currentRequest->parseHeader();
        }
        else
        {
            // Return if header has not received yet (can`t be processed further)
            return;
        }
    }

    // Update request state
    // Check if the request has been received completely
    client->currentRequest->updateRequestState();

    // Check the request state
    // And send to the processing pipeline if possible

    if (client->currentRequest->isRequestReceived())
    {
        requestHandler->addRequest(client->currentRequest);
        client->currentRequest = nullptr;
        send(client->socket, char{0x01});
    }
}

void TCPIP::TCPIPServer::receiveData(int clientSocket)
{
    auto buffer = clients[clientSocket]->getBuffer();

    // If client does not have buffer already
    // try to obtain it
    // If the attempt has failed, leave and try next time
    if (!buffer && !tryGetClientBuffer(clientSocket))
    {
        return;
    }

    size_t bytesRead = recv(clientSocket, buffer->appendBufferData(), BUFFER_SIZE - buffer->bytesUsed, MSG_NOSIGNAL);

    if (!bytesRead)
    {
        clientDisconnected(clientSocket);
        return;
    }

    buffer->bytesUsed += bytesRead;
    Logger::log(std::format("Read {}, current request length {}", bytesRead, buffer->bytesUsed));
    processReceivedData(clientSocket);
}

bool TCPIP::TCPIPServer::tryGetClientBuffer(int clientSocket)
{
    auto client = clients[clientSocket];

    if (client->getBuffer())
    {
        Logger::log("Client already owns a buffer or has active request");
    }
    else if (auto rv = queue->getFreeBufferNonBlock(); rv)
    {
        client->buffer = std::make_shared<TCPIP::Buffer>(std::move(rv.value()));
        client->buffer->owningClientID = client->socket;
        return true;
    }

    return false;
}

TCPIP::TCPIPServer::~TCPIPServer()
{
    auto v = rand();
    v += rand();
}

void TCPIP::TCPIPServer::clientDisconnected(int clientSocket)
{
    if (clients.find(clientSocket) != clients.end())
    {
        epoll_ctl(epollFD, EPOLL_CTL_DEL, clientSocket, nullptr);
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
        Logger::log(std::format("Client {}:{} has disconnected", clients[clientSocket]->clientIP, clients[clientSocket]->clientPort));
        clients.erase(clientSocket);
    }
}

