#pragma once
#include "ICopyTool.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../writer/BufferedFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"
#include "StopWatch.h"
#include <memory>
#include <thread>
#include <unordered_map>

class BPCopyTool : public ICopyTool
{
public:
    BPCopyTool(std::unique_ptr<BufferedReader> fileReader,
        std::unique_ptr<BufferedFileWriter> fileWriter, std::shared_ptr<FixedBufferQueue> queue) :
            fileReader(std::move(fileReader)),
            fileWriter(std::move(fileWriter)),
            queue(std::move(queue)),
            sw(StopWatch::createAutoStartWatch("copy tool benchmark"))
        {}

    virtual void copy() override
    {
        queue->open();

        fileReader->open();
        fileWriter->create();

        writingThread = std::jthread(&BPCopyTool::writingFunction, this);

        while (!fileReader->isReadFinished())
        {
            fileReader->read();
        }

        queue->close();
    }

    ~BPCopyTool() = default;
    
private:
    void writingFunction()
    {
        while (!fileReader->isReadFinished() || !queue->isEmpty())
        {
            fileWriter->write();
        }
    }

    StopWatch sw;

    std::unique_ptr<BufferedReader> fileReader;
    std::unique_ptr<BufferedFileWriter> fileWriter;
    std::shared_ptr<FixedBufferQueue> queue;
    
    std::jthread writingThread;
};