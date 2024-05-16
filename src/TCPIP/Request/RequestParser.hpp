#pragma once
#include "IRequest.h"
#include "RequestHeader.hpp"
#include "FileInfoReceived.hpp"
#include "../Server/ConnectedClient.hpp"

namespace TCPIP
{
    class RequestParser : public IRequest
    {
    public:
        RequestParser(ConnectedClient& client, TCPIP::RequestHeader const& header)
            : client(client), header(header)
        {}

        void handle() override
        {
            switch (header.type)
            {
                case RequestType::FILE_INFO_RECEIVED:
                {
                    RequestHandler::addRequest(std::make_shared<TCPIP::File>())
                }
            }
        }

    private:
        ConnectedClient &client;
        TCPIP::RequestHeader const &header;
    };
}