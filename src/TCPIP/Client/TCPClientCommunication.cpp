#include <arpa/inet.h>
#include <unistd.h>
#include "TCPClientCommunication.hpp"

bool TCPIP::TCPClientCommunication::connect()
{
    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverIp.c_str());
    serverAddress.sin_port = htons(serverPort);
    return ::connect(socket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == 0;
}

TCPIP::TCPClientCommunication::TCPClientCommunication(std::string serverIp, short serverPort)
    :   serverIp(std::move(serverIp)), serverPort(serverPort), socket(0)
{}

size_t TCPIP::TCPClientCommunication::send(unsigned char *data, size_t size)
{
    return ::send(socket, data, size, 0);
}

std::vector<unsigned char> TCPIP::TCPClientCommunication::receive(size_t size)
{
    std::vector<unsigned char> buffer(size);
    recv(socket, buffer.data(), size, MSG_NOSIGNAL);
    return buffer;
}

void TCPIP::TCPClientCommunication::disconnect()
{
    if (socket)
    {
        shutdown(socket, SHUT_RDWR);
        close(socket);
        socket = -1;
    }
}
