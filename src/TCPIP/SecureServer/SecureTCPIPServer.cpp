#include "SecureTCPIPServer.hpp"

TCPIP::SecureTCPIPServer::SecureTCPIPServer(std::shared_ptr<FixedBufferQueue> queue,
    std::unique_ptr<IRequestHandler> requestHandler, std::shared_ptr<RSAEncryption> rsaEncryption, std::shared_ptr<RSAKey> serverRSAKey, std::shared_ptr<FileLogger> logger)
        : TCPIPServer(queue, std::move(requestHandler), logger), rsaEncryption(std::move(rsaEncryption)), serverRSAKey(std::move(serverRSAKey))
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