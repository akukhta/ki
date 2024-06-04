#pragma once
#include "IClientCommunication.hpp"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace TCPIP {
    class TCPClientCommunication : public IClientCommunication
    {
    public:
        TCPClientCommunication(std::string serverIp, short serverPort);

        bool connect() override;
        size_t send(unsigned char *data, size_t size) override;
        std::vector<unsigned char> receive(size_t size) override;
        void disconnect() override;

    private:
        std::string serverIp;
        short serverPort;
        int socket;
        sockaddr_in serverAddress;
    };
}