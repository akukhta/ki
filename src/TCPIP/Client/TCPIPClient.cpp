#include"TCPIPClient.hpp"
#include <utility>
#include <filesystem>
#include <format>
#include "../../common/Serializer.hpp"
#include "../Common/JsonSettingsParser.hpp"
#include "../Request/RequestCreator.hpp"
#include "../Common/Utiles.hpp"

TCPIP::TCPIPClient::TCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication,std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue)
    : clientCommunication(std::move(clientCommunication)), queue(std::move(queue))
{
    if (!this->clientCommunication->connect())
    {
        throw std::runtime_error("Can`t connect to the server");
    }
}

void TCPIP::TCPIPClient::sendFileChunk(TCPIP::Buffer &buffer)
{
    auto ptr = buffer.getData();
    Serializer<SerializerType::NoBuffer>::overwrite(ptr, 0, std::to_underlying(TCPIP::RequestType::FILE_CHUNK_RECEIVED));
    Serializer<SerializerType::NoBuffer>::overwrite(ptr, sizeof(RequestHeader::type), static_cast<short>(buffer.bytesUsed));

    auto bytesSent = clientCommunication->send(ptr, buffer.bytesUsed + RequestHeader::noAligmentSize());

    progressBar->addToValue(bytesSent);

    if (receiveResponse() == ServerResponse::CRITICAL_ERROR)
    {
        throw std::runtime_error("Internal Server Error");
    }
}

void TCPIP::TCPIPClient::sendFileInfo(std::string const& fileName)
{
    auto buffer = TCPIP::RequestCreator::createFileInfoRequest(fileName);
    clientCommunication->send(buffer.data(), buffer.size());

    if (receiveResponse() != ServerResponse::REQUEST_RECEIVED)
    {
        throw std::runtime_error("Internal Server Error");
    }
}

TCPIP::ServerResponse TCPIP::TCPIPClient::receiveResponse() {
    auto buffer = clientCommunication->receive(sizeof(ServerResponse));

    if (buffer.size() == sizeof(ServerResponse))
    {
        return *reinterpret_cast<ServerResponse *>(buffer.data());
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

TCPIP::TCPIPClient::~TCPIPClient()
{
    clientCommunication->disconnect();
}
