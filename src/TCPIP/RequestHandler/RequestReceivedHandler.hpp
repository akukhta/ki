#pragma once
#include <memory>
#include <format>
#include "IHandler.hpp"
#include "../../queue/BufferedQueue.hpp"
#include "../../writer/MultiFileWriter.hpp"
#include "../RequestHeader.hpp"
#include "../../common/Serializer.hpp"
#include "../FileInfo.hpp"
#include "../../common/Logger.hpp"

class RequestReceivedHandler : public IHandler
{
public:
    RequestReceivedHandler(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue,
        std::shared_ptr<MultiFileWriter> fileWriter)
        : queue(queue), fileWriter(fileWriter) {}

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
                auto fileInfo = TCPIP::FileInfo::deserialize(request.getRequestBuffer()->getRequestData());

                fileInfo.port = request.clientPort;
                fileInfo.senderIP = request.clientIP;

                fileWriter->registerNewFile(request.clientID, fileInfo);

                request.completeRequest();

                Logger::log(std::format("Client {}:{} : file info received({}/{})", request.clientIP, request.clientPort, fileInfo.fileName, fileInfo.fileSize));

                break;
            }

            case TCPIP::Request::FILE:
            {
                request.getRequestBuffer()->owningClientID = request.clientID;

                queue->returnBuffer(std::move(*request.getRequestBuffer()));
                request.completeRequest();
                Logger::log(std::format("Client {}:{} : file chunk received", request.clientIP, request.clientPort));
                break;
            }
        }
    }

private:
    std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
    std::shared_ptr<MultiFileWriter> fileWriter;
};