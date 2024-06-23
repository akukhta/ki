#pragma once
#include "../Client/TCPIPClient.hpp"
#include "../Security/IEncryption.hpp"
#include "../Security/Chacha20Key.hpp"

// use RSA to encrypt/decrypt file chunks
// use chacha20 to encrypt/decrypt answers from the server
namespace TCPIP {
    class SecureTCPIPClient : public TCPIPClient
    {
    public:
        SecureTCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication, std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue, std::unique_ptr<IEncryption> encryption, std::unique_ptr<Chacha20Key> key);
        virtual ~SecureTCPIPClient() = default;

    protected:
        void prepareBufferToSend(Buffer &buffer) override;

    private:
        std::unique_ptr<IEncryption> encryption;
        std::unique_ptr<Chacha20Key> key;
    };
}