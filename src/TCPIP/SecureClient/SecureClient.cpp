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

std::vector<unsigned char> TCPIP::SecureTCPIPClient::receive(size_t len)
{
    auto buffer = clientCommunication->receive(len);
    return std::move(buffer);
}

void TCPIP::SecureTCPIPClient::receiveKey()
{
    auto key = receive(442);
    rsaPublicKey = std::make_unique<RSAKey>(RSAKey::loadKeyFromMem(key.data(), key.size(), RSAKey::KeyType::PUBLIC));
}
