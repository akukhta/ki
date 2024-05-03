#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <thread>

namespace TCPIP
{
    class TCPIPServer
    {
    public:
        TCPIPServer();

        void start();
        size_t getConnectedClientsAmount();

    private:
        void runFunction();

        static size_t constexpr MAX_EVENTS_PER_ITER = 32;
        epoll_event events[MAX_EVENTS_PER_ITER];

        int masterSocket;
        int epollFD;
        epoll_event masterSocketEvent;

        sockaddr_in socketAddress;

        std::atomic<size_t> connectedClients = 0;

        std::jthread serverThread;
    };
}