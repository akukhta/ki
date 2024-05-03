#pragma once
#include "ICopyTool.hpp"
#include "StopWatch.h"
#include "../writer/MultiFileWriter.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../queue/BufferedQueue.hpp"

namespace TCPIP
{
    enum ToolType : char {Client, Server};
}

class TCPIPTool : public ICopyTool
{
public:
    explicit TCPIPTool(TCPIP::ToolType type, std::unique_ptr<BufferedReader<NonIPCTag>> fileReader,
        std::unique_ptr<MultiFileWriter> fileWriter,
        std::shared_ptr<FixedBufferQueue<NonIPCTag>> queue)
        : type(type), fileReader(std::move(fileReader)),
          fileWriter(std::move(fileWriter)), queue(std::move(queue)),
          sw(StopWatch::createAutoStartWatch("tcpip copy tool benchmark"))
          {}

    void copy() override
    {
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
    }

private:
    TCPIP::ToolType type;

    StopWatch sw;
    std::unique_ptr<MultiFileWriter> fileWriter;
    std::unique_ptr<BufferedReader<NonIPCTag>> fileReader;
    std::shared_ptr<FixedBufferQueue<NonIPCTag>> queue;

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