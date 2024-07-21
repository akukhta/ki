#pragma once
#include "../Client/TCPIPClient.hpp"
#include "IEncryption.hpp"

namespace TCPIP
{
    class SecureTCPIPClient : public TCPIPClient
    {
    public:
        SecureTCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication, std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue, std::unique_ptr<IEncryption> encryption);

    private:
        void dataReceived(unsigned char *data, size_t len) override;

        void dataProcessBeforeSend(unsigned char *data, size_t len) override;

    private:
        std::unique_ptr<IEncryption> encryption;
    };
}