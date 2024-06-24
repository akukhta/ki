#pragma once
#include <memory>
#include "AbstractKey.hpp"
#include <openssl/rsa.h>
#include <vector>
#define KEYBITS 2048

namespace TCPIP
{
    class RSAKey : public AbstractKey
    {
    private:
        using RSAKeyPtr = RSA*;

    public:
        enum class KeyType : char {PRIVATE, PUBLIC, PAIR};

        static RSAKey generateKey();
        static RSAKey loadKeyFromMem(unsigned char* data, size_t size, KeyType type);
        std::vector<char> getPublicKeyBin();

        ~RSAKey();
        RSAKey(RSAKeyPtr rsaKeyInternal, KeyType type);
        RSAKey(RSAKey const&) = delete;
        RSAKey(RSAKey && other);

    private:
        RSAKeyPtr getInternalKey();
        friend class RSAEncryption;
        RSAKeyPtr rsaKeyInternal = nullptr;
        KeyType type;
    };
}