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
#include "../../common/FileLogger.hpp"

class ToolFactory;

namespace TCPIP
{
    /// TCPIP Server
    class TCPIPServer :public IServer
    {
    public:
        TCPIPServer(std::shared_ptr<FixedBufferQueue> queue, std::unique_ptr<IRequestHandler> requestHandler, std::shared_ptr<FileLogger> logger = nullptr);
        ~TCPIPServer();

        /// Starts the server in its own thread
        virtual void run() override;

    private:
        void runFunction();

        friend class ::ToolFactory;
        /// Function to notify client when file writing fnished
        void fileWriteFinished(int clientSocket);

        // epoll
        static size_t constexpr MAX_EVENTS_PER_ITER = 32;
        epoll_event events[MAX_EVENTS_PER_ITER];
        int epollFD;
        epoll_event masterSocketEvent;

        // network
        int masterSocket;
        sockaddr_in socketAddress;

        std::jthread serverThread;

        /// New client connects to the server
        void connectClient();

        /// Process received data
        void processReceivedData(int clientSocket);
        void receiveData(int clientSocket);
        void clientDisconnected(int clientSocket);
        void addSocketToEpoll(int socket);
        void handleEpollEvents();
        void handleScheduledClients();
        void sendResponse(TCPIP::ServerResponse response, int socket);

        /// Non blocking function to try to obtain a buffer for client's request
        bool tryGetClientBuffer(int clientSocket);

        std::unordered_map<int, TCPIP::RequestHeader> headerCache;
        std::unordered_map<int, std::shared_ptr<TCPIP::ConnectedClient>> clients;
        std::unique_ptr<IRequestHandler> requestHandler;
        std::shared_ptr<FixedBufferQueue> queue;
        std::shared_ptr<FileLogger> logger;

        std::queue<int> scheduledClients;
        int eventsTriggered = 0;
    };
}