#pragma once
#include "ICommand.h"
#include "RequestHeader.hpp"
#include "../Server/ConnectedClient.hpp"

namespace TCPIP
{
    class FileInfoReceived : public ICommand
    {
    public:
        void execute(std::shared_ptr<TCPIP::ConnectedClient> client) override
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