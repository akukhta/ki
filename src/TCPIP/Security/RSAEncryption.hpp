#pragma once
#include "IEncryption.hpp"

namespace TCPIP
{
    class RSAEncryption : public IEncryption
    {
    public:
        RSAEncryption();
        void encrypt(Buffer &buffer, AbstractKey &key) override;
        void decrypt(Buffer &buffer, AbstractKey &key) override;

    private:
        size_t blockSize;
    };
}