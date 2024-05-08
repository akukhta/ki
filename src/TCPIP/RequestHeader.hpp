#pragma once
#include "TCPIPRequests.hpp"
#include <cstdlib>

namespace TCPIP
{
    struct RequestHeader
    {
        Request requestType;
        size_t requestLength;
    };
}