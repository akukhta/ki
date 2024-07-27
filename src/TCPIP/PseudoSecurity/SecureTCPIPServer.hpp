#pragma once
#include "../Server/TCPIPServer.hpp"

namespace TCPIP {
    class SecureTCPIPServer : public TCPIPServer
    {
    public:
        SecureTCPIPServer(std::shared_ptr<FixedBufferQueue> queue, std::unique_ptr<IRequestHandler> requestHandler, std::shared_ptr<FileLogger> logger = nullptr);

    protected:
        void dataReceived(unsigned char *data, size_t len) override;
        void dataReadyToSend(unsigned char *data, size_t len) override;
    };
}