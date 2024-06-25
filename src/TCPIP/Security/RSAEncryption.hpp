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
        void encrypt(char* data, size_t len, AbstractKey& key) override;
        void decrypt(char* data, size_t len, AbstractKey& key) override;

    private:
        size_t blockSize;
    };
}