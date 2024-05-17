#pragma once
#include <unordered_map>
#include "../Server/TCPIPQueue.hpp"
#include "../../writer/MultiFileWriter.hpp"
#include "../Server/ConnectedClient.hpp"
#include "ICommand.h"

namespace TCPIP
{
    /// Class that creates commands as flyweight
    /// To reduce the size of shared stored data
    /// i.e no need to store pointer to the queue in every command
    /// the queue is one and its pointer can be stored in shared section
    class CommandFactory
    {
    public:
        CommandFactory(std::shared_ptr<TCPIP::FixedBufferQueue> queue, std::shared_ptr<MultiFileWriter> writer);

        std::shared_ptr<ICommand> createCommand(RequestType type, std::shared_ptr<ConnectedClient> client);
        std::shared_ptr<ICommand> createCommand(RequestType type);


    private:
        std::shared_ptr<TCPIP::FixedBufferQueue> queue;
        std::shared_ptr<MultiFileWriter> writer;
        std::unordered_map<RequestType, std::shared_ptr<ICommand>> flyweightCommands;
    };
}