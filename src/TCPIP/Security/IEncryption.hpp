#pragma once
#include <cstdlib>

namespace TCPIP
{
    class IEncryption
    {
    public:
        virtual ~IEncryption() = default;

        virtual void encrypt(unsigned char* data, size_t len) = 0;
        virtual void decrypt(unsigned char* data, size_t len) = 0;
    };
}