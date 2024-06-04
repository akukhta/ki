#pragma once
#include <vector>

namespace TCPIP
{
    /// Client's business logic interface
    class IClient
    {
    public:
        virtual ~IClient() = default;

        virtual void connectToServer() = 0;
        virtual void disconnect() = 0;
        virtual void sendFile(std::string const &fileName) = 0;

    protected:
        virtual std::vector<unsigned char> receive() = 0;
    };
}
