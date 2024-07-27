#pragma once

namespace TCPIP
{
    class XorEncryption
    {
    public:
        static void processData(unsigned char *data, size_t len)
        {
            unsigned char const* keyPtr = reinterpret_cast<unsigned char const *>(&XorEncryption::key);

            for (size_t i = 0; i < len; i++)
            {
                //data[i] ^= keyPtr[i % sizeof(XorEncryption::key)];
                data[i] ^= 24555;
            }
        }

    private:
        static inline size_t const key = 0xf2788a88bbc6512;
    };
}