#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <unordered_map>
#include <memory>
#include "TCPIPClientConnection.hpp"

namespace TCPIP
{
    class TCPIPServer
    {
    public:
        TCPIPServer(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue);

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

        std::jthread serverThread;

        std::vector<unsigned char> buffer;

        void connectClient();

        void processRequest(int clientSocket);

        std::unordered_map<int, TCPIP::ClientConnection> connectedClients;
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
    };
}