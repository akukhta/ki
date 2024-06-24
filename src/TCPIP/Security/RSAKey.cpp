#include "RSAKey.hpp"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <vector>
#include "../Common/Utiles.hpp"

TCPIP::RSAKey TCPIP::RSAKey::generateKey()
{
    RSAKeyPtr rsaKeyInternal = RSA_new();

    std::unique_ptr<BIGNUM, void(*)(BIGNUM*)> e = std::unique_ptr<BIGNUM, void(*)(BIGNUM*)>(BN_new(), &BN_free);

    if (!e || !BN_set_word(e.get(), RSA_F4) || !RSA_generate_key_ex(rsaKeyInternal, KEYBITS, e.get(), nullptr))
    {
        throw std::runtime_error(TCPIP::Utiles::getOpenSSLError());
    }

    return RSAKey(rsaKeyInternal, KeyType::PAIR);
}

TCPIP::RSAKey::~RSAKey()
{
    RSA_free(rsaKeyInternal);
}

TCPIP::RSAKey TCPIP::RSAKey::loadKeyFromMem(unsigned char *data, size_t size, TCPIP::RSAKey::KeyType type)
{
    static std::string_view pemPrivateKeyHeaderBeg = "-----BEGIN RSA PRIVATE KEY-----";
    static std::string_view pemPrivateKeyHeaderEnd = "-----END RSA PRIVATE KEY-----";
    static std::string_view pemPublicKeyHeaderBeg = "-----BEGIN RSA PUBLIC KEY-----";
    static std::string_view pemPublicKeyHeaderEnd = "-----END RSA PUBLIC KEY-----";

    if (type == KeyType::PAIR)
    {
        throw std::runtime_error("Loading a pair of RSA keys from memory is not supported");
    }

    std::vector<unsigned char> buffer;
    buffer.reserve(KEYBITS + std::max(pemPrivateKeyHeaderBeg.size(), pemPublicKeyHeaderBeg.size())
        + std::max(pemPrivateKeyHeaderEnd.size(), pemPublicKeyHeaderEnd.size()));

    auto& headerBegin = type == KeyType::PRIVATE ? pemPrivateKeyHeaderBeg : pemPublicKeyHeaderBeg;
    std::copy(headerBegin.begin(), headerBegin.end(), std::back_inserter(buffer));

    std::copy(data, data + size, std::back_inserter(buffer));

    auto& headerEnd = type == KeyType::PRIVATE ? pemPrivateKeyHeaderEnd : pemPublicKeyHeaderEnd;
    std::copy(headerEnd.begin(), headerEnd.end(), std::back_inserter(buffer));

    RSAKeyPtr rsaKey = nullptr;

    BIO* bufio = BIO_new_mem_buf(buffer.data(), buffer.size());

    if (type == KeyType::PUBLIC)
    {
        rsaKey = PEM_read_bio_RSA_PUBKEY(bufio, nullptr, nullptr, nullptr);
    }
    else if (type == KeyType::PRIVATE)
    {
        rsaKey = PEM_read_bio_RSAPrivateKey(bufio, nullptr, nullptr, nullptr);
    }

    BIO_free(bufio);

    if (rsaKey == nullptr)
    {
        throw std::runtime_error("Can`t load RSA Key: " + TCPIP::Utiles::getOpenSSLError());
    }

    return TCPIP::RSAKey(rsaKey, type);
}

TCPIP::RSAKey::RSAKey(TCPIP::RSAKey::RSAKeyPtr rsaKeyInternal, KeyType type) :
        rsaKeyInternal(rsaKeyInternal), type(type)
{
    ;
}

TCPIP::RSAKey::RSAKeyPtr TCPIP::RSAKey::getInternalKey()
{
    return rsaKeyInternal;
}

std::vector<char> TCPIP::RSAKey::getPublicKeyBin()
{
    BIO* buffer = BIO_new(BIO_s_mem());
    PEM_write_bio_RSA_PUBKEY(buffer, rsaKeyInternal);

    char* ptrToBuf;
    BIO_get_mem_ptr(buffer, ptrToBuf);
    std::vector<char> res(ptrToBuf, ptrToBuf + BIO_pending(buffer));

    BIO_free(buffer);
    return res;
}

TCPIP::RSAKey::RSAKey(TCPIP::RSAKey &&other)
{
    this->rsaKeyInternal = other.rsaKeyInternal;
    other.rsaKeyInternal = nullptr;
}
