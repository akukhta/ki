#pragma once
#include "TCPIPRequests.hpp"
#include <cstdlib>

namespace TCPIP {
    struct RequestHeader
    {
        size_t requestLength;

        union
        {
            Request requestType;
            char requestTypeAsByte;
        };

        static size_t StructureSizeNoAligment()
        {
            return sizeof(requestLength) + sizeof(requestType);
        }
    };
}