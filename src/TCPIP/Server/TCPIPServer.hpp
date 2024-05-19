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
#include "../Request/IRequestHandler.hpp"

namespace TCPIP
{
    class TCPIPServer :public IServer
    {
    public:
        TCPIPServer(std::shared_ptr<FixedBufferQueue> queue, std::unique_ptr<IRequestHandler> requestHandler);

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
        void validateRequest(std::shared_ptr<ConnectedClient> client);
        void receiveRequest(std::shared_ptr<ConnectedClient> client);
        bool tryGetClientBuffer(std::shared_ptr<ConnectedClient> client);

        std::unordered_map<int, TCPIP::RequestHeader> headerCache;
        std::unordered_map<int, std::shared_ptr<TCPIP::ConnectedClient>> clients;
        std::unique_ptr<IRequestHandler> requestHandler;
        std::shared_ptr<FixedBufferQueue> queue;
    };
}