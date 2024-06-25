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
    if (type == KeyType::PAIR)
    {
        throw std::runtime_error("Loading a pair of RSA keys from memory is not supported");
    }

    RSAKeyPtr rsaKey = nullptr;

    BIO* bufio = BIO_new_mem_buf(data, size);

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
    BIO* mem = BIO_new(BIO_s_mem());
    if (!mem) {
        throw std::runtime_error("Failed to create BIO");
    }

    if (!PEM_write_bio_RSA_PUBKEY(mem, rsaKeyInternal)) {
        BIO_free(mem);
        throw std::runtime_error("Failed to write RSA public key to BIO");
    }

    BUF_MEM* mem_ptr;
    BIO_get_mem_ptr(mem, &mem_ptr);
    std::vector<char> buffer(mem_ptr->data, mem_ptr->data + mem_ptr->length);

    BIO_free(mem);
    return buffer;
}

TCPIP::RSAKey::RSAKey(TCPIP::RSAKey &&other)
{
    this->rsaKeyInternal = other.rsaKeyInternal;
    other.rsaKeyInternal = nullptr;
}
