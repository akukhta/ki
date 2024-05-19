#pragma once
#include "ICopyTool.hpp"
#include "StopWatch.h"
#include "../writer/MultiFileWriter.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../queue/BufferedQueue.hpp"
#include "../TCPIP/Server/IServer.hpp"
#include "../TCPIP/Client/IClient.hpp"
#include "../TCPIP/Server/TCPIPServer.hpp"

class TCPIPTool : public ICopyTool
{
public:
    explicit TCPIPTool(std::unique_ptr<BufferedReader<TCPIPTag>> fileReader,
        std::shared_ptr<MultiFileWriter> fileWriter,
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue,
        std::shared_ptr<TCPIP::IServer> server, std::unique_ptr<TCPIP::IClient> client)
        : fileReader(std::move(fileReader)),
          fileWriter(std::move(fileWriter)), queue(std::move(queue)), server(std::move(server)),
          client(std::move(client)), sw(StopWatch::createAutoStartWatch("tcpip copy tool benchmark"))
          {}

    ~TCPIPTool()
    {
        auto v =rand();
        v += rand();
    }

    void copy() override
    {

        if (client)
        {
            client->run();
            readingFunction();
            client->join();
        }
        else if (server)
        {
            server->run();
            writingFunction();
        }

        std::cout << "end" << std::endl;
    }

private:

    StopWatch sw;
    std::shared_ptr<MultiFileWriter> fileWriter;
    std::unique_ptr<BufferedReader<TCPIPTag>> fileReader;
    std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
    std::shared_ptr<TCPIP::IServer> server;
    std::unique_ptr<TCPIP::IClient> client;

    std::jthread fileioThread;

    void readingFunction()
    {
        queue->open();

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
        queue->open();

        while (true)
        {
            fileWriter->write();
        }

        queue->close();
    }
};