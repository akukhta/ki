#pragma once
#include "ClientRequest.hpp"

class IRequestHandler
{
public:
    virtual void handleRequest(ClientRequest& request) = 0;
};