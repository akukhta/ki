#pragma once
#include <vector>
#include <future>
#include "ICopyTool.hpp"
#include "StopWatch.h"
#include "../writer/MultiFileWriter.hpp"
#include "../reader/TCPIPBufferedFileReader.h"
#include "../queue/BufferedQueue.hpp"
#include "../TCPIP/Server/IServer.hpp"
#include "../TCPIP/Client/IClient.hpp"
#include "../TCPIP/Server/TCPIPServer.hpp"

class TCPIPTool : public ICopyTool
{
public:
    explicit TCPIPTool(std::shared_ptr<MultiFileWriter> fileWriter, std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue,
        std::unique_ptr<TCPIP::IServer> server)
            :  fileWriter(std::move(fileWriter)), queue(std::move(queue)),
               server(std::move(server)), sw(StopWatch::createAutoStartWatch("tcpip copy tool benchmark"))
    {
    }

    explicit TCPIPTool(std::unique_ptr<TCPIP::BufferedReader> fileReader,
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue, std::unique_ptr<TCPIP::IClient> client, std::vector<std::string> filesToSend)
            :    fileReader(std::move(fileReader)), queue(std::move(queue)),
                 client(std::move(client)), filesToSend(std::move(filesToSend)),
                 sw(StopWatch::createAutoStartWatch("tcpip copy tool benchmark"))
    {
    }

    void copy() override
    {
        if (client)
        {
            client->connectToServer();

            for (auto const & file: filesToSend)
            {
                queue->open();
                queue->startReading();
                auto readingTask = std::async(std::launch::async, &TCPIPTool::read, this, file);
                client->sendFile(file);
                readingTask.get();
            }
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
    std::unique_ptr<TCPIP::BufferedReader> fileReader;
    std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
    std::unique_ptr<TCPIP::IServer> server;
    std::unique_ptr<TCPIP::IClient> client;
    std::vector<std::string> filesToSend;

    std::jthread fileioThread;

    void read(std::string const &file)
    {
        fileReader->startReading(file);

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