#pragma once
#include "ClientRequest.hpp"

namespace TCPIP
{
    class SecureClientRequest : public ClientRequest
    {
    public:
        void parseHeader() override {
            ClientRequest::parseHeader();
        }
    };
}