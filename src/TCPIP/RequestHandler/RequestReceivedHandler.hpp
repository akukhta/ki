#pragma once
#include <memory>
#include "IHandler.hpp"
#include "../../queue/BufferedQueue.hpp"
#include "../../writer/MultiFileWriter.hpp"
#include "../RequestHeader.hpp"

class RequestReceivedHandler : public IHandler
{
public:
    RequestReceivedHandler(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue,
        std::shared_ptr<MultiFileWriter> fileWriter);

    void handle(TCPIP::ClientRequest &request) override
    {
        if (request.state != TCPIP::RequestState::REQUEST_RECEIVED)
        {
            IHandler::handle(request);
        }

        switch(request.header.requestType)
        {
            case TCPIP::Request::FILE_INFO:
            {
                fileWriter->registerNewFile(request.getClientID(), )
                break;
            }

        }
    }

private:
    std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
    std::shared_ptr<MultiFileWriter> fileWriter;
};