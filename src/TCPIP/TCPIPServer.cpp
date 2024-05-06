#include "TCPIPServer.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "TCPUtiles.hpp"
#include "TCPIPRequests.hpp"

TCPIP::TCPIPServer::TCPIPServer(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue) :
    queue(queue), buffer(sizeof(size_t))
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

void TCPIP::TCPIPServer::start()
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
                size_t bytesRead = recv(events[i].data.fd, connectedClients[events[i].data.fd].getBuffer(), BUFFER_SIZE, MSG_NOSIGNAL);
                connectedClients[events[i].data.fd].returnBuffer(bytesRead);

                processRequest(events[i].data.fd);
            }
        }
    }
}

size_t TCPIP::TCPIPServer::getConnectedClientsAmount()
{
    return connectedClients.size();
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

    connectedClients.insert({slaveSocket, TCPIP::ClientConnection(clientIP, queue)});
}

void TCPIP::TCPIPServer::processRequest(int clientSocket)
{
    auto buffer = connectedClients[clientSocket].getBuffer();

    auto type = static_cast<Request>(buffer[0]);
    auto size = *reinterpret_cast<short*>(buffer[1]);

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
}
