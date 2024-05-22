#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <unordered_map>
#include <memory>
#include <concepts>
#include <queue>
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
        void runFunction();
    private:

        static size_t constexpr MAX_EVENTS_PER_ITER = 32;
        epoll_event events[MAX_EVENTS_PER_ITER];

        int masterSocket;
        int epollFD;
        epoll_event masterSocketEvent;

        sockaddr_in socketAddress;

        std::jthread serverThread;

        void connectClient();
        void processReceivedData(int clientSocket);
        void receiveData(int clientSocket);
        void clientDisconnected(int clientSocket);
        void addSocketToEpoll(int socket);
        void handleEpollEvents();
        void handleScheduledClients();

        bool send(unsigned char *data, size_t bufferSize, int socket);
        void sendResponse(TCPIP::ServerResponse response, int socket);

        bool tryGetClientBuffer(int clientSocket);

        std::unordered_map<int, TCPIP::RequestHeader> headerCache;
        std::unordered_map<int, std::shared_ptr<TCPIP::ConnectedClient>> clients;
        std::unique_ptr<IRequestHandler> requestHandler;
        std::shared_ptr<FixedBufferQueue> queue;

        std::queue<int> scheduledClients;
        int eventsTriggered = 0;
    };
}