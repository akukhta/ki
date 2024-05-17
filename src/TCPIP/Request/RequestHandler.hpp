#pragma once
#include <utility>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include "ICommand.h"

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

        void addRequest(std::shared_ptr<ICommand> request)
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
        std::queue<std::shared_ptr<ICommand>> requests;
        std::jthread handlingThread;
    };
}