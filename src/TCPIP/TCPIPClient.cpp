#include "TCPIPClient.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <utility>
#include <filesystem>
#include <format>
#include "../common/Serializer.hpp"
#include "../common/Logger.hpp"
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

    std::getchar();

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

    Serializer<SerializerType::NoBuffer>::serialize(ptr, std::to_underlying(TCPIP::Request::FILE));
    Serializer<SerializerType::NoBuffer>::serialize(ptr, buffer.bytesUsed);
}

void TCPIP::TCPIPClient::ssend(unsigned char *ptr, size_t bufferSize)
{
    size_t sentBytes = ::send(socketFD, ptr, bufferSize, 0);
    Logger::log(std::format("Sent {} from {} bytes requested", sentBytes, bufferSize));
}

void TCPIP::TCPIPClient::sendFileInfo()
{
    Serializer<SerializerType::InternalBuffer> serializer;
    serializer.serialize(std::to_underlying(TCPIP::Request::FILE_INFO));
    serializer.serialize(size_t{0});
    serializer.serialize(fileName);
    serializer.serialize(std::filesystem::path(fileName).filename().string());

    auto &buffer = serializer.getBuffer();

    serializer.overwrite(sizeof(TCPIP::Request), buffer.size() - sizeof(TCPIP::Request) - sizeof(size_t));
    Logger::log("TCPIPClient: File info sent");

    ssend(buffer.data(), buffer.size());
}
