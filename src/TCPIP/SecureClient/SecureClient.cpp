#include "SecureClient.hpp"
#include "../Request/RequestCreator.hpp"
#include "../Security/RSAEncryption.hpp"

TCPIP::SecureTCPIPClient::SecureTCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication,
    std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue, std::unique_ptr<IEncryption> encryption, std::unique_ptr<Chacha20Key> key)
        : TCPIPClient(std::move(clientCommunication), std::move(queue)), encryption(std::move(encryption)), key(std::move(key)),
        keyEncryption(std::make_unique<RSAEncryption>())
{
    // Exchange rsa keys with the server
    receiveRsaPublicKey();
    sendKeyPair();
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

void TCPIP::SecureTCPIPClient::receiveRsaPublicKey()
{
    auto key = receive(451);
    rsaPublicKey = std::make_unique<RSAKey>(RSAKey::loadKeyFromMem(key.data(), key.size(), RSAKey::KeyType::PUBLIC));
}

void TCPIP::SecureTCPIPClient::sendKeyPair()
{
    auto buffer = RequestCreator::createKeyPairRequest(key->getKey(), key->getIV());
    keyEncryption->encrypt(reinterpret_cast<char*>(buffer.data()), buffer.size(), *(rsaPublicKey.get()));
    clientCommunication->send(buffer.data(), buffer.size());
}