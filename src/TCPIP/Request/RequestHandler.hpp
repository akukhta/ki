#pragma once
#include <utility>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <functional>
#include "ClientRequest.hpp"
#include "../Server/TCPIPQueue.hpp"
#include "../../writer/MultiFileWriter.hpp"
#include "IRequestHandler.hpp"

namespace std
{
    template <>
    struct hash<TCPIP::RequestType>
    {
        size_t operator()(TCPIP::RequestType const &type) const
        {
            return hash<char>()(to_underlying(type));
        }
    };
}

namespace TCPIP
{
    class RequestHandler : public IRequestHandler
    {
    public:
        RequestHandler(std::shared_ptr<TCPIP::FixedBufferQueue> queue, std::shared_ptr<MultiFileWriter> writer);
        ~RequestHandler();

        void addRequest(std::shared_ptr<ClientRequest> request) override;
        void startHandling() override;

    private:
        void handle(std::stop_token token);

        std::mutex mutex;
        std::condition_variable cv;

        std::queue<std::shared_ptr<ClientRequest>> requests;
        std::shared_ptr<TCPIP::FixedBufferQueue> queue;
        std::shared_ptr<MultiFileWriter> writer;
        std::jthread handlingThread;

        void fileInfoReceived(std::shared_ptr<ClientRequest> request);
        void fileChunkReceived(std::shared_ptr<ClientRequest> request);

        static std::unordered_map<TCPIP::RequestType, std::function<void(TCPIP::RequestHandler&, std::shared_ptr<ClientRequest>)>> handlerFunctions;
    };
}