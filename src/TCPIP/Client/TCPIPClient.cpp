#include"TCPIPClient.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <utility>
#include <filesystem>
#include <format>
#include "../../common/Serializer.hpp"
#include "../../common/Logger.hpp"
#include "../Common/FileInfo.hpp"
#include "../Common/TCPIPToolSettingsParser.hpp"
#include "../Request/RequestCreator.hpp"

TCPIP::TCPIPClient::TCPIPClient(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue)
    : queue(std::move(queue))
{
    socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddress.sin_family = AF_INET;
}

void TCPIP::TCPIPClient::connectToServer()
{
    auto settings = TCPIPToolSettingsParser::getInstance();

    serverAddress.sin_addr.s_addr = inet_addr(settings->getServerIP().c_str());
    serverAddress.sin_port = htons(settings->getServerPort());
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

std::vector<unsigned char> TCPIP::TCPIPClient::receive()
{
    std::vector<unsigned char> buffer{sizeof(size_t)};
    recv(socketFD, buffer.data(), buffer.size(), 0);

    size_t sizeOfBuffer = *reinterpret_cast<size_t*>(buffer.data());
    buffer.resize(sizeOfBuffer);
    recv(socketFD, buffer.data(), buffer.size(), 0);
    return buffer;
}

void TCPIP::TCPIPClient::sendFileChunk(TCPIP::Buffer &buffer)
{
    auto ptr = buffer.getData();
    Serializer<SerializerType::NoBuffer>::overwrite(ptr, 0, std::to_underlying(TCPIP::RequestType::FILE_CHUNK_RECEIVED));
    Serializer<SerializerType::NoBuffer>::overwrite(ptr, sizeof(RequestHeader::type), static_cast<short>(buffer.bytesUsed));

    sendToServer(ptr, buffer.bytesUsed + RequestHeader::noAligmentSize());

    if (receiveResponse() == ServerResponse::CRITICAL_ERROR)
    {
        throw std::runtime_error("Internal Server Error");
    }
}

void TCPIP::TCPIPClient::sendToServer(unsigned char *ptr, size_t bufferSize)
{
    size_t sentBytes = ::send(socketFD, ptr, bufferSize, 0);
}

void TCPIP::TCPIPClient::sendFileInfo(std::string const& fileName)
{
    auto buffer = TCPIP::RequestCreator::createFileInfoRequest(fileName);
    sendToServer(buffer.data(), buffer.size());

    if (receiveResponse() != ServerResponse::REQUEST_RECEIVED)
    {
        throw std::runtime_error("Internal Server Error");
    }
}

TCPIP::ServerResponse TCPIP::TCPIPClient::receiveResponse() {
    TCPIP::ServerResponse response;
    size_t bytesRead = recv(socketFD, &response, sizeof(response), MSG_NOSIGNAL);

    if (bytesRead == sizeof(response))
    {
        return response;
    }
    else
    {
        throw std::runtime_error("Error occurred while received a response from the server");
    }
}

void TCPIP::TCPIPClient::sendFile(const std::string &fileName)
{
    size_t sent = 0;
    sendFileInfo(fileName);

    while (queue->isReadFinished() == false || queue->isEmpty() == false)
    {
        auto buffer = queue->getFilledBuffer().value();
        sendFileChunk(buffer);
        sent += buffer.bytesUsed;
        queue->returnBuffer(std::move(buffer));
    }

    if (receiveResponse() == ServerResponse::FILE_RECEIVED)
    {
        Logger::log(std::format("The file {} successfully sent", fileName));
    }
}

void TCPIP::TCPIPClient::disconnect()
{
    shutdown(socketFD, SHUT_RDWR);
    close(socketFD);
}
