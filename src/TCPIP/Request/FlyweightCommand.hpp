#pragma once
#include <memory>
#include <format>
#include <utility>
#include "ICommand.h"
#include "../Server/ConnectedClient.hpp"
#include "../../common/Logger.hpp"

namespace TCPIP
{
    class FlyweightCommand : public ICommand
    {
    public:
        FlyweightCommand(std::shared_ptr<ICommand> command, std::shared_ptr<ConnectedClient> client)
            : command(std::move(command)), client(std::move(client)) {}

        void execute() override
        {
            if (!command || !command)
            {
                Logger::log(std::format("The {} command can't be executed for {} client",
                    (command ? std::to_string(std::to_underlying(command->getType())) : "unknown"),
                    (client ? std::to_string(client->socket) : "unknown")));
                return;
            }
            else
            {
                command->execute(client);
            }
        }

        void execute(std::shared_ptr<struct ConnectedClient> client) override
        {
            // Should not be called
            std::unreachable();
        }

    private:
        std::shared_ptr<ICommand> command;
        std::shared_ptr<ConnectedClient> client;
    };
}
