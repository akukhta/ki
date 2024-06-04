#pragma once
#include <vector>

namespace TCPIP
{
    /// Client's business logic interface
    class IClient
    {
    public:
        virtual ~IClient() = default;
        virtual void sendFile(std::string const &fileName) = 0;
    };
}
