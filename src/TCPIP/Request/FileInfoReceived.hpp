#pragma once
#include "IRequest.h"
#include "RequestHeader.hpp"
#include "../Server/ConnectedClient.hpp"

namespace TCPIP
{
    class FileInfoReceived : public IRequest
    {
    public:
        FileInfoReceived(std::span<unsigned>)
        {}

        void handle() override
        {
            switch (header.type)
            {
                case RequestType::FILE_INFO_RECEIVED:
                {

                }
            }
        }

    private:
        ConnectedClient &client;
        TCPIP::RequestHeader const &header;
    };
}