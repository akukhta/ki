#pragma once
#include <cstdlib>
#include <vector>

namespace TCPIP {
    /// Underlying communication protocol interface
    class IClientCommunication {
    public:
        virtual ~IClientCommunication() = default;

        virtual bool connect() {return true;};
        virtual size_t send(unsigned char *data, size_t size) = 0;
        virtual std::vector<unsigned char> receive(size_t size) = 0;
        virtual void disconnect() = 0;
    };
}