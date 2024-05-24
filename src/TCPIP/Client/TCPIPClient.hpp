#pragma once
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <memory>
#include "IClient.hpp"
#include "../../queue/BufferedQueue.hpp"

namespace TCPIP
{
    class TCPIPClient : public IClient
    {
    public:
        TCPIPClient(std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue);

        virtual void connectToServer() override;
        virtual void disconnect() override;
        virtual void sendFile(std::string const &fileName) override;

    private:
        void sendToServer(unsigned char *ptr, size_t bufferSize);
        virtual std::vector<unsigned char> receive() override;
        void sendFileChunk(TCPIP::Buffer &buffer);
        void sendFileInfo(std::string const& fileName);

        TCPIP::ServerResponse receiveResponse();

        int socketFD;
        sockaddr_in serverAddress;
        bool isConnected = false;
        std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue;
    };
}