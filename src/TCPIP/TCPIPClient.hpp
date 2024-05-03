#pragma once
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace TCPIP
{
    class TCPIPClient
    {
    public:
        TCPIPClient();

        void connectToServer();
        void send(std::vector<unsigned char> const& data);
        std::vector<unsigned char> receive();

    private:
        int socketFD;
        sockaddr_in serverAddress;
        bool isConnected = false;
    };
}