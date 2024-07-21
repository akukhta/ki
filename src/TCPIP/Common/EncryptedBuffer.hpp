#pragma once
#include "Buffer.hpp"

namespace TCPIP
{
    class EncryptedBuffer : public Buffer
    {
        size_t encryptedBytes = 0;
    };
}
