#include "RequestHandler.hpp"
#include "../../common/Serializer.hpp"
#include "../Server/ConnectedClient.hpp"

std::unordered_map<TCPIP::RequestType, std::function<void(TCPIP::RequestHandler&, std::shared_ptr<TCPIP::ClientRequest>)>> TCPIP::RequestHandler::handlerFunctions =
{
    std::make_pair(TCPIP::RequestType::FILE_INFO_RECEIVED,
     std::bind(&TCPIP::RequestHandler::fileInfoReceived, std::placeholders::_1, std::placeholders::_2))
};


TCPIP::RequestHandler::RequestHandler(std::shared_ptr<TCPIP::FixedBufferQueue> queue,
    std::shared_ptr<MultiFileWriter> writer)
        : queue(std::move(queue)), writer(std::move(writer))
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
    while (token.stop_requested())
    {
        std::unique_lock<std::mutex> lk(mutex);
        cv.wait(lk, [this](){return !requests.empty();});

        auto &request = requests.front();
        requests.pop();

        handlerFunctions[request->getRequestType()](*this, request);
    }
}

TCPIP::RequestHandler::~RequestHandler()
{
    handlingThread.request_stop();
}

void TCPIP::RequestHandler::fileInfoReceived(std::shared_ptr<ClientRequest> request)
{
    auto &buffer = request->ownerClient->buffer;
    FileInfo fileInfo = FileInfo::deserialize(buffer->getRequestData());

    writer->registerNewFile(request->ownerClient->socket, fileInfo);

    request->ownerClient->buffer->reset();
    request->ownerClient->currentRequest = nullptr;
}

void TCPIP::RequestHandler::fileChunkReceived(std::shared_ptr<ClientRequest> request)
{
    queue->returnBuffer(std::move(*request->ownerClient->buffer));
}


