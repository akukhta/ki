#pragma once
#include <utility>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include "ClientRequest.hpp"

namespace TCPIP
{
    class RequestHandler
    {
    public:
        static std::shared_ptr<RequestHandler> getInstance()
        {
            static std::shared_ptr<RequestHandler> instance{new RequestHandler};
            return instance;
        }

        void addRequest(std::shared_ptr<ClientRequest> request)
        {
            std::scoped_lock lk{mutex};
            requests.push(std::move(request));
        }

    private:
        RequestHandler()
        {
            //handlingThread =
        }


        std::mutex mutex;
        std::queue<std::shared_ptr<ClientRequest>> requests;
        std::jthread handlingThread;
    };
}