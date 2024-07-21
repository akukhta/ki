#include "SecureTCPIPServer.hpp"

TCPIP::SecureTCPIPServer::SecureTCPIPServer(std::shared_ptr<FixedBufferQueue> queue,
    std::unique_ptr<IRequestHandler> requestHandler, std::shared_ptr<IEncryption> chachaEncryption, std::shared_ptr<RSAEncryption> rsaEncryption, std::shared_ptr<RSAKey> serverRSAKey, std::shared_ptr<KeyManager<Chacha20Key>> keyManager, std::shared_ptr<FileLogger> logger)
        : TCPIPServer(queue, std::move(requestHandler), logger), rsaEncryption(std::move(rsaEncryption)), chacha20Encryption(std::move(chachaEncryption)), serverRSAKey(std::move(serverRSAKey)), keyManager(std::move(keyManager))
{
    publicKeyBin = this->serverRSAKey->getPublicKeyBin();
}

void TCPIP::SecureTCPIPServer::connectClient()
{
    TCPIPServer::connectClient();
    sendPublicKey(lastConnectedClient);
}

void TCPIP::SecureTCPIPServer::sendPublicKey(int socket)
{
    send(socket, publicKeyBin.data(), publicKeyBin.size(), MSG_NOSIGNAL);
}

void TCPIP::SecureTCPIPServer::dataReceived(std::shared_ptr<TCPIP::ClientRequest> &request)
{
    if (request->buffer->bytesUsed >= RequestHeader::noAligmentSize())
    {
        chacha20Encryption->decrypt(reinterpret_cast<char *>(request->buffer->data), RequestHeader::noAligmentSize(),
                                    keyManager->getKey(request->buffer->owningClientID));
    }
}
