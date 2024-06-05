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
#include "../Server/MultiFileWriter.hpp"
#include "IRequestHandler.hpp"
#include "../../common/FileLogger.hpp"

/// std::hash specialization for RequestType in order to store handler for each request in unordered map
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
        RequestHandler(std::shared_ptr<TCPIP::FixedBufferQueue> queue, std::shared_ptr<MultiFileWriter> writer, std::shared_ptr<FileLogger> logger = nullptr);
        ~RequestHandler();

        /// Add a request to the processing queue
        void addRequest(std::shared_ptr<ClientRequest> request) override;

        /// Start handling thread
        void startHandling() override;

    private:
        void handle(std::stop_token token);

        // Sync
        std::mutex mutex;
        std::condition_variable cv;

        /// Requests queue
        std::queue<std::shared_ptr<ClientRequest>> requests;

        /// Queue (memory pool) to return used buffers into
        std::shared_ptr<TCPIP::FixedBufferQueue> queue;

        /// File writer to register new file for writing
        std::shared_ptr<MultiFileWriter> writer;

        std::jthread handlingThread;

        std::shared_ptr<FileLogger> logger;

        // Request handlers
        void fileInfoReceived(std::shared_ptr<ClientRequest> request);
        void fileChunkReceived(std::shared_ptr<ClientRequest> request);

        /// Request handler map
        /// I've not used the class COR intentionally
        /// Since it requires iteration through the chain of handlers ( == O(n) complexity vs O(1) complexity)
        static std::unordered_map<TCPIP::RequestType, std::function<void(TCPIP::RequestHandler&, std::shared_ptr<ClientRequest>)>> handlerFunctions;
    };
}