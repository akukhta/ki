#pragma once
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <memory>
#include <functional>
#include "IClient.hpp"
#include "../../queue/BufferedQueue.hpp"
#include "IClientCommunication.hpp"

namespace TCPIP
{
    class TCPIPClient : public IClient
    {
    public:
        TCPIPClient(std::unique_ptr<IClientCommunication> clientCommunication, std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue);
        ~TCPIPClient() override;

        void sendFile(std::string const &fileName) override;
        void setSendFinishedCallback(std::function<void(size_t)> callback);

    protected:
        /// Template method function that is being called before sending a buffer to the server
        /// Can be used, for example, to encrypt the data before sending
        /// \param buffer reference to a buffer is going to be sent
        virtual void prepareBufferToSend(TCPIP::Buffer &buffer) {};
        std::unique_ptr<IClientCommunication> clientCommunication;

    private:

        void sendFileChunk(TCPIP::Buffer &buffer);
        void sendFileInfo(std::string const& fileName);
        TCPIP::ServerResponse receiveResponse();

        /// underlying implementation of communication protocol
        std::shared_ptr<::FixedBufferQueue<TCPIPTag>> queue;
        std::function<void(size_t)> sendFinishedCallback;
    };
}