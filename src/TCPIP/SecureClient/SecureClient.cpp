#include "SecureClient.hpp"

TCPIP::SecureTCPIPClient::SecureTCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication,
    std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue, std::unique_ptr<IEncryption> encryption, std::unique_ptr<Chacha20Key> key)
        : TCPIPClient(std::move(clientCommunication), std::move(queue)), encryption(std::move(encryption)), key(std::move(key))
{
    // Exchange rsa keys with the server
}

void TCPIP::SecureTCPIPClient::prepareBufferToSend(TCPIP::Buffer &buffer)
{
    encryption->encrypt(buffer, *key.get());
}

