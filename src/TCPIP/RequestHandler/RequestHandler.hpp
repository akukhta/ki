#pragma once
#include "IRequestHandler.hpp"
#include "../../queue/BufferedQueue.hpp"
#include "../../writer/MultiFileWriter.hpp"
#include "IHandler.hpp"
#include <memory>
#include <unordered_map>

namespace TCPIP
{
    class RequestHandler : public IRequestHandler
    {
    public:
        RequestHandler(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue, std::shared_ptr<MultiFileWriter> fileWriter);

        virtual void handleRequest(ClientRequest& request) override;

    private:
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
        std::shared_ptr<MultiFileWriter> fileWriter;

        std::shared_ptr<IHandler> handlers;
    };
}