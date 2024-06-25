#include "ChaCha20Encryption.hpp"
#include "Chacha20Key.hpp"

TCPIP::ChaCha20Encryption::ChaCha20Encryption()
{

}

void TCPIP::ChaCha20Encryption::encrypt(TCPIP::Buffer &buffer, TCPIP::AbstractKey &key)
{
    encrypt(reinterpret_cast<char*>(buffer.data), buffer.bytesUsed, key);
}

void TCPIP::ChaCha20Encryption::decrypt(TCPIP::Buffer &buffer, TCPIP::AbstractKey &key)
{
    decrypt(reinterpret_cast<char*>(buffer.data), buffer.bytesUsed, key);
}

void TCPIP::ChaCha20Encryption::encrypt(char *data, size_t len, TCPIP::AbstractKey &key)
{
    auto& cKey = static_cast<Chacha20Key&>(key);

    auto _key = cKey.getKey();
    auto _iv = cKey.getIV();

    enc.SetKeyWithIV(reinterpret_cast<CryptoPP::byte*>(_key.data()), _key.size(),
                     reinterpret_cast<CryptoPP::byte*>(_iv.data()), _iv.size());
    enc.ProcessData(reinterpret_cast<unsigned char*>(data), reinterpret_cast<unsigned char*>(data), len);
}

void TCPIP::ChaCha20Encryption::decrypt(char *data, size_t len, TCPIP::AbstractKey &key)
{
    auto& cKey = static_cast<Chacha20Key&>(key);

    auto _key = cKey.getKey();
    auto _iv = cKey.getIV();

    dec.SetKeyWithIV(reinterpret_cast<CryptoPP::byte*>(_key.data()), _key.size(),
                     reinterpret_cast<CryptoPP::byte*>(_iv.data()), _iv.size());
    dec.ProcessData(reinterpret_cast<unsigned char*>(data), reinterpret_cast<unsigned char*>(data), len);
}
