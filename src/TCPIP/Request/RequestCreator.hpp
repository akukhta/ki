#pragma once
#include "RequestTypes.h"
#include <vector>
#include <string>
#include "../Common/Buffer.hpp"

namespace TCPIP {
    /// Helper class to create cliet's requests
    class RequestCreator
    {
    public:
        RequestCreator() = delete;

        static std::vector<unsigned char> createFileInfoRequest(std::string const &fileName);
        static TCPIP::Buffer& createFileChunkRequest(TCPIP::Buffer &buffer);
    };
}