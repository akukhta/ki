#include "SecureTCPIPServer.hpp"

TCPIP::SecureTCPIPServer::SecureTCPIPServer(std::shared_ptr<FixedBufferQueue> queue,
    std::unique_ptr<IRequestHandler> requestHandler, std::shared_ptr<FileLogger> logger,
    std::unique_ptr<IEncryption> encryption) :
        TCPIPServer(queue, std::move(requestHandler), logger), encryption(std::move(encryption))
{
}

void TCPIP::SecureTCPIPServer::dataReceived(unsigned char *data, size_t len)
{
    encryption->decrypt(data, len);
}

void TCPIP::SecureTCPIPServer::dataProcessBeforeSend(unsigned char *data, size_t len)
{
    encryption->encrypt(data, len);
}
