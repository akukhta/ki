#pragma once
#include "ICopyTool.hpp"
#include "../reader/MMapFileReader.hpp"
#include "../writer/MMapFileWriter.hpp"
#include "../queue/IQueue.hpp"
#include "StopWatch.h"
#include <memory>
#include <thread>


template <class ChunkType>
class ParallelCopyTool : public ICopyTool
{
public:
    ParallelCopyTool(std::unique_ptr<MMapFileReader<ChunkType>> fileReader,
        std::unique_ptr<MMapFileWriter<ChunkType>> fileWriter, std::unique_ptr<IQueue<ChunkType>> queue) :
            fileReader(std::move(fileReader)), fileWriter(std::move(fileWriter)), queue(std::move(queue))
        {
            this->queue->open();
        }

    void copy() override
    {
        fileReader->open();
        fileWriter->create();

        writingThread = std::jthread(&ParallelCopyTool::writingFunction, this);

        while (!fileReader->isReadFinished())
        {
            queue->push(fileReader->read());
        }

        queue->close();
        fileReader->finishRead();
    }

    ~ParallelCopyTool() override = default;

private:
    void writingFunction()
    {
        while (!fileReader->isReadFinished() || !queue->isEmpty())
        {
            auto buf = queue->pop();
            
            if (buf)
            {
                fileWriter->write(std::move(*buf));
            }
            else
            {
                break;
            }
        }

        fileWriter->finishWrite();
    }

    std::unique_ptr<MMapFileReader<ChunkType>> fileReader;
    std::unique_ptr<MMapFileWriter<ChunkType>> fileWriter;
    std::unique_ptr<IQueue<ChunkType>> queue;

    StopWatch sw = StopWatch::createAutoStartWatch("mmap copy tool benchmark");

    std::jthread writingThread;
};