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
        TCPIPClient(std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue, std::string const &fileName);

        virtual void run() override;

    protected:
        virtual void connectToServer() override;
        virtual void send(std::vector<unsigned char> const& data) override;
        void ssend(unsigned char *ptr, size_t bufferSize);
        virtual std::vector<unsigned char> receive() override;

    private:

        void runFunction();

        void createFileChunkRequest(TCPIP::Buffer &buffer);

        void sendFileInfo();

        int socketFD;
        sockaddr_in serverAddress;
        bool isConnected = false;
        std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue;
        std::string fileName;
        std::jthread clientThread;
    };
}