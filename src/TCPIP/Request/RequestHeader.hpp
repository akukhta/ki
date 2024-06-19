#pragma once
#include <stdlib.h>
#include "RequestTypes.h"

namespace TCPIP
{
    /// Client's request header
    struct RequestHeader
    {
        /// Size of the request's payload data
        short requestDataSize;

        /// Type of the request
        union
        {
            RequestType type;
            char typeAsByte;
        };

        /// Get size of the header without padding
        static size_t noAligmentSize()
        {
            return sizeof(type) + sizeof(requestDataSize);
        }
    };
}