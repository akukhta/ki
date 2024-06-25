#pragma once
#include "AbstractKey.hpp"
#include "cryptopp/secblock.h"
#include <span>

namespace TCPIP
{
    class Chacha20Key : public AbstractKey
    {
    public:
        Chacha20Key() = default;
        Chacha20Key(Chacha20Key&& other);
        ~Chacha20Key() = default;

        static Chacha20Key generateRandomKey();
        static Chacha20Key loadKeyFromMemory(char const* key, size_t keyLength, char const* iv, size_t ivLength);

        std::span<char> getKey();
        std::span<char> getIV();

        size_t getKeySize() const noexcept;
        size_t getIVSize() const noexcept;
    private:
        CryptoPP::SecByteBlock key{32};
        CryptoPP::SecByteBlock iv{8};
    };
}