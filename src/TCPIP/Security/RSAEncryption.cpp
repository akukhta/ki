#include "RSAEncryption.hpp"
#include "RSAKey.hpp"
#include "../Common/Utiles.hpp"

void TCPIP::RSAEncryption::encrypt(TCPIP::Buffer &buffer, TCPIP::AbstractKey &key)
{
    RSAKey& RsaKey = static_cast<RSAKey&>(key);

    blockSize = RSA_size(RsaKey.getInternalKey());
    size_t iterationsAmount = TCPIP::Utiles::satCeilingSub(buffer.bytesUsed, blockSize);

    for (size_t i = 0; i < iterationsAmount; i++)
    {
        int rc = RSA_public_encrypt(blockSize, reinterpret_cast<unsigned char*>(buffer.data + (i * blockSize)),
                                    reinterpret_cast<unsigned char*>(buffer.data + (i * blockSize)), RsaKey.getInternalKey(), RSA_NO_PADDING);

        if (rc == -1)
        {
            throw std::runtime_error(TCPIP::Utiles::getOpenSSLError());
        }
    }
}

void TCPIP::RSAEncryption::decrypt(TCPIP::Buffer &buffer, TCPIP::AbstractKey &key)
{
    RSAKey& RsaKey = static_cast<RSAKey&>(key);

    blockSize = RSA_size(RsaKey.getInternalKey());
    size_t iterationsAmount = TCPIP::Utiles::satCeilingSub(buffer.bytesUsed, blockSize);

    for (size_t i = 0; i < iterationsAmount; i++)
    {
        int rc = RSA_private_decrypt(blockSize, reinterpret_cast<const unsigned char*>(buffer.data + (i * blockSize)),
                                    reinterpret_cast<unsigned char*>(buffer.data + (i * blockSize)), RsaKey.getInternalKey(), RSA_NO_PADDING);
        if (rc == -1)
        {
            throw std::runtime_error(TCPIP::Utiles::getOpenSSLError());
        }
    }
}
