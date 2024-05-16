#pragma once
#include <utility>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include "IRequest.h"

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

        void addRequest(std::shared_ptr<IRequest> request)
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
        std::queue<std::shared_ptr<IRequest>> requests;
        std::jthread handlingThread;
    };
}