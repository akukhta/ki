#pragma once
#include "../Client/TCPIPClient.hpp"

namespace TCPIP {
    class SecureTCPIPClient : public TCPIPClient
    {
    public:
        SecureTCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication, std::shared_ptr<::FixedBufferQueue<TCPIPTag>> &queue);

    protected:
        void dataReadyToSend(unsigned char *data, size_t len) override;

        void dataReceived(unsigned char *data, size_t len) override;
    };
}