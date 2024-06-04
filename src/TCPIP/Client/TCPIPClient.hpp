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
#include "IClientCommunication.hpp"

namespace TCPIP
{
    class TCPIPClient : public IClient
    {
    public:
        TCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication, std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue);
        ~TCPIPClient();

        virtual void sendFile(std::string const &fileName) override;

    private:

        void sendFileChunk(TCPIP::Buffer &buffer);
        void sendFileInfo(std::string const& fileName);
        TCPIP::ServerResponse receiveResponse();

        /// underlying implementation of communication protocol
        std::unique_ptr<IClientCommunication> clientCommunication;
        std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue;
        std::unique_ptr<UI::CLIProgressBar> progressBar;

        bool isConnected = false;
    };
}