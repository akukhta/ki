#include "SecureTCPIPClient.hpp"
#include "XorEncryption.hpp"

TCPIP::SecureTCPIPClient::SecureTCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication, std::shared_ptr<::FixedBufferQueue<TCPIPTag>> &queue)
    : TCPIPClient(std::move(clientCommunication), queue)
{

}

void TCPIP::SecureTCPIPClient::dataReadyToSend(unsigned char *data, size_t len)
{
    XorEncryption::processData(data, len);
}

void TCPIP::SecureTCPIPClient::dataReceived(unsigned char *data, size_t len)
{
    XorEncryption::processData(data, len);
}
