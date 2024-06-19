#pragma once
#include <memory>
#include "AbstractKey.hpp"
#include <openssl/rsa.h>
#define KEYBITS 2048

namespace TCPIP
{
    class RSAKey : public AbstractKey
    {
    private:
        using RSAKeyPtr = RSA*;
        enum class KeyType : char {PRIVATE, PUBLIC, PAIR};

    public:
        static RSAKey generateKey();
        static RSAKey loadKeyFromMem(unsigned char* data, size_t size, KeyType type);

        ~RSAKey();
        RSAKey(RSAKeyPtr rsaKeyInternal, KeyType type);
        RSAKey(RSAKey const&) = delete;

    private:
        RSAKeyPtr getInternalKey();
        friend class RSAEncryption;
        RSAKeyPtr rsaKeyInternal = nullptr;
        KeyType type;
    };
}