#include "RequestHandler.hpp"

TCPIP::RequestHandler::RequestHandler(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue,
    std::shared_ptr<MultiFileWriter> fileWriter)
        : queue(queue), fileWriter(fileWriter)
{

}

void TCPIP::RequestHandler::handleRequest(TCPIP::ClientRequest &request)
{
    ;
}
