#include "Chacha20Encryption.hpp"
#include <random>

void TCPIP::Chacha20Encryption::encrypt(unsigned char *data, size_t len)
{
    enc.ProcessData(data, data, len);
}

void TCPIP::Chacha20Encryption::decrypt(unsigned char *data, size_t len)
{
    dec.ProcessData(data, data, len);
}

void TCPIP::Chacha20Encryption::setKeyIV(std::vector<char> key, std::vector<char> IV)
{
    CryptoPP::byte keyAsByte[CryptoPP::ChaCha::DEFAULT_KEYLENGTH];
    std::copy(key.begin(), key.end(), keyAsByte);

    CryptoPP::byte ivAsByte[8];
    std::copy(IV.begin(), IV.end(), ivAsByte);

    this->key = key;
    this->iv = IV;

    enc.SetKeyWithIV(keyAsByte, std::size(keyAsByte), ivAsByte, std::size(ivAsByte));
    dec.SetKeyWithIV(keyAsByte, std::size(keyAsByte), ivAsByte, std::size(ivAsByte));
}

std::vector<char> &TCPIP::Chacha20Encryption::getKey()
{
    return key;
}

std::vector<char> &TCPIP::Chacha20Encryption::getIV()
{
    return iv;
}

std::unique_ptr<TCPIP::Chacha20Encryption> TCPIP::Chacha20Encryption::generateEncryption()
{
    auto encryption = std::make_unique<TCPIP::Chacha20Encryption>();

    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()};  // Generates random integers
    std::uniform_int_distribution<int> dist {1, 52};

    auto gen = [&](){
        return dist(mersenne_engine);
    };

    std::vector<char> key(32);
    std::generate(key.begin(), key.end(), gen);

    std::vector<char> iv(8);
    std::generate(iv.begin(), iv.end(), gen);

    encryption->setKeyIV(key, iv);
    return encryption;
}

