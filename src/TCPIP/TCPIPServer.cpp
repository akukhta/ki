#include "TCPIPServer.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "TCPUtiles.hpp"
#include "TCPIPRequests.hpp"

TCPIP::TCPIPServer::TCPIPServer(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue, std::unique_ptr<IRequestHandler> requestHandler) :
    queue(queue), requestHandler(std::move(requestHandler))
{
    masterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(5505);
    socketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(masterSocket, reinterpret_cast<sockaddr*>(&socketAddress), sizeof(socketAddress));
    TCPIP::Utiles::setSocketNonBlock(masterSocket);

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
        size_t eventsTriggered = epoll_wait(epollFD, events, MAX_EVENTS_PER_ITER, -1);

        for (size_t i = 0; i < eventsTriggered; i++)
        {
            if (events[i].data.fd == masterSocket)
            {
                connectClient();
            }
            else
            {
                if (clientRequests.find(events[i].data.fd) == clientRequests.end())
                {
                    continue;
                }

                auto &request = clientRequests.at(events[i].data.fd);
                auto buffer = request.getRequestBuffer();

                size_t bytesRead = recv(events[i].data.fd, buffer->appendBufferData(), request.bytesToRead, MSG_NOSIGNAL);

                buffer->bytesUsed += bytesRead;

                requestHandler->handleRequest(request);
            }
        }
    }
}

size_t TCPIP::TCPIPServer::getConnectedClientsAmount()
{
    return clientRequests.size();
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

    clientRequests.insert({slaveSocket, TCPIP::ClientRequest{clientIP, clientAddress.sin_port, queue, slaveSocket}});
}

void TCPIP::TCPIPServer::processRequest(int clientSocket)
{
    /*
    auto buffer = clientRequests[clientSocket].getRequestBuffer();

    auto type = static_cast<Request>(buffer->getData()[0]);
    auto size = *reinterpret_cast<short*>(buffer->getData()[1]);

    switch(type)
    {
        case Request::FILE_INFO:
        {
            //
        }

        case Request::FILE:
        {
            //
        }
    }
    */
}
