#include "SecureTCPIPClient.hpp"

TCPIP::SecureTCPIPClient::SecureTCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication,
    std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue, std::unique_ptr<IEncryption> encryption)
    : TCPIPClient(std::move(clientCommunication), queue), encryption(std::move(encryption))
{
}

void TCPIP::SecureTCPIPClient::dataReceived(unsigned char *data, size_t len)
{
    encryption->decrypt(data, len);
}

void TCPIP::SecureTCPIPClient::dataProcessBeforeSend(unsigned char *data, size_t len)
{
    encryption->encrypt(data, len);
}
