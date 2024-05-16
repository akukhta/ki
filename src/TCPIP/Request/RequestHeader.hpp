#pragma once
#include "RequestTypes.h"

namespace TCPIP
{
    struct RequestHeader
    {
        union
        {
            RequestType type;
            char typeAsByte;
        };

        short messageLength;

        static size_t noAligmentSize()
        {
            return sizeof(type) + sizeof(messageLength);
        }
    };
}