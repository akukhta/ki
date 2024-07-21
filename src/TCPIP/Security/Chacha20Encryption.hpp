#pragma once
#include "IEncryption.hpp"
#include "cryptopp/secblock.h"
#include "cryptopp/chacha.h"
namespace TCPIP
{
    class Chacha20Encryption : public IEncryption
    {
    public:
        void encrypt(unsigned char *data, size_t len) override;
        void decrypt(unsigned char *data, size_t len) override;
        void setKeyIV(std::vector<char> key, std::vector<char> IV);
        std::vector<char>& getKey();
        std::vector<char>& getIV();

        static std::unique_ptr<Chacha20Encryption> generateEncryption();
    private:
        std::vector<char> key;
        std::vector<char> iv;
        CryptoPP::ChaCha::Encryption enc;
        CryptoPP::ChaCha::Decryption dec;
    };
}