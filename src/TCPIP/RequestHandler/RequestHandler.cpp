#include "RequestHandler.hpp"
#include "NewRequestHandler.hpp"
#include "HeaderReceivedHandler.hpp"
#include "HeaderReceivingHandler.hpp"
#include "RequestReceivingHandler.hpp"
#include "RequestReceivedHandler.hpp"

TCPIP::RequestHandler::RequestHandler(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue,
    std::shared_ptr<MultiFileWriter> fileWriter)
        : queue(queue), fileWriter(fileWriter)
{
    handlers = std::make_shared<NewRequestHandler>();
    handlers->appendHandler(std::make_shared<HeaderReceivingHandler>());
    handlers->appendHandler(std::make_shared<HeaderReceived>());
    handlers->appendHandler(std::make_shared<RequestReceivingHandler>());
    handlers->appendHandler(std::make_shared<RequestReceivedHandler>(this->queue, this->fileWriter));
}

void TCPIP::RequestHandler::handleRequest(TCPIP::ClientRequest &request)
{
    handlers->handle(request);
}
