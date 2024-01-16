#pragma once
#include "ICopyTool.hpp"
#include "../reader/MMapFileReader.hpp"
#include "../writer/MMapFileWriter.hpp"
#include "../queue/IQueue.hpp"
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

    virtual void copy() override
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

    ~ParallelCopyTool()
    {
        fileReader->finishRead();
        fileWriter->finishWrite();
        queue->close();
    }
    
private:
    void writingFunction()
    {
        while (true)
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

    std::jthread writingThread;
};