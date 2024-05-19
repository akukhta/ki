#pragma once
#include <vector>

namespace TCPIP
{
    class IClient
    {
    public:
        virtual void run() = 0;
        virtual ~IClient() = default;
        virtual void join() = 0;

    protected:
        virtual void connectToServer() = 0;
        virtual void send(std::vector<unsigned char> const& data) = 0;
        virtual std::vector<unsigned char> receive() = 0;
    };
}
