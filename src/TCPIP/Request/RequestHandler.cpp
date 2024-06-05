#include "RequestHandler.hpp"
#include "../Server/ConnectedClient.hpp"

std::unordered_map<TCPIP::RequestType, std::function<void(TCPIP::RequestHandler&, std::shared_ptr<TCPIP::ClientRequest>&)>> TCPIP::RequestHandler::handlerFunctions =
{
    {TCPIP::RequestType::FILE_INFO_RECEIVED,
            [](RequestHandler& handler, std::shared_ptr<ClientRequest> &request){handler.fileInfoReceived(request);}},

    {TCPIP::RequestType::FILE_CHUNK_RECEIVED,
            [](RequestHandler& handler, std::shared_ptr<ClientRequest> &request){handler.fileChunkReceived(request);}}
};


TCPIP::RequestHandler::RequestHandler(std::shared_ptr<TCPIP::FixedBufferQueue> queue,
    std::shared_ptr<MultiFileWriter> writer, std::shared_ptr<FileLogger> logger)
        : queue(std::move(queue)), writer(std::move(writer)), logger(std::move(logger))
{
}

void TCPIP::RequestHandler::addRequest(std::shared_ptr<ClientRequest> request)
{
    std::scoped_lock lk{mutex};
    request->state = RequestState::PROCESSING;
    requests.push(std::move(request));
    cv.notify_one();
}

void TCPIP::RequestHandler::startHandling()
{
    handlingThread = std::jthread(&TCPIP::RequestHandler::handle, this);
}

void TCPIP::RequestHandler::handle(std::stop_token token)
{
    while (!token.stop_requested())
    {
        std::unique_lock<std::mutex> lk(mutex);
        cv.wait(lk, [this](){return !requests.empty();});

        auto request = requests.front();
        requests.pop();

        if (handlerFunctions.find(request->getRequestType()) != handlerFunctions.end())
        {
            handlerFunctions[request->getRequestType()](*this, request);
        }
        else
        {
            if(logger)
            {
                logger->log("Incorrect handler");
            }
        }
    }
}

TCPIP::RequestHandler::~RequestHandler()
{
    handlingThread.request_stop();
}

void TCPIP::RequestHandler::fileInfoReceived(std::shared_ptr<ClientRequest>& request)
{
    auto &buffer = request->buffer;
    FileInfo fileInfo = FileInfo::deserialize(buffer->getRequestData());

    writer->registerNewFile(request->ownerClient->socket, fileInfo, request->ownerClient->clientIP);

    request->buffer->reset();
    queue->releaseBuffer(std::move(*request->buffer));

    if (logger)
    {
        logger->log("Receiving {} from {}", fileInfo.fileName, request->ownerClient->clientIP);
    }
}

void TCPIP::RequestHandler::fileChunkReceived(std::shared_ptr<ClientRequest>& request)
{
    queue->returnBuffer(std::move(*request->buffer));
}


