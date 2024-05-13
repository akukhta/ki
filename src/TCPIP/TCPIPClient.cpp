#include "TCPIPClient.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <utility>
#include <filesystem>
#include "../common/Serializer.hpp"
#include "TCPIPRequests.hpp"
#include "FileInfo.hpp"

TCPIP::TCPIPClient::TCPIPClient(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue, std::string const &fileName)
    : queue(std::move(queue)), fileName(fileName)
{
    socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddress.sin_family = AF_INET;
}

void TCPIP::TCPIPClient::connectToServer()
{
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(5505);
    isConnected = connect(socketFD, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == 0;
}

void TCPIP::TCPIPClient::send(const std::vector<unsigned char> &data)
{
    size_t sizeOfBuffer = data.size();

    ::send(socketFD, reinterpret_cast<char*>(&sizeOfBuffer), sizeof(size_t), 0);
    ::send(socketFD, data.data(), data.size(), 0);
}

std::vector<unsigned char> TCPIP::TCPIPClient::receive()
{
    std::vector<unsigned char> buffer{sizeof(size_t)};
    recv(socketFD, buffer.data(), buffer.size(), 0);

    size_t sizeOfBuffer = *reinterpret_cast<size_t*>(buffer.data());
    buffer.resize(sizeOfBuffer);
    recv(socketFD, buffer.data(), buffer.size(), 0);
    return buffer;
}

void TCPIP::TCPIPClient::run()
{
    connectToServer();

    sendFileInfo();

    while (!queue->isReadFinished() && !queue->isEmpty())
    {
        auto buffer = queue->getFilledBuffer().value();
        createFileChunkRequest(buffer);
        ssend(buffer.getData(), sizeof(TCPIP::RequestHeader) + buffer.bytesUsed);
        queue->returnBuffer(std::move(buffer));
    }
}

void TCPIP::TCPIPClient::runFunction() {

}

void TCPIP::TCPIPClient::createFileChunkRequest(TCPIP::Buffer &buffer)
{
    auto ptr = buffer.getData();

    Serializer serializer;
    serializer.serialize(&ptr, std::to_underlying(TCPIP::Request::FILE));
    serializer.serialize(&ptr, buffer.bytesUsed);
}

void TCPIP::TCPIPClient::ssend(unsigned char *ptr, size_t bufferSize)
{
    ::send(socketFD, ptr, bufferSize, 0);
}

void TCPIP::TCPIPClient::sendFileInfo()
{
    FileInfo info;
    info.fileSize = std::filesystem::file_size(fileName);
    info.fileName = std::filesystem::path(fileName).filename();

    auto fileInfoBuffer = info.serialize();

    ssend(fileInfoBuffer.data(), fileInfoBuffer.size());
}
