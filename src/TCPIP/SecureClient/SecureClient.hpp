#pragma once
#include "../Client/TCPIPClient.hpp"
#include "../Security/IEncryption.hpp"
#include "../Security/RSAKey.hpp"

namespace TCPIP {
    class SecureTCPIPClient : public TCPIPClient
    {
    public:
        SecureTCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication, std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue, std::unique_ptr<IEncryption> encryption, RSAKey& key);
        virtual ~SecureTCPIPClient() = default;

    protected:
        void prepareBufferToSend(Buffer &buffer) override;

    private:
        std::unique_ptr<IEncryption> encryption;
        RSAKey& key;
    };
}