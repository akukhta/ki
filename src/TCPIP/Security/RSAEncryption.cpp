#include "RSAEncryption.hpp"
#include "RSAKey.hpp"
#include "../Common/Utiles.hpp"

void TCPIP::RSAEncryption::encrypt(TCPIP::Buffer &buffer, TCPIP::AbstractKey &key)
{
    encrypt(reinterpret_cast<char*>(buffer.data), buffer.bytesUsed, key);
}

void TCPIP::RSAEncryption::decrypt(TCPIP::Buffer &buffer, TCPIP::AbstractKey &key)
{
    decrypt(reinterpret_cast<char*>(buffer.data), buffer.bytesUsed, key);
}

TCPIP::RSAEncryption::RSAEncryption()
{
    ;
}

void TCPIP::RSAEncryption::encrypt(char *data, size_t len, TCPIP::AbstractKey &key)
{
    RSAKey& RsaKey = static_cast<RSAKey&>(key);

    blockSize = RSA_size(RsaKey.getInternalKey());
    size_t iterationsAmount = TCPIP::Utiles::satCeilingSub(len, blockSize);

    for (size_t i = 0; i < iterationsAmount; i++)
    {
        int rc = RSA_public_encrypt(blockSize, reinterpret_cast<unsigned char*>(data + (i * blockSize)),
                                    reinterpret_cast<unsigned char*>(data + (i * blockSize)), RsaKey.getInternalKey(), RSA_NO_PADDING);

        if (rc == -1)
        {
            throw std::runtime_error(TCPIP::Utiles::getOpenSSLError());
        }
    }
}

void TCPIP::RSAEncryption::decrypt(char *data, size_t len, TCPIP::AbstractKey &key)
{
    RSAKey& RsaKey = static_cast<RSAKey&>(key);

    blockSize = RSA_size(RsaKey.getInternalKey());
    size_t iterationsAmount = TCPIP::Utiles::satCeilingSub(len, blockSize);

    for (size_t i = 0; i < iterationsAmount; i++)
    {
        int rc = RSA_private_decrypt(blockSize, reinterpret_cast<const unsigned char*>(data + (i * blockSize)),
                                     reinterpret_cast<unsigned char*>(data + (i * blockSize)), RsaKey.getInternalKey(), RSA_NO_PADDING);
        if (rc == -1)
        {
            throw std::runtime_error(TCPIP::Utiles::getOpenSSLError());
        }
    }
}
