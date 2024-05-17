#include "CommandFactory.hpp"
#include "FileInfoReceived.hpp"
#include "ObtainBufferRequest.hpp"
#include "FlyweightCommand.hpp"

TCPIP::CommandFactory::CommandFactory(std::shared_ptr<TCPIP::FixedBufferQueue> queue,
    std::shared_ptr<MultiFileWriter> writer) : queue(std::move(queue)), writer(std::move(writer))
        { }

std::shared_ptr<ICommand> TCPIP::CommandFactory::createCommand(TCPIP::RequestType type, std::shared_ptr<ConnectedClient> client)
{
    return std::shared_ptr<ICommand>();
}
