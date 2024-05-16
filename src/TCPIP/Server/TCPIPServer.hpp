#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <unordered_map>
#include <memory>
#include "IServer.hpp"
#include "TCPIPQueue.hpp"
#include "ConnectedClient.hpp"
#include "../Request/RequestHeader.hpp"

namespace TCPIP
{
    class TCPIPServer :public IServer
    {
    public:
        TCPIPServer(std::shared_ptr<FixedBufferQueue> queue);

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
        void validateRequest(ConnectedClient &client);
        void receiveRequest(ConnectedClient &client);

        std::unordered_map<int, TCPIP::RequestHeader> headerCache;
        std::unordered_map<int, TCPIP::ConnectedClient> clients;
        std::shared_ptr<FixedBufferQueue> queue;
    };
}