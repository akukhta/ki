#pragma once
#include "IEncryption.hpp"
#include "cryptopp/chacha.h"

namespace TCPIP {
    class ChaCha20Encryption : public IEncryption
    {
    public:
        ChaCha20Encryption();

        void encrypt(Buffer& buffer, AbstractKey& key) override;
        void decrypt(Buffer& buffer, AbstractKey& key) override;

    private:
        CryptoPP::ChaCha::Encryption enc;
        CryptoPP::ChaCha::Decryption dec;
    };
}
