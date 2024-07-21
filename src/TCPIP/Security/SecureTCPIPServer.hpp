#pragma once
#include "../Server/TCPIPServer.hpp"
#include "IEncryption.hpp"
#include <iostream>

namespace TCPIP
{
    class SecureTCPIPServer : public TCPIPServer
    {
    public:
        SecureTCPIPServer(std::shared_ptr<FixedBufferQueue> queue, std::unique_ptr<IRequestHandler> requestHandler, std::shared_ptr<FileLogger> logger = nullptr, std::unique_ptr<IEncryption> encryption = nullptr);

        ~SecureTCPIPServer()
        {
            std::cout << "SSS DESTROYED\n";
        }

    private:
        void dataReceived(unsigned char *data, size_t len) override;
        void dataProcessBeforeSend(unsigned char *data, size_t len) override;

    private:
        std::unique_ptr<IEncryption> encryption;
    };
}