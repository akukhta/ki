#pragma once
#include "TCPIPRequests.hpp"
#include <cstdlib>

namespace TCPIP {
    struct RequestHeader
    {

    union
    {
        Request requestType;
        char requestTypeAsByte;
    };

    size_t requestLength;
    };
}