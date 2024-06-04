#pragma once
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <memory>
#include "IClient.hpp"
#include "../../queue/BufferedQueue.hpp"
#include "../../common/CLIProgressBar.hpp"

namespace TCPIP
{
    class TCPIPClient : public IClient
    {
    public:
        TCPIPClient(std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue);
        ~TCPIPClient();

        virtual void connectToServer() override;
        virtual void disconnect() override;
        virtual void sendFile(std::string const &fileName) override;

    private:

        size_t sendToServer(unsigned char *ptr, size_t bufferSize);
        virtual std::vector<unsigned char> receive() override;
        void sendFileChunk(TCPIP::Buffer &buffer);
        void sendFileInfo(std::string const& fileName);

        TCPIP::ServerResponse receiveResponse();

        std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue;
        std::unique_ptr<UI::CLIProgressBar> progressBar;
        int socketFD;
        sockaddr_in serverAddress;
        bool isConnected = false;
    };
}