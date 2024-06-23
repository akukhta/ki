#include "Chacha20Key.hpp"
#include "cryptopp/osrng.h"
#include "cryptopp/chacha.h"

TCPIP::Chacha20Key TCPIP::Chacha20Key::generateRandomKey()
{
    CryptoPP::AutoSeededRandomPool rng;

    Chacha20Key key;
    rng.GenerateBlock(key.key, CryptoPP::ChaCha::DEFAULT_KEYLENGTH);
    rng.GenerateBlock(key.iv, CryptoPP::ChaCha::IV_LENGTH);

    return key;
}

TCPIP::Chacha20Key::Chacha20Key(TCPIP::Chacha20Key &&other)
    : key(std::move(other.key)), iv(std::move(other.iv))
{
}

TCPIP::Chacha20Key TCPIP::Chacha20Key::loadKeyFromMemory(const char *key, size_t keyLength, const char *iv, size_t ivLength)
{
    Chacha20Key cKey;

    cKey.key = CryptoPP::SecByteBlock(reinterpret_cast<CryptoPP::byte const*>(key), keyLength);
    cKey.iv = CryptoPP::SecByteBlock(reinterpret_cast<CryptoPP::byte const*>(iv), ivLength);

    return cKey;
}

CryptoPP::SecByteBlock &TCPIP::Chacha20Key::getKey()
{
    return key;
}

CryptoPP::SecByteBlock &TCPIP::Chacha20Key::getIV()
{
    return iv;
}

size_t TCPIP::Chacha20Key::getKeySize() const noexcept
{
    return key.size();
}

size_t TCPIP::Chacha20Key::getIVSize() const noexcept
{
    return iv.size();
}
