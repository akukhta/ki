#pragma once
#include <memory>
#include "RequestTypes.h"

namespace TCPIP
{
    class ICommand
    {
    public:
        /// API function for the real commands
        /// \param client
        virtual void execute(std::shared_ptr<class ConnectedClient> client) = 0;

        /// API function fur flyweight command wrapper
        /// And commands that do not require action to be executed
        virtual void execute() {};

        virtual ~ICommand() = default;

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