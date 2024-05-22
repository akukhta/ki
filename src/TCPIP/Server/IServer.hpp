#pragma once

namespace TCPIP
{
    class IServer
    {
    public:
        virtual void run() = 0;
        virtual ~IServer() = default;
    };
}