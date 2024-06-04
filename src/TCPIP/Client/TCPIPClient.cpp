#include"TCPIPClient.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <utility>
#include <filesystem>
#include <format>
#include "../../common/Serializer.hpp"
#include "../Common/JsonSettingsParser.hpp"
#include "../Request/RequestCreator.hpp"
#include "../Common/Utiles.hpp"

TCPIP::TCPIPClient::TCPIPClient(std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue)
    : queue(std::move(queue))
{
    socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddress.sin_family = AF_INET;
}

void TCPIP::TCPIPClient::connectToServer()
{
    auto settings = JsonSettingsParser::getInstance();

    serverAddress.sin_addr.s_addr = inet_addr(settings->getServerIP().c_str());
    serverAddress.sin_port = htons(settings->getServerPort());
    isConnected = connect(socketFD, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == 0;
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

    auto bytesSent = sendToServer(ptr, buffer.bytesUsed + RequestHeader::noAligmentSize());

    progressBar->addToValue(bytesSent);

    if (receiveResponse() == ServerResponse::CRITICAL_ERROR)
    {
        throw std::runtime_error("Internal Server Error");
    }
}

size_t TCPIP::TCPIPClient::sendToServer(unsigned char *ptr, size_t bufferSize)
{
    return send(socketFD, ptr, bufferSize, 0);
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
    progressBar = std::make_unique<UI::CLIProgressBar>(TCPIP::Utiles::getFileNameOnly(fileName), std::filesystem::file_size(fileName), 0);

    size_t sent = 0;
    sendFileInfo(fileName);

    while (queue->isReadFinished() == false || queue->isEmpty() == false)
    {
        auto buffer = queue->getFilledBuffer().value();
        sendFileChunk(buffer);
        sent += buffer.bytesUsed;
        queue->returnBuffer(std::move(buffer));
        progressBar->draw();
    }

    if (receiveResponse() != ServerResponse::FILE_RECEIVED)
    {
        Logger::log("An error occurred during sending of a file");
    }
}

void TCPIP::TCPIPClient::disconnect()
{
    if (socketFD)
    {
        shutdown(socketFD, SHUT_RDWR);
        close(socketFD);
        socketFD = -1;
    }
}

TCPIP::TCPIPClient::~TCPIPClient()
{
    disconnect();
}
