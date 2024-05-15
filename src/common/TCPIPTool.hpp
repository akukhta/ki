#pragma once
#include "ICopyTool.hpp"
#include "StopWatch.h"
#include "../writer/MultiFileWriter.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../queue/BufferedQueue.hpp"
#include "../TCPIP/IServer.hpp"
#include "../TCPIP/IClient.hpp"

class TCPIPTool : public ICopyTool
{
public:
    explicit TCPIPTool(std::unique_ptr<BufferedReader<TCPIPTag>> fileReader,
        std::shared_ptr<MultiFileWriter> fileWriter,
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue,
        std::unique_ptr<TCPIP::IServer> server, std::unique_ptr<TCPIP::IClient> client)
        : fileReader(std::move(fileReader)),
          fileWriter(std::move(fileWriter)), queue(std::move(queue)), server(std::move(server)),
          client(std::move(client)), sw(StopWatch::createAutoStartWatch("tcpip copy tool benchmark"))
          {}

    void copy() override
    {
        if (client)
        {
            client->run();
            readingFunction();
        }
        else if (server)
        {
            server->run();
            writingFunction();
        }
    }

private:

    StopWatch sw;
    std::shared_ptr<MultiFileWriter> fileWriter;
    std::unique_ptr<BufferedReader<TCPIPTag>> fileReader;
    std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
    std::unique_ptr<TCPIP::IServer> server;
    std::unique_ptr<TCPIP::IClient> client;

    std::jthread fileioThread;

    void readingFunction()
    {
        fileReader->open();

        while (!fileReader->isReadFinished())
        {
            fileReader->read();
        }

        fileReader->finishRead();
        queue->finishRead();
        queue->close();
    }

    void writingFunction()
    {
        while (true)
        {
            fileWriter->write();
        }
    }
};