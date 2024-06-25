#pragma once
#include <vector>
#include "../Server/TCPIPServer.hpp"
#include "../Security/RSAKey.hpp"
#include "../Security/RSAEncryption.hpp"

namespace TCPIP
{
    //1. Server generates RSA Keys
    //2. Server sends public RSA key to a connected client
    //3. A Client generates ChaCha20 key and IV and encrypts it with the RSA Key
    //4. Client sends the key pair to the server
    class SecureTCPIPServer : public TCPIPServer
    {
    public:
        SecureTCPIPServer(std::shared_ptr<FixedBufferQueue> queue, std::unique_ptr<IRequestHandler> requestHandler, std::shared_ptr<RSAEncryption> rsaEncryption, std::shared_ptr<RSAKey> serverRSAKey, std::shared_ptr<FileLogger> logger = nullptr);

    protected:
        void connectClient() override;

    private:
        void sendPublicKey(int socket);

        std::shared_ptr<RSAKey> serverRSAKey;
        std::vector<char> publicKeyBin;
        std::shared_ptr<RSAEncryption> rsaEncryption;
    };
}