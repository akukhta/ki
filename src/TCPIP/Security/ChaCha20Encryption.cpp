#include "ChaCha20Encryption.hpp"
#include "Chacha20Key.hpp"

TCPIP::ChaCha20Encryption::ChaCha20Encryption()
{

}

void TCPIP::ChaCha20Encryption::encrypt(TCPIP::Buffer &buffer, TCPIP::AbstractKey &key)
{
    auto& cKey = static_cast<Chacha20Key&>(key);

    enc.SetKeyWithIV(cKey.getKey(), cKey.getKeySize(), cKey.getIV(), cKey.getIVSize());
    enc.ProcessData(buffer.data, buffer.data, buffer.bytesUsed);
}

void TCPIP::ChaCha20Encryption::decrypt(TCPIP::Buffer &buffer, TCPIP::AbstractKey &key)
{
    auto& cKey = static_cast<Chacha20Key&>(key);

    dec.SetKeyWithIV(cKey.getKey(), cKey.getKeySize(), cKey.getIV(), cKey.getIVSize());
    dec.ProcessData(buffer.data, buffer.data, buffer.bytesUsed);
}
