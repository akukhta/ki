#pragma once
#include "RequestTypes.h"

namespace TCPIP
{
    class IRequest
    {
    public:
        virtual void handle() = 0;
        virtual ~IRequest() = default;

        /// Returns the type of the current request
        /// The main purpose is to avoid expensive dynamic_cast calls
        /// \return type of current request
        RequestType getType()
        {
            return type;
        }

    protected:
        RequestType type;
    };
}