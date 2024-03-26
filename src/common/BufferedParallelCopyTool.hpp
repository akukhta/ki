#pragma once
#include "ICopyTool.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../writer/BufferedFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"
#include "StopWatch.h"
#include <memory>
#include <thread>

class BPCopyTool : public ICopyTool
{
public:
    BPCopyTool(std::unique_ptr<BufferedReader<std::mutex, std::condition_variable, std::unique_lock, std::deque>> fileReader,
        std::unique_ptr<BufferedFileWriter<std::mutex, std::condition_variable, std::unique_lock, std::deque>> fileWriter, std::shared_ptr<FixedBufferQueue<std::mutex, std::condition_variable, std::unique_lock, std::deque>> queue) :
            fileReader(std::move(fileReader)),
            fileWriter(std::move(fileWriter)),
            queue(std::move(queue)),
            sw(StopWatch::createAutoStartWatch("vbuf copy tool benchmark"))
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

    std::unique_ptr<BufferedReader<std::mutex, std::condition_variable, std::unique_lock, std::deque>> fileReader;
    std::unique_ptr<BufferedFileWriter<std::mutex, std::condition_variable, std::unique_lock, std::deque>> fileWriter;
    std::shared_ptr<FixedBufferQueue<std::mutex, std::condition_variable, std::unique_lock, std::deque>> queue;
    
    std::jthread writingThread;
};