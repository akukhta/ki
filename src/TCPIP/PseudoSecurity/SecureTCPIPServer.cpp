#include "SecureTCPIPServer.hpp"
#include "XorEncryption.hpp"

void TCPIP::SecureTCPIPServer::dataReceived(unsigned char *data, size_t len)
{
    XorEncryption::processData(data, len);
}

void TCPIP::SecureTCPIPServer::dataReadyToSend(unsigned char *data, size_t len)
{
    XorEncryption::processData(data, len);
}

TCPIP::SecureTCPIPServer::SecureTCPIPServer(std::shared_ptr<FixedBufferQueue> queue,
    std::unique_ptr<IRequestHandler> requestHandler, std::shared_ptr<FileLogger> logger) :
        TCPIPServer(queue, std::move(requestHandler), logger)
{

}
