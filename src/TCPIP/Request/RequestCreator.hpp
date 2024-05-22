#pragma once
#include "RequestTypes.h"
#include <vector>
#include <string>
#include "../../queue/TCPIPBuffer.hpp"

namespace TCPIP {
    class RequestCreator
    {
    public:
        RequestCreator() = delete;

        static std::vector<unsigned char> createFileInfoRequest(std::string const &fileName);
        static TCPIP::Buffer& createFileChunkRequest(TCPIP::Buffer &buffer);;
    };
}