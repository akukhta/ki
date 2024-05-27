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
#include "../Request/RequestCreator.hpp"

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
        if (scheduledClients.empty())
        {
            eventsTriggered = epoll_wait(epollFD, events, MAX_EVENTS_PER_ITER, -1);
        }
        else
        {
            handleScheduledClients();
            eventsTriggered = epoll_wait(epollFD, events, MAX_EVENTS_PER_ITER, 500);
        }

        handleEpollEvents();
    }
}

void TCPIP::TCPIPServer::connectClient()
{
    sockaddr_in clientAddress;
    socklen_t addrLen = sizeof(clientAddress);

    int clientSocket = accept(masterSocket, reinterpret_cast<sockaddr*>(&clientAddress), &addrLen);
    TCPIP::Utiles::setSocketNonBlock(clientSocket);
    addSocketToEpoll(clientSocket);

    auto clientIP = inet_ntoa(*reinterpret_cast<in_addr*>(&clientAddress.sin_addr));
    clients.emplace(clientSocket, std::make_shared<TCPIP::ConnectedClient>(clientSocket, clientIP, htons(clientAddress.sin_port))).first->second;
    Logger::log(std::format("New Client {}:{} connected", clientIP, htons(clientAddress.sin_port)));
}

void TCPIP::TCPIPServer::processReceivedData(int clientSocket)
{
    // Header has been just received
    auto client = clients[clientSocket];

    // Check if the processing current request is valid
    if (!client->currentRequest)
    {
        //Logger::log("Processed request is nullptr");
        throw std::runtime_error("Processed request is nullptr");
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
        sendResponse(TCPIP::ServerResponse::REQUEST_RECEIVED, clientSocket);
    }
}

void TCPIP::TCPIPServer::receiveData(int clientSocket)
{
    auto client = clients[clientSocket];

    // If client does not have buffer already
    // try to obtain it
    // If the attempt has failed, leave and try next time
    if (!client->currentRequest)
    {
        client->currentRequest = std::make_shared<TCPIP::ClientRequest>(client);
    }

    if (!client->currentRequest->buffer)
    {
        if (!tryGetClientBuffer(clientSocket))
        {
            epoll_ctl(epollFD, EPOLL_CTL_DEL, clientSocket, nullptr);
            scheduledClients.push(clientSocket);
            Logger::log("Can`t obtain a buffer for the client right now, the request receiving postponed");
            return;
        }
    }

    auto buffer = clients[clientSocket]->currentRequest->buffer;

    size_t bytesRead = 0;

    bytesRead = recv(clientSocket, buffer->appendBufferData(), BUFFER_SIZE - buffer->bytesUsed,
                     MSG_NOSIGNAL);

    if (!bytesRead && errno != EWOULDBLOCK && errno != EAGAIN)
    {
        clientDisconnected(clientSocket);
        return;
    }

    buffer->bytesUsed += bytesRead;
    processReceivedData(clientSocket);
}

bool TCPIP::TCPIPServer::tryGetClientBuffer(int clientSocket)
{
    auto client = clients[clientSocket];

    if (!client->currentRequest)
    {
        client->currentRequest = std::make_shared<TCPIP::ClientRequest>(client);
    }

    if (client->currentRequest->buffer)
    {
        Logger::log("Client already owns a buffer or has active request");
    }
    else if (auto rv = queue->getFreeBufferNonBlock(); rv.has_value())
    {
        client->currentRequest->buffer = std::make_shared<TCPIP::Buffer>(std::move(rv.value()));
        client->currentRequest->buffer->owningClientID = client->socket;
        return true;
    }

    return false;
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

void TCPIP::TCPIPServer::addSocketToEpoll(int socket)
{
    epoll_event slaveSocketEvent;
    slaveSocketEvent.data.fd = socket;
    slaveSocketEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
    epoll_ctl(epollFD, EPOLL_CTL_ADD, socket, &slaveSocketEvent);
}

void TCPIP::TCPIPServer::handleEpollEvents()
{
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

void TCPIP::TCPIPServer::handleScheduledClients()
{
    Logger::log("handling scheduled events");

    while (!scheduledClients.empty())
    {
        int scheduledSocket = scheduledClients.front();

        if (clients.find(scheduledSocket) == clients.end())
        {
            // The client has already disconnected
            scheduledClients.pop();
            continue;
        }

        if (!tryGetClientBuffer(scheduledSocket))
        {
            // if we can`t obtain a buffer, there is no point to continue the iteration
            return;
        }

        scheduledClients.pop();
        receiveData(scheduledSocket);
        addSocketToEpoll(scheduledSocket);
    }
}

bool TCPIP::TCPIPServer::send(unsigned char *data, size_t bufferSize, int socket)
{
    return ::send(socket, data, bufferSize, MSG_NOSIGNAL) == bufferSize;
}

void TCPIP::TCPIPServer::sendResponse(TCPIP::ServerResponse response, int socket)
{
    ::send(socket, &response, sizeof(response), MSG_NOSIGNAL);
}

void TCPIP::TCPIPServer::fileWriteFinished(int clientSocket)
{
    sendResponse(ServerResponse::FILE_RECEIVED, clientSocket);
}
