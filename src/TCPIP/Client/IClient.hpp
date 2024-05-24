#pragma once
#include <vector>

namespace TCPIP
{
    class IClient
    {
    public:
        //virtual void run() = 0;
        virtual ~IClient() = default;

        virtual void connectToServer() = 0;
        virtual void disconnect() = 0;
        virtual void sendFile(std::string const &fileName) = 0;

    protected:
        //virtual void send(std::vector<unsigned char> const& data) = 0;
        virtual std::vector<unsigned char> receive() = 0;
    };
}
