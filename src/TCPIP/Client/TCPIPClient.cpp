#include "TCPIPClient.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <utility>
#include <filesystem>
#include <format>
#include "../../common/Serializer.hpp"
#include "../../common/Logger.hpp"
#include "../Common/FileInfo.hpp"

TCPIP::TCPIPClient::TCPIPClient(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue, std::string const &fileName)
    : queue(std::move(queue)), fileName(fileName)
{
    socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddress.sin_family = AF_INET;
}

void TCPIP::TCPIPClient::connectToServer()
{
    serverAddress.sin_addr.s_addr = inet_addr("192.168.0.87");
    serverAddress.sin_port = htons(5505);
    isConnected = connect(socketFD, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == 0;

#ifdef DEBUG
    {
        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);
        if (getsockname(socketFD, (struct sockaddr *) &sin, &len) != -1)
            std::cout << "Client socket uses " << ntohs(sin.sin_port) << " port";
    }
#endif
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
    clientThread = std::jthread(&TCPIP::TCPIPClient::runFunction, this);
}

void TCPIP::TCPIPClient::runFunction()
{
    connectToServer();

    sendFileInfo();
    //std::this_thread::sleep_for(std::chrono::seconds (1));
    char a;
    recv(socketFD, &a, 1, MSG_NOSIGNAL);
    //std::getchar();

    while (queue->isReadFinished() == false || queue->isEmpty() == false)
    {
        auto buffer = queue->getFilledBuffer().value();
        createFileChunkRequest(buffer);
        queue->returnBuffer(std::move(buffer));
        recv(socketFD, &a, 1, MSG_NOSIGNAL);
        //std::getchar();
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    shutdown(socketFD, SHUT_RDWR);
    close(socketFD);
}

void TCPIP::TCPIPClient::createFileChunkRequest(TCPIP::Buffer &buffer)
{
    auto ptr = buffer.getData();
    Serializer<SerializerType::NoBuffer>::overwrite(ptr, 0, std::to_underlying(TCPIP::RequestType::FILE_CHUNK_RECEIVED));
    Serializer<SerializerType::NoBuffer>::overwrite(ptr, sizeof(RequestHeader::type), static_cast<short>(buffer.bytesUsed));

    ssend(ptr, buffer.bytesUsed + RequestHeader::noAligmentSize());
}

void TCPIP::TCPIPClient::ssend(unsigned char *ptr, size_t bufferSize)
{
    size_t sentBytes = ::send(socketFD, ptr, bufferSize, 0);
    //Logger::log(std::format("Sent {} from {} bytes requested", sentBytes, bufferSize));
}

void TCPIP::TCPIPClient::sendFileInfo()
{
    Serializer<SerializerType::InternalBuffer> serializer;
    serializer.serialize(std::to_underlying(RequestType::FILE_INFO_RECEIVED));
    serializer.serialize(short{0});
    serializer.serialize(std::filesystem::file_size(fileName));
    serializer.serialize(std::filesystem::path(fileName).filename().string());
    serializer.overwrite(sizeof(RequestType), static_cast<short>(serializer.getBuffer().size() - sizeof(RequestType) - sizeof(short)));
    auto &buffer = serializer.getBuffer();

    Logger::log("TCPIPClient: File info sent");

    ssend(buffer.data(), buffer.size());
}
