#pragma once
#include "ICopyTool.hpp"
#include "../reader/BufferedFileReader.hpp"
#include "../writer/BufferedFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"
#include <memory>
#include <thread>

class BPCopyTool : public ICopyTool
{
public:
    BPCopyTool(std::unique_ptr<BufferedReader> fileReader,
        std::unique_ptr<BufferedFileWriter> fileWriter, std::shared_ptr<FixedBufferQueue<Buffer>> queue) :
            fileReader(std::move(fileReader)), fileWriter(std::move(fileWriter))
        {}

    virtual void copy() override
    {
        fileReader->open();
        fileWriter->create();

        //writingThread = std::jthread(&BPCopyTool::writingFunction, this);

        while (!fileReader->isReadFinished())
        {
           fileReader->read();
        }

        fileReader->finishRead();
    }

    ~BPCopyTool()
    {
        fileReader->finishRead();
        fileWriter->finishWrite();
    }
    
private:
    void writingFunction()
    {
        while (!fileReader->isReadFinished() || !queue->isEmpty())
        {
            fileWriter->write();
        }

        fileWriter->finishWrite();
    }

    std::unique_ptr<BufferedReader> fileReader;
    std::unique_ptr<BufferedFileWriter> fileWriter;
    std::shared_ptr<FixedBufferQueue<Buffer>> queue;
    
    std::jthread writingThread;
};