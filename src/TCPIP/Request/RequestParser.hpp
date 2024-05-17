#pragma once
#include "ICommand.h"
#include "RequestHeader.hpp"
#include "FileInfoReceived.hpp"
#include "../Server/ConnectedClient.hpp"

namespace TCPIP
{
    class RequestParser : public ICommand
    {
    public:
        RequestParser() = default;

        void execute(std::shared_ptr<class ConnectedClient> client) override
        {
            /*
            switch (header.type)
            {
                case RequestType::FILE_INFO_RECEIVED:
                {
                    //RequestHandler::addRequest(std::make_shared<TCPIP::File>())
                }
            }
            */
        }
    };
}