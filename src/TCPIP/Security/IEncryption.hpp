#pragma once
#include <any>
#include "../Common/Buffer.hpp"
#include "AbstractKey.hpp"

namespace TCPIP
{
    class IEncryption
    {
    public:
        virtual ~IEncryption() = default;
        virtual void encrypt(Buffer &buffer, AbstractKey &key) = 0;
        virtual void decrypt(Buffer &buffer, AbstractKey &key) = 0;
    };
}