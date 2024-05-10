#pragma once
#include "ICopyTool.hpp"
#include "StopWatch.h"
#include "../writer/MultiFileWriter.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../queue/BufferedQueue.hpp"
#include "../TCPIP/IServer.hpp"

class TCPIPTool : public ICopyTool
{
public:
    explicit TCPIPTool(std::unique_ptr<BufferedReader<TCPIPTag>> fileReader,
        std::shared_ptr<MultiFileWriter> fileWriter,
        std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue,
        std::unique_ptr<IServer> server)
        : fileReader(std::move(fileReader)),
          fileWriter(std::move(fileWriter)), queue(std::move(queue)), server(std::move(server)),
          sw(StopWatch::createAutoStartWatch("tcpip copy tool benchmark"))
          {}

    void copy() override
    {
        /*
        if (type == TCPIP::Client)
        {
            queue->open();

            fileReader->open();
            fileioThread = std::jthread(&TCPIPTool::readingFunction, this);

            queue->close(); // ???
        }
        else if (type == TCPIP::Server)
        {
            fileioThread = std::jthread(&TCPIPTool::writingFunction, this);
        }
         */
    }

private:

    StopWatch sw;
    std::shared_ptr<MultiFileWriter> fileWriter;
    std::unique_ptr<BufferedReader<TCPIPTag>> fileReader;
    std::shared_ptr<FixedBufferQueue<TCPIPTag>> queue;
    std::unique_ptr<IServer> server;

    std::jthread fileioThread;

    void readingFunction()
    {
        while (!fileReader->isReadFinished())
        {
            fileReader->read();
        }

        fileReader->finishRead();
    }

    void writingFunction()
    {
        while (!queue->isReadFinished() && !queue->isEmpty())
        {
            fileWriter->write();
        }
    }
};