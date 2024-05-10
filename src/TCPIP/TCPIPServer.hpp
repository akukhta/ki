#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <unordered_map>
#include <memory>
#include "ClientRequest.hpp"
#include "IServer.hpp"

namespace TCPIP
{
    class TCPIPServer :public IServer
    {
    public:
        TCPIPServer(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue);

        virtual void run() override;
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

        void connectClient();

        void processRequest(int clientSocket);

        std::unordered_map<int, TCPIP::ClientRequest> clientRequests;
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
    };
}